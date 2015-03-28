//***************************************************************************
// kmp3sel/configfile.cpp
// (c) Georg Acher, acher@in.tum.de
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id: configfile.cpp,v 1.1.1.1 2002/04/29 23:18:15 acher Exp $
//***************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "configfile.h"

#define log(a, args... )
/*---------------------------------------------------------------------*/
void config_free_class(config_class_t *cc)
{
	free(cc);
}
/*---------------------------------------------------------------------*/
config_class_t *config_new_class(char* class_key, int instance)
{
	config_class_t *cc;
	cc=(config_class_t *)malloc(sizeof(config_class_t));
	memset(cc,0,sizeof(config_class_t));
	strncpy(cc->class_key,class_key,32);
	cc->instance=instance;
	cc->config=NULL;
	cc->next=NULL;
	return cc;
}
/*---------------------------------------------------------------------*/
void config_free_element(config_elem_t *ce)
{
	if (ce->type=='s')
		free(ce->value);
	free(ce);
}
/*---------------------------------------------------------------------*/
config_elem_t *config_new_element(char* key, char type, void *value)
{
	config_elem_t *ce;
	ce=(config_elem_t *)malloc(sizeof(config_class_t));
	memset(ce,0,sizeof(config_elem_t));
	strncpy(ce->key,key,32);
	ce->next=NULL;
	ce->type=type;
	if (type=='i')
	{
		ce->value=value;
	}
	if (type=='s')
	{
		ce->value=malloc(strlen((char*)value)+1);
		strcpy((char*)ce->value,(char*)value);
	}
	return ce;
}
/*---------------------------------------------------------------------*/
config_template_t *config_find_template(char *class_key, char* key, 
					config_template_t *ct)
{
	while(ct->key[0])
	{
		if (!strcmp(ct->class_key,class_key) && 
		    !strcmp(ct->key,key))
			return ct;
		ct++;
	}
	return 0;
}
/*---------------------------------------------------------------------*/
config_class_t *config_find_class(config_class_t* cc,char *class_key, 
				  int instance)
{
	while(cc)
	{
		if (!strcmp(cc->class_key,class_key) && (cc->instance==instance))
			return cc;
		cc=cc->next;
	}
	return NULL;
}
/*---------------------------------------------------------------------*/
config_elem_t *config_find_key(config_class_t* cc,char *key)
{
	config_elem_t *ce;
	
	ce=cc->config;
	while(ce)
	{
		if (!strcmp(ce->key,key))
			return ce;
		ce=ce->next;
	}
	return NULL;
}
/*---------------------------------------------------------------------*/
config_elem_t *config_find_class_key(config_class_t* cc, char* class_key,
				     int instance, char* key)
{
	config_class_t* xcc;
	
	xcc=config_find_class(cc,class_key,instance);
	if (!xcc)
		return NULL;
	return config_find_key(xcc,key);
}
/*---------------------------------------------------------------------*/
int config_find_class_key_int(config_class_t* cc, char* class_key,
				     int instance, char* key)
{
	config_elem_t *ce;

	ce=config_find_class_key(cc,class_key,instance,key);
	if (ce)
		return (int)ce->value;
	else
		return 0;
}
/*---------------------------------------------------------------------*/
char* config_find_class_key_string(config_class_t* cc, char* class_key,
				     int instance, char* key)
{
	config_elem_t *ce;

	ce=config_find_class_key(cc,class_key,instance,key);
	if (ce)
		return (char*)ce->value;
	else
		return NULL;
}
/*---------------------------------------------------------------------*/
int config_find_key_int(config_class_t* cc,  char* key)
{
	config_elem_t *ce;
	ce= config_find_key(cc,key);
	if (ce)
		return (int)ce->value;
	else
		return 0;
}
/*---------------------------------------------------------------------*/
char* config_find_key_string(config_class_t* cc,  char* key)
{
	config_elem_t *ce;
	ce= config_find_key(cc,key);
	if (ce)
		return (char*)ce->value;
	else
		return NULL;
}
/*---------------------------------------------------------------------*/
void config_add_class(config_class_t *root, config_class_t *cc)
{
	while(root->next)
		root=root->next;

	root->next=cc;
}
/*---------------------------------------------------------------------*/
void config_add_element(config_class_t *root, config_elem_t *elem)
{
	config_elem_t *ce;

	if (root->config)
	{
		ce=root->config;
		while(ce->next)
			ce=ce->next;
		ce->next=elem;
	}
	else
		root->config=elem;
}
/*---------------------------------------------------------------------*/
int config_clean_instance(config_class_t *cc,char* class_key)
{
	int instance=0;
	while(cc)
	{
		if (!strcmp(cc->class_key,class_key))
			cc->instance=instance++;
		cc=cc->next;
	}
	return instance;
}
/*---------------------------------------------------------------------*/

