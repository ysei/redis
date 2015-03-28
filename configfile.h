//***************************************************************************
// kmp3sel/configfile.h
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
// $Id: configfile.h,v 1.1.1.1 2002/04/29 23:18:15 acher Exp $
//***************************************************************************

typedef struct
{
	char *value;
	char *text;
} choice_t;

typedef struct
{
	char class_key[32];
	char key[32];
	char type;
	choice_t *ch;
} config_template_t;

typedef struct cfet
{
	char key[32];
	char type;  // i/s
	void  *value;
	struct cfet *next;
	choice_t *ch;
} config_elem_t;

typedef struct cfct
{
	char class_key[32];
	int instance;
	config_elem_t *config;
	struct cfct *next;
	config_template_t *templates;
} config_class_t;

config_class_t *config_find_class(config_class_t* cc,char *class_key, 
				  int instance);

config_elem_t *config_find_class_key(config_class_t* cc, char* class_key,
				     int instance, char* key);

int config_find_class_key_int(config_class_t* cc, char* class_key,
			      int instance, char* key);

char* config_find_class_key_string(config_class_t* cc, char* class_key,
				 int instance, char* key);


int config_find_key_int(config_class_t* cc, char* key);
char* config_find_key_string(config_class_t* cc, char* key);


void config_set_class_key(config_class_t **cc, char* class_key, int instance,
		    char *key, char type, void *value);

void config_set_class_key_int(config_class_t **cc, char* class_key, int instance,
			char *key, int value);

void config_set_class_key_string(config_class_t **cc, char* class_key, int instance,
			   char *key, char *value);

void config_set_key(config_class_t *cc,char *key, char type, void *value);
void config_set_key_int(config_class_t *cc,char *key, int value);
void config_set_key_string(config_class_t *cc,char *key, char* value);


int config_read_file(config_class_t **cc, char* filename, config_template_t *cts);

void config_dump_file(FILE *fd, config_class_t *cc, char* hdr);
void config_dump_class(FILE *fd,config_class_t *cc);
void config_clone_add_class(config_class_t **cc, config_class_t *oo);
void config_clone_add_class_X(config_class_t **cc, config_class_t *oo, int instance);

void config_apply_choices(config_class_t *cc, config_template_t *cts);
int config_get_choice_enum(config_class_t *cc, char* class_key, int instance,
				char *key);
				
void config_set_choice_enum(config_class_t *cc, char* class_key, int instance,
				char *key, int i);

choice_t *config_find_choices(config_class_t *cc, char* class_key,
					int instance, char* key);
					
void config_delete_class(config_class_t **cc,char *class_key, int instance);

int config_delete_key(config_class_t *cc, char* class_key, int instance,
		      char *key);
void config_delete_all(config_class_t **cc);
