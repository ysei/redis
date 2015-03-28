/* 
 *      modess.cpp - address mode decoding and low-level stuff for dis68k
 *      (c) 2001 Georg Acher, georg@acher.org
 *      
 *      #include <gpl-stuff.h>
 *
 *     $Id: 68k_modes.cpp,v 1.1.1.1 2002/04/29 23:18:30 acher Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../discore.h"
#include "68k_dis.h"
/*------------------------------------------------------------------------*/
// format effective address, convert to string

int format_ea(dispc_t *pc, char *str, int mode, int reg, int size)
{
	int displ;
	int extw;
	char tmp[128];
	int ret=0,scale;
	char *scaletab[4]={"","*2","*4","*8"};

//	printf("%08x (%02x): mode %i, reg %i\n",pc->pc, pc->opc,mode,reg);fflush(stdout);
	switch (mode)
	{
	case 0:
		sprintf(str,"D%i",reg);
		break;
	case 1:
		sprintf(str,"A%i",reg);
		break;
	case 2:
		sprintf(str,"(A%i)",reg);
		break;
	case 3:
		sprintf(str,"(A%i)+",reg);
		break;
	case 4:
		sprintf(str,"-(A%i)",reg);
		break;
	case 5:
		displ=get_word(pc);
		if (displ>32767)
			sprintf(str,"-$%x(A%i)",65536-displ,reg);
		else
			sprintf(str,"$%x(A%i)",displ,reg);
		break;
	case 6:
		extw=get_word(pc);
		displ=extw&255;
		if (BE(extw,8))
		{
			ret=1; // not 68k
			break;
		}
		scale=BEX(extw,9,2);
		
		if (displ>127)
			sprintf(str,"-$%x(A%i,%c%i.%c%s)",256-displ,reg,
				(BE(extw,15)?'A':'D'),BEX(extw,12,2),
				(BE(extw,11)?'L':'W'),scaletab[scale]);
		else
			sprintf(str,"$%x(A%i,%c%i.%c%s)",displ,reg,
				(BE(extw,15)?'A':'D'),BEX(extw,12,2),
				(BE(extw,11)?'L':'W'),scaletab[scale]);
		break;
	case 7:
		switch (reg)
		{
		case 0:
			displ=get_word(pc);
			if (BE(displ,15))
			    displ|=0xffff0000;
			format_pc(pc,displ,".W",str);
			break;
		case 1:
			displ=get_long(pc);
			format_pc(pc,displ,".L",str);
			break;
			
		case 2:
			displ=get_word(pc);
			if (displ>32767)
				displ-=65536;
			format_pc(pc,pc->base+pc->pc+2+displ,"",tmp);
			sprintf(str,"%s(PC)",tmp);		       
			break;

// 1234(PC,D0)
		case 3:
			extw=get_word(pc);
			displ=extw&255;
			if (BE(extw,8))
			{
				ret=1; // not 68k
				break;
			}
			if (displ>127)
				displ=displ-256;
			format_pc(pc,pc->base+pc->pc+2+displ,"",tmp);

			sprintf(str,"%s(PC,%c%i.%c)",tmp,
				(BE(extw,15)?'A':'D'),BEX(extw,12,2),
				(BE(extw,11)?'L':'W'));
			break;

		case 4:
			if (size==1)
				displ=get_word(pc)&255;
			if ((size&7)==2)
			{
				displ=get_word(pc);
				if (BE(displ,15))
					displ|=0xffff0000;
			}
			if (size==4)
				displ=get_long(pc);
/*
			if (size&8) // extend for MOVEA.W #x,Ay
			{
				if (BE(displ,15))
					displ|=0xffff0000;
			}
*/
			format_constant(pc,displ,size,str);

			break;

		case 5:	
		case 6:
		case 7:
			ret=1;
			break;			
		}
	}
	return ret;
}
/*------------------------------------------------------------------------*/
// movem reglist
int format_reglist(int mask, int dir, char *str)
{
	int n;
	int start=-1;
	char tmp[128];
	int first=1;
	
	*str=0;
	mask&=0xffff;
	if (dir==0)
	{
		for(n=0;n<17;n++)
		{
			if (BE(mask,n) && start==-1)
				start=n;

			if (!BE(mask,n) && start!=-1)
			{
				if (n==start+1)
					sprintf(tmp,"%c%i",
						((n-1)>7?'A':'D'),(n-1)&7);
				else
					sprintf(tmp,"%c%i-%c%i",
						(start>7?'A':'D'),start&7,
						(n-1>7?'A':'D'),(n-1)&7);
				if (!first)
					strcat(str,"/");
				strcat(str,tmp);
				first=0;
				start=-1;			
			}
		}
	}
	else
	{
		for(n=15;n>=0;n--)
		{
			if (BE(mask,n) && start==-1)
				start=n;

			if ( ((n==0) || !BE(mask,n))&& start!=-1)
			{
				if (n==start-1)
					sprintf(tmp,"%c%i",
						((15-(n+1))>7?'A':'D'),(15-(n+1))&7);
				else
					sprintf(tmp,"%c%i-%c%i",
						((15-start)>7?'A':'D'),(15-start)&7,
						((15-(n+1))>7?'A':'D'),(15-(n+1))&7);
				if (!first)
					strcat(str,"/");
				strcat(str,tmp);
				first=0;
				start=-1;
			}		
		}
	}
	return 0;
}
