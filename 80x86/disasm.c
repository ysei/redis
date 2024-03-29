#include<stdio.h>
#include<string.h>

#define X86_SEG_ES 1
#define X86_SEG_CS 2
#define X86_SEG_SS 3
#define X86_SEG_DS 4

char *x86_imm0[]={
	"add","or" ,"adc","sbb",
	"and","sub","xor","cmp"
};

char *x86_imm1[]={
	"add",""  ,"adc","sbb",
	""  ,"sub",""   ,"cmp"
};

char *x86_rot[]={
	"rol","ror","rcl","rcr",
	"shl","shr",""   ,"sar"
};

char *x86_seg[]={
	"", "es","cs","ss","ds"
};

char *x86_jxx[]={
	"o","no","c","nc","z","nz","na","a",
	"s","ns","pe","po","l","ge","le","g"
};

char *x86_regs[]={
	"al","cl","dl","bl","ah","ch","dh","bh",
	"ax","cx","dx","bx","sp","bp","si","di"
};

char *x86_idpp[]={
	"inc","dec","push","pop"
};

char *x86_grp1[]={
	"test",""    ,"not","neg",
	"mul" ,"imul","div","idiv"
};

char *x86_grp2[]={
	"inc","dec" ,"call near","call far",
	"jmp near","jmp far","push",""
};

typedef struct
{
	int cpu;	// CPU (numeric)
	
	int pc;		// program counter
	int seg;	// segment
	int opc;	// opcode 
	
	char *str;	// output string (disassembly)
	char *hex;	// output string (hex code)

	int done;	// disassembly done(=1)
	int ill;	// illegal opcode(=1)

	int lock;	// locked (x86)
	int rep;	// repeated (x86)

	int brk;	// command was either jmp, ret or similar

} pc_t;

typedef struct
{
	int cpu;	// CPU
	int opc;	// opcode
	char *str;	// mnemonic
} x86_opc_t;

x86_opc_t x86_imp[]={
	{ 86,0x06,"push es"},
	{ 86,0x07,"pop es"},
	{ 86,0x0e,"push cs"},
	{ 86,0x16,"push ss"},
	{ 86,0x17,"pop ss"},
	{ 86,0x1e,"push ds"},
	{ 86,0x1f,"pop ds"},
	{ 86,0x27,"daa"},
	{ 86,0x2f,"das"},
	{ 86,0x37,"aaa"},
	{ 86,0x3f,"aas"},
	{186,0x60,"pusha"},
	{186,0x61,"popa"},
	{ 86,0x98,"cbw"},
	{ 86,0x99,"cwd"},
	{ 86,0x9b,"wait"},
	{ 86,0x9c,"pushf"},
	{ 86,0x9d,"popf"},
	{ 86,0x9e,"sahf"},
	{ 86,0x9f,"lahf"},
	{ 86,0xa4,"movsb"},
	{ 86,0xa5,"movsw"},
	{ 86,0xa6,"cmpsb"},
	{ 86,0xa7,"cmpsw"},
	{ 86,0xaa,"stosb"},
	{ 86,0xab,"stosw"},
	{ 86,0xac,"lodsb"},
	{ 86,0xad,"lodsw"},
	{ 86,0xae,"scasb"},
	{ 86,0xaf,"scasw"},
	{ 86,0xc3,"ret"},
	{186,0xc9,"leave"},
	{ 86,0xcb,"reti"},
	{ 86,0xcc,"int3"},
	{ 86,0xce,"into"},
	{ 86,0xcf,"iret"},
	{ 86,0xd4,"aam"},
	{ 86,0xd5,"aad"},
	{ 86,0xd7,"xlat"},
	{ 86,0xec,"in al,dx"},
	{ 86,0xed,"in ax,dx"},
	{ 86,0xee,"out dx,al"},
	{ 86,0xef,"out dx,ax"},
	{ 86,0xf4,"hlt"},
	{ 86,0xf5,"cmc"},
	{ 86,0xf8,"clc"},
	{ 86,0xf9,"stc"},
	{ 86,0xfa,"cli"},
	{ 86,0xfb,"sti"},
	{ 86,0xfc,"cld"},
	{ 86,0xfd,"std"},

	{0xff,0,0}
};

