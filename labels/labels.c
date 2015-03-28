#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include"labels.h"
#include"tools.h"


// find label type
int find_ltype(char *str)
{
int i;
	i=0;
	while(labdef[i].str!=0)
	{
		if(strcmp(labdef[i].str,str)==0)
			return i;
		i++;
	}
	
	return -1;
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
int lbl_parse(FILE *fd, lbl_t **lbl, unsigned char *mem)
{
lbl_t lab;
labdef_t ld;

int i,j,k,l,x,line,len,found,lcnt;

char *buf,*ptr, *dummy, *pos;
char *buf_pat, *buf_cmd, *pptr;
	
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
				
				i=find_ltype(dummy);
				ld=labdef[i];
				
				if(i!=-1)
				{
					if(strcmp(ld.str,dummy)==0)
					{
						memset(&lab,0,sizeof(lbl_t));
						
						lab.type=ld.type;
						lab.flags=ld.flags;
						lab.seg=0;

						if(ld.flags&HAS_SEG)
							lab.seg=1;
	
						if(ld.flags&HAS_NAME)
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
								lab.name[len-1]=0;
							}	
						}
	
						if(ld.flags&HAS_START)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								lab.start=strtoul(ptr,0,0);
							}
						}
						
						if(ld.flags&HAS_END)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								lab.end=strtoul(ptr,0,0);
							}
						}
	
						if(ld.flags&HAS_SIZE)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								lab.size=strtoul(ptr,0,0);
							}
						}
	
						if(ld.flags&HAS_BASE)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								len=advance(&ptr,len,line);
								lab.base=strtoul(ptr,0,0);
							}
						}
	
						if(ld.flags&HAS_TYPE)
						{
							if(len<0)
								lab.type=-1;
							else 
							{
								len=advance(&ptr,len,line);
								x=find_ltype(ptr);
								if(x<0)
								{
									warning("unkown vector type",line);
									found++;
								}
								else	
								{
									lab.vtype=labdef[x].type;
									lab.vflags=labdef[x].flags;
								}
							}
						}

						j=0;
						lab.pat=NULL;
						if(ld.flags&HAS_PAT)
						{
							if(len<0)
								lab.type=-1;
							else
							{
								l=lab.start;
								
								len=advance(&ptr,len,line);
//								fprintf(stderr,"dbg: \"%s\"\n",ptr);
								while(((*ptr)!=0)&&(lab.type>=0))
								{
									j++;
									lab.pat=(pat_t *)realloc(lab.pat,j*sizeof(pat_t));
									
									// isolate single pattern entry
									pptr=strchr(ptr,',');
									if(pptr==NULL)
										len=strlen(ptr);
									else
									{
										len=(int)(pptr-ptr);
									}
									
									buf_pat=strdup(ptr);
									buf_pat[len]=0;

//									if(pptr!=NULL)
										ptr+=len+1;

									// get label type of entry
									buf_cmd=strchr(buf_pat,' ');
									if(buf_cmd==NULL)
									{
										lab.type=-1;
										fprintf(stderr,"Pattern contains command without definition.\n");
									}
									else
									{
										dummy=strdup(buf_pat);
										dummy[(int)(buf_cmd-buf_pat)]=0; 
									}

									len=strlen(dummy);									
									fprintf(stderr,"Analyzing %d:\"%s\"...\n",j,dummy);

									k=find_ltype(dummy);
									if(k!=-1)
									{
										ld=labdef[k];
										switch(ld.type)
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
											case LBL_PAT:	
												fprintf(stderr,"Pattern nesting, label file is not flattened.\n");
												lab.type=-1;
												break;
											default:
												len=advance(&buf_pat,len,line);
												lab.pat[j-1].type=ld.type;
												
												if(ld.flags&HAS_SIZE)
												{
													if(strlen(buf_pat)==0)
														lab.type=-1;
													else
													{
														lab.pat[j-1].size=strtoul(buf_pat,0,0);
														fprintf(stderr,"\tsize: %ld\n",strtoul(buf_pat,0,0));
													}
												}
	
												if(ld.flags&HAS_BASE)
												{
													len=advance(&buf_pat,len,line);
													if(strlen(buf_pat)==0)
														lab.type=-1;
													else
													{
														lab.pat[j-1].base=strtoul(buf_pat,0,0);
														fprintf(stderr,"\tbase: %ld\n",strtoul(buf_pat,0,0));
													}
												}
	
												if(ld.flags&HAS_TYPE)
												{
													len=advance(&buf_pat,len,line);
													if(strlen(buf_pat)==0)
														lab.type=-1;
													else
													{
														for(i=strlen(buf_pat);i>0;i--)
														{
															if(isspace(buf_pat[i]))
																buf_pat[i]=0;
														}
														x=find_ltype(buf_pat);
														lab.pat[j-1].vtype=labdef[x].type;
														lab.pat[j-1].vflags=labdef[x].flags;
														fprintf(stderr,"\ttype: %d\n",labdef[find_ltype(buf_pat)].type);
														if(lab.pat[j-1].vtype<0)
															lab.type=-1;
													}
												}
												lab.pat[j-1].start=l;
												l+=lab.pat[j-1].size;
												
												fprintf(stderr,"\t%x: %s\n",lab.pat[j-1].start,(lab.type<0)?"error":"ok");
										}												
									}											
								}
							}
						}
						lab.plen=j;
					}
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
					fprintf(stderr,"[%s]\n",(*lbl)[x].name);
					
					if(lab.type==LBL_PAT)
					{
						fprintf(stderr,"plen[%d]\n",(*lbl)[x].plen);
						(*lbl)[x].pat=(pat_t *)malloc(lab.plen*sizeof(pat_t));

						memcpy(&((*lbl)[x].pat),&(lab.pat),sizeof(pat_t));

						for(j=0;j<(*lbl)[x].plen;j++)
							fprintf(stderr,"%d -> %d\n",(*lbl)[x].pat[j].type,(*lbl)[x].pat[j].type);

					}		
					if((x!=lcnt)&&((lab.type==LBL_CMTA)||(lab.type==LBL_CMTL)))
						(*lbl)[x].type=i;

					if((lcnt&255)==0)
					{
						(*lbl)=(lbl_t *)realloc(*lbl,((lcnt>>8)+1)*256*sizeof(lbl_t));
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
		if((*lbl)[i].type==LBL_PAT)
		{
			fprintf(stderr,"label %d contains pattern\n",i);
			k=0;
			for(j=0;j<(*lbl)[i].plen;j++)
			{
				if(
					((*lbl)[i].pat[j].type==LBL_VECMLE)||
					((*lbl)[i].pat[j].type==LBL_VECMBE)||
					((*lbl)[i].pat[j].type==LBL_VECILE)||
					((*lbl)[i].pat[j].type==LBL_VECIBE)
				)
				{
					if(find_label(*lbl,lcnt,(*lbl)[i].pat[j].start,(*lbl)[i].pat[j].seg)==lcnt)
					{
						sprintf(dummy,"plbl_%04x",(*lbl)[i].pat[j].start);
						memset(&lab,0,sizeof(lbl_t));

						lab.start=(*lbl)[i].pat[j].start;
						lab.seg=(*lbl)[i].pat[j].seg;
						lab.type=(*lbl)[i].pat[j].vtype;
						lab.flags=(*lbl)[i].pat[j].vflags;
						
/*						if(lab.seg==0)
							lab.type=LBL_LABM;
						else
							lab.type=LBL_LABI;
*/
						lab.name=strdup(dummy);
						
						memcpy(&((*lbl)[lcnt]),&lab,sizeof(lbl_t));
						lcnt++;

						if((lcnt&255)==0)
						{
							(*lbl)=(lbl_t *)realloc(*lbl,((lcnt>>8)+1)*256*sizeof(lbl_t));
							fprintf(stderr,"Resizing label array to %d.\n",((lcnt>>8)+1)*256);
						}

						fprintf(stderr,"Label %s (%d) inserted from pattern. Please check correctness of type.\n",lab.name,lab.type);
					} 
					else
						fprintf(stderr,"Label %d:%d for address %04x already present.\n",i,j,lcnt);
				}
				else
					fprintf(stderr,"\tPattern %d:%d is type %d, not vector.\n",i,j,(*lbl)[i].pat[j].type);
			}
		}

		i++;
	} while(i<lcnt);
		
	// insert vectors into label array if not already present
	i=0;
	do
	{
		if(((*lbl)[i].type==LBL_VECMLE)||((*lbl)[i].type==LBL_VECMBE)||((*lbl)[i].type==LBL_VECMLE)||((*lbl)[i].type==LBL_VECMBE))
		{
			j=(*lbl)[i].start;
			do
			{
				x=0;
				for(k=0;k<(*lbl)[i].size;k++,j++)
				{
					if(((*lbl)[i].type==LBL_VECMLE)||((*lbl)[i].type==LBL_VECILE))
					{
						x<<=8;
						x|=mem[j];
					} else
						x|=(mem[j]<<(k<<3));
				}
				x|=(*lbl)[i].base;

				k=0;
				if(((*lbl)[i].type==LBL_VECILE)||((*lbl)[i].type==LBL_VECMLE))
					k=1;
			
				if(find_label(*lbl,lcnt,x,k)==lcnt)
				{
					sprintf(dummy,"vlbl_%04x",j);
					memset(&lab,0,sizeof(lbl_t));

					lab.start=x;
					lab.seg=k;
					lab.type=(*lbl)[i].vtype;
					lab.flags=(*lbl)[i].vflags;
/*					if(lab.seg==0)
						lab.type=LBL_LABM;
					else
						lab.type=LBL_LABI;
*/					lab.name=strdup(dummy);
					
					memcpy(&((*lbl)[lcnt]),&lab,sizeof(lbl_t));
					lcnt++;

					if((lcnt&255)==0)
					{
						fprintf(stderr,"Resizing label array to %d.\n",((lcnt>>8)+1)*256);
						(*lbl)=(lbl_t *)realloc(*lbl,((lcnt>>8)+1)*256*sizeof(lbl_t));
					}

					fprintf(stderr,"Label %s inserted from vectors. Please check correctness of type.\n",lab.name);
				}
			} while(j<(*lbl)[i].end);
		}
		
		i++;
	} while(i<lcnt);

	return lcnt;
}		

