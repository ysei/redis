/* 
 *      labels.cpp - label management for dis68k
 *      original concept (c) 2001 Georg Acher, georg@acher.org
 *
 *	reworked for more efficient label file format
 *	(c) 2001 Rainer Buchty, rainer@buchty.net
 *      
 *      #include <gpl-stuff.h>
 *
 *     $Id: labels.cpp,v 1.1.1.1 2002/04/29 23:18:16 acher Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "discore.h"

#define RESIZE_STEP 1024

int num_labels;
int max_labels=RESIZE_STEP;

label_t *labels;
/*------------------------------------------------------------------------*/
void init_labels(void)
{
	labels=(label_t*)malloc(RESIZE_STEP*sizeof(label_t));
	num_labels=0;
}
/*------------------------------------------------------------------------*/
void store_label(char *name, int start, int end, char *desc, char dtype, char dsize, char *seq)
{
int i, found;

#ifdef DEBUG
	printf("Label <%s> at %x/%x (%c:%d:%p)\n",name,start,end,dtype,dsize,seq);
#endif
	found=0;

	for(i=0;i<num_labels;i++) {
		if (labels[i].pc==start) {
			if (dtype=='c') 
				labels[i].desc=strdup(desc);
			else {
				labels[i].name=strdup(name);
				labels[i].end=end;
				labels[i].dtype=dtype;
				labels[i].dsize=dsize;
				if (seq!=NULL)
					labels[i].seq=strdup(seq);
			}
			found=1;
			break;
		}
	}

#ifdef DEBUG
	printf("\t Label found: %d\n",found);
#endif 

	if (!found) {
		labels[num_labels].pc=start;
		labels[num_labels].name=strdup(name);
		if (dtype=='c')
			labels[num_labels].desc=strdup(desc);
		else
			labels[num_labels].desc=0;
		labels[num_labels].end=end;
		labels[num_labels].dtype=dtype;
		labels[num_labels].dsize=dsize;
		if (seq!=NULL)
			labels[num_labels].seq=strdup(seq);
		num_labels++;

#ifdef DEBUG
		printf("\t Label stored.\n");
#endif
	}

	
	if (num_labels>=max_labels)
	{
		max_labels+=RESIZE_STEP;
		labels=(label_t*)realloc(labels,max_labels*sizeof(label_t));

#ifdef DEBUG
		printf("Resized label storage.\n");
#endif

	}
}
/*------------------------------------------------------------------------*/
int label_sorter(const void *aa, const void *bb)
{
        label_t *a=(label_t*)aa;
        label_t *b=(label_t*)bb;
//	printf("SSSS: %x %x\n",a->pc,b->pc);
	if (a->pc > b->pc)
		return 1;
	if (a->pc == b->pc)
		return 0;
	return -1;
}
/*------------------------------------------------------------------------*/
label_t *find_label(int p)
{
	void *l;
	label_t x;
	x.pc=p;

	l=bsearch(&x,labels,num_labels,sizeof(label_t),label_sorter);
//	printf("S: %x, f: %p\n",p,l);
	return (label_t*)l;
}        
/*------------------------------------------------------------------------*/
void sort_labels(void)
{
	qsort(labels,num_labels,sizeof(label_t),label_sorter);
}
/*------------------------------------------------------------------------*/

/*
// format:
// labelname equ $value
void read_labels(char *fname)
{
	char modes[]={
		's','t','b','w','l','p'
	};
	char label[256]; //,value[256];
	char line[512];
	FILE * file;
	char *p,*p1,*seq;
	int n,i;
	int mode;

	int dstart, dend, dtype, dsize, dcnt, stype, ssize, slen;

	file=fopen(fname,"r");
	if (!file)
	{
		perror(fname);
		exit(-1);
	}

	mode=0;

	seq=NULL;
	while(!feof(file))
	{

		line[0]=0;
		fgets(line,511,file);
		if (strlen(line)<5)
			continue;
		if (line[0]==';')
			continue;

		n=0;
		sscanf(line,"%s%n",label,&n);
		p=strstr(line+n,"equ ");
		if (p) {
			p1=strchr(p+4,'$');
			if (p1)
				p=p1+1;
			else
				p+=4;
			sscanf(p,"%x",&n);
			store_label(label,n,mode,0,-1,0,0);
		}

		else {
			p=strstr(line+n,"seg ");
			if (p) {
				p1=strdup(p+4);
				dcnt=0;
				seq=0;
				while ( (p=strtok(p1,","))!=NULL ) {
					if(p[0]=='$')
						p++;
					p1=NULL;
					switch(dcnt) {
					case 0:	dstart=strtoul(p,0,16);
						break;
					case 1: dend=strtoul(p,0,16);
						break;
					case 2:	for(i=0;i<6;i++) {
							if (p[0]==modes[i])
								dtype=i;
						}
						if (strlen(p)<2)
							dsize=0;
						else
							dsize=strtoul(p+1,0,10);
						break;
					default:
						for(i=0;i<6;i++) {
							if (p[0]==modes[i])
								stype=i;
						}
						if (strlen(p)<2)
							ssize=0;
						else
							ssize=strtoul(p+1,0,10);
						seq=(char*)realloc(seq,(dcnt-2)*2+1);
						sprintf(seq,"%s%c%c",seq,stype+1,ssize+1);
//						printf("%d: %d\n",dcnt-2,strlen(seq));
						break;
					}
					dcnt++;
					slen=dcnt<<1;
				}
//				printf("from %x to %x: %d %d %d\n",dstart,dend,dtype,dsize,slen);
				store_label(label,dstart,dend,dtype,dsize,seq);

			} else {
				continue;
			}
		}

//		n=strtoul(value,NULL,16);
	}
	fclose(file);
	
//	printf("Number of labels %i\n",num_labels);

}
*/

