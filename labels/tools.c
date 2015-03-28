#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include"tools.h"

int errout(char *str, int line, int mode)
{
	fprintf(stderr,"%s: %s",(mode==0)?"Warning":"Error",str);
	if(line>0)
		fprintf(stderr," in line %d",line);
	fprintf(stderr,"\n");

	if(mode==1)
		exit(-1);

	return 0;
}

// output warning message
void warning(char *str, int line)
{
	errout(str,line,0);
}

// output error message
void error(char *str, int line)
{
	errout(str,line,1);
}

// remove leading white-spaces
void despace(char **buf)
{
char *ptr;
	ptr=(*buf);
	while(isspace(*ptr)&&(!isnl(*ptr))&&((*ptr)!=0))
		ptr++;
		
	(*buf)=ptr;
}

// get number
int getnum(char **buf)
{
int x;
	despace(buf);
	x=strtoul(*buf,0,0);

	while((**buf)!=' ')
		(*buf)++;

	despace(buf);

	return x;
}

// advance pointer to next item in list
int advance(char **ptr, int len, int line)	
{
int x;
	(*ptr)+=len;
	despace(ptr);

	x=(int)1+(strchr(*ptr,' ')-(*ptr));
	if(strlen(*ptr)==0)
	{
		warning("short label definition",line);
		return -1;
	}
	return x;
}

// read line from text file
void read_line(FILE *fd, char **buf)
{
int i,x;

	(*buf)=(char *)malloc(256);
	memset(*buf,0,256);
	
	i=0;
	while(1)
	{
		x=fgetc(fd);
		if((isnl(x))||(feof(fd)))
			break;
		(*buf)[i]=x;

		i++;
		if((i&255)==0)
		{
			(*buf)=(char *)realloc(*buf,((i>>8)+1)*256);
			fprintf(stderr,"read_line buffer reallocation to %d bytes\n",((i>>8)+1)*256);
		}
	}
}

char *to_bin(int in, int size)
{
int i;
unsigned long num;
char *buf=NULL;

	buf=(char *)realloc(buf,size);
	
	num=(unsigned long)(in);
	for(i=0;i<size;i++)
	{
		buf[i]=num&0xff;
		num>>=8;
	}
	
	return buf;
}	