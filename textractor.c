/*
 * textractor.cpp - extract ASCII text from binary data
 * size denotes length of text fragments
 *
 * (c) 2001 by Rainer Buchty (rainer@buchty.net)
 *
 */

#include <stdio.h>
#include "discore.h"

int textractor(int xstart, int xend, int size, dispc_t *pc) 
{
	int ccnt, last, in, start, end,len;

	len=0;
	start=xstart-pc->base;
	if (end!=0)
		end=xend-pc->base;
	else
		end=start+size;

	if (size==0)
		size=end-start;
	
	ccnt=0;
	while(start<=end) {
			
		if (ccnt==0) {
			if (start+size>end)
				break;

			printf("%08x\tDC.B\t",start+pc->base);
			last=0;
		}

		in=get_byte_n(pc,start);
		len++;

		if ((in>0x1f)&&(in<0x60)) {
			if (last==0) {
				if (ccnt>0)
					printf(",");
				printf("\"");
			}
			printf("%c",in);
			last=1;
		} else {
			if (last==1)
				printf("\"");
			if (ccnt>0)
				printf(",");
			printf("$%02x",in);
			last=0;
		}
		if (ccnt==size) {
			if (last==1)
				printf("\"");
			printf("\n");
			ccnt=0;
		} else
			ccnt++;
		start++;
	}

	return len;
}				
