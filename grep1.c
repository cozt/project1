#include <signal.h>
#include <setjmp.h>
#include "grep1.h"
int main(int argc, char *argv[]) {
	char *p1, *p2; SIG_TYP oldintr; argv++;
	if (argc>1) {
		p1 = *argv;
		p2 = savedfile;
		while ((*p2++ = *p1++))
			if (p2 >= &savedfile[sizeof(savedfile)])
				p2--;
		globp = "r";
	}
	zero = (unsigned *)malloc(nlall*sizeof(unsigned));
	init();
	if (oldintr!=SIG_IGN)
		signal(SIGINT, onintr);
	if (oldhup!=SIG_IGN)
		signal(SIGHUP, onhup);
	setjmp(savej);
	commands();
	return 0;
}
void commands(void) {
	unsigned int *a1; int c, temp; char lastsep;
	for (;;) {
	if (pflag) { pflag = 0; addr1 = addr2 = dot; print(); }
	c = '\n';
	for (addr1 = 0;;) {
		lastsep = c;
		a1 = address();
		c = getchr();
		if (c!=',' && c!=';') break;
		if (lastsep==',') error(Q);
		if (a1==0) { a1 = zero+1; if (a1>dol) a1--; }
		addr1 = a1;
		if (c==';') dot = a1;
	}
	if (lastsep!='\n' && a1==0) a1 = dol;
	if ((addr2=a1)==0) { given = 0; addr2 = dot; }
	else given = 1;
	if (addr1==0) addr1 = addr2;
	switch(c) {
	case 'E': fchange = 0; c = 'e';
	case 'e': if (vflag && fchange) { fchange = 0; error(Q); } filename(c); init(); addr2 = zero; goto caseread;
	case 'g': global(1); continue;
	case 'p':
	case 'P': print(); continue; quit(0);
	case 'r': filename(c);
	caseread: if ((io = open(file, 0)) < 0) { lastc = '\n'; error(file); } setwide(); ninbuf = 0; c = zero != dol; append(getfile, addr2); fchange = c; continue;
	case EOF: return;
	}
	error(Q);
	}
}
void print(void) {
	unsigned int *a1;
	a1 = addr1;
	do {
		if (listn) {
			count = a1-zero;
			putchr('\t');
		}
		puts(getline(*a1++));
	} while (a1 <= addr2);
	dot = addr2;
	listf = 0;
	listn = 0;
	pflag = 0;
}
unsigned int *address(void) {
	int sign=1;
	unsigned int *a, *b;
	int opcnt=0, nextopand=-1, c;
	a = dot;
	do {
		do c = getchr(); while (c==' ' || c=='\t');
		if ('0'<=c && c<='9') {
			peekc = c;
			if (!opcnt)
				a = zero;
			a += sign*getnum();
		} else switch (c) {
			a = zero;
			do a++; while (a<=dol && names[c-'a']!=(*a&~01));
			break;
			break;
		default:
			if (nextopand == opcnt) {
				a += sign;
				if (a<zero || dol<a) continue;       /* error(Q); */
			}
			if (c!='+' && c!='-' && c!='^') {
				peekc = c;
				if (opcnt==0)	a = 0;
				return (a);
			}
			sign = 1;
			if (c!='+') sign = -sign;
			nextopand = ++opcnt;
			continue;
		}
		sign = 1;
		opcnt++;
	} while (zero<=a && a<=dol);
	error(Q);
	return 0;
}

int getnum(void) {
	int r=0, c;
	while ((c=getchr())>='0' && c<='9')	r = r*10 + c - '0';
	peekc = c;
	return (r);
}