int main(int argc, char **argv)
{
FILE *fd;
char *lnam, *mnam, *onam;
lbl_t *lbl;
int x,i,j;
unsigned char *mem;

	argv++;
	
	lnam=(char *)malloc(strlen(*argv)+4);
	mnam=(char *)malloc(strlen(*argv)+4);
	onam=(char *)malloc(strlen(*argv)+4);
	
	sprintf(lnam,"%s.lfl",*argv);
	sprintf(mnam,"%s.bin",*argv);
	sprintf(onam,"%s.lpp",*argv);
	
	fd=fopen(mnam,"r");
	if(!fd)
		error("Can't open binary file.",0);
	mem=(char *)malloc(65536);
	i=0;
	while(!feof(fd))
	{
		mem[i]=fgetc(fd);
		i++;
		if(i%65536==0)
			mem=(char *)realloc(mem,((i>>16)+1)*65536);
	}	
	fclose(fd);
		
	fd=fopen(lnam,"r");
	if(!fd)
		error("Can't open flattened label file.",0);
	
	x=lbl_parse(fd,&lbl,mem);
	fclose(fd);

	fprintf(stderr,"Got %d labels.\n",x);

	fd=fopen(onam,"w+");	
	fprintf(fd,"00000000\n");
	
	for(i=0;i<x;i++)
	{
		fprintf(fd,"%02x",strlen(lbl[i].name));
		fprintf(fd,"-%s",lbl[i].name);
		fprintf(fd,"-%02x",lbl[i].type);
		fprintf(fd,"-%02x",lbl[i].flags);
		fprintf(fd,"-%08x",lbl[i].start);
		fprintf(fd,"-%08x",lbl[i].end);
		fprintf(fd,"-%08x",lbl[i].base);
		fprintf(fd,"-%02x",lbl[i].size);
		fprintf(fd,"-%04x",lbl[i].plen);		
		if(lbl[i].flags&HAS_PAT)
			for(j=0;j<lbl[i].plen;j++)
			{
				fprintf(stderr,"!%d!\n",j);
				fprintf(fd,"-%02x",lbl[i].pat[j].type);
				fprintf(fd,"-%02x",lbl[i].pat[j].flags);
				fprintf(fd,"-%02x",lbl[i].pat[j].size);
				fprintf(fd,"-%08x",lbl[i].pat[j].base);
			}	
		fprintf(fd,"\n");
	}
	
	fseek(fd,8,SEEK_SET);
	i=0;
	do
	{
		i+=fgetc(fd);
		i&=0xffffffff;
	} while(!feof(fd));
	
	rewind(fd);
	fprintf(fd,"%08x",i&0xffffffff);

	fclose(fd);
	
	exit(0);
}
