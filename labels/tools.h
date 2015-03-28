#ifndef ISNL
	#define isnl(i)	(((i)=='\n')||((i)=='\r')||((i)=='\f'))
	#define ISNL
#endif

int errout(char *str, int line, int mode);
void warning(char *str, int line);
void error(char *str, int line);
void despace(char **buf);
int getnum(char **buf);
int advance(char **ptr, int len, int line);
void read_line(FILE *fd, char **buf);
char *to_bin(int in, int size);