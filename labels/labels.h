/***************/
/* label types */
/***************/
#define LBL_CMTL	0x01
#define LBL_CMTA	0x02

#define LBL_LABM	0x03
#define LBL_LABI	0x04
#define LBL_LABV	0x05

#define LBL_NUMLE	0x06
#define LBL_NUMBE	0x07

#define LBL_VECMLE	0x08
#define LBL_VECMBE	0x09
#define LBL_VECILE	0x0a
#define LBL_VECIBE	0x0b

#define LBL_TXT		0x0c
#define LBL_STR0	0x0d
#define LBL_STR7	0x0e
#define LBL_STRP	0x0f

#define LBL_PAT		0x10

/****************/
/* parser flags */
/****************/
#define HAS_NAME	0x01
#define HAS_START	0x02
#define HAS_END		0x04
#define HAS_SIZE	0x08
#define HAS_BASE	0x10
#define HAS_TYPE	0x20
#define HAS_SEG		0x40
#define HAS_PAT		0x80

typedef struct
{
	char *str;
	int type;
	int flags;
} labdef_t;

labdef_t labdef[]=
{
	// source comments
	{"#",0,0},					
	{";",0,0},					

	// disassembly comments
	{"c",LBL_CMTL,HAS_NAME|HAS_START},		// line comment (short)	
	{"cl",LBL_CMTL,HAS_NAME|HAS_START},		// line comment 
	{"ca",LBL_CMTA,HAS_NAME|HAS_START},		// appended comment

	// labels
	{"l",LBL_LABM,HAS_START|HAS_NAME},		// mem label (short)
	{"lm",LBL_LABM,HAS_START|HAS_NAME},		// mem label
	{"li",LBL_LABI,HAS_START|HAS_NAME|HAS_SEG},	// i/o label
	{"lmv",LBL_LABV,HAS_START|HAS_NAME|HAS_BASE},	// immediate value reflecting mem address
	{"liv",LBL_LABV,HAS_START|HAS_NAME|HAS_BASE|HAS_SEG},	// immediate value reflecting i/o address
	
	// numerics
	{"n",LBL_NUMLE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE},	// LE
	{"N",LBL_NUMBE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE},	// BE

	// strings & chars
	{"t",LBL_TXT,HAS_START|HAS_END|HAS_NAME|HAS_SIZE},	// text
	{"s",LBL_STR0,HAS_START|HAS_END|HAS_NAME},		// string (0-ended, short)
	{"s0",LBL_STR0,HAS_START|HAS_END|HAS_NAME},		// string (0-ended)
	{"s7",LBL_STR7,HAS_START|HAS_END|HAS_NAME},		// string (bit7-ended)
	{"sp",LBL_STRP,HAS_START|HAS_END|HAS_NAME},		// pascal string (size+str)

	// vectors
	{"v",LBL_VECMLE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE|HAS_BASE|HAS_TYPE},
	{"V",LBL_VECMBE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE|HAS_BASE|HAS_TYPE},
	{"vm",LBL_VECMLE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE|HAS_BASE|HAS_TYPE},
	{"Vm",LBL_VECMBE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE|HAS_BASE|HAS_TYPE},
	{"vi",LBL_VECILE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE|HAS_BASE|HAS_TYPE},
	{"Vi",LBL_VECIBE,HAS_START|HAS_END|HAS_NAME|HAS_SIZE|HAS_BASE|HAS_TYPE},

	// patterns
	{"p",LBL_PAT,HAS_START|HAS_END|HAS_NAME|HAS_PAT},
	
	{0,0,0}
};		

typedef struct
{
	int type;
	int flags;
	int start;
	int seg;
	int size;
	int base;
	int vtype;
	int vflags;
} pat_t;

typedef struct
{
	int type;
	int flags;
	int start;
	int end;
	int seg;
	char *name;
	char *cmt;
	int size;
	int base;
	int vtype;
	int vflags;
	
	int plen;
	pat_t *pat;
} lbl_t;

/* 
	label format
	<type> <name> <start> <end> <size> <base> <type> <pattern>
*/	