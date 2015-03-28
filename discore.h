#ifndef __INCLUDE_DISCORE_H
#define __INCLUDE_DISCORE_H

typedef struct 
{
        unsigned char *buf;
        int pc; // pos of opcode base
        int opc; // current opcode
        int len;
        int base;
        int size;
        int total_len;
        int vector_table;
} dispc_t;

typedef struct
{
	int pc;
	char *name;
	char *desc;
	int end;
	int dtype;
	int dsize;
	char *seq;
} label_t;

#if 0
class disbase 
{
public:
	disbase(void);
	~disbase(void);
	

	unsigned int get_byte(dispc_t *pc);
	unsigned int get_word(dispc_t *pc);
	unsigned int get_long(dispc_t *pc);
	unsigned int get_byte_n(dispc_t *pc, int p);
	unsigned int get_word_n(dispc_t *pc, int p);
	unsigned int get_long_n(dispc_t *pc, int p);
	int get_displacement5(dispc_t *pc);
	int get_displacement8(dispc_t *pc);
	int get_displacement16(dispc_t *pc);
	int get_displacement32(dispc_t *pc);
	void format_pc(dispc_t *pc, int p, char *ext,char *str);
	void format_constant(dispc_t *pc, int p, int size, char *str);
	void show_vectors(dispc_t *pc);
	void disass(dispc_t *pc, char* str);	

};
#endif


unsigned int get_byte(dispc_t *pc);
unsigned int get_word(dispc_t *pc);
unsigned int get_long(dispc_t *pc);
unsigned int get_byte_n(dispc_t *pc, int p);
unsigned int get_word_n(dispc_t *pc, int p);
unsigned int get_long_n(dispc_t *pc, int p);

int get_displacement5(dispc_t *pc);
int get_displacement8(dispc_t *pc);
int get_displacement16(dispc_t *pc);
int get_displacement32(dispc_t *pc);
void format_pc(dispc_t *pc, int p, char *ext, char *str);

void format_constant(dispc_t *pc, int p, int size, char *str);

int seperate_mask(char* mask, int startbit, int val, int datas[8]);
int match_mask(char *mask, int startbit, int val);

void read_labels(char *labelfile);
label_t *find_label(int p);
void store_label(char* name, int start, int end, char *desc, char dtype, char dsize, char *seq);
void init_labels(void);
void sort_labels(void);

// dump.cpp
int dump(int xstart, int xend, int size, int step, dispc_t *pc);

// textractor.cpp
int textractor(int xstart, int xend, int size, dispc_t *pc);

// strextractor.cpp
int strextractor(int xstart, int xend, dispc_t *pc);
// discore.cpp
void discore(dispc_t *pc, int cpu, char *labelfile);


// bitextract
#define BE(x,y) ((x>>y)&1)
#define BEX(x,y,z) ((x>>y)&((1<<z)-1))


//#include "68k/68k_dis.h"
//#include "6809/6809_dis.h"

#endif