x86_opc_t x86_imm8[]={
	{ 86,0x04,"add al,"},
	{ 86,0x0c,"or al,"},
	{ 86,0x14,"adc al,"},
	{ 86,0x1c,"sbb al,"},
	{ 86,0x24,"and al,"},
	{ 86,0x2c,"sub al,"},
	{ 86,0x34,"xor al,"},
	{ 86,0x3c,"cmp al,"},
	{186,0x6a,"push "},
	{ 86,0xa8,"test al,"},
	{ 86,0xb0,"mov al,"},
	{ 86,0xb1,"mov cl,"},
	{ 86,0xb2,"mov dl,"},
	{ 86,0xb3,"mov bl,"},
	{ 86,0xb4,"mov bh,"},
	{ 86,0xb5,"mov ch,"},
	{ 86,0xb6,"mov dh,"},
	{ 86,0xb7,"mov bh,"},
	{ 86,0xcd,"int "},
	{ 86,0xe4,"in al,"},
	{ 86,0xe6,"out "},

	{0xff,0,0}
};

x86_opc_t x86_imm16[]={
	{ 86,0x05,"add ax,"},
	{ 86,0x0d,"or ax,"},
	{ 86,0x15,"adc ax,"},
	{ 86,0x1d,"sbb ax,"},
	{ 86,0x25,"and ax,"},
	{ 86,0x2d,"sub ax,"},
	{ 86,0x35,"xor ax,"},
	{ 86,0x3d,"cmp ax,"},
	{186,0x68,"push "},
	{ 86,0xa9,"test al,"},
	{ 86,0xb8,"mov ax,"},
	{ 86,0xb9,"mov cx,"},
	{ 86,0xba,"mov dx,"},
	{ 86,0xbb,"mov bx,"},
	{ 86,0xbc,"mov sp,"},
	{ 86,0xbd,"mov bp,"},
	{ 86,0xbe,"mov si,"},
	{ 86,0xbf,"mov di,"},
	{ 86,0xc2,"ret "},
	{ 86,0xca,"ret "},
	{ 86,0xe5,"in ax,"},
	{ 86,0xe7,"out "},

	{0xff,0,0}
};

typedef struct
{
	int cpu;	// CPU
	int opc;	// opcode
	char *str;	// mnemonic
	int dir;	// direction: from/to register
	int size;	// size resides on bit "size"
	int isseg;	// register is segment register
} x86_mrm_t;

x86_mrm_t x86_mrms[]={
	{ 86,0x00,"add",0,0,0},
	{ 86,0x01,"add",0,0,0},
	{ 86,0x02,"add",1,0,0},
	{ 86,0x03,"add",1,0,0},
	{ 86,0x08,"or",0,0,0},
	{ 86,0x09,"or",0,0,0},
	{ 86,0x0a,"or",1,0,0},
	{ 86,0x0b,"or",1,0,0},
	{ 86,0x10,"adc",0,0,0},
	{ 86,0x11,"adc",0,0,0},
	{ 86,0x12,"adc",1,0,0},
	{ 86,0x13,"adc",1,0,0},
	{ 86,0x18,"sbb",0,0,0},
	{ 86,0x19,"sbb",0,0,0},
	{ 86,0x1a,"sbb",1,0,0},
	{ 86,0x1b,"sbb",1,0,0},
	{ 86,0x20,"and",0,0,0},
	{ 86,0x21,"and",0,0,0},
	{ 86,0x22,"and",1,0,0},
	{ 86,0x23,"and",1,0,0},
	{ 86,0x28,"sub",0,0,0},
	{ 86,0x29,"sub",0,0,0},
	{ 86,0x2a,"sub",1,0,0},
	{ 86,0x2b,"sub",1,0,0},
	{ 86,0x30,"xor",0,0,0},
	{ 86,0x31,"xor",0,0,0},
	{ 86,0x32,"xor",1,0,0},
	{ 86,0x33,"xor",1,0,0},
	{ 86,0x38,"cmp",0,0,0},
	{ 86,0x39,"cmp",0,0,0},
	{ 86,0x3a,"cmp",1,0,0},
	{ 86,0x3b,"cmp",1,0,0},
	{186,0x62,"bound",0,0,0},
	{186,0x69,"imul",1,0,0},
	{186,0x6b,"imul",1,0,0},
	{ 86,0x84,"test",0,0,0},
	{ 86,0x85,"test",0,0,0},
	{ 86,0x86,"xchg",0,0,0},
	{ 86,0x87,"xchg",0,0,0},
	{ 86,0x88,"mov",0,0,0},
	{ 86,0x89,"mov",0,0,0},
	{ 86,0x8a,"mov",1,0,0},
	{ 86,0x8b,"mov",1,0,0},
	{ 86,0x8c,"mov",0,0,1},
	{ 86,0x8d,"lea",1,0,0},
	{ 86,0x8e,"mov",1,0,1},
	{ 86,0xc4,"les",1,0,0},
	{ 86,0xc5,"lds",1,0,0},
	
	{0xff,0,0,0,0,0}
};

