/* 
 *      68k_dis.cpp - Core functions for 68k disassembly
 *      (c) 2001 Georg Acher, georg@acher.org
 *
 *      #include <gpl-stuff.h>
 *
 *     $Id: 68k_dis.cpp,v 1.1.1.1 2002/04/29 23:18:29 acher Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../discore.h"

#include "68k_dis.h"
#include "68k_tables.h"

//extern mc68cmd_t *mc68k_groups[16];
//extern char *mc68k_vectors[];

/*------------------------------------------------------------------------*/
// prints vector table and inserts vectors automatically in label list.
void mc68k_show_vectors(dispc_t *pc)
{
	int i=0;
	int p,po,px;
	char str[128];

	po=pc->pc;
	pc->pc=pc->vector_table;

	// 68k vector table
	while(mc68k_vectors[i])
	{
		px=pc->pc+pc->base+pc->len;
		p=get_long(pc);
		format_pc(pc,p,"",str);
		printf("%08x\t\t\t\tDC.L %s\t; %s\n",
			       px,str,mc68k_vectors[i]);
			store_label(mc68k_vectors[i],p,p,0,'l',0,0);
		i++;
	}
	pc->pc=po;
}

/*------------------------------------------------------------------------*/
mc68cmd_t *mc68k_search_cmd(dispc_t *pc, mc68cmd_t *cmds, char *str)
{
	int x;
	x=pc->opc&0xfff;

	while(1)
	{
//		printf("sc: %s %x\n",cmds->mask,pc->opc);
		if (match_mask(cmds->mask, 11, x))
		    break;
		    cmds++;
	}       		

	return cmds;
}

/*------------------------------------------------------------------------*/
/* find decoder for opcode pattern, continue search if pattern
   didn't match exactly in decoder due to prohibited sizes/modes
*/
void mc68k_disass(dispc_t *pc, char* str)
{
	int opc;
	int group;
	mc68cmd_t *cmd;
	int datas[8];
	int ret;

	pc->len=0;
	opc=get_word(pc);

	group=BEX(opc,12,4);

	pc->opc=opc;
	cmd=mc68k_groups[group];

	do 
	{
		cmd=mc68k_search_cmd(pc, cmd, str);
		
		seperate_mask(cmd->mask, 11,  opc&0xfff, datas);
		if (cmd->callback)
			ret=cmd->callback(pc,cmd,datas,str);
		else
			ret=d_default(pc,cmd,datas,str);
		if (ret)
		{
			cmd++; // retry, opc didn't match exactly
			pc->len=2;
		}

	} while(ret);
}