//
// return value from numeric string
//
int calc_adr(char *ptr, int len) {
char dummy[256];
int ret;
//	dummy=(char *)malloc(len);
//	memset(dummy,0,len);

	if (ptr[0]=='$') {
		strncpy(dummy,ptr+1,len);
		ret=strtoul(dummy,0,16);
	} else {
		strncpy(dummy,ptr,len);
		ret=strtoul(dummy,0,10);
	}
//	free(dummy);
	return ret;
}

//
// get next entry of label file
//
char *get_next(char *ptr) {
/*
int x1,x2;
	x1=(int)strchr(ptr,' ');
	x2=(int)strchr(ptr,'\t');
	
	if ((x2<x1)&&(x2!=0))
		return (char *)(x2+1);
	else if (x1!=0)
		return (char *)(x1+1);
	else
		return (char *)(ptr+strlen(ptr)+1);
*/

		char *xptr=ptr;

		while( !isspace(*xptr) && (*xptr!=0) ) xptr++;
		while(  isspace(*xptr)) xptr++;
		
		return xptr;

}

//
void read_labels(char *fname) {
FILE *file;
char *ptr=NULL, *xptr=NULL, *infile=NULL, *inptr=NULL, *end=NULL;

char *adr_fmt=NULL, fmt_cmd, fmt_size, *adr_name=NULL;
int cmd, adr_start, adr_end, adr_step;

int size, len, leave, argc;

	if ((file=fopen(fname,"r"))==NULL) {
		printf("Can't open \"%s\" for reading.\n",fname);
		return;
	}
	
	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);
	
//	printf("%s has %d bytes... ",fname,size);
	
	infile=(char *)malloc(size);
	fread(infile,1,size,file);
	fclose(file);

//	printf("Read.\n");	

	ptr=infile;
	while ( (inptr=(char *)strtok(ptr,"\n"))!=NULL ) 
	{
		ptr=NULL;

		cmd=inptr[0];
#ifdef DEBUG
		printf("Processing command %c...\n",cmd);
#endif
		argc=0;	leave=0;
		if ((cmd=='l')||(cmd=='c')||(cmd=='b')||(cmd=='w')||(cmd=='l')||(cmd=='t')||(cmd=='s')||(cmd=='d')) 
		{
			do 
			{
				inptr=(char *)get_next(inptr);
				end=(char *)get_next(inptr);			
				len=end-inptr+1;
				if (len==-1)
					len=strlen(inptr);
#ifdef DEBUG
				printf("\t");
#endif
				switch(argc) 
				{

				// step 0: get start address
				case 0:
					adr_start=calc_adr(inptr,len);
					adr_end=adr_start;

					// these don't have end addresses, therefore skip step 1
					if ((cmd=='l')||(cmd=='c'))
						argc++;
#ifdef DEBUG
					printf("Got start address: %x\n",adr_start);
#endif
					break;

				// step 1: get end address
				case 1:
					adr_end=calc_adr(inptr,len);
#ifdef DEBUG
					printf("Got end address: %x\n",adr_end);
#endif
					break;

				// step 2: get label name
				case 2:
					if (cmd=='c') 
						len=strlen(inptr);

					if ((cmd=='b')||(cmd=='t')||(cmd=='w')||(cmd=='d'))
						len-=1;

					adr_name=(char *)malloc(len+1);
					adr_step=0;

					memset(adr_name,0,len+1);
					strncpy(adr_name,inptr,len);
#ifdef DEBUG
					printf("Got label name: [%s]\n",adr_name);
#endif
					break;

				// step 3: repeat information
				case 3:
					if ((cmd!='l')&&(cmd!='c'))
						adr_step=calc_adr(inptr,len);
#ifdef DEBUG
					printf("Got repeat info: %d\n",adr_step);
#endif
					break;

				// step 4+: parse pattern
				default:
					if (cmd=='p') {
#ifdef DEBUG
						printf("Got data sequence: ");
#endif
						adr_fmt=(char *)malloc(3);
						xptr=inptr;
						while( (inptr=(char *)strtok(xptr,","))!=NULL ) {
							xptr=NULL;
						
							fmt_cmd=inptr[0];
							fmt_size=calc_adr(inptr+1,strlen(inptr+1));
							if(fmt_size==0)
								fmt_size++;
								
#ifdef DEBUG
							printf("%c%d ",fmt_cmd,fmt_size);
#endif
							adr_fmt=(char *)realloc(adr_fmt,1+(1<<(argc-3)));
							sprintf(adr_fmt,"%s%c%c",adr_fmt,fmt_cmd,fmt_size);
						}
					} 
					leave=1;
					break;
				}
				argc++;
			} while(leave==0);

#ifdef DEBUG
			printf("\n");
#endif
			store_label(adr_name,adr_start,adr_end,adr_name,cmd,adr_step,adr_fmt);
		}
	}
}