unsigned char mem[65536];

char *zalloc(int num)
{
	return (char *)malloc(num);
}

unsigned char get_byte(pc_t *pc)
{
unsigned char x;

	x=(unsigned char)mem[pc->pc];
	(pc->pc)+=1;
	return x;
}

char *align(char *in)
{
	if(strlen(in)<28)
		memset(in+strlen(in),32,28-strlen(in));
	return in+1;
}

int x86_calc_mod(pc_t *pc, int rm, int mod)
{
int x,y;
	switch(mod&3)
	{
		case 0:
			if(rm==6)
			{
				x=get_byte(pc);
				y=get_byte(pc);
				sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);
				x|=(y<<8);

				break;
			}
				return 0;
		case 1:
			x=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,x);
			y=((0xff*(x>>7))<<8)|x;
			x=y;
			break;
			
		case 2:
			x=get_byte(pc);
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);
			x|=(y<<8);
			break;
		case 3:
			return 0;
	}

	return x;
}

void x86_calc_ea(pc_t *pc, int rm, int mod)
{
char *dstr, *sstr;
int disp,x;

	dstr=(char *)malloc(8);
	memset(dstr,0,5);

	sstr=(char *)malloc(8);
	memset(sstr,0,4);
	
	if(pc->seg!=0)
		sprintf(sstr,"%s:",x86_seg[pc->seg]);
		
	if(mod!=3)
		disp=x86_calc_mod(pc,rm,mod);
	
	if(disp&0x8000)
		x=32768-(disp&0x7fff);
	else
		x=disp;
		
	if(disp>0)
	{
		if(x<256)
			sprintf(dstr,"%c0x%02x",(disp&0x8000)?'-':'+',x);
		else 
			sprintf(dstr,"%c0x%04x",(disp&0x8000)?'-':'+',x);
	}
			
	switch(rm)
	{
		case 0:
			sprintf(pc->str,"%s[%sbx+si%s]",pc->str,sstr,dstr);
			break;
		case 1:
			sprintf(pc->str,"%s[%sbx+di%s]",pc->str,sstr,dstr);
			break;
		case 2:
			sprintf(pc->str,"%s[%sbp+si%s]",pc->str,sstr,dstr);
			break;
		case 3:
			sprintf(pc->str,"%s[%sbp+di%s]",pc->str,sstr,dstr);
			break;
		case 4:
			sprintf(pc->str,"%s[%ssi%s]",pc->str,sstr,dstr);
			break;
		case 5:
			sprintf(pc->str,"%s[%sdi%s]",pc->str,sstr,dstr);
			break;
		case 6:
			if(mod!=0)
				sprintf(pc->str,"%s[%sbx%s]",pc->str,sstr,dstr);
			else
				sprintf(pc->str,"%s[%s0x%04x]",pc->str,sstr,disp);
			break;
		case 7:
			sprintf(pc->str,"%s[%sbx%s]",pc->str,sstr,dstr);
			break;
	}
	
	free(dstr);
	free(sstr);
}