void config_clone_add_class_X(config_class_t **cc, config_class_t *oo, int instance)
{
	config_class_t *ncc,*xcc;
	config_elem_t *ce, *nce;

	ncc=(config_class_t*)malloc(sizeof(config_class_t));
	memcpy(ncc,oo,sizeof(config_class_t));
	ncc->next=NULL;
	ncc->config=NULL;

	if (*cc)
	{
		xcc=*cc;
		while(xcc->next)
			xcc=xcc->next;

		xcc->next=ncc;
	}
	else
		*cc=ncc;
	
	ce=oo->config;
	while(ce)
	{
		nce=config_new_element(ce->key, ce->type, ce->value);
		//TRACE("CLONE %p, %p\n",nce,ce->ch);
		nce->ch=ce->ch;
		nce->next=0;
		config_add_element(ncc, nce);
		ce=ce->next;
	}
	ncc->instance=instance;
}
/*---------------------------------------------------------------------*/
void config_clone_add_class(config_class_t **cc, config_class_t *oo)
{
	config_clone_add_class_X(cc, oo,0);
	config_clean_instance(*cc, oo->class_key);
}
/*---------------------------------------------------------------------*/
void config_set_class_key(config_class_t **cc, char* class_key, int instance,
		    char *key, char type, void *value)
{
	config_class_t *xcc;
	config_elem_t *xce;
	config_template_t *ct,*cts;
	
	if (*cc)
	{
		xcc=config_find_class(*cc, class_key, instance);
		if (!xcc)
		{
			xcc=config_new_class(class_key,instance);
			config_add_class(*cc,xcc);
		}
	}
	else
	{
		xcc=config_new_class(class_key,instance);
		*cc=xcc;
	}
	
	cts=xcc->templates;
	
	xce=config_find_key(xcc,key);
	if (!xce)
	{
		xce=config_new_element(key,type,value);
		config_add_element(xcc,xce);
	}
	else
	{
		if (xce->type=='i')
		{
			xce->value=value;
		}
		if (xce->type=='s')
		{
			if (xce->value)
				free(xce->value);
			xce->value=malloc(strlen((char*)value)+1);
			strcpy((char*)xce->value,(char*)value);
		}	
	}
	
	if (cts)
	{
		ct=config_find_template(class_key,key,cts);
		if (ct)
		{
			xce->ch=ct->ch;
		}
	}
}
/*---------------------------------------------------------------------*/
void config_set_class_key_int(config_class_t **cc, char* class_key, int instance,
		    char *key, int value)
{
	config_set_class_key(cc, class_key,instance,key, 'i', (void*)value);
}
/*---------------------------------------------------------------------*/
void config_set_class_key_string(config_class_t **cc, char* class_key, int instance,
		    char *key, char *value)
{
	config_set_class_key(cc, class_key, instance, key, 's', (void*)value);
}
/*---------------------------------------------------------------------*/
int config_delete_key(config_class_t *cc, char* class_key, int instance,
		      char *key)
{
	config_class_t *xcc;
	config_elem_t *ce,*lce;
	
	xcc=config_find_class(cc, class_key, instance);
	if (!xcc)
		return 0;
	
	ce=xcc->config;
	if (!ce)
		return 0;
	if (!strcmp(ce->key,key))
	{
		xcc->config=ce->next;
		config_free_element(ce);
		return 0;
	}

	lce=NULL;
	while(ce)
	{
		if (!strcmp(ce->key,key))
		{
			lce->next=ce->next;
			config_free_element(ce);
		}
		lce=ce;
		ce=ce->next;
	}
	return 0;
}
/*---------------------------------------------------------------------*/
void config_delete_class(config_class_t **cc,char *class_key, int instance)
{
	config_class_t *xcc,*lxcc=NULL;
	config_elem_t *ce,*lce;
	
	xcc=*cc;
	while(xcc)
	{
		if (!strcmp(xcc->class_key,class_key) && (xcc->instance==instance))
		{
			if (!lxcc)
				*cc=xcc->next;
			else
				lxcc->next=xcc->next;

			ce=xcc->config;
			if (ce)
			{
				while(ce)
				{
					lce=ce->next;
					config_free_element(ce);
					ce=lce;
				}
			}

			lxcc=xcc->next;
			config_free_class(xcc);
		
			//config_clean_instance(lxcc, class_key);
			return ;
		}
		lxcc=xcc;
		xcc=xcc->next;	
	}
	return ;

}
/*---------------------------------------------------------------------*/
void config_delete_all(config_class_t **cc)
{
	config_class_t *xcc,*lxcc=NULL;
	config_elem_t *ce,*lce;
	
	xcc=*cc;
	while(xcc)
	{
		ce=xcc->config;
		if (ce)
		{
			while(ce)
			{
				lce=ce->next;
				config_free_element(ce);
				ce=lce;
			}
		}
		lxcc=xcc->next;
		config_free_class(xcc);
		xcc=lxcc;
	}
	*cc=NULL;
}
/*---------------------------------------------------------------------*/
void config_parse_file(config_class_t **cc, FILE *file, config_template_t *cts)
{
	char buf[1024],class_key[1024],key[1024], value[1024];
	config_template_t *ct;
	config_template_t defct={"","",'s',NULL};
	char *p;
	int n;
	int instance=0;

	class_key[0]=key[0]=value[0]=0;
	while(!feof(file))
	{
		fgets(buf, 1023,file);
//		puts(buf);
		if ((buf[0]=='#')||(buf[0]==';')||(buf[0]==' ')||(buf[0]=='\n'))
			continue;

		if (buf[0]=='[')
		{
			instance=0;
			sscanf(buf+1,"%[^]0-9]%i",class_key,&instance);
			//printf("class <%s>, i %i, %s\n",class_key,instance,buf);
			continue;
		}

		key[0]=value[0]=0;
		p=buf;
		n=0;

		while(*p&&(*p!=' ')&&(*p!='\t')&&(*p!='\n'))
		{
			key[n++]=*p++;
		}

		key[n]=0;

		while(*p&&((*p==' ') || (*p=='\t')))
			p++;

		ct=config_find_template(class_key,key,cts);
		if (!ct)
		{
			ct=&defct;
		}
		if (ct->type=='i')
		{
			int val=0;
			sscanf(p,"%i",&val);
			config_set_class_key(cc,class_key, instance,
				       key, ct->type, (void*)val);
		}
		if (ct->type=='s')
		{
			if (*p=='"')
			{
				sscanf(p+1,"%[^\"\n]",value);
			}
			else
				sscanf(p,"%[^ \n\t]",value);
			config_set_class_key(cc,class_key, instance,
				       key, ct->type, (void*)value);
		}
	}
	 
}
/*---------------------------------------------------------------------*/
int config_read_file(config_class_t **cc,char* filename, config_template_t *cts)
{
	FILE *file;

	log (LOG_INFO, "Reading configuration file %s\n",filename);
	file=fopen(filename,"r");
	if (!file)
	{
		log(LOG_ERR,"Configuration %s unreadable: %s\n",filename,
		    strerror(errno));
		return -1;
	}
	config_parse_file(cc,file, cts);
	fclose(file);
	return 0;
}
/*---------------------------------------------------------------------*/
void config_dump_class(FILE *fd,config_class_t *cc)
{
	config_elem_t *ce;
	if (!cc)
		return;
	ce=cc->config;
	fprintf(fd,"[%s%i]\n",cc->class_key,cc->instance);
	while(ce)
	{
		if (ce->key[0]!='?')
		{
			if (ce->type=='i')
				fprintf(fd,"%s %i\n",ce->key,(int)ce->value);
			if (ce->type=='s')
				fprintf(fd,"%s \"%s\"\n",ce->key, (char*)ce->value);
		}
		ce=ce->next;
	}
}
/*---------------------------------------------------------------------*/
void config_dump_file(FILE *fd, config_class_t *cc, char* hdr)
{
	
	fprintf(fd,"# %s\n",hdr);
	while(cc)
	{
		config_dump_class(fd,cc);
		fprintf(fd,"\n");
		cc=cc->next;
	}
}
/*---------------------------------------------------------------------*/
void config_apply_choices(config_class_t *cc, config_template_t *cts)
{
	config_template_t *ct;
	config_elem_t *ce;

	while(cc)
	{
		ce=cc->config;
		while(ce)
		{
			ct=config_find_template(cc->class_key,ce->key,cts);
			if (ct)
			{
				ce->ch=ct->ch;
			}
			ce=ce->next;
		}
		cc->templates=cts;
		cc=cc->next;
	}

}
/*---------------------------------------------------------------------*/
int config_get_choice_enum(config_class_t *cc, char* class_key, int instance,
				char *key)
{
	char* str;
	config_elem_t *ce;
	choice_t *ch;
	int n=0;
	
	ce=config_find_class_key(cc,class_key,instance,key);

	
	if (!ce)
		return 0;
		
	str=(char*)ce->value;
	ch=ce->ch;
	
	while(ch->value)
	{
		if (!strcmp(ch->value,str))
			return n;
		n++;
		ch++;
	}
	return 0;
}
/*---------------------------------------------------------------------*/				
// works only if class_key alread used
void config_set_choice_enum(config_class_t *cc, char* class_key, int instance,
				char *key, int i)
{
	char* str;
	config_elem_t *ce;
	choice_t *ch;
	int n=0;
	config_class_t *xcc;
	
	ce=config_find_class_key(cc,class_key,instance,key);

	
	if (!ce)
		return;
		
	str=(char*)ce->value;
	ch=ce->ch;
	
	while(ch->value)
	{
		if (i==n)
		{
			xcc=cc;
			config_set_class_key(&xcc, class_key, instance, key, 
				's', (void*)ch->value);
			return;
		}
		n++;
		ch++;
	}
	return;	
}
/*---------------------------------------------------------------------*/
choice_t *config_find_choices(config_class_t *cc, char* class_key,
					int instance, char* key)
{
	config_elem_t *ce;

	ce=config_find_class_key(cc,class_key,instance,key);
	if (ce)
		return ce->ch;	
	return NULL;
}
