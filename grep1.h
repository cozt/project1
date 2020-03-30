#ifndef GREP1_H
#define GREP1_H
#define	BLKSIZE	4096
#define	NBLK	2047
#define	NULL	0
#define	FNSIZE	128
#define	LBSIZE	4096
#define	ESIZE	256
#define	GBSIZE	256
#define	NBRA	5
#define	EOF	-1
#define	KSIZE	9
#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14
#define	CCIRC	15
#define	STAR	01
char	Q[]	= "";
char	T[]	= "TMP";
#define	READ	0
#define	WRITE	1
int	peekc, lastc,given, ninbuf, io, pflag;
char	savedfile[FNSIZE], file[FNSIZE],linebuf[LBSIZE],rhsbuf[LBSIZE/2], expbuf[ESIZE+4], genbuf[LBSIZE];
unsigned int	*addr1, *addr2, *dot, *dol, *zero;
long	count;
char	*nextip, *linebp;
long	lseek(int, long, int);
int	open(char *, int);
int	creat(char *, int);
int	read(int, char*, int);
int	write(int, char*, int);
int	close(int);
int	fork(void);
int	execl(char *, ...);
int	exit(int);
int	wait(int *);
int	unlink(char *);
int	vflag	= 1, oflag, listf,listn,col,tfile=-1, tline,iblock	= -1,oblock	= -1;
char	*globp, *tfname, *loc1, *loc2;
char	ibuff[BLKSIZE];
char	obuff[BLKSIZE];
int	ichanged,nleft;
char	WRERR[]	= "WRITE ERROR";
int	names[26];
int	anymarks;
char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra;
int	subnewa;
int	subolda;
int	fchange;
int	wrapp;
int	bpagesize = 20;
unsigned nlall = 128;
char	*mktemp(char *);
char	tmpXXXXX[50] = "/tmp/eXXXXX";
char	*malloc(int);
/* these two are not in ansi, but we need them */
#define	SIGHUP	1	/* hangup */
#define	SIGQUIT	3	/* quit (ASCII FS) */
jmp_buf	savej;
typedef void	(*SIG_TYP)(int);
SIG_TYP	oldhup;
SIG_TYP	oldquit;
char	*realloc(char *, int);
char *getblock(unsigned int atl, int iof);
char *getline(unsigned int tl);
char *place(char *sp, char *l1, char *l2);
void add(int i);
int advance(char *lp, char *ep);
int append(int (*f)(void), unsigned int *a);
int backref(int i, char *lp);
void blkio(int b, char *buf, int (*iofcn)(int, char*, int));
void callunix(void);
int cclass(char *set, int c, int af);
void commands(void);
void compile(int eof);
int compsub(void);
void dosub(void);
void error(char *s);
int execute(unsigned int *addr);
void exfile(void);
void filename(int comm);
void gdelete(void);
int getchr(void);
int getcopy(void);
int getfile(void);
int getnum(void);
int getsub(void);
int gettty(void);
int gety(void);
void global(int k);
void init(void);
unsigned int *address(void);
void join(void);
void move(int cflag);
void newline(void);
void nonzero(void);
void onhup(int n);
void onintr(int n);
void print(void);
void putchr(int ac);
void putd(void);
void putfile(void);
int putline(void);
void puts(char *sp);
void quit(int n);
void rdelete(unsigned int *ad1, unsigned int *ad2);
void reverse(unsigned int *a1, unsigned int *a2);
void setwide(void);
void setnoaddr(void);
void squeeze(int i);
void substitute(int inglob);
#endif
