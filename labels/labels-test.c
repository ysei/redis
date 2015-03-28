#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include"labels.h"

#define isnl(i) (((i)=='\n')||((i)=='\r')||((i)=='\f'))

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
		if(i&255==0)
		{
			(*buf)=(char *)realloc(*buf,((i>>8)+1)*256);
			fprintf(stderr,"read_line buffer reallocation to %d bytes\n",((i>>8)+1)*256);
		}
	}
}

// find label
int find_label(lbl_t *lbl, int lcnt, int adr, int seg)
{
int i;
	for(i=0;i<lcnt;i++)
	{
		if((lbl[i].start==adr)&&(lbl[i].seg==seg))
			return i;
	}

	return lcnt;
}
		
// main parser
int lbl_parse(FILE *fd, lbl_t **lbl)
{
lbl_t lab;
int i,j,k,x,line,len,found,lcnt;

char *buf,*ptr,*dummy, *pos;
	
	lcnt=0;
	(*lbl)=(lbl_t *)malloc(256*sizeof(lbl_t));

	line=0;
	do
	{
		read_line(fd,&buf);
		line++;

		if((strlen(buf)!=0)&&(!isnl(*buf)))
		{
		
			ptr=buf;
			pos=strchr(ptr,' ');		
			if(!pos)
				warning("short label entry",line);
			else
			{
				len=(int)(pos-ptr);
				dummy=strdup(ptr);
				dummy[len]=0;
				
				i=0;
				lab.type=-1;
				while((labdef[i].str!=0)&&(lab.type==-1))
				{
					if(strcmp(labdef[i].str,dummy)==0)
					{
						lab.type=labdef[i].type;
						lab.seg=0;
						if(labdef[i].flags&HAS_SEG)
							lab.seg=1;
	
						if(labdef[i].flags&HAS_NAME)
						{
							// comments come in ""
							if((lab.type==LBL_CMTA)||(lab.type==LBL_CMTL))
							{
								ptr+=len;
								despace(&ptr);

								ptr++;
								pos=strchr(ptr,'\"');
								if(pos==NULL)
								{
									warning("comment doesn't end",line);
									lab.type=-1;
								} 
								else
								{
									len=1+(int)(pos-ptr);
									lab.cmt=strdup(ptr);
									lab.cmt[len-1]=0;
								}
							} 

							// everything else is just <space> delimited
							else
							{
								len=advance(&ptr,len,line);
								lab.name=strdup(ptr);
								lab.name[len]=0;
							}	
						}
	
						if(labdef[i].flags&HAS_START)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								x=strtoul(ptr,0,0);
								lab.start=x;
							}
						}
						
						if(labdef[i].flags&HAS_END)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								x=strtoul(ptr,0,0);
								lab.end=x;
							}
						}
	
						if(labdef[i].flags&HAS_SIZE)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								x=strtoul(ptr,0,0);
								lab.size=x;
							}
						}
	
						if(labdef[i].flags&HAS_BASE)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								x=strtoul(ptr,0,0);
								lab.base=x;
							}
						}
	
						if(labdef[i].flags&HAS_PAT)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								j=0;
								lab.pat=NULL;
								while((*ptr)!=0)
								{
									lab.pat=realloc(lab.pat,j+1,strlen(pat_t));
									
									pptr=strchr(ptr,",");
									if(pptr==NULL)
										len=strlen(ptr);
									else
										len=(int)(pptr-ptr);
									buf_pat=strdup(ptr);
									buf_pat[len]=0;
									
									buf_cmd=strchr(buf_pat," ");
									if(buf_cmd==NULL)
									{
										lab.type=-1;
										fprintf(stderr,"Pattern contains command without definition.\n");
									}
									else
									{
										dummy=strdup(buf,cmd);
										dummy[(int)(buf_cmd-buf_pat)]=0; 
									}
									
									k=0;
									while((labdef[k].str!=0)&&(lab.type!=1))									
									{
										if(strcmp(dummy,labdef[k].str)==0)
										{
											switch labdef[k].type
											{
												case LBL_CMTA:
												case LBL_CMTL:
												case LBL_LABM:
												case LBL_LABI:
												case LBL_STR0:
												case LBL_STR7:
												case LBL_STRP:
													fprintf(stderr,"Pattern contains illegal label type.\n");
													lab.type=-1;
													break;
												default:
													if(labdef[k].flags&HAS_SIZE)
													{
														lab.pat[j].size=strtoul(												
									}									
																				
									if((*ptr)==',')
									{
										j++;
										ptr++;
									}
								}
								lab.pat=strdup(ptr);
							}
						}

						if(labdef[i].flags&HAS_TYPE)
						{
							if(len<0)
								lab.type=-1;
							else 
							{
								len=advance(&ptr,len,line);
								x=0;
								while(labdef[x].str!=0)
								{
									if(strcmp(labdef[x].str,ptr)==0)
									{
										lab.vtype=labdef[x].type;
										break;
									}
									else x++;
								}
								if(labdef[x].str==0)
								{
									warning("unkown vector type",line);
									found++;
								}
							}
						}
					}
				
					i++;
				}
			
				if(lab.type<0)
					warning("Unable to process entry",line);
				else
					fprintf(stderr,"Line %d identified as label type %d.\n",line,lab.type);				
			}

			if(lab.type>=0)
			{
				if(lab.type>0)
				{
					x=find_label(*lbl,lcnt,lab.start,lab.seg);
					if(x!=lcnt)
					{
						i=(*lbl)[x].type;
						if((i!=LBL_CMTA)&&(i!=LBL_CMTL))
							warning("Label redeclaration",line);
					} else
						lcnt++;

					memcpy(&((*lbl)[x]),&lab,sizeof(lbl_t));
					if((x!=lcnt)&&((lab.type==LBL_CMTA)||(lab.type==LBL_CMTL)))
						(*lbl)[x].type=i;

					if((lcnt&255)==0)
					{
						(*lbl)=(lbl_t *)realloc(((lcnt>>8)+1)*256*sizeof(lbl_t));
						fprintf(stderr,"Resizing label array to %d.\n",((lcnt>>8)+1)*256);
					}
				} else
					fprintf(stderr,"Skipping...\n");
			}
			fprintf(stderr,"\n");
		}
		free(buf);
		
	} while(!feof(fd));

	// insert patterns into label array if not already present
	i=0;
	do
	{
		if(lbl[i].type==LBL_PAT)
		{
			k=0;
			for(j=0;j<lbl[i].plen;j++)
			{
				if(
					(lbl[i].pat[j].type==LBL_VECMLE)||
					(lbl[i].pat[j].type==LBL_VECMBE)||
					(lbl[i].pat[j].type==LBL_VECILE)||
					(lbl[i].pat[j].type==LBL_VECIBE)
				)
				{
					if(find_label(lbl[i].pat[j].start,lbl[i].pat[j].seg)==lcnt)
					{
						sprintf(dummy,"lbl_%04x_%d_%d",lbl[i].pat[j].start,lbl[i].pat[j].seg,j);
						memset(lab,0,sizeof(lbl));

						lab.start=lbl[i].pat[j].start;
						lab.seg=lbl[i].pat[j].seg;
						if(lab.seg==0)
							lab.type=LBL_LABM;
						else
							lab.type=LBL_LABI;
						lab.name=strdup(dummy);
						
						memcpy(&((*lbl)[x]),&lab,sizeof(lbl_t));
						lcnt++;

						if((lcnt&255)==0)
						{
							(*lbl)=(lbl_t *)realloc(((lcnt>>8)+1)*256*sizeof(lbl_t));
							fprintf(stderr,"Resizing label array to %d.\n",((lcnt>>8)+1)*256);
						}

						fprintf(stderr,"Label %s inserted from pattern. Please check correctness of type.\n",lab.name);
					}
				}
			}
		}
	} while(i<lcnt);
		
	// insert vectors into label array if not already present
	i=0;
	do
	{
		if((lbl[i].type==LBL_VECMLE)||(lbl[i].type==LBL_VECMBE)||(lbl[i].type==LBL_VECMLE)||(lbl[i].type==LBL_VECMBE))
		{
				
			for(j=lbl[i].start;j<=lbl[i].end;j++)
			{
				x=0;
				for(k=0;k<lbl[i].size;k++)
				{
					if((lbl[i].type==LBL_VECMLE)||(lbl[i].type==LBL_MECILE))
					{
						x<<=8;
						x|=mem[k];
					} else
						x|=(mem[k]<<(8*k));
				}
				x|=lbl[i].base;
				k=0;
				if((lbl[i].type==LBL_VECILE)||(lbl[i]==LBL_VECMLE))
					k=1;
			
				if(find_label(x,k)==lcnt)
				{
					sprintf(dummy,"lbl_%04x_%d_%d",lbl[i].pat[j].start,lbl[i].pat[j].seg,j);
					memset(lab,0,sizeof(lbl));

					lab.start=lbl[i].pat[j].start;
					lab.seg=lbl[i].pat[j].seg;
					if(lab.seg==0)
						lab.type=LBL_LABM;
					else
						lab.type=LBL_LABI;
					lab.name=strdup(dummy);
					
					memcpy(&((*lbl)[x]),&lab,sizeof(lbl_t));
					lcnt++;

					if((lcnt&255)==0)
					{
						(*lbl)=(lbl_t *)realloc(((lcnt>>8)+1)*256*sizeof(lbl_t));
						fprintf(stderr,"Resizing label array to %d.\n",((lcnt>>8)+1)*256);
					}

					fprintf(stderr,"Label %s inserted from pattern. Please check correctness of type.\n",lab.name);
				}
			}
		}
	} while(i<lcnt);
	return lcnt;
}		

int main(int argc, char **argv)
{
FILE *fd;
lbl_t *lbl;
int x,i;	
	argv++;
	
	fd=fopen(*argv,"r");
	if(!fd)
		error("Can't open file.",0);
	
	x=lbl_parse(fd,&lbl);
	fprintf(stderr,"Got %d labels.\n",x);

	for(i=0;i<x;i++)
		printf("\t%04x: %s\n",lbl[i].start,lbl[i].name);
}
