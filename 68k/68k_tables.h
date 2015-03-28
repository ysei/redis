/* 
 *      tables.h - command parsing tables for dis68
 *      (c) 2001 Georg Acher, georg@acher.org
 *      
 *      #include <gpl-stuff.h>
 *
 *     $Id: 68k_tables.h,v 1.1.1.1 2002/04/29 23:18:30 acher Exp $
 */

mc68cmd_t mc68cmds_0[]={
	{"000000111100","ORI", "%s,CCR", NULL, F_IMB},
	{"000001111100","ORI", "%s,SR",  NULL, F_IMW},

	{"001000111100","ANDI", "%s,CCR", NULL, F_IMB},
	{"001001111100","ANDI", "%s,SR",  NULL, F_IMW},

	{"101000111100","EORI", "%s,CCR", NULL, F_IMB},
	{"101001111100","EORI", "%s,SR",  NULL, F_IMW},

	{"100000aaabbb","BTST", "", d_bitopss},
	{"100001aaabbb","BCHG", "", d_bitopss},
	{"100010aaabbb","BCLR", "", d_bitopss},
	{"100011aaabbb","BSET", "", d_bitopss},

	{"0000aabbbccc","ORI", "",  d_immalu},
	{"0010aabbbccc","ANDI", "", d_immalu},
	{"0100aabbbccc","SUBI","", d_immalu},
	{"0110aabbbccc","ADDI", "", d_immalu},
	{"1010aabbbccc","EORI", "", d_immalu},
	{"1100aabbbccc","CMPI", "", d_immalu},

	{"aaa100bbbccc","BTST", "", d_bitopsd},
	{"aaa101bbbccc","BCHG", "", d_bitopsd},
	{"aaa110bbbccc","BCLR", "", d_bitopsd},
	{"aaa111bbbccc","BSET", "", d_bitopsd},

	{"aaa1bc001ddd","MOVEP", "", d_movep},      
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_1[]={
	{"aaabbbcccddd","MOVE.B","", d_moveb},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_2[]={
	{"aaabbbcccddd","MOVE.L","", d_movel},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_3[]={
	 {"aaabbbcccddd","MOVE.W","", d_movew},
	 {"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_4[]={

	{"111001110110","TRAPV","", NULL, 0},
	{"101011111100","ILLEGAL","", NULL, 0},
	{"111001110000","RESET","", NULL, 0},
	{"111001110001","NOP","", NULL, 0},
	{"111001110010","STOP","%s", NULL, F_IMW},
	{"111001110011","RTE","", NULL, 0},
	{"111001110101","RTS","", NULL, 0},
	{"111001110111","RTR","", NULL, 0},

	{"100001000aaa","SWAP","%s", NULL, F_DREG},
	{"100010000aaa","EXT.W","%s", NULL, F_DREG},
	{"100011000aaa","EXT.L","%s", NULL, F_DREG},
	{"111001010aaa","LINK","%s", NULL, F_LINK},
	{"111001100aaa","MOVE","%s,USP", NULL, F_AREG},
	{"111001101aaa","MOVE","USP,%s", NULL, F_AREG},
	{"111001011aaa","UNLK","%s", NULL, F_AREG},

	{"11100100aaaa","TRAP","%s", NULL, F_IMD},

	{"001011aaabbb","MOVE","CCR,%s", NULL, F_EAW},
	{"010011aaabbb","MOVE","%s,CCR", NULL, F_EAW},
	{"000011aaabbb","MOVE","SR,%s", NULL, F_EAW},
	{"011011aaabbb","MOVE","%s,SR", NULL, F_EAW},
	{"100000aaabbb","NBCD","%s", NULL, F_EAW},
	{"100001aaabbb","PEA", "%s", NULL, F_EAW},
	{"111010aaabbb","JSR","", d_jmp},
	{"101011aaabbb","TAS","%s", NULL, F_EAW },
	{"111011aaabbb","JMP","", d_jmp},


	{"0000aabbbccc","NEGX","", d_alu1},
	{"0010aabbbccc","CLR","", d_alu1},
	{"0100aabbbccc","NEG","", d_alu1},
	{"0110aabbbccc","NOT","", d_alu1},
	{"1010aabbbccc","TST","", d_alu1},

	{"1a001bcccddd","MOVEM","", d_movem},
	{"aaa111bbbccc","LEA","", d_lea},
	{"aaabb0cccddd","CHK","", d_chk},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
	
};

mc68cmd_t mc68cmds_5[]={
	{"aaaa11001bbb","DB","", d_dbcc}, //DBcc
	{"aaaa11bbbccc","S","", d_scc}, // Scc
	{"aaa0bbcccddd","ADDQ","", d_aluq},
	{"aaa1bbcccddd","SUBQ","", d_aluq},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_6[]={
	{"0000bbbbbbbb","BRA", "", d_bsr},
	{"0001aaaaaaaa","BSR", "", d_bsr},
	{"0000bbbbbbbb","BRA", "", d_bcc},
	{"aaaabbbbbbbb","B", "", d_bcc}, // Bcc
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_7[]={
	{"aaa0bbbbbbbb","MOVEQ","", d_moveq},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_8[]={

	{"aaa011bbbccc","DIVU","", d_alus},
	{"aaa10000bccc","SBCD","", d_alub},
	{"aaa111bbbccc","DIVS","", d_alus},
	{"aaabccdddeee","OR","", d_alu},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}

};

mc68cmd_t mc68cmds_9[]={
	{"aaab11cccddd","SUBA","",d_alua},
	{"aaa1bb00cddd","SUBX","", d_alux},
	{"aaabccdddeee","SUB","", d_alu},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_a[]={
	{"aaaaaaaaaaaa","LINE_A","#", d_unknown},
};

mc68cmd_t mc68cmds_b[]={
	{"aaa1bb001ccc","CMPM","", d_cmpm },
	{"aaab11cccddd","CMPA","", d_alua},
	{"aaa0bbcccddd","CMP","", d_alud},
	{"aaa1bbcccddd","EOR","", d_alu},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}	
};

mc68cmd_t mc68cmds_c[]={
	
	{"aaa10000bccc","ABCD","", d_alub},
	{"aaa110001bbb","EXG","%s,%s", NULL, F_2DREG},
	{"aaa111bbbccc","MULS","", d_alus},
	{"aaa011bbbccc","MULU","", d_alus},
	{"aaabccdddeee","AND","", d_alu},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
};

mc68cmd_t mc68cmds_d[]={
	{"aaa1bb00ceee","ADDX","", d_alux},
	{"aaab11cccddd","ADDA","", d_alua},
	{"aaabccdddeee","ADD","", d_alu},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}

};

mc68cmd_t mc68cmds_e[]={
	{"000011aaabbb","ASR", "", d_shiftm},
	{"000111aaabbb","ASL", "", d_shiftm},
	{"001111aaabbb","LSL", "", d_shiftm},
	{"001011aaabbb","LSR", "", d_shiftm},
	{"011011aaabbb","ROR", "", d_shiftm},
	{"011111aaabbb","ROL", "", d_shiftm},
	{"010111aaabbb","ROXL", "", d_shiftm},
	{"010011aaabbb","ROXR", "", d_shiftm},

	{"aaa0bbc00ddd","ASR", "", d_shiftd},
	{"aaa1bbc00ddd","ASL", "", d_shiftd},
	{"aaa0bbc01ddd","LSR", "", d_shiftd},
	{"aaa1bbc01ddd","LSL", "", d_shiftd},
	{"aaa0bbc11ddd","ROR", "", d_shiftd},
	{"aaa1bbc11ddd","ROL", "", d_shiftd},
	{"aaa1bbc10ddd","ROXL", "", d_shiftd},
	{"aaa0bbc10ddd","ROXR", "",d_shiftd},
	{"aaaaaaaaaaaa","DC.W", "", d_unknown}
	
};

mc68cmd_t mc68cmds_f[]={
	{"aaaaaaaaaaaa","LINE_F","#", d_unknown},
};


mc68cmd_t *mc68k_groups[16]={
	mc68cmds_0,
	mc68cmds_1,
	mc68cmds_2,
	mc68cmds_3,
	mc68cmds_4,
	mc68cmds_5,
	mc68cmds_6,
	mc68cmds_7,
	mc68cmds_8,
	mc68cmds_9,
	mc68cmds_a,
	mc68cmds_b,
	mc68cmds_c,
	mc68cmds_d,
	mc68cmds_e,
	mc68cmds_f};

char *condcodes[16]={"T", "F", "HI","LS","CC","CS","NE","EQ",
		   "VC","VS","PL","MI","GE","LT","GT","LE"};

char *condcodes_dbcc[16]={"T", "RA", "HI","LS","CC","CS","NE","EQ",
		   "VC","VS","PL","MI","GE","LT","GT","LE"};

char *mc68k_vectors[]={"Reset_SP","Reset_PC","Bus_Error","Address_Error",
		 "Illegal_Instruction","Division_by_Zero","CHK_Instruction",
		 "TRAPV_Instruction","Privilege_Violation",
		 "Trace_Trap","Line_A_Emulator","Line_F_Emulator",
		 "Vector_12","Vector_13","Vector_14","Uninitialized_Interrupt",
		 "Vector_16","Vector_17","Vector_18","Vector_19",
		 "Vector_20","Vector_21","Vector_22","Vector_23",
		 "Spurious_Interrupt","Auto_Interrupt_1","Auto_Interrupt_2",
		 "Auto_Interrupt_3","Auto_Interrupt_4","Auto_Interrupt_5",
		 "Auto_Interrupt_6","Auto_Interrupt_7",
		 "TRAP_0","TRAP_1","TRAP_2","TRAP_3",
		 "TRAP_4","TRAP_5","TRAP_6","TRAP_7",
		 "TRAP_8","TRAP_9","TRAP_10","TRAP_11",
		 "TRAP_12","TRAP_13","TRAP_14","TRAP_15",NULL};
