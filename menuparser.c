#include <stdio.h>
#include <stdlib.h>

int mtab[]=
{
	0x0c80,0x07c3,0x0144,0x0000,
	0x0165,0x016c,0x071f,0x0173,0x0180,0x0000,
	0x0790,0x01a2,0x018c,0x0183,0x0188,0x05c9,0x02ca,0x0000,
	0x0793,0x018c,0x024c,0x0195,0x013d,0x07db,0x01af,0x07f0,0x01bb,0x0000,
	0x0793,0x0ac4,0x024c,0x015c,0x013d,0x0000,
	0x01c2,0x0388,0x0180,0x0000,
	0x0165,0x06a9,0x0388,0x0180,0x0000,
	0x02ec,0x024c,0x0388,0x0000,
	0x031c,0x030a,0x0000,
	0x045e,0x0303,0x0000,
	0x0408,0x024e,0x0315,0x0000,
	0x0364,0x0357,0x0369,0x0000,
	0x036d,0x0394,0x0000,
	0x039a,0x3100,0x039a,0x3200,0x028a,0x0485,0x0000,
	0x03c0,0x024c,0x03c6,0x024c,0x03a8,0x0000,
	0x03d4,0x024e,0x03da,0x024c,0x0000,
	0x02a3,0x024e,0x03ef,0x0000,
	0x03f5,0x024c,0x03fe,0x0000,
	0x0408,0x024c,0x040e,0x0000,
	0x03de,0x03eb,0x0000,
	0x043b,0x0433,0x0000,
	0x0425,0x3100,0x0425,0x3200,0x0443,0x07a2,0x0449,0x0000,
	0x0450,0x3100,0x0450,0x3200,0x0450,0x3300,0x0364,0x046f,0x0000,
	0x0364,0x046a,0x0000,
	0x0364,0x0473,0x0000,
	0x045e,0x0464,0x0000,
	0x047f,0x024c,0x03ef,0x0000,
	0x02b4,0x024e,0x028f,0x0000,
	0x0257,0x04bb,0x0388,0x0000,
	0x0257,0x0388,0x024c,0x024e,0x0000,
	0x03da,0x024e,0x04e8,0x024c,0x0357,0x0250,0x04ee,0x024c,0x0000,
	0x04c8,0x04d4,0x0000,
	0x04ce,0x04d4,0x0000,
	0x079c,0x0388,0x07db,0x04db,0x0000,
	0x0518,0x04bb,0x06d3,0x0000,
	0x0799,0x0539,0x06d3,0x0542,0x0000,
	0x079c,0x0518,0x0556,0x0000,
	0x07cf,0x055d,0x0000,
	0x0a86,0x05b0,0x2d00,0x0a40,0x05b0,0x2d00,0x0a86,0x039a,0x2d00,0x0a40,0x039a,0x2d00,0x05be,0x05c4,0x024c,0x0000,
	0x05c9,0x05ce,0x0000,
	0x024c,0x0388,0x024c,0x0a48,0x0000,
	0x058c,0x024c,0x05be,0x0000,
	0x0793,0x057a,0x0254,0x072a,0x02ec,0x024c,0x0388,0x0000,
	0x0799,0x057a,0x05a2,0x0000,
	0x079c,0x0594,0x0556,0x07d5,0x05d6,0x05df,0x0000,
	0x072f,0x057a,0x05ac,0x024c,0x0000,
	0x059c,0x057a,0x0000,
	0x07de,0x06c3,0x07f0,0x067e,0x024e,0x0000,
	0x07cc,0x0670,0x0000,
	0x0a96,0x41c3,0x0000,
	0x0a96,0x42c3,0x0000,
	0x03da,0x024e,0x063f,0x0008,0x0684,0x07a5,0x068c,0x0000,
	0x031c,0x024c,0x033b,0x0000,
	0x07c0,0x0735,0x07f6,0x073b,0x0000,
	0x069a,0x079f,0x068c,0x0000,
	0x069a,0x079c,0x057a,0x06d3,0x024c,0x06db,0x0000,
	0x069a,0x07a5,0x057a,0x06bb,0x0000,
	0x069a,0x0799,0x057a,0x06b2,0x071f,0x06bb,0x0000,
	0x0ae2,0x0b34,0x0000,
	0x0af0,0x0b34,0x0000,
	0x0afa,0x0b34,0x0000,
	0x0ae2,0x0b28,0x0000,
	0x0af0,0x0b28,0x0000,
	0x0afa,0x0b28,0x0000,
	0x07c6,0x072f,0x069a,0x072a,0x06ca,0x0000,
	0x07c6,0x072f,0x06ca,0x072a,0x069a,0x0000,
	0x0793,0x069a,0x024c,0x0703,0x0000,
	0x0ac4,0x079f,0x068c,0x0000,
	0x0ac4,0x07a5,0x06a4,0x024c,0x0b6a,0x0000,
	0x0ac4,0x07a5,0x06a4,0x024c,0x0b72,0x0000,
	0x06d3,0x024c,0x0750,0x0000,
	0x06c3,0x024c,0x0750,0x0000,
	0x06a9,0x06d3,0x0000,
	0x06ca,0x024c,0x06db,0x0000,
	0x06a9,0x06c3,0x0000,
	0x06ca,0x024c,0x06bb,0x0000,
	0x06f9,0x024e,0x06ff,0x0000,
	0x0716,0x06c3,0x024c,0x06b2,0x0000,
	0x0801,0x024c,0x080f,0x024c,0x024e,0x07e1,0x081e,0x0000,
	0x07fa,0x024c,0x080f,0x024c,0x024e,0x07e1,0x081e,0x0000,
	0x0450,0x024e,0x028f,0x0000,
	0x0826,0x0294,0x0000,
	0x0801,0x024c,0x0813,0x0000,
	0x0801,0x024c,0x0854,0x0000,
	0x0822,0x024e,0x029e,0x0000,
	0x0450,0x024e,0x0369,0x0000,
	0x082b,0x03ad,0x0000,
	0x080f,0x024e,0x03ad,0x0000,
	0x0866,0x024e,0x0869,0x0000,
	0x0863,0x024e,0x0869,0x0000,
	0x0863,0x024e,0x0869,0x0896,0x0000,
	0x084b,0x024e,0x0294,0x0000,
	0x082b,0x024c,0x03ad,0x0000,
	0x08b2,0x0450,0x0000,
	0x0456,0x024e,0x028f,0x0000,
	0xffff
};

