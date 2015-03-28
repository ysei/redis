/*
 * dump.cpp - dump binary data
 * takes size (0/1/2)=(b/w/l) and items per line (step)
 * step=0 fills line up to zero mark
 *
 * (c) 2001 by Rainer Buchty, rainer@buchty.net
 */

#include <stdio.h>
#include "discore.h"

int dump(int xstart, int xend, int size, int step, dispc_t *pc) 
{
	char *csize[]={"","B","W","","L"};
	int i,start,end,len,in,val,xstep;

#ifdef DEBUG
	printf("%x %x %s %d\n",xstart,xend,csize[size],step);
#endif

	len=0;
	start=xstart-pc->base;

	if (xend!=0)
		end=xend-pc->base;
	else
		end=start+step*size-1;

	if (step==0)
		xstep=1;
	else
		xstep=step;

	while(start<=end) {
		printf("%08x\tDC.%s\t",start+pc->base,csize[size]);

		i=0;
		do {
			if (i*size+start>end)
				break;

			if (i>0)
				printf(",");

			in=get_byte_n(pc,i*size+start);
			printf("$%02x",in);
			val=in;
			len++;

			if (size>1) {
				in=get_byte_n(pc,i*size+start+1);
				val=(val<<8)|in;
				printf("%02x",in);
				len++;
				if (size==4) {
					in=get_word_n(pc,i*size+start+2);
					val=(val<<16)|in;
					printf("%04x",in);
					len+=2;
				}
			}
				
			i++;

			if (step==0) {
				xstep++;
				if (val==0) break;
			}
				
		} while (i<xstep);

		if (step!=0)
			start+=(step*size);
		else
			start+=(i*size);
		printf("\n");

	}

	return len;
}
