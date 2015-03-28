/* 
 *      dis68.h - defines and prototypes for dis68k
 *      (c) 2001 Georg Acher, georg@acher.org
 *      
 *      #include <gpl-stuff.h>
 *
 *     $Id: 68k_dis.h,v 1.1.1.1 2002/04/29 23:18:29 acher Exp $
 */

#define F_AREG 1
#define F_DREG 2
#define F_EAW 4
#define F_IMB 8
#define F_IMD 16
#define F_IMW 32
#define F_LINK 64
#define F_2DREG 128

typedef struct mc68cmd_s
{
	char *mask;
	char *mnem;
	char *para;
	int (*callback)(dispc_t *pc, struct mc68cmd_s *cmd, int datas[], char *str);
	int flags;

} mc68cmd_t;


#define SIZECHAR(x) ((x==0?'B':(x==1?'W':'L')))
#define SIZEINT(x) ((x==0?1:(x==1?2:4)))

// decode prototypes;
int d_bitopsd(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_bitopss(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_immalu(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_movep(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_unknown(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_movel(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_movew(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_moveb(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_default(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_movem(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_jmp(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alu1(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_lea(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_chk(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_dbcc(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_scc(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alua(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_cmpm(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_moveq(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_bsr(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_bcc(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_aluq(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alu(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alud(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alus(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alux(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_alub(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_shiftd(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);
int d_shiftm(dispc_t *pc, mc68cmd_t *cmd, int datas[], char *str);


// 68k_dis.cpp
void mc68k_show_vectors(dispc_t *pc);
mc68cmd_t* mc68k_search_cmd(dispc_t *pc, mc68cmd_t *cmds, char *str);
void mc68k_disass(dispc_t *pc, char* str);


// modes.cpp
int format_ea(dispc_t *pc, char *str, int mode, int reg, int size);
int format_reglist(int mask, int dir, char *str);
//int match_mask(char *mask, int val);
//int seperate_mask(char* mask, int val, int datas[8]);