void setwide(void) {
	if (!given) {
		addr1 = zero + (dol>zero);
		addr2 = dol;
	}
}
void filename(int comm) {
	char *p1, *p2;
	int c= getchr();
	count = 0;
	if (c=='\n' || c==EOF) {
		p1 = savedfile;
		if (*p1==0 && comm!='f') error(Q);
		p2 = file;
		while ((*p2++ = *p1++));
		return;
	}
	if (c!=' ') error(Q);
	while ((c = getchr()) == ' ');
	if (c=='\n') error(Q);
	p1 = file;
	do {
		if (p1 >= &file[sizeof(file)-1] || c==' ' || c==EOF) error(Q);
		*p1++ = c;
	} while ((c = getchr()) != '\n');
	*p1++ = 0;
	if (savedfile[0]==0 || comm=='e' || comm=='f') {
		p1 = savedfile;
		p2 = file;
		while ((*p1++ = *p2++));
	}
}
void onintr(int n) {}
void onhup(int n) {}
void error(char *s) {}
int getchr(void) {
	char c;
	if ((lastc=peekc)) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0)
			return(lastc);
		globp = 0;
		return(EOF);
	}
	if (read(0, &c, 1) <= 0)
		return(lastc = EOF);
	lastc = c&0177;
	return(lastc);
}
int getfile(void) {
	int c;
	char *lp= linebuf, *fp=nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0){
				if (lp>linebuf) {*genbuf = '\n';}
				else {return(EOF);}
			}
			fp = genbuf;
			while(fp < &genbuf[ninbuf]) {
				if (*fp++ & 0200) break;
			}
			fp = genbuf;
		}
		c = *fp++;
		if (c=='\0') continue;
		if (c&0200 || lp >= &linebuf[LBSIZE]) {
			lastc = '\n';
			error(Q);
		}
		*lp++ = c;
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}

int append(int (*f)(void), unsigned int *a) {
	unsigned int *a1, *a2, *rdot;
	int nline=0, tl;
	dot = a;
	while ((*f)() == 0) {
		if ((dol-zero)+1 >= nlall) {
			unsigned *ozero = zero;
			nlall += 1024;
			dot += zero - ozero;
			dol += zero - ozero;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot)
			*--a2 = *--a1;
		*rdot = tl;
	}
	return(nline);
}
char *getline(unsigned int tl) {
	char *bp, *lp;
	int nl;
	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while ((*lp++ = *bp++))
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), READ);
			nl = nleft;
		}
	return(linebuf);
}

int putline(void) {
	char *bp, *lp;
	int nl;
	unsigned int tl;
	fchange = 1;
	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while ((*bp = *lp++)) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}
char *getblock(unsigned int atl, int iof) {
	int bno, off;
	bno = (atl/(BLKSIZE/2));
	off = (atl<<1) & (BLKSIZE-1) & ~03;
	nleft = BLKSIZE - off;
	if (bno==iblock) {
		ichanged |= iof;
		return(ibuff+off);
	}
	if (bno==oblock) return(obuff+off);
	oblock = bno;
	return(obuff+off);
}

