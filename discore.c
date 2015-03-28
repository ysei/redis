/* 
 *      discore.cpp - Core functions for 68k disassembly
 *      (c) 2001 Georg Acher, georg@acher.org
 *      
 *	support for data extraction
 *	(c) 2001 Rainer Buchty, rainer@buchty.net

 *      #include <gpl-stuff.h>
 *
 *     $Id: discore.cpp,v 1.1.1.1 2002/04/29 23:18:16 acher Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "discore.h"

#include "68k/68k_dis.h"
#include "6809/6809_dis.h"
#include "8085/8085_dis.h"

void discore(dispc_t *pc, int cpu, char *labelfile)
{      
	char str[512];
	int n;
	unsigned int xpc,i,j,dtype,dsize;
	label_t *l;

	// show machine vectors (if existing)
	switch(cpu) 
	{
		case 6309:
			if (pc->base+pc->total_len>0xffe0)
				mc6809_show_vectors(pc,1);
			break;
		case 6809:
			if (pc->base+pc->total_len>0xffe0)
				mc6809_show_vectors(pc,0);
			break;
		case 8085:
			break;
		case 68000:
		case 68010:
		case 68020:
		case 68030:
		case 68040:
		case 68060:
			if (pc->vector_table!=-1)
				mc68k_show_vectors(pc);
			break;
	}

	// on 68k the first 48*4 bytes are vectors		
	if ((cpu>=68000 && cpu<=68060)&&(pc->vector_table==pc->pc))
		pc->pc=48*4;

	// if labelfile exist, load it
	if (labelfile[0])
		read_labels(labelfile);

	// sort auto-generated and loaded labels
	sort_labels();

	// restrict assembly to valid memory range
	if ((cpu==6809)&&(pc->total_len+pc->base>0xffe0))
		pc->total_len&=0xffe0;

	if ((cpu==8085)&&(pc->total_len+pc->base>0xffff))
		pc->total_len&=0xffff;

	// disassemble / print data
	while(pc->pc<pc->total_len)
	{
//		printf("--- %08x %04x\n",pc->pc,get_word_n(pc,pc->pc));

		// get label
		l=find_label(pc->pc+pc->base);

		// we found a label and it's not type 'l'
		if ((l) && (l->dtype!='l')) 
		{

			// if set, print description/explanation
			if (l->desc!=0)
				printf("\n; %s",l->desc);

			// label name
			printf("\n%s:\n",l->name);

			// data output
			switch(l->dtype) 
			{
				case 's':	// string (null-terminated text)
					pc->pc+=strextractor(l->pc, l->end, pc);
					break;
				case 't':	// text (of given length)
					pc->pc+=textractor(l->pc, l->end, l->dsize, pc);
					break;
				case 'p': // pattern
					xpc=l->pc;
					do 
					{
						for(i=0;i<strlen(l->seq);i+=2) 
						{
							dtype=l->seq[i]-1;
							dsize=l->seq[i+1]-1;
							if (dsize==0)
								dsize++;
						
#ifdef DEBUG
							printf("%d/%d: %d %d\n",i,strlen(l->seq),dtype,dsize);
#endif
							switch(dtype) {
							case 's':
								j=strextractor(xpc,0,pc);
								pc->pc+=j;	
								xpc+=j;
								break;
							case 't':
								j=textractor(xpc,xpc+dsize,dsize,pc);
								pc->pc+=j;
								xpc+=j;
								break;
							case 'b':
								dtype=1;
								goto proc_dfield;
							case 'w':
								dtype=2;
								goto proc_dfield;
							case 'd':
								dtype=4;
proc_dfield:
								j=dump(xpc,xpc-1+dtype*dsize,dtype,dsize,pc);
								pc->pc+=j;
								xpc+=j;
								break;
							default:
								break;
							}
						}
					printf("\n");
				} while (xpc<=(unsigned int)l->end);
				break;
			case 'b':	// byte
				dtype=1;
				goto proc_data;
			case 'w':	// word = 2 bytes
				dtype=2;
				goto proc_data;
			case 'd':	// double = 4 bytes
				dtype=4;
proc_data:
				pc->pc+=dump(l->pc, l->end, dtype, l->dsize, pc); 
				break;
			default:	// unknown type
				break;
			}
		}	 

		else 
		{
			if (cpu==6809)
				mc6809_disass(pc,str,0);
			else if (cpu==6309)
				mc6809_disass(pc,str,1);
			else if (cpu==8085)
				i8085_disass(pc,str);
			else
				mc68k_disass(pc,str);
				

			if (l) 
			{
				if (l->desc!=0)
					printf("\n; %s",l->desc);
				printf("\n%s:\n",l->name);
			}

			if ((cpu!=6809) && (cpu!=8085))
			{
				printf("%08x\t",pc->pc+pc->base);
				for(n=0;n<pc->len;n+=2)
					printf("%04x ", get_word_n(pc,pc->pc+n));
				for(n=0;n<8-pc->len;n+=2)
					printf("\t");
				if (pc->len>4)
					printf("\t");

			} 
			else 
			{
				printf("%04x\t",pc->pc+pc->base);
				for(n=0;n<pc->len;n++)
					printf("%02x ", get_byte_n(pc,pc->pc+n));

				if (pc->len<3)
					printf("\t");
				printf("\t");
			}

			printf("%s\n",str);

			pc->pc+=pc->len;
		}

	}

	// for 6309/6809 the vectors come last
	if (cpu==6809)
		mc6809_show_vectors(pc,0);
	else if (cpu==6309)
		mc6809_show_vectors(pc,1);
}