unsigned char *buf;

#ifdef DEBUG	

int tabcnt=0;

void indent()
{
	int ptr;
	printf("%2d\t",tabcnt);
	for(ptr=0;ptr<tabcnt;ptr++)
		printf("\t");
}
#endif

int render(int outptr)
{
	int ptr, ret, adr;
	unsigned char val;

#ifdef DEBUG	
	// indent
	printf("\n\t");
	indent();
#endif		

	// init pointers
	ptr=outptr;	// running pointer
	ret=outptr;	// reference pointer

	// get current value
	val=buf[ptr];

	// it's a pointer, resolve it
	if(val<0x20)
	{
		adr=( (val&0xff)<<8) | (buf[ptr+1]&0xff);
		ptr+=2;
				
		if((val!=0)&&(adr!=0))
		{
#ifdef DEBUG	
			printf("<%04x deref %04x>",ptr-2,adr); fflush(stdout);
			tabcnt++;
#endif

			render(adr);

#ifdef DEBUG	
			printf("\n");
			indent();
			printf("<endref, next %04x (%2d)>",ptr,ptr-ret); fflush(stdout);
			tabcnt--;
#endif
		}

		else
		{
			if(val==0)
				ptr--;
#ifdef DEBUG	
			printf("<%04x zeroptr %04x>",ptr-2,ptr);
#endif
		}

	}
	
	// it's a string, render it
	else
	{
#ifdef DEBUG	
		printf("[%04x-->",ptr); fflush(stdout);
#endif
		for(; (val=buf[ptr])!=0; ptr++)
		{
			if((val>=0x20)&&(val<=0x5f)) 
				printf("%c",val); 
			else 
				printf("(%02x)",val);
		} 
		
#ifdef DEBUG	
		printf("<--%04x(%2d)] ",ptr,ptr-ret); fflush(stdout);
#endif
	}

		return ptr;
}

int main(int argc, char **argv)
{
	FILE *bin;
	int a;
	
	buf=malloc(131072);
	
	bin=fopen("68k.bin","r");
	fread(buf,131072,1,bin);
	fclose(bin);
	printf("Loaded.\n\n"); fflush(stdout);

	if(argc==1)
		// start of table
		a=0x8c2;
	else
		a=(int)strtoul(argv[1],NULL,0);

	printf("%06x:\t",a+0xc00000);

	while(a<0xc3c)	//mtab[i]!=0xffff)
	{
		// hitting a null pointer -> skip
		if(buf[a]==0)
		{
			a++;
			if(buf[a]==0) a++;
			printf("\n%06x:\t",a+0xc00000);
		}
		// not zero, either pointer or string
		else
		{
			a=render(a);
		}
	}
}
