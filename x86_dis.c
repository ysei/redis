void x86_memmod(mc68pc_t *pc, int mem, int mod, char *str) 
{
	char disp[32];

	if ((mem==6)&&(mod==0))
		sprintf(disp,"%04x",get_word(pc));

	else {
		if(mem<4)
			sprintf(disp,"B%c+",((mem>>1)&1)?'P':'X');
		else if((mem==6)&&(mem==7))
			sprintf(disp,"B",((mem>>1)&1)?'P':'X');

		if(mod==1)
			sprintf(disp,"%s+%08x",disp,get_byte(pc));
		else if(mod==2)
			sprintf(disp,"%s+%04x",disp,get_word(pc));
	}

	str=strdup(disp);
}


