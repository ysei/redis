#include <stdio.h>
#include "discore.h"

int little_endian=0;

/*------------------------------------------------------------------------*/
unsigned int get_byte(dispc_t *pc)
{
	unsigned int a;
	unsigned int p;
	p=pc->pc+pc->len;
	a=pc->buf[p];
	pc->len+=1;
	return a;
}
/*------------------------------------------------------------------------*/
unsigned int get_word(dispc_t *pc)
{
	unsigned int a;
	unsigned int p;
	p=pc->pc+pc->len;
	if (!little_endian)
		a=(pc->buf[p]<<8)|(pc->buf[p+1]);
	else
		a=(pc->buf[p])|(pc->buf[p+1]<<8);
	pc->len+=2;
	return a;
}
/*------------------------------------------------------------------------*/
unsigned int get_long(dispc_t *pc)
{
	unsigned int a;
	unsigned int p;
	p=pc->pc+pc->len;
	if (!little_endian)
		a=(pc->buf[p]<<24)|(pc->buf[p+1]<<16)|
			(pc->buf[p+2]<<8)|(pc->buf[p+3]);
	else
		a=(pc->buf[p])|(pc->buf[p+1]<<8)|
			(pc->buf[p+2]<<16)|(pc->buf[p+3]<<24);
		
	pc->len+=4;
	return a;
}
/*------------------------------------------------------------------------*/
unsigned int get_byte_n(dispc_t *pc, int p)
{
	unsigned int a;
	a=pc->buf[p];
	return a;
}
/*------------------------------------------------------------------------*/
unsigned int get_word_n(dispc_t *pc, int p)
{
	unsigned int a;
	if (!little_endian)
		a=(pc->buf[p]<<8)|(pc->buf[p+1]);
	else
		a=(pc->buf[p])|(pc->buf[p+1]<<8);
	return a;
}
/*------------------------------------------------------------------------*/
// get without increasing PC
unsigned int get_long_n(dispc_t *pc, int p)
{
	unsigned int a;
	if (!little_endian)
		a=(pc->buf[p]<<24)|(pc->buf[p+1]<<16)|
			(pc->buf[p+2]<<8)|(pc->buf[p+3]);
	else
		a=(pc->buf[p])|(pc->buf[p+1]<<8)|
			(pc->buf[p+2]<<16)|(pc->buf[p+3]<<24);
		

	return a;
}
/*------------------------------------------------------------------------*/
int get_displacement5(dispc_t *pc)
{
	int displ;
	displ=get_byte(pc)&0x1f;

	if (displ>15)
		displ=displ-32;
	return displ;
}
/*------------------------------------------------------------------------*/
int get_displacement8(dispc_t *pc)
{
	int displ;
	displ=get_byte(pc);
	if (displ>127)
		displ=displ-256;
	return displ;
}
/*------------------------------------------------------------------------*/
int get_displacement16(dispc_t *pc)
{
	int displ;
	displ=get_word(pc);
	if (displ>32767)
		displ=displ-65536;
	return displ;
}
/*------------------------------------------------------------------------*/
int get_displacement32(dispc_t *pc)
{
	int displ;
	displ=get_long(pc);
	return displ;
}
/*------------------------------------------------------------------------*/
// PC to hex or label if defined
void format_pc(dispc_t *pc, int p, char *ext,char *str)
{
	label_t *l;
	l=find_label( p);
	if (!l)
		sprintf(str,"$%x%s",p,ext);
	else
		sprintf(str,"%s",l->name);
}
/*------------------------------------------------------------------------*/
// PC to hex or label if defined
void format_constant(dispc_t *pc, int p, int size, char *str)
{
	label_t *l;
	l=find_label( p);
	if (!l)
		sprintf(str,"#$%x",p);
	else
		sprintf(str,"#%s",l->name);
}
/*------------------------------------------------------------------------*/
// returns 0: mask doesn't match value

int match_mask(char *mask, int startbit, int val)
{
	int n;
	for(n=startbit;n>=0;n--)
	{
//		printf("%c %i\n",*mask,BE(val,n));
		if (( *mask=='1' && !BE(val,n)))
		    return 0;
		if ((*mask=='0' && BE(val,n)))
		    return 0;
		mask++;
	}
	return 1;
}
/*------------------------------------------------------------------------*/
// seperates mask into data
int seperate_mask(char* mask, int startbit, int val, int datas[8])
{
	int n;
	int i=-1;
	char last=0;

	for(n=0;n<8;n++)
		datas[n]=0;

	for(n=startbit;n>=0;n--)
	{
		if (*mask>='a')
		{
			if (*mask!=last)
			{
				i++;
			}
			datas[i]=(datas[i]<<1)+BE(val,n);
			last=*mask;
		}
		mask++;
	}
	return i;
}
/*------------------------------------------------------------------------*/