void init(void) {
	int *markp;
	close(tfile);
	tline = 2;
	for (markp = names; markp < &names[26]; ) *markp++ = 0;
	subnewa = 0;
	anymarks = 0;
	iblock = -1;
	oblock = -1;
	ichanged = 0;
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	dot = dol = zero;
}
void global(int k) {
	char *gp;
	int c;
	unsigned int *a1;
	char globuf[GBSIZE];
	setwide();
	if ((c=getchr())=='\n') error(Q);
	compile(c);
	gp = globuf;
	while ((c = getchr()) != '\n') {
		if (c=='\\') {
			c = getchr();
			if (c!='\n')  *gp++ = '\\';
		}
		*gp++ = c;
	}
	if (gp == globuf) *gp++ = 'p';
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k) *a1 |= 01;
	}
	for (a1=zero; a1<=dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands();
			a1 = zero;
		}
	}
}
void compile(int eof) {
	int c;
	char *ep= expbuf;
	char *lastep;
	char bracket[NBRA], *bracketp;
	int cclcnt;
	bracketp = bracket;
	if ((c = getchr()) == '\n') {
		peekc = c;
		c = eof;
	}
	if (c == eof) {
		if (*ep==0) error(Q);
		return;
	}
	nbra = 0;
	if (c=='^') {
		c = getchr();
		*ep++ = CCIRC;
	}
	peekc = c;
	lastep = 0;
	for (;;) {
		c = getchr();
		if (c == '\n') {
			peekc = c;
			c = eof;
		}
		if (c==eof) {
			*ep++ = CEOF;
			return;
		}
		if (c!='*')
			lastep = ep;
		switch (c) {
		case '\\':
			if ((c = getchr())=='(') {
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			}
			if (c == ')') {
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			if (c>='1' && c<'1'+NBRA) {
				*ep++ = CBACK;
				*ep++ = c-'1';
				continue;
			}
			*ep++ = CCHR;
			*ep++ = c;
			continue;
		case '.':
			*ep++ = CDOT;
			continue;
		case '*':
			*lastep |= STAR;
			continue;
		case '$':
			*ep++ = CDOL;
			continue;
		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c=getchr()) == '^') {
				c = getchr();
				ep[-2] = NCCL;
			}
			do {
				if (c=='-' && ep[-1]!=0) {
					if ((c=getchr())==']') {
						*ep++ = '-';
						cclcnt++;
						break;
					}
					while (ep[-1]<c) {
						*ep = ep[-1]+1;
						ep++;
						cclcnt++;
					}
				}
				*ep++ = c;
				cclcnt++;
			} while ((c = getchr()) != ']');
			lastep[1] = cclcnt;
			continue;
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
}
int execute(unsigned int *addr) {
	char *p1, *p2=expbuf;
	int c;
	for (c=0; c<NBRA; c++) { braslist[c] = 0; braelist[c] = 0; }
if (addr == (unsigned *)0) { if (*p2==CCIRC) return(0);	p1 = loc2;}
else if (addr==zero) return(0); else p1 = getline(*addr);
if (*p2==CCIRC) { loc1 = p1;return(advance(p1, p2+1)); }
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1!=c) continue;
			if (advance(p1, p2)) {
				loc1 = p1;
				return(1);
			}
		} while (*p1++);
		return(0);
	}
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return(1);
		}
	} while (*p1++);
	return(0);
}
int advance(char *lp, char *ep) {
	char *curlp;
	int i;
	for (;;) switch (*ep++) {
	case CCHR:
		if (*ep++ == *lp++) continue; return(0);
	case CDOT: if (*lp++) continue; return(0);
	case CDOL: if (*lp==0) continue; return(0);
	case CEOF:	loc2 = lp; return(1);
	case CCL:
		if (cclass(ep, *lp++, 1)) { ep += *ep; continue; }
		return(0);
	case NCCL:
		if (cclass(ep, *lp++, 0)) { ep += *ep; continue; }
		return(0);
	case CBRA:	braslist[*ep++] = lp; continue;
	case CKET: braelist[*ep++] = lp; continue;
	case CBACK: if (braelist[i = *ep++]==0) error(Q); return(0);
	case CBACK|STAR: if (braelist[i = *ep++] == 0)	error(Q); curlp = lp;
	while (lp >= curlp) { if (advance(lp, ep)) return(1); lp -= braelist[i] - braslist[i]; } continue;
	case CDOT|STAR: curlp = lp;
	while (*lp++);
	case CCHR|STAR: curlp = lp;
	while (*lp++ == *ep);
	ep++;
	case CCL|STAR:
	case NCCL|STAR: curlp = lp; while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)));
	ep += *ep; default: error(Q);
	}
}
int cclass(char *set, int c, int af) { int n; if (c==0) return(0); n = *set++; while (--n) if (*set++ == c) return(af);
return(!af);
}
void puts(char *sp) {	col = 0; while (*sp) putchr(*sp++); 	putchr('\n'); }
char	line[70]; char	*linp	= line;
void putchr(int ac) { char *lp=linp; int c =ac; *lp++ = c;
if(c == '\n' || lp >= &line[64]) {	linp = line; write(oflag?2:1, line, lp-line);	return;}
linp = lp;
}