char **regblk(pc_t *pc, int isseg, int reg)
{
char **regs;
	if(isseg==1)
	{
		regs=&x86_seg[1];
		if (reg&8)
			pc->ill=1;	
	}
	else
		regs=x86_regs;

	return regs;
}

void x86_mrm(pc_t *pc, int dir, int size, int isseg)
{
int x, y, reg, mod, rm, w;

char **regs;

	x=get_byte(pc);
	sprintf(pc->hex,"%s %02x",pc->hex,x);
	
	mod=(x>>6);
	reg=(x>>3)&7;
	rm=x&7;
	w=(pc->opc>>size)&1;

	if(dir==1)
	{
		regs=regblk(pc,isseg,reg);
		sprintf(pc->str,"%s%s,",pc->str,regs[reg+(w<<3)]);
	}

	if(mod!=3)
		x86_calc_ea(pc,rm,mod);
	else	
	{
		regs=regblk(pc,0,reg);
		if(isseg==1)
			rm+=8;
		else
			rm+=(w<<3);
		sprintf(pc->str,"%s%s",pc->str,regs[rm]);	
	}

	if(dir==0)
	{
		regs=regblk(pc,isseg,reg);
		sprintf(pc->str,"%s,%s",pc->str,regs[reg+(w<<3)]);
	}

	if((pc->opc==0x69)||(pc->opc==0x6b))
	{
		x=get_byte(pc);
		sprintf(pc->hex,"%s %02x",pc->hex,x);
		if(pc->opc==0x69)
		{
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,y);
			sprintf(pc->str,"%s,0x%02x%02x",pc->str,y,x);
		}
		else
			sprintf(pc->str,"%s,0x%02x",pc->str,x);
	}
}
	
