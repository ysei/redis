/*
 * strextractor.cpp - extract zero-delimited strings from binary data
 *
 * (c) 2001 by Rainer Buchty, rainer@buchty.net
 *
 */

#include <stdio.h>
#include "discore.h"

int strextractor(int xstart, int xend, dispc_t *pc) {
unsigned int last, in, start, end, ccnt, len;

	len=0;

	start=xstart-pc->base;

	if (xend!=0)
		end=xend-pc->base;
	else
		end=0;
	
	last=0; ccnt=0; 
	while(1) {

		in=get_byte_n(pc,start);
		len++;

		if ((ccnt==0)||(in==0x7f)||((in<0x10)&&(in>0))) {
			if (ccnt!=0) {
				if (last==1)
					printf("\"");
				printf("\n");
			}
			
			printf("%08x\tDC.B\t",start+pc->base);
			last=0;
			ccnt=0;
		}


		if ((in>0x1f)&&(in<0x7f)) {
			if (last==0) {
				if (ccnt>0)
					printf(",");
				printf("\"");
			}
			printf("%c",in);
			last=1;
		} 
		
		else {
			if (last==1)
				printf("\"");
			if (ccnt>0) 
				printf(",");
			printf("$%02x",in);
			last=0;
		}

		ccnt++;
		
		if (in==0) {
			printf("\n");
			ccnt=0;
			last=0;
		}

		start++;

		if (((end!=0)&&(start>end))||((end==0)&&(in==0)))
			return len;
	} 

	return len;
}				
