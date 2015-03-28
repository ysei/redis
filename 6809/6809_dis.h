typedef struct
{
	int opc;
	int page;
	int size;
	char *hex;
	char *cmd;
} mc6809_opc_t;

typedef struct
{
	int opc;
	char *name;
	int bytes;
	void (*callback)(dispc_t *pc, mc6809_opc_t *opc, int mode);
} mc6809_cmd_t;

void mc6809_mnemonics(mc6809_opc_t *opc, char *data, int mode);
void mc6809_ill(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_imp(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_imm(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_xfr(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_dir(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_ext(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_rel(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_idx(dispc_t *pc, mc6809_opc_t *opc, int mode);
void mc6809_stk(dispc_t *pc, mc6809_opc_t *opc, int mode);

void mc6809_disass(dispc_t *pc, char *str, int mode);
void mc6809_show_vectors(dispc_t *pc, int mode);