void x86_disasm(pc_t *pc)
{
int x,y,z,i,found;
char *str;
	
do{
	found=0;
	
	pc->opc=get_byte(pc);
	sprintf(pc->hex,"%s %02x",pc->hex,pc->opc);
		
	// single-byte commands
	switch(pc->opc)
	{
		// mov imm
		case 0xc6:
		case 0xc7:
			pc->done=1;
			
			x=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,x);
			
			if((pc->opc&0x38)!=0)
			{
				pc->ill=1;
				break;
			}	

			sprintf(pc->str,"%smov %s ",pc->str,(pc->opc&1)?"word":"byte");

			y=(x>>6);
			z=(x&7);
			if(y!=3)
				x86_calc_ea(pc,z,y);
			else
			{
				pc->ill=1;
				break;
			}

			x=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,x);
			
			if(pc->opc&1)
			{
				y=get_byte(pc);
				sprintf(pc->hex,"%s %02x",pc->hex,y);
				sprintf(pc->str,"%s,0x%02x%02x",pc->str,y,x);
			} else
				sprintf(pc->str,"%s,0x%02x",pc->str,x);

			break;

		// mov ax/al
		case 0xa0:
		case 0xa1:
		case 0xa2:
		case 0xa3:
			pc->done=1;
			
			x=get_byte(pc);
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);

			sprintf(pc->str,"%smov ",pc->str);
			if(!(pc->opc&2))
				sprintf(pc->str,"%sa%c,",pc->str,(pc->opc&1)?'x':'l');

			sprintf(pc->str,"%s[0x%02x%02x]",pc->str,y,x);

			if(pc->opc&2)
				sprintf(pc->str,"%s,a%c",pc->str,(pc->opc&1)?'x':'l');
			break;
						
		// immediate group
		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:

		// test
		case 0xa8:
		case 0xa9:
		
		// shift by immediate
		case 0xc0:
		case 0xc1:

		// shift by 1 or CL
		case 0xd0:
		case 0xd1:
		case 0xd2:
		case 0xd3:

		// group 1
		case 0xf6:
		case 0xf7:
		
		// group 2:
		case 0xfe:
		case 0xff:

			pc->done=1;
			
			x=get_byte(pc);
			y=(x>>3)&7;
			sprintf(pc->hex,"%s %02x",pc->hex,x);			

			switch(pc->opc&0xf0)
			{
				case 0x80:
					if(pc->opc&2)
						str=x86_imm1[y];
					else
						str=x86_imm0[y];
					break;
				case 0xa0:
					if(pc->opc&8)
						str=strdup("test");
					break;
				case 0xc0:
				case 0xd0:
					str=x86_rot[y];
					break;
				case 0xf0:
					if(pc->opc&8)
					{
						str=x86_grp2[y];
						if((y==4)||(y==5))
							pc->brk=1;						
					}
					else
						str=x86_grp1[y];
					break;
			}
			
			if(*str==0)
			{
				pc->ill=1;
				break;
			}
			else
				sprintf(pc->str,"%s%s ",pc->str,str);
			
			y=(x>>6);
			z=(x&7);
			if(y!=3)
			{
				sprintf(pc->str,"%s%s ",pc->str,(pc->opc&1)?"word":"byte");
				x86_calc_ea(pc,z,y);
			}
			else
			{
				y=((pc->opc&1)<<3)|z;
				sprintf(pc->str,"%s%s",pc->str,x86_regs[y]);
			}
								
			switch(pc->opc&0xf0)
			{
				case 0xa0:
					if(!(pc->opc&8))
						break;
				case 0x80:
					x=get_byte(pc);
					sprintf(pc->hex,"%s %02x",pc->hex,x);
					if((pc->opc&3)==1)
					{	y=get_byte(pc);
						sprintf(pc->hex,"%s %02x",pc->hex,y);
						sprintf(pc->str,"%s,0x%02x%02x",pc->str,y,x);
					} else
						sprintf(pc->str,"%s,0x%02x",pc->str,x);
					break;
				case 0xc0:
					x=get_byte(pc);
					sprintf(pc->hex,"%s %02x",pc->hex,x);
					sprintf(pc->str,"%s,0x%02x",pc->str,x);
					break;
				case 0xd0:
					if(pc->opc&2)
						sprintf(pc->str,"%s,cl",pc->str);
					else
						sprintf(pc->str,"%s,1",pc->str);
					break;
				case 0xf0:
					if(((pc->opc&0xf)>7)||((x&0x38)!=0))
						break;
						
					x=get_byte(pc);
					sprintf(pc->hex,"%s %02x",pc->hex,x);
					if(pc->opc&1)
					{
						y=get_byte(pc);
						sprintf(pc->hex,"%s %02x",pc->hex,y);
						sprintf(pc->str,"%s,0x%02x%02x",pc->str,y,x);
					} else
						sprintf(pc->str,"%s,0x%02x",pc->str,x);
					break;
			}
			
			break;
		
		case 0x26:	
			if(pc->seg==0)
			{
				pc->seg=X86_SEG_ES;
				found=1;
			}
			else
				pc->ill=1;
			break;
		case 0x2e:	
			if(pc->seg==0)
			{
				pc->seg=X86_SEG_CS;
				found=1;
			}
			else
				pc->ill=1;
			break;
		case 0x36:	
			if(pc->seg==0)
			{
				pc->seg=X86_SEG_SS;
				found=1;
			}
			else
				pc->ill=1;
			break;
		case 0x3e:	
			if(pc->seg==0)
			{
				pc->seg=X86_SEG_DS;
				found=1;
			}
			else
				pc->ill=1;
			break;
		case 0xf0:
			if(pc->lock==0)
			{
//				sprintf(pc->str,"lock ");
				pc->lock=1;
				found=1;
			}
			else
				pc->ill=1;
			break;
		case 0xf2:
			if(pc->rep==0)
			{
//				sprintf(pc->str,"repnz ");
				pc->rep=1;
				found=1;
			}
			else
				pc->ill=1;
			break;
		case 0xf3:
			if(pc->rep==0)
			{
//				sprintf(pc->str,"repz ");
				pc->rep=2;
				found=1;
			}
			else
				pc->ill=1;
			break;
		default:
			i=0;
			do{
				if(pc->opc==x86_imp[i].opc)
				{
					if(pc->cpu<=x86_imp[i].cpu)
						sprintf(pc->str,"%s%s",pc->str,x86_imp[i].str);
					else 
						pc->ill=1;
					pc->done=1;
					break;
				}
				i++;
			} while(x86_imp[i].cpu!=0xff);	
	}

	// immediate8 commands
	i=0;
	do{
		if(pc->opc==x86_imm8[i].opc)
		{
			if(pc->cpu<=x86_imm8[i].cpu)
			{
				x=get_byte(pc);
				sprintf(pc->hex,"%s %02x",pc->hex,x);
				sprintf(pc->str,"%s%s0x%02x",pc->str,x86_imm8[i].str,x);
			} else
				pc->ill=1;

			if(pc->opc==0xe6)
				sprintf(pc->str,"%s,al",pc->str);	
			pc->done=1;
			break;
		}
		i++;
	} while(x86_imm8[i].cpu!=0xff);

	// immediate16 commands
	i=0;
	do{
		if(pc->opc==x86_imm16[i].opc)
		{
			if(pc->cpu<=x86_imm16[i].cpu)
			{
				x=get_byte(pc);
				y=get_byte(pc);
				sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);
				sprintf(pc->str,"%s%s0x%02x%02x",pc->str,x86_imm16[i].str,y,x);
			} else
				pc->ill=1;
			if(pc->cpu==0xe7)
				sprintf(pc->str,"%s,ax",pc->str);	
			pc->done=1;
			break;
		}
		i++;
	} while(x86_imm16[i].cpu!=0xff);

	//mrm commands
	i=0;
	do{
		if(pc->opc==x86_mrms[i].opc)
		{
			if(pc->cpu<=x86_mrms[i].cpu)
			{
				sprintf(pc->str,"%s%s ",pc->str,x86_mrms[i].str);
				x86_mrm(pc,x86_mrms[i].dir,x86_mrms[i].size,x86_mrms[i].isseg);
			} else
				pc->ill=1;
			pc->done=1;
			break;
		}
		i++;
	} while(x86_mrms[i].cpu!=0xff);
	
	if((pc->done==0)&&(pc->ill==0))
	{
		// jxx/loop
		if( ((pc->opc&0xf0)==0x70)||(pc->opc==0xe0)||(pc->opc==0xe1)||(pc->opc==0xe2)||(pc->opc==0xe3)||(pc->opc==0xeb) )
		{
			x=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,x);
			
			x=((x+pc->pc)&0xff)|(pc->pc&0xff00);

			switch(pc->opc)
			{
				case 0xe0:
					sprintf(pc->str,"loopnz");
					break;
				case 0xe1:
					sprintf(pc->str,"loopz");
					break;
				case 0xe2:
					sprintf(pc->str,"loop");
					break;
				case 0xe3:
					sprintf(pc->str,"jcxz");
					break;
				case 0xeb:
					sprintf(pc->str,"jmp short");
					break;
				default:
					sprintf(pc->str,"j%s",x86_jxx[(pc->opc)&0x0f]);
					break;
			}

			sprintf(pc->str,"%s 0x%04x",pc->str,x);
			pc->done=1;
		}
		
		// XCHG/NOP
		else if(((pc->opc&0xf0)==0x90)&&((pc->opc&0x0f)<8))
		{
			if(pc->opc==0x90)
				sprintf(pc->str,"nop");
			else
				sprintf(pc->str,"xchg ax,%s",x86_regs[8+(pc->opc&7)]);
			pc->done=1;
		}

		// INC/DEC/PUSH/POP
		else if(((pc->opc&0xf0)==0x40)||((pc->opc&0xf0)==0x50))
		{
			x=((((pc->opc&0xf0)>>4)-4)<<1)+((pc->opc&8)>>3);
			y=(pc->opc&7)+8;
			sprintf(pc->str,"%s %s",x86_idpp[x],x86_regs[y]);
			pc->done=1;
		}

		// MOV reg,imm
		else if((pc->opc&0xf0)==0xb0)
		{
			x=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,x);
			sprintf(pc->str,"mov %s,0x",x86_regs[pc->opc&0x0f]);

			if((pc->opc&0x0f)>7)
			{
				y=get_byte(pc);
				sprintf(pc->hex,"%s %02x",pc->hex,y);
				sprintf(pc->str,"%s%02x%02x",pc->str,y,x);
			}
			else
				sprintf(pc->str,"%s%02x",pc->str,x);
			pc->done=1;
		}

		// enter
		else if(pc->opc==0xc8)
		{
			x=get_byte(pc);
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);
			x!=(y<<8);
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x",pc->hex,y);
			sprintf(pc->str,"enter 0x");
			if(x<256)
				sprintf(pc->str,"%s%02x",pc->str,x);
			else
				sprintf(pc->str,"%s%04x",pc->str,x);
			sprintf(pc->str,"%s,0x%02x",pc->str,y);
			pc->done=1;
		}
		
		// call/jmp far
		else if((pc->opc==0x9a)||(pc->opc==0xea))
		{
			x=get_byte(pc);
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);
			x|=(y<<8);

			if(pc->opc==0xea)
				sprintf(pc->str,"jmp");
			else
				sprintf(pc->str,"call");

			y=get_byte(pc);
			z=get_byte(pc);
			sprintf(pc->hex,"%s %02x %02x",pc->hex,z,y);
			y|=(z<<8);
		
			sprintf(pc->str,"%s 0x%08x",pc->str,(y<<16)|x);
			pc->done=1;
		}

		// call/jmp
		else if((pc->opc==0xe8)||(pc->opc==0xe9))
		{
			x=get_byte(pc);
			y=get_byte(pc);
			sprintf(pc->hex,"%s %02x %02x",pc->hex,x,y);

			x|=(y<<8);
			x=(x+pc->pc)&0xffff;			

			sprintf(pc->str,"%s 0x%04x",(pc->opc==0xe8)?"call":"jmp",x);
			pc->done=1;
		}
		else if(found==0)
		{
			pc->ill=1;
			pc->done=1;
		}
	}

} while((pc->done==0)&&(pc->ill==0));

	// repeated string command
	if(pc->rep==1)
	{
		switch(pc->opc)
		{
			case 0xa4:
			case 0xa5:
			case 0xaa:
			case 0xab:
				sprintf(pc->str,"rep %s",pc->str);
				break;
			case 0xa6:
			case 0xa7:
			case 0xae:
			case 0xaf:
				sprintf(pc->str,"repe %s",pc->str);
				break;
			default:
				pc->ill=1;
				break;
		}		
	}
	else if(pc->rep==2)
	{
		switch(pc->opc)
		{
			case 0xa4:
			case 0xa5:
			case 0xaa:
			case 0xab:
			case 0xa6:
			case 0xa7:
			case 0xae:
			case 0xaf:
				sprintf(pc->str,"repne %s",pc->str);
				break;
			default:
				pc->ill=1;
				break;
		}		
	}	

	// locked command
	if(pc->lock==1)
		sprintf(pc->str,"lock %s",pc->str);
		
	if(pc->ill==1)
		sprintf(pc->str,"???");

	switch(pc->opc)
	{
		case 0xc2:
		case 0xc3:
		case 0xca:
		case 0xcb:
		case 0xcf:
		case 0xe9:
		case 0xea:
		case 0xeb:
			pc->brk=1;
			pc->done=1;
	}
	
}

int main(int argc, char **argv)
{
FILE *file;
pc_t pc;
int adr;
	argv++;
	
	file=fopen(*argv,"r");
	if(file==NULL)
	{
		fprintf(stderr,"Unable to open \"%s\".\n",*argv);
		exit(-1);
	}
	
	argv++;
	fread(mem,65536,1,file);
	fclose(file);

	pc.pc=strtoul(*argv,0,0);
	
	pc.cpu=86;
	pc.done=1;
	
	pc.str=(char *)malloc(256);
	pc.hex=(char *)malloc(256);
	
	do{
		if(pc.done==1)
		{
			memset(pc.str,0,256);
			memset(pc.hex,0,256);
		}
		
		adr=pc.pc;
		pc.seg=0;
		pc.done=0;
		pc.ill=0;
		pc.rep=0;
		pc.brk=0;		

		x86_disasm(&pc);
		if(pc.done==1)
			printf("%04x:\t%s%s\n",adr,align(pc.hex),pc.str);
		if(pc.brk==1)
			printf("\n;\n");
			
	} while(pc.pc<0x10000);

	free(pc.str);
	free(pc.hex);
}