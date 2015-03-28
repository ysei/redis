#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"flattener.h"
#include"tools.h"

int analyze(char *in)
{
int uc, dc;
char *buf=in;
char last=0;

	uc=dc=0;
	
	while((*buf)!=0)
	{
		if((*buf)=='(')
		{
			if(last=='(')
				dc++;
			last='(';
			uc++;
		}
		else if ((*buf)==')')
		{
			last=')';
			uc--;
		}	
		buf++;
	}
	
	dc++;
	
	if(uc!=0)
	{
		fprintf(stderr,"\tBracket Mismatch (%d).\n",-uc);
		exit(-1);
	} else
		fprintf(stderr,"\t%d hierarchy levels found.\n",dc);
		
	return dc;
}

void flatten(lvl_t *lvl, char *in, int xc)
{
char *xxx, *buf;
int len, cnt, i;
	fprintf(stderr,"%d: %s\n",xc,in);
	
	buf=in;
		
	while(1)
	{
		// new hierarchy level
		if((*buf)=='(')
		{
			buf++;
			
			// init flattener
			lvl[xc+1].start=buf;
			(*lvl[xc+1].str)=0;
			
			flatten(lvl,buf,xc+1);
			buf=lvl[xc].start;
						
			// merge level strings
			xxx=strdup(lvl[xc].str);
			len=strlen(lvl[xc+1].str)*lvl[xc+1].cnt+strlen(xxx)+lvl[xc+1].cnt+1;			
			lvl[xc].str=(char *)realloc(lvl[xc].str,len);
			memset(lvl[xc].str,0,len);

			sprintf(lvl[xc].str,"%s",xxx);
			for(i=0;i<lvl[xc+1].cnt;i++)
			{
				if(i!=0)
					sprintf(lvl[xc].str,"%s,%s",lvl[xc].str,lvl[xc+1].str);
				else
					sprintf(lvl[xc].str,"%s%s",lvl[xc].str,lvl[xc+1].str);
			}
		}		

		// close hierarchy level
		else if ((*buf)==')')
		{
			buf++;
			
			cnt=strtoul(buf,0,0);
			if(cnt==0)
				cnt=1;
			lvl[xc].cnt=cnt;

			// get overall hierarchy length
			while(isxdigit(*buf))
				buf++;

			lvl[xc-1].start=buf;
			
			return;
		}

		// end of string
		else if ((*buf)==0)
		{
			if(xc!=0)
			{
				fprintf(stderr,"\tThis shouldn't happen...\n");
				exit(-1);
			}

			return;
		}			

		// normal content
		else
		{
			len=strlen(lvl[xc].str)+2;
			lvl[xc].str=(char *)realloc(lvl[xc].str,len);
			sprintf(lvl[xc].str,"%s%c",lvl[xc].str,*buf);

			buf++;
		}
	}

}

int main(int argc, char **argv)
{
lvl_t *lvl;
char *buf, *ptr;
char *iname, *oname;
FILE *in, *out;
int pcnt, line, cnt, i, pos;
	if(argc!=2)
	{
		fprintf(stderr,"Use: flattener <proj>\n");
		exit(-1);
	}
	
	argv++;

	iname=(char *)malloc(strlen(*argv)+5);
	sprintf(iname,"%s.lbl",*argv);
		
	in=fopen(iname,"r");
	if(in==NULL)
	{
		fprintf(stderr,"Can't open %s for reading.\n",iname);
		exit(-1);
	}	

	oname=(char *)malloc(strlen(*argv)+5);
	sprintf(oname,"%s.lfl",*argv);

	out=fopen(oname,"w");
	if(out==NULL)
	{
		fprintf(stderr,"Can't open %s for writing.\n",oname);
		exit(-1);
	}

	line=1;
	cnt=0;
	
	do
	{
		fprintf(stderr,"Processing line %d...\n",line);
		read_line(in,&buf);
		fprintf(stderr,"\t%s\n",buf);
		
		cnt++;
		
		ptr=buf;
		despace(&ptr);

			
		if(strlen(ptr)!=0)
		{
			pos=(int)(strchr(ptr,' ')-ptr)-1;
			
			if((*ptr)!='p')
				fprintf(out,"%s\n",ptr);
			else
			{
				fprintf(stderr,"\n");
					
				// command
				fprintf(out,"p ");
				ptr++;
				despace(&ptr);

				// name
				pos=(int)(strchr(ptr,' ')-ptr)+1;
				ptr[pos-1]=0;
				fprintf(out,"%s ",ptr);
				ptr+=pos;
				
				// start address
				fprintf(out,"%d ",getnum(&ptr));
					
				// end address
				fprintf(out,"%d ",getnum(&ptr));
						
				// pattern
				if((*ptr)==0)
				{
					fprintf(stderr,"Missing pattern.\n");
					exit(-1);
				}
				pcnt=analyze(ptr)+1;
	
				lvl=(lvl_t *)malloc(cnt*sizeof(lvl_t));
				for(i=0;i<pcnt;i++)
				{
					lvl[i].str=malloc(1);
					(*lvl[i].str)=0;
				}
				flatten(lvl,ptr,0);
				fprintf(out,"%s ",lvl[0].str);
			}
		} else
			fprintf(out,"\n");
					
		line++;

	} while(!feof(in));

	fclose(in); 
	fclose(out);
	
	exit(0);
}