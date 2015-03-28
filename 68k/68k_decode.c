/* 
 *      decode.cpp - decoding opcode groups for dis68k
 *      (c) 2001 Georg Acher, georg@acher.org
 *      
 *      #include <gpl-stuff.h>
 *
 *     $Id: 68k_decode.cpp,v 1.1.1.1 2002/04/29 23:18:29 acher Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../discore.h"
#include "68k_dis.h"

extern char *condcodes[];
extern char *condcodes_dbcc[];

static char newln[]="\n-----------------------------------------------------------------------------\n";

#define CHECK_SIZE(x) if (datas[x]==3) return 1

// comments for calls: order of arguments in datas[]
/*------------------------------------------------------------------------*/
// Bit operations, dynamic bitpos: d0=reg, d1=mode2, d2=reg
int d_bitopsd(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];

	if (datas[1]==1)
		return 1;
	if (format_ea(pc, dest, datas[1], datas[2], 0))
		return 1;
	sprintf(str,"%s D%i,%s",cmd->mnem, datas[0],dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// Bit operations, static: mode reg
int d_bitopss(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	int num;
	if (datas[0]==1)
		return 1;
	num=get_word(pc)&31;
	if (format_ea(pc, dest, datas[0], datas[1], 0))
		return 1;
	sprintf(str,"%s #$%x,%s",cmd->mnem, num, dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU immediate, size 0, mode 1, reg 2
int d_immalu(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	int num;

	CHECK_SIZE(0);

	if (datas[1]==1)
		return 1;

	if (datas[0]!=2)
		num=get_word(pc);
	else
		num=get_long(pc);
	if (format_ea(pc, dest, datas[1], datas[2], 0))
		return 1;
/*		
	sprintf(str,"%s.%c #$",cmd->mnem, SIZECHAR(datas[0]));
		
	switch(SIZECHAR(datas[0]))
	{
		case 'B':	sprintf(str,"%02x",num); break;
		case 'W': sprintf(str,"%04x",num); break;
		case 'L': sprintf(str,"%08x",num); break;
	}
	
	sprintf(str,",%s",dest);
*/
	sprintf(str,"%s.%c #$%x,%s",cmd->mnem, SIZECHAR(datas[0]),num,dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// MOVEP: dreg, dir, size, areg
int d_movep(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	int displ;
	int sign=0;

	displ=get_word(pc);
	if (displ>32767)
	{
		displ=65536-displ;
		sign=1;
	}

	if (datas[1]==0)
		sprintf(str,"%s.%c %s%x(A%i),D%i", cmd->mnem,
			(datas[2]?'L':'W'),(sign?"-":""),displ,datas[3],datas[2]);
	else
		sprintf(str,"%s.%c D%i,%s%x(A%i)", cmd->mnem,
			(datas[2]?'L':'W'),datas[2],(sign?"-":""),displ,datas[3]);
		
	return 0;
}
/*------------------------------------------------------------------------*/
// Unknown...
int d_unknown(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	sprintf(str,"%s %s$%x",cmd->mnem, cmd->para, pc->opc);
	return 0;
}

/*------------------------------------------------------------------------*/
// MOVE.L: dreg, dmode smode, dreg
int d_movel(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	char src[128];
	
	if (format_ea(pc,src, datas[2],datas[3], 4))
		return 1;
	if (format_ea(pc,dest, datas[1],datas[0], 4))
		return 1;

	sprintf(str,"%s %s,%s",cmd->mnem, src,dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// MOVE.W: dreg, dmode smode, dreg
int d_movew(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	char src[128];
	
	if (format_ea(pc,src, datas[2],datas[3], (datas[1]==1)?2+8:2))
		return 1;
	if (format_ea(pc,dest, datas[1],datas[0], 2))
		return 1;

	sprintf(str,"%s %s,%s",cmd->mnem, src,dest);
	return 0;
}

/*------------------------------------------------------------------------*/
// MOVE.B: dreg, dmode smode, dreg
int d_moveb(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	char src[128];
	
	if (format_ea(pc,src, datas[2],datas[3], 1))
		return 1;
	if (format_ea(pc,dest, datas[1],datas[0], 1))
		return 1;

	sprintf(str,"%s %s,%s",cmd->mnem, src,dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// Default CMD assembly
int d_default(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	char templ[128];

	dest[0]=0;

	if (cmd->flags&F_AREG)
	{
		sprintf(dest,"A%i",datas[0]);
	}

	if (cmd->flags&F_DREG)
	{
		sprintf(dest,"D%i",datas[0]);
	}

	if (cmd->flags&F_EAW)
	{
		if (format_ea(pc,dest,datas[0],datas[1],2))
			return 1;
	}

	if (cmd->flags&F_IMD)
	{
		sprintf(dest,"#%x",datas[0]);
	}

	if (cmd->flags&F_IMB)
	{
		int num;
		num=get_word(pc)&255;
		sprintf(dest,"#%x",num);
	}

	if (cmd->flags&F_IMW)
	{
		int num;
		num=get_word(pc);
		sprintf(dest,"#%x",num);
	}

	if (cmd->flags&F_LINK)
	{
		int num;
		num=get_word(pc);
		sprintf(dest,"A%i,#%x",datas[0],num);
	}

	if (cmd->flags& F_2DREG)
	{
		sprintf(dest,"D%i,D%i",datas[0],datas[1]);
	}
	sprintf(templ,"%s %s",cmd->mnem, cmd->para);
	sprintf(str,templ,dest);

        if ((strcmp(cmd->mnem,"RTS")==0)||(strcmp(cmd->mnem,"RTE")==0))
                sprintf(str,"%s%s",str,newln);

	return 0;
}
/*------------------------------------------------------------------------*/
// MOVEM: dir, size, mode, reg
int d_movem(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	char reglist[128];
	int mask;
	mask=get_word(pc);

	if (format_ea(pc,dest,datas[2],datas[3],0))
		return 1;
	
	format_reglist(mask, datas[2]==4?1:0, reglist);

	if (datas[0]==0)
		sprintf(str,"%s.%c %s,%s",cmd->mnem, datas[1]?'L':'W',reglist,dest);
	else
		sprintf(str,"%s.%c %s,%s",cmd->mnem, datas[1]?'L':'W',dest,reglist);
	return 0;
}
/*------------------------------------------------------------------------*/
int d_jmp(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];

	if (format_ea(pc,dest,datas[0],datas[1],0))
		return 1;

	sprintf(str,"%s %s",cmd->mnem, dest);

        if (strcmp(cmd->mnem,"JMP")==0)
                sprintf(str,"%s%s",str,newln);

	return 0;
}
/*------------------------------------------------------------------------*/
// ALU Ops, one operand: size, mode, reg
int d_alu1(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];

	CHECK_SIZE(0);

	if (format_ea(pc,dest,datas[1],datas[2],SIZEINT(datas[0])))
		return 1;

		sprintf(str,"%s.%c %s",cmd->mnem, SIZECHAR(datas[0]), dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// LEA: areg, mode, reg
int d_lea(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	
	if (format_ea(pc,dest,datas[1],datas[2],0))
		return 1;

	sprintf(str,"%s %s,A%i",cmd->mnem, dest,datas[0]);

	return 0;
}
/*------------------------------------------------------------------------*/
// CHK: dreg, size, mode, reg
int d_chk(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	
	if (format_ea(pc,dest,datas[2],datas[3],SIZEINT(datas[1])))
		return 1;

	sprintf(str,"%s.%c %s,D%i",cmd->mnem, SIZECHAR(datas[2]),dest,datas[0]);

	return 0;
}
/*------------------------------------------------------------------------*/
// DBcc: cond, dreg 
int d_dbcc(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	int displ;
	int p;

	displ=get_displacement16(pc);
	p=pc->pc+pc->base+2+displ;
	format_pc(pc,p,"",dest);
	
	sprintf(str,"%s%s D%i,%s",cmd->mnem, condcodes_dbcc[datas[0]],
		datas[1],dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// SCC: cond, mode, dreg 
int d_scc(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	
	if (format_ea(pc,dest,datas[1],datas[2],0))
		return 1;

	sprintf(str,"%s%s %s",cmd->mnem, condcodes[datas[0]],dest);
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU A-REG: arge, size, mode, reg
int d_alua(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char ea[128];

	if (format_ea(pc,ea,datas[2],datas[3],datas[1]==0?(2+8):4))
		return 1;

	sprintf(str,"%s.%c %s,A%i",cmd->mnem,datas[1]==0?'W':'L', ea, datas[0]);
	return 0;
}
/*------------------------------------------------------------------------*/
// CMPM: areg, size, areg
int d_cmpm(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	if (datas[1]==3)
		return 1;

	sprintf(str,"%s.%c (A%i)+,(A%i)+",cmd->mnem,SIZECHAR(datas[1]),
		datas[2],datas[0]);
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU Quick, imm, size, mode, reg
int d_aluq(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];
	int imm=datas[0];

	// removed, rendered 56xx ADDQ useless. 
	// mode=110 is also 68k, not just 68020++
//	CHECK_SIZE(0);

	if (format_ea(pc,dest,datas[2],datas[3],SIZEINT(datas[1])))
		return 1;

	if (imm==0)
		imm=8;

	sprintf(str,"%s.%c #%i,%s",cmd->mnem, SIZECHAR(datas[1]),imm, dest);
	return 0;
}
/*------------------------------------------------------------------------*/
int d_bsr(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	int displ=datas[0];
	int p;
	char dest[128];

	if (!displ)
		displ=get_displacement16(pc);
	else if (displ==0xff)
		displ=get_displacement32(pc);
	else if (displ>127)
		displ=displ-256;
		
	p=pc->pc+pc->base+2+displ;
	format_pc(pc,p,"",dest);
	sprintf(str,"%s %s",cmd->mnem,dest);

        if (strcmp(cmd->mnem,"BRA")==0)
                sprintf(str,"%s%s",str,newln);

	return 0;
}
/*------------------------------------------------------------------------*/
// cc, displ
int d_bcc(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	int displ=datas[1];
	int p;
	char dest[128];

	if (!displ)
		displ=get_displacement16(pc);
	else if (displ>127)
		displ=displ-256;
		
	p=pc->pc+pc->base+2+displ;
	format_pc(pc,p,"",dest);
	sprintf(str,"%s%s %s",cmd->mnem,condcodes[datas[0]],dest);
	return 0;
}

/*------------------------------------------------------------------------*/
// MOVEQ: dreg, imm
int d_moveq(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	int imm=datas[1];
/*
	if (imm>127)
		sprintf(str,"%s #-$%x,D%i",cmd->mnem,256-imm,datas[0]);
	else
*/
		sprintf(str,"%s #$%x,D%i",cmd->mnem,imm,datas[0]);
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU-Ops: data, dir, size, mode, reg
int d_alu(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char ea[128];

	CHECK_SIZE(2);

	if (format_ea(pc,ea,datas[3],datas[4],SIZEINT(datas[2])))
		return 1;
	
	if (datas[1]==0)
		sprintf(str,"%s.%c %s,D%i",cmd->mnem,SIZECHAR(datas[2]),
			ea, datas[0]);
	else
		sprintf(str,"%s.%c D%i,%s",cmd->mnem,SIZECHAR(datas[2]),
			datas[0], ea);
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU-Ops with <ea>,D-destination: data, size, mode, reg
int d_alud(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char ea[128];
	
	CHECK_SIZE(1);

	if (format_ea(pc,ea,datas[2],datas[3],SIZEINT(datas[1])))
		return 1;
	
	sprintf(str,"%s.%c %s,D%i",cmd->mnem,SIZECHAR(datas[1]),
			ea, datas[0]);
	
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU-Ops with <ea>,D-destination, no size: data, mode, reg
int d_alus(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char ea[128];
	
	if (format_ea(pc,ea,datas[1],datas[2],2))
		return 1;
	
	sprintf(str,"%s %s,D%i",cmd->mnem,
			ea, datas[0]);
	
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU-Ops Xtended: reg, size, mode, reg
int d_alux(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	CHECK_SIZE(1);

	if (datas[2])
		sprintf(str,"%s.%c D%i,D%i", cmd->mnem, SIZECHAR(datas[1]),
			datas[3],datas[0]);
	else
		sprintf(str,"%s.%c -(A%i),-(A%i)", cmd->mnem, SIZECHAR(datas[1]),
			datas[3],datas[0]);
	return 0;
}
/*------------------------------------------------------------------------*/
// ALU-Ops BCD, reg, mode, reg
int d_alub(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	if (datas[1])
		sprintf(str,"%s D%i,D%i", cmd->mnem, datas[2],datas[0]);
	else
		sprintf(str,"%s -(A%i),-(A%i)", cmd->mnem, datas[2],datas[0]);
	return 0;
}
/*------------------------------------------------------------------------*/
// SHIFT-Ops D-reg: data/count, size, d/c, reg
int d_shiftd(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{

	CHECK_SIZE(1);

	if (datas[2]==0)
		sprintf(str,"%s.%c #%i,D%i",cmd->mnem, SIZECHAR(datas[1]),
			datas[0],datas[3]);
	else
		sprintf(str,"%s.%c D%i,D%i",cmd->mnem, SIZECHAR(datas[1]),
			datas[0],datas[3]);
	return 0;
}
/*------------------------------------------------------------------------*/
// SHIFT-Ops memory: mode, reg
int d_shiftm(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str)
{
	char dest[128];

	if (format_ea(pc, dest, datas[1], datas[2], 0))
		return 1;
	
	sprintf(str,"%s %s",cmd->mnem, dest);

	return 0;
}
