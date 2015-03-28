/*

;llaaaatt(dd*)cc

; 	start marker
ll	number of data bytes
aaaa	target address
tt	record type
	00	data
	01	end of file
	02	extended segment address
	04	extended linear address
(dd*)	data 
cc	checksum (sum over data bytes & 0xff, 2's complement)


extended linear address:
	ll=02
	aaaa=0000
	tt=04
	dddd=upper 16 bits of address
	cc
	
extended segment address:
	ll=02
	aaaa=0000
	tt=02
	dddd=segment of address
	cc
	
end of file:
	ll=00
	aaaa=0000
	tt=01
	cc=ff

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	int len;
	int adr;
	int typ;
	char *buf;
	int chk;
} hex_t;

int read_line(FILE *fd, hex_t *hex, int line)
{
int i,sum=0,offs=0;
char *x;

	if(fgetc(fd)!=':')
	{
		fprintf(stderr,"Line %d: No Intel Hex file.\n",i);
		exit(-1);
	}	
	
	x=(char *)malloc(257);
	memset(x,0,257);

	fread(x,2,1,fd);
	hex->len=strtoul(x,0,16);
	
	fread(x,4,1,fd);
	hex->adr=strtoul(x,0,16);
	
	memset(x,0,4);
	fread(x,2,1,fd);
	hex->typ=strtoul(x,0,16);
	
	memset(x,0,4);
	if(hex->len>0)
	{
		fread(x,2*hex->len,1,fd);
		hex->buf=strdup(x);
	} 
	else
	{
		hex->buf=(char *)malloc(1);
		hex->buf[0]=0;
	}			

	memset(x,0,hex->len);
	fread(x,2,1,fd);
	hex->chk=strtoul(x,0,16);

	x[2]=0;
	if((hex->typ==2)||(hex->typ==4))
	{
		x[0]=hex->buf[0];
		x[1]=hex->buf[1];
		offs=strtoul(x,0,16)&0xffff;
		if(hex->typ==2)
			offs<<=4;
		else
			offs<<=16;	
	}

	sum=0; 
	x[2]=0;
	for(i=0;i<2*hex->len;)
	{
		x[0]=hex->buf[i++];
		x[1]=hex->buf[i++];
		sum+=(strtoul(x,0,16)&0xff);
	}
	sum+=hex->len;
	sum+=hex->adr>>8;
	sum+=hex->adr&0xff;
	sum+=hex->typ;

	sum^=0xff;
	sum++;
	sum&=0xff;
/*
	printf(
		"Line %d:\n"
		"\tLength:\t%02x\n"
		"\tAddr:\t%04x\n"
		"\tType:\t%02x\n"
		"\tChksum:\t%02x\n"
		"\tData:\t%s\n\n"
	,line,hex->len,hex->adr,hex->typ,hex->chk,hex->buf);
*/	
	if(sum!=hex->chk)
		fprintf(stderr,"Line %d: Hex file checksum error - %02x (expected %02x).\n",line,sum,hex->chk);

	do {
			i=fgetc(fd);
	} while((!feof(fd))&&(i!=':'));
	
	if(i==':')
		fseek(fd,-1,SEEK_CUR);
	
	free(x);

	return offs|hex->adr;
}

int main(int argc, char **argv)
{
FILE *fd;
unsigned char *mem,x[3];
hex_t *hex=NULL;
unsigned int i=0,j,k,size,addr,offs,ptr,min,max;

	argv++;
	
	fd=fopen(*argv,"r");
	
	do
	{
		if((i&255)==0)		
			hex=(hex_t *)realloc(hex,256*sizeof(hex_t)*((i>>8)+1));

		j=read_line(fd,&(hex[i]),i+1);
		if(i==0)
		{
			min=j;
			max=j;
		}
		else 
		{	
			if (j<min)
				min=j;
			else if(max<j)
				max=j;
		}
		i++;
	} while(!feof(fd));

	printf("Read %d lines.\n",i);

	printf("Address range: %04x to %04x\n",min,max);
	size=0;
	for(j=0;j<i;j++)
		if(size<hex[j].adr)
			size=hex[j].adr;		

	for(j=0;j<8;j++)
		if((size>>8)<(1<<j))
			size=(1<<j)*256;

	fprintf(stderr,"Eprom size: %d bytes\n",size);
	mem=(char *)malloc(size);
		
	offs=0;
	for(j=0;j<i;j++)
	{
		switch(hex[j].typ)
		{
		case 0x00:
			ptr=offs+hex[j].adr;
			x[2]=0;
			for(k=0;k<2*hex[j].len;)
			{
				x[0]=hex[j].buf[k++];
				x[1]=hex[j].buf[k++];
				mem[ptr++]=strtoul(x,0,16);
			}
			break;
		case 0x01:
			j=i;
			break;
		case 0x02:
			offs=strtoul(hex[j].buf,0,16)<<4;
			break;
		case 0x04:
			offs=strtoul(hex[j].buf,0,16)<<16;
			break;
		default:
			fprintf(stderr,"Unkown type of entry %d.\n",j);
			exit(-1);
		}
	}

	for(i=0;i<size;i+=16)
	{
		printf("%04x:  ",i);
		for(j=0;j<16;j++)
			printf("%02x ",mem[i+j]&0xff);
		printf("  ");
		for(j=0;j<16;j++)
		{
			if((mem[i+j]<32)||(mem[i+j]>126))
				printf(".");
			else
				printf("%c",mem[i+j]);
		}
		printf("\n");
	}
	exit(0);	
}	