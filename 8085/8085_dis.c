/*
 *      8085_dis.cpp - Core functions for 8085/Z80 disassembly
 *      (c) 2002 Georg Acher, georg@acher.org
 *
 *      #include <gpl-stuff.h>
 *
 *     $Id: 8085_dis.cpp,v 1.2 2002/05/06 09:40:57 acher Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../discore.h"

#include "8085_tables.h"

extern int little_endian;
/*------------------------------------------------------------------------*/
int i8085_decode(dispc_t *pc, i8085cmds_t *cmd, int datas[8], char *str)
{
	char dest[128],dest1[128];
	int i,p;
	int flags=cmd->flags;

	if (flags==0)
		sprintf(str,"%s %s",cmd->mnem, cmd->para);
	else if (flags==I8085_REG1)
	{
		sprintf(dest,cmd->para, regnum2str[datas[0]]);
		sprintf(str,"%s %s",cmd->mnem, dest);
	}
	else if (flags==I8085_REG2)
	{
		sprintf(dest,cmd->para, regnum2str[datas[0]],regnum2str[datas[1]]);
		sprintf(str,"%s %s",cmd->mnem, dest);
	}
	else if (flags==I8085_I8)
	{
		i=get_byte(pc);
		format_constant(pc, i, 1, dest);
		sprintf(dest1, cmd->para,  dest);
		sprintf(str,"%s %s",cmd->mnem, dest1);
	}
	else if (flags==(I8085_I8|I8085_REG1))
	{
		i=get_byte(pc);
		format_constant(pc, i, 1, dest);
		sprintf(dest1, cmd->para, regnum2str[datas[0]], dest);
		sprintf(str,"%s %s",cmd->mnem, dest1);
	}
	else if (flags==I8085_I16)
	{
		i=get_word(pc);
		format_constant(pc, i, 2, dest);
		sprintf(dest1, cmd->para, dest);
		sprintf(str,"%s %s",cmd->mnem, dest1);
	}
	else if (flags==I8085_REGPAIR)
	{
		sprintf(dest,cmd->para, regpair2str1[datas[0]]);
		sprintf(str,"%s %s",cmd->mnem, dest);
	}
	else if (flags==I8085_REGPAIR2)
	{
		sprintf(dest,cmd->para, regpair2str2[datas[0]]);
		sprintf(str,"%s %s",cmd->mnem, dest);
	}
	else if (flags==(I8085_REL8|I8085_CC))
	{
		i=get_displacement8(pc);
		p=pc->pc+pc->base+3+i;
		format_pc(pc, p, "", dest);
		sprintf(dest1, cmd->para, cc2str[datas[0]], dest);
		sprintf(str,"%s %s", cmd->mnem, dest1);
	}
	else if (flags==(I8085_I16|I8085_CC))
	{
		i=get_displacement16(pc);
		
		format_pc(pc, i, "", dest);
		sprintf(dest1, cmd->para, cc2str[datas[0]], dest);
		sprintf(str,"%s %s", cmd->mnem, dest1);
	}
	else if (flags==I8085_CC)
	{
		sprintf(str,"%s %s", cmd->mnem, cc2str[datas[0]], dest);
	}
	else if (flags==I8085_RST)
	{
		i=datas[0]*8;
		format_pc(pc, i, "", dest);
		sprintf(str,"%s %s", cmd->mnem, dest);
	}
	else
	{
		printf("########### %x\n", flags);
	}
	return 0;
}
/*------------------------------------------------------------------------*/
i8085cmds_t *i8085_search_cmd(dispc_t *pc, i8085cmds_t *cmds, char *str)
{
	int x;
	x=pc->opc;

	while(1)
	{
//		printf("sc: %s %x\n",cmds->mask,pc->opc);
		if (match_mask(cmds->mask, 7, x))
		    break;
		cmds++;
	}       		

	return cmds;
}

/*------------------------------------------------------------------------*/
void i8085_disass(dispc_t *pc, char* str)
{
	int opc;
	i8085cmds_t *cmd;
	int datas[8];
        int ret;

	pc->len=0;
	little_endian=1;
	cmd=i8085cmds;
	pc->opc=get_byte(pc);
	do
	{
		
		cmd=i8085_search_cmd(pc, cmd, str);		

		seperate_mask(cmd->mask, 7,  pc->opc, datas);

		ret=i8085_decode(pc, cmd, datas, str);
                if (ret)
                {
                        cmd++; // retry, opc didn't match exactly
                }

        } while(ret);
}
