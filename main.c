/* 
 *      main.cpp - argument parsing for dis68k
 *      (c) 2001 Georg Acher, georg@acher.org
 *      
 *      #include <gpl-stuff.h>
 *
 *     $Id: main.cpp,v 1.1.1.1 2002/04/29 23:18:16 acher Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "discore.h"

dispc_t pc;
/*------------------------------------------------------------------------*/
int read_file(dispc_t *pc, char *fname)
{
	FILE * file;
	int len;

	file=fopen(fname,"r");
	if (!file)
	{
		perror(fname);
		exit(-1);
	}
	fseek(file,0,SEEK_END);
	len=ftell(file);
	fseek(file,0,SEEK_SET);
	
	pc->total_len=len;
	pc->buf=(unsigned char*)malloc(len+32);
	fread(pc->buf,1,len,file);
	fclose(file);
	printf("FILE %s has length $%x bytes\n",fname,len);

	return 0;
}
/*------------------------------------------------------------------------*/
void usage(void)
{
	fprintf(stderr,"redis - a disassembler for reverse engineering\n"
		"       68k/8085 module & early adoption (c) 2001-2002 Georg Acher, georg@acher.org\n"
		"       6809 module & labelfile (c) 2001 Rainer Buchty, rainer@buchty.net\n"
		"Usage: redis <options> file\n"
		"Options:\n"
		" -b <hexnumber>  : Assume base address (default 0) \n"
		" -L <labelfile>  : Label file\n"  
		" -l <hexnumber>  : Length\n"
		" -s <hexnumber>  : Start at offset (relative to base address)\n"
		" -v <hexnumber>  : Assume default vector table at location x\n"
		" -c <cpu>        : Select CPU\n"
		"                   Valid CPUs are 6809, 8085, 68000/08/10/20/30/40/60\n"
		"                   Default CPU is 68000\n"
);	       
	exit(0);
}
/*------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	char c;
	int xlen=0;
	int cpu=68000;

	char fname[256];
	char labelfile[256];

	pc.base=0;
	pc.pc=0;
	pc.total_len=0;
	pc.vector_table=-1;
	labelfile[0]=0;
	

	while(1)
	{
		c = getopt(argc, argv, "b:L:l:s:v:c:?");
		
		if (c==-1)
			break;

		switch(c)
		{
		case 'b':
			pc.base=strtol(optarg,NULL,16);
			break;
		case 'L':
			strncpy(labelfile,optarg,256);
			break;
		case 'l':
			xlen=strtol(optarg,NULL,16);
			break;
		case 's':
			pc.pc=strtol(optarg,NULL,16);
			break;
		case 'v':
			pc.vector_table=strtol(optarg,NULL,16);
			break;
		case 'c':
			cpu=strtoul(optarg,NULL,10);
			break;
		default:
			usage();
			break;
		}
	}

	if (optind < argc)
		strncpy(fname,argv[optind++],256);
	else
		usage();


	read_file(&pc,fname);
	
	if (xlen!=0)
		pc.total_len=pc.pc+xlen;
	
	init_labels();

/*
	if (pc.vector_table!=-1)
		show_vector_table(&pc);

	if (labelfile[0])
		read_labels(labelfile);

	if (pc.vector_table==pc.pc)
		pc.pc=48*4;

	sort_labels();
*/
	discore(&pc,cpu,labelfile);
}
