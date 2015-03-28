/*
 * 6809 disassembler module
 * (c) 2001 by Rainer Buchty, rainer@buchty.net
 * 
 * #include<gpl-stuff.h>
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../discore.h"
#include "6809_dis.h"
#include "6809_tables.h"
#include "6309_tables.h"

//extern char *mc6809_vectors[];

void mc6809_show_vectors(dispc_t *pc, int mode) 
{
	int i, p;
	label_t *l;
	char str[128];

	for(i=0;i<16;i+=2) {
		p=get_word_n(pc,i+((pc->total_len-1)&0xfff0));
		l=find_label(0xfff0+i);
		if (l)
			sprintf(str,"%s",l->name);
		else
			sprintf(str,"$%04x",p);

		// 6809
		if(mode==0)
		{
			if (pc->pc+pc->base>0xffef) {
				if (i==0)
					printf("\n; 6809 Hardware Vectors\n");
				printf("%04x\t%02x %02x\t\tDC.W\t%s\t; %s\n",0xfff0+i,p>>8,p&0xff,str,mc6809_vectors[i>>1]);
			}

			store_label(mc6809_vectors[i>>1],p,p,0,'l',0,0);
		} 
		
		// 6309
		else 
		{
			if (pc->pc+pc->base>0xffef) {
				if (i==0)
					printf("\n; 6309 Hardware Vectors\n");
				printf("%04x\t%02x %02x\t\tDC.W\t%s\t; %s\n",0xfff0+i,p>>8,p&0xff,str,mc6309_vectors[i>>1]);
			}

			store_label(mc6309_vectors[i>>1],p,p,0,'l',0,0);
		}
	}
}

void mc6809_mnemonics(mc6809_opc_t *opc, char *data, int mode) {
	mc6809_cmd_t *cmd;
	char disp[256];
	char newln[]="\n-----------------------------------------------------------------------------";

	// 6809 or 6309?
	if(mode==0)
		cmd=mc6809_groups[opc->page];
	else
		cmd=mc6309_groups[opc->page];
		
	while(cmd->opc!=opc->opc)
		cmd++;

	if (data!=0)
		sprintf(disp,"%s\t%s",cmd->name,data);
	else
		strcpy(disp,cmd->name);

	switch(opc->opc) {
	case 0x0e:
	case 0x16:
	case 0x20:
	case 0x39:
	case 0x3c:
	case 0x6e:
	case 0x7e:
		if (opc->page!=0)
			break;
	case 0x3f:
		sprintf(disp,"%s%s",disp,newln);
		break;
	}

	opc->cmd=strdup(disp);
}

void mc6809_ill(dispc_t *pc, mc6809_opc_t *opc, int mode) 
{
	opc->cmd=strdup("????");
	pc->len=1;
}			

void mc6809_imp(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	mc6809_mnemonics(opc,0,mode);
	pc->len=1;
}

void mc6809_xfr(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	char *regs[]={	
		"D","X","Y", "U", "S","PC","","",
		"A","B","CC","DP","", "",  "",""
	};

	char disp[16];
	int byte;

	byte=get_byte(pc);

	if ( (strlen(regs[byte>>4])==0)||(strlen(regs[byte&15])==0) )
		mc6809_ill(pc,opc,mode);

	sprintf(disp,"%s,%s",regs[byte>>4],regs[byte&15]);
	mc6809_mnemonics(opc,disp,mode);

	pc->len=2;
}

void mc6809_dir(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	char disp[16];
	int byte;

	byte=get_byte(pc);

	sprintf(disp,"$%02x",byte);
	mc6809_mnemonics(opc,disp,mode);

	pc->len=2;
}

void mc6809_ext(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	char disp[16];
	int byte;

	byte=get_word(pc);

	sprintf(disp,"$%04x",byte);
	mc6809_mnemonics(opc,disp,mode);

	pc->len=3;
}

void mc6809_imm(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	char disp[16];
	int byte;

	if (opc->size==1) {
		byte=get_byte(pc);
	} else
		byte=get_word(pc);

	if (opc->size==1)
		sprintf(disp,"#$%02x",byte);
	else
		sprintf(disp,"#$%04x",byte);

	mc6809_mnemonics(opc,disp,mode);

	pc->len=1+opc->size;
}

void mc6809_rel(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	char dummy[16];
	int disp;

	if (opc->size==1)
		disp=get_displacement8(pc);
	else
		disp=get_displacement16(pc);

	if (opc->size==1)
		sprintf(dummy,"$%04x",pc->pc+pc->base+disp+1);
	else
		sprintf(dummy,"$%04x",pc->pc+pc->base+disp+1);
	mc6809_mnemonics(opc,dummy,mode);

	pc->len=1+opc->size;
}

void mc6809_idx(dispc_t *pc, mc6809_opc_t *opc,int mode)
{
	char *regs[]={"X","Y","U","S"};
	char disp[16], *dummy;
		
	int pbyte, reg, ill;

	pbyte=get_byte(pc);
	reg=(pbyte>>5)&3;
	pc->len=2;

	disp[0]=0; ill=0;

	// 5bit offset
	if ((pbyte>>7)==0) {
			pc->pc-=2;
			pbyte=get_displacement5(pc);
			sprintf(disp,"%c$%02x,%s",((pbyte>>4)?'-':'+'),pbyte,regs[reg]);
			pc->pc+=2;
			pc->len=2;
	}

	// everything else...
	else {
		switch(pbyte&0x1f) {
		case 0x00:	// postinc/byte
			sprintf(disp,",%s+",regs[reg]);
			if (pbyte&0x10)
				ill=1;
			break;
		case 0x01:	// postinc/word
			sprintf(disp,",%s++",regs[reg]);
			break;
		case 0x02:	// predec/byte
			sprintf(disp,",-%s",regs[reg]);
			if (pbyte&0x10)
				ill=1;
			break;
		case 0x03:	// predec/word
			sprintf(disp,",--%s",regs[reg]);
			break;
		case 0x04:	// no offset
			sprintf(disp,",%s",regs[reg]);
			break;
		case 0x05:	// B,R
			sprintf(disp,"B,%s",regs[reg]);
			break;
		case 0x06:	// A,R
			sprintf(disp,"A,%s",regs[reg]);
			break;
		case 0x07:	// E,R (6309-only)
			sprintf(disp,"E,%s",regs[reg]);
			if(mode!=1)
				ill=1;
			break;
		case 0x08:	// byte,R
			pc->pc++;
			sprintf(disp,"$%02x,%s",get_displacement8(pc),regs[reg]);
			pc->pc--;
			pc->len=3;
			break;
		case 0x09:	// word,R
			pc->pc++;
			sprintf(disp,"$%04x,%s",get_displacement16(pc),regs[reg]);
			pc->pc--;
			pc->len=4;
			break;
		case 0x0a:	// F,R (6309-only)
			sprintf(disp,"F,%s",regs[reg]);
			if(mode!=1)
				ill=1;
			break;
		case 0x0b:	// D,R
			sprintf(disp,"D,%s",regs[reg]);
			break;
		case 0x0c:	// byte,PC
			pc->pc++;
			sprintf(disp,"$%02x,PC",get_displacement8(pc));
			pc->pc-=1;
			pc->len=3;
			break;
		case 0x0d:	// word,PC
			pc->pc++;
			sprintf(disp,"$%04x,PC",get_displacement16(pc));
			pc->pc--;
			pc->len=4;
			break;
		case 0x0e:	// W,R (6309-only)
			sprintf(disp,"W,%s",regs[reg]);
			if(mode!=1)
				ill=1;
			break;
		case 0x0f:	// [word]
			if ((!pbyte&0x10)||((pbyte&0x10)&&(!pbyte&0x60)))
				ill=1;
			else {
				pc->pc++;
				sprintf(disp,"%04x",get_displacement16(pc));
				pc->pc--;
				pc->len=4;
			}
			break;
		default:
			ill=1;
		}

		// bit 4 set -> indirect addressing
		if (pbyte&0x10)
			sprintf(disp,"[%s]",disp);
	}

	dummy=strdup(disp);

	if (ill)
		mc6809_ill(pc,opc,mode);
	else
		mc6809_mnemonics(opc,disp,mode);

}

void mc6809_stk(dispc_t *pc, mc6809_opc_t *opc, int mode)
{
	char newln[]="\n-----------------------------------------------------------------------------";

	char *regs[]={
		"PC","U","X","Y","DP","B","A","CC",
		"PC","S","X","Y","DP","B","A","CC"
	};

	char disp[64];
	int pbyte, i;

	pbyte=get_byte(pc);

	disp[0]=0;
	for(i=7;i>=0;i--) {
		if ((1<<i)&pbyte) {
			if (strlen(disp)>0)
				sprintf(disp,"%s,",disp);
			if (((opc->opc)&3)<5)
				sprintf(disp,"%s%s",disp,regs[7-i]);
			else
				sprintf(disp,"%s%s",disp,regs[8+7-i]);
		}
	}

	if (pbyte&0x80)
		sprintf(disp,"%s%s",disp,newln);

	mc6809_mnemonics(opc,disp,mode);			
	pc->len=2;
}

void mc6809_disass(dispc_t *pc, char *str, int mode) 
{
	int len[]={254,38,9};

	int byte, page, cnt;
	mc6809_opc_t opc;
	mc6809_cmd_t *cmd;

	pc->len=0;

	byte=get_word(pc);
	sprintf(str,"%02x",byte>>8);
	pc->pc++;

	switch(byte>>8) {
	case 0x10:
	case 0x11:
		page=1+((byte>>8)&1);
		byte&=0xff;
		sprintf(str,"%s %02x",str,byte);
		pc->pc++;
		break;
	default:
		byte>>=8;
		page=0;
	}

	// 6809 vs. 6309
	if(mode==0)
		cmd=mc6809_groups[page];
	else
		cmd=mc6309_groups[page];

	cnt=0; opc.page=0xff;
	while(cnt<len[page]) {
		cnt++;

		if (byte==cmd->opc) {
			opc.opc=byte;
			opc.page=page;
			opc.size=cmd->bytes;

/*
			printf("imp %p\till %p\trel %p\timm %p\n",mc6809_imp,mc6809_ill,mc6809_rel,mc6809_imm);
			printf("dir %p\text %p\tidx %p\tstk %p\n",mc6809_dir,mc6809_ext,mc6809_idx,mc6809_stk);

			printf("calling %p on page %d with %02x\n",cmd->callback,page,opc.opc);
*/
			pc->len=0;

			cmd->callback(pc,&opc,mode);

			break;
		} else
			cmd++;			
	}

	if (page==0)
		pc->pc--;
	else {
		pc->pc-=2;	
		if (opc.page!=0xff)
			pc->len++;
		else
			opc.cmd=strdup("????");
	}

	strcpy(str,opc.cmd);	
}
