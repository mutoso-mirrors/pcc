# if 0
static char *sccsid ="@(#)local2.c	1.39 (Berkeley) 5/11/88";
# endif

# include "pass2.h"
# include <ctype.h>

# define putstr(s)	fputs((s), stdout)

#if 0
# ifdef FORT
int ftlab1, ftlab2;
# endif
/* a lot of the machine dependent parts of the second pass */

# define BITMASK(n) ((1L<<n)-1)

void stasg(NODE *p);
#endif
void sconv(NODE *p, int);
void acon(NODE *p);
int argsize(NODE *p);
void genargs(NODE *p);
#if 0
int collapsible(NODE *, NODE *);
int degenerate(NODE *p);

#endif


void
lineid(int l, char *fn)
{
	/* identify line l and file fn */
	printf("#	line %d, file %s\n", l, fn);
}

/*
 * End of block.
 */
void
eobl2()
{
	register OFFSZ spoff;	/* offset from stack pointer */
	extern int ftlab1, ftlab2;
	extern  int retlab;	/* From pass 1 */

	spoff = maxoff;
	if (spoff >= AUTOINIT)
		spoff -= AUTOINIT;
	spoff /= SZINT;
	/* return from function code */
	printf("L%d:\n", retlab);
	printf("	move 017,016\n");
	printf("	pop 017,016\n");
	printf("	popj 017,\n");

	/* Prolog code */
	printf("L%d:\n", ftlab1);
	printf("	push 017,016\n");
	printf("	move 016,017\n");
	if (spoff)
		printf("	addi 17,%llo\n", spoff);
	printf("	jrst L%d\n", ftlab2);
}

#if 0

struct hoptab { int opmask; char * opstring; } ioptab[] = {

	{ PLUS,	"add", },
	{ MINUS,	"sub", },
	{ MUL,	"mul", },
	{ DIV,	"div", },
	{ OR,	"bis", },
	{ ER,	"xor", },
	{ AND,	"bic", },
	{ -1, "" },
};
#endif

/*
 * add/sub/...
 *
 * Param given:
 *	R - Register
 *	M - Memory
 *	C - Constant
 */
void
hopcode(int f, int o)
{
	char *str;
	char *mod = "";

	if (asgop(o))
		o = NOASG o;
	switch (f) {
	case 'R':
		break;
	case 'M':
		mod = "m";
		break;
	case 'C':
		mod = "i";
		break;
	default:
		cerror("hopcode %c", f);
	}

	switch (o) {
	case PLUS:
		str = "add";
		break;
	case MINUS:
		str = "sub";
		break;
	case AND:
		str = "and";
		break;
	case OR:
		str = "ior";
		break;
	case LS:
		str = "lsh";
		mod = "";
		break;
	default:
		cerror("hopcode2: %d", o);
	}
	printf("%s%s", str, mod);
#if 0
	struct hoptab *q;

	if(asgop(o))
		o = NOASG o;
	for( q = ioptab;  q->opmask>=0; ++q ){
		if( q->opmask == o ){
			printf( "%s%c", q->opstring, tolower(f));
			return;
			}
		}
	cerror( "no hoptab for %s", opst[o] );
#endif
}

char *
rnames[] = {  /* keyed to register number tokens */
	"0", "01", "02", "03", "04", "05", "06", "07",
	"010", "011", "012", "013", "014", "015", "016", "017",
};

int rstatus[] = {
	0, SAREG|STAREG, SAREG|STAREG, SAREG|STAREG,
	SAREG|STAREG, SAREG|STAREG, SAREG|STAREG, SAREG|STAREG,
	SAREG, SAREG, SAREG, SAREG, SAREG, SAREG, SAREG, SAREG,
};

int
tlen(p) NODE *p;
{
	switch(p->in.type) {
		case CHAR:
		case UCHAR:
			return(1);

		case SHORT:
		case USHORT:
			return(SZSHORT/SZCHAR);

		case DOUBLE:
			return(SZDOUBLE/SZCHAR);

		case INT:
		case UNSIGNED:
		case LONG:
		case ULONG:
			return(SZINT/SZCHAR);

		case LONGLONG:
		case ULONGLONG:
			return SZLONGLONG/SZCHAR;

		default:
			if (!ISPTR(p->in.type))
				cerror("tlen type %d not pointer");
			return SZPOINT/SZCHAR;
		}
}

#if 0
/*
 * Return true if type conversion needed.
 */
static int
mixtypes(NODE *p, NODE *q)
{
	TWORD tp, tq;

	tp = p->in.type;
	tq = q->in.type;

	return( (tp==FLOAT || tp==DOUBLE) != (tq==FLOAT || tq==DOUBLE) );
}

static void
prtype(NODE *n)
{
	switch (n->in.type)
		{

		case DOUBLE:
			putchar('d');
			return;

		case FLOAT:
			putchar('f');
			return;

		case LONG:
		case ULONG:
		case INT:
		case UNSIGNED:
			putchar('l');
			return;

		case LONGLONG:
		case ULONGLONG:
			putchar('q');
			return;

		case SHORT:
		case USHORT:
			putchar('w');
			return;

		case CHAR:
		case UCHAR:
			putchar('b');
			return;

		default:
			if (!ISPTR( n->in.type ))
				cerror("zzzcode- bad type %d", n->in.type);
			else
				putchar('l');
		}
}

#if 0
static void
longlongops(NODE *p)
{
	int op = p->in.op;

	if (asgop(op))
		op = NOASG op;

	switch (op) {
	case PLUS:
		putstr("addl2\t");
		adrput(r);
		putchar(',');
		adrput(l);
		
	default:
		cerror("no longlongops for op %d\n", p->in.op);
	}
}
#endif
#endif

static char *
ccbranches[] = {
	"jumpe",	/* jumpe */
	"jumpn",	/* jumpn */
	"jumple",	/* jumple */
	"jumpl",	/* jumpl */
	"jumpge",	/* jumpge */
	"jumpg",	/* jumpg */
	"jumple",	/* jumple (jlequ) */
	"jumpl",	/* jumpl (jlssu) */
	"jumpge",	/* jumpge (jgequ) */
	"jumpg",	/* jumpg (jgtru) */
};

static char *
binskip[] = {
	"e",	/* jumpe */
	"n",	/* jumpn */
	"le",	/* jumple */
	"l",	/* jumpl */
	"ge",	/* jumpge */
	"g",	/* jumpg */
};

/*
 * Do a binary comparision, and jump accordingly.
 */
static void
twocomp(NODE *p)
{
	int o = p->in.op;
	extern int negrel[];
	int isscon = 0, iscon = p->in.right->in.op == ICON;

	if (o < EQ || o > GT)
		cerror("bad binary conditional branch: %s", opst[o]);

	if (iscon)
		isscon = p->in.right->tn.lval >= 0 &&
		    p->in.right->tn.lval < 01000000;

	printf("	ca%c%s ", iscon && isscon ? 'i' : 'm',
	    binskip[negrel[o-EQ]-EQ]);
	adrput(getlr(p, 'L'));
	putchar(',');
	if (iscon && (isscon == 0)) {
		printf("[ .long ");
		adrput(getlr(p, 'R'));
		putchar(']');
	} else
		adrput(getlr(p, 'R'));
	printf("\n	jrst L%d\n", p->bn.label);
}

/*
 * Compare byte/word pointers.
 * XXX - do not work for highest bit set in address
 */
static void
ptrcomp(NODE *p)
{
	printf("	rot "); adrput(getlr(p, 'L')); printf(",6\n");
	printf("	rot "); adrput(getlr(p, 'R')); printf(",6\n");
	twocomp(p);
}

/*
 * Do a binary comparision of two long long, and jump accordingly.
 * XXX - can optimize for constants.
 */
static void     
twollcomp(NODE *p)
{       
	int o = p->in.op;
	int iscon = p->in.right->in.op == ICON;
	int m;

	if (o < EQ || o > GT)
		cerror("bad long long conditional branch: %s", opst[o]);

	/* Special strategy for equal/not equal */
	if (o == EQ || o == NE) {
		if (o == EQ)
			m = getlab();
		printf("	came ");
		upput(getlr(p, 'L'), SZLONG);
		putchar(',');
		if (iscon)
			printf("[ .long ");
		upput(getlr(p, 'R'), SZLONG);
		if (iscon)
			putchar(']');
		printf("\n	jrst L%d\n", o == EQ ? m : p->bn.label);
		printf("	cam%c ", o == EQ ? 'n' : 'e');
		adrput(getlr(p, 'L'));
		putchar(',');
		if (iscon)
			printf("[ .long ");
		adrput(getlr(p, 'R'));
		if (iscon)
			putchar(']');
		printf("\n	jrst L%d\n", p->bn.label);
		if (o == EQ)
			printf("L%d:\n", m);
		return;
	}
	/* First test upper */
	printf("	cam%ce ", o == GT || o == GE ? 'l' : 'g');
	upput(getlr(p, 'L'), SZLONG);
	putchar(',');
	if (iscon)
		printf("[ .long ");
	upput(getlr(p, 'R'), SZLONG);
	if (iscon)
		putchar(']');
	printf("\n	jrst L%d\n", p->bn.label);

	/* Test equality */
	printf("	came ");
	upput(getlr(p, 'L'), SZLONG);
	putchar(',');
	if (iscon)
		printf("[ .long ");
	upput(getlr(p, 'R'), SZLONG);
	if (iscon)
		putchar(']');
	printf("\n	jrst L%d\n", m = getlab());

	/* Test lower. Only works with pdp10 format for longlongs */
	printf("	cam%c%c ", o == GT || o == GE ? 'l' : 'g',
	    o == LT || o == GT ? 'e' : ' ');
	adrput(getlr(p, 'L'));
	putchar(',');
	if (iscon)  
		printf("[ .long ");
	adrput(getlr(p, 'R'));
	if (iscon)
		putchar(']');
	printf("\n	jrst L%d\n", p->bn.label);
	printf("L%d:\n", m);
}

/*
 * Print the correct instruction for constants.
 */
static void
constput(NODE *p)
{
	CONSZ val = p->in.right->tn.lval;
	int reg = p->in.left->tn.rval;

	/* Only numeric constant */
	if (p->in.right->in.name == '\0') {
		if (val == 0) {
			printf("movei %s,0", rnames[reg]);
		} else if ((val & 0777777000000) == 0) {
			printf("movei %s,0%llo", rnames[reg], val);
		} else if ((val & 0777777) == 0) {
			printf("hrlzi %s,0%llo", rnames[reg], val >> 18);
		} else {
			printf("move %s,[ .long 0%llo]", rnames[reg], val);
		}
		/* Can have more tests here, hrloi etc */
		return;
	} else {
		if (val == 0)
			printf("move %s,[ .long %s]", rnames[reg],
			    p->in.right->in.name);
		else
			printf("move %s,[ .long %s+0%llo]", rnames[reg],
			    p->in.right->in.name, val);
	}
}

/*
 * Return true if the constant can be bundled in an instruction (immediate).
 */
static int
oneinstr(NODE *p)
{
	if (p->in.name[0] != '\0')
		return 0;
	if ((p->tn.lval & 0777777000000ULL) != 0)
		return 0;
	return 1;
}

/*
 * Handle xor of constants separate.
 * Emit two instructions instead of one extra memory reference.
 */
static void
emitxor(NODE *p)               
{                       
	CONSZ val;
	int reg;

	if (p->in.op != EREQ)
		cerror("emitxor");
	if (p->in.right->in.op != ICON)
		cerror("emitxor2");
	val = p->in.right->tn.lval;
	reg = p->in.left->tn.rval;
	if (val & 0777777)
		printf("	trc 0%o,0%llo\n", reg, val & 0777777);
	if (val & 0777777000000)
		printf("	tlc 0%o,0%llo\n", reg, (val >> 18) & 0777777);
}

/*
 * Print an instruction that takes care of a byte or short (less than 36 bits)
 */
static void
outvbyte(NODE *p)
{
	NODE *l = p->in.left;
	int lval, bsz, boff;

	lval = l->tn.lval;
	l->tn.lval &= 0777777;
	bsz = (lval >> 18) & 077;
	boff = (lval >> 24) & 077;

	if ((bsz == 18) && (boff == 0 || boff == 18)) {
		printf("hr%cm", boff ? 'r' : 'l');
		return;
	}
	cerror("outvbyte: bsz %d boff %d", bsz, boff);
}

/*
 * Emit a "load short" instruction, from OREG to REG.
 * If reg is 016 (frame pointer), it can be converted 
 * to a halfword instruction, otherwise use ldb.
 * Sign extension must also be done here.
 */
static void
emitshort(NODE *p)
{
	CONSZ off = p->tn.lval;
	int reg = p->tn.rval;
	int issigned = !ISUNSIGNED(p->in.type);
	int ischar = BTYPE(p->in.type) == CHAR || BTYPE(p->in.type) == UCHAR;

	if (reg) { /* Can emit halfword instructions */
		if (off < 0) { /* argument, use move instead */
			printf("	move ");
		} else if (ischar) {
			printf("	ldb ");
			adrput(getlr(p, '1'));
			printf(",[ .long 0%02o11%02o%06o ]\n",
			    (int)(27-(9*(off&3))), reg, (int)off/4);
			if (issigned) {
				printf("	lsh ");
				adrput(getlr(p, '1'));
				printf(",033\n	ash ");
				adrput(getlr(p, '1'));
				printf(",-033\n");
			}
			return;
		} else {
#ifdef notyet
			printf("	h%cr%c ", off & 1 ? 'r' : 'l',
			    issigned ? 'e' : 'z');
#endif
			printf("	ldb ");
			adrput(getlr(p, '1'));
			printf(",[ .long 0%02o22%02o%06o ]\n",
			    (int)(18-(18*(off&1))), reg, (int)off/2);
			if (issigned) {
				printf("	hrre ");
				adrput(getlr(p, '1'));
				putchar(',');
				adrput(getlr(p, '1'));
				putchar('\n');
			}
			return;
		}
		p->tn.lval /= 2;
	} else {
		if (off != 0)
			cerror("emitshort with off");
		printf("	ldb ");
		adrput(getlr(p, '1'));
		printf(",%s\n", rnames[reg]);
		if (issigned) {
			if (ischar) {
				printf("	lsh ");
				adrput(getlr(p, '1'));
				printf(",033\n	ash ");
				adrput(getlr(p, '1'));
				printf(",-033\n");
			} else {
				printf("	hrre ");
				adrput(getlr(p, '1'));
				putchar(',');
				adrput(getlr(p, '1'));
				putchar('\n');
			}
		}
		return;
	}
	adrput(getlr(p, '1'));
	putchar(',');
	adrput(getlr(p, 'L'));
	putchar('\n');
}

/*
 * Store a short from a register. Destination is a OREG.
 */
static void
storeshort(NODE *p)
{
	NODE *l = p->in.left;
	CONSZ off = l->tn.lval;
	int reg = l->tn.rval;
	int ischar = BTYPE(p->in.type) == CHAR || BTYPE(p->in.type) == UCHAR;

	if (l->in.op == NAME) {
		if (ischar) {
			printf("	dpb ");
			adrput(getlr(p, 'R'));
			printf(",[ .long 0%02o%010o+%s ]\n",
			    070+((int)off&3), (int)(off/4), l->in.name);
			return;
		}
		printf("	hr%cm ", off & 1 ? 'r' : 'l');
		l->tn.lval /= 2;
		adrput(getlr(p, 'R'));
		putchar(',');   
		adrput(getlr(p, 'L'));
		putchar('\n');
		return;
	}

	if (off || reg == STKREG) { /* Can emit halfword instructions */
		if (off < 0) { /* argument, use move instead */
			printf("	movem ");
		} else if (ischar) {
			printf("	dpb ");
			adrput(getlr(p, '1'));
			printf(",[ .long 0%02o11%02o%06o ]\n",
			    (int)(27-(9*(off&3))), reg, (int)off/4);
			return;
		} else {
			printf("	hr%cm ", off & 1 ? 'r' : 'l');
		}
		l->tn.lval /= 2;
		adrput(getlr(p, 'R'));
		putchar(',');
		adrput(getlr(p, 'L'));
	} else {
		printf("	dpb ");
		adrput(getlr(p, 'R'));
		putchar(',');
		l = getlr(p, 'L');
		l->in.op = REG;
		adrput(l);
		l->in.op = OREG;
	}
	putchar('\n');
}

/*
 * Add an int to a pointer. Both args are in registers, store value
 * in the right register.
 */
static void     
addtoptr(NODE *p) 
{                   
	NODE *l = p->in.left;
	int pp = l->in.type & TMASK1; /* pointer to pointer */
	int ty = l->in.type;
	int ischar = BTYPE(ty) == CHAR || BTYPE(ty) == UCHAR;

	if (!ischar && BTYPE(ty) != SHORT && BTYPE(ty) != USHORT)
		cerror("addtoptr != CHAR/SHORT");
	printf("	ad%s ", pp ? "d" : "jbp");
	adrput(getlr(p, 'R'));
	putchar(',');
	adrput(getlr(p, 'L'));
	putchar('\n');
}

/*
 * Add a constant to a pointer.
 */
static void     
addcontoptr(NODE *p) 
{                   
	NODE *l = p->in.left;
	int pp = l->in.type & TMASK1; /* pointer to pointer */
	int ty = l->in.type;
	int ischar = BTYPE(ty) == CHAR || BTYPE(ty) == UCHAR;

	if (!ischar && BTYPE(ty) != SHORT && BTYPE(ty) != USHORT)
		cerror("addtoptr != SHORT/CHAR");
	if (pp) {
		printf("	addi ");
		adrput(getlr(p, 'L'));
		putchar(',');
		adrput(getlr(p, 'R'));
		putchar('\n');
		if ((p->in.type & TMASK1) == 0) {
			/* Downgrading to pointer to short */
			/* Must make short pointer */
			printf("	tlo ");
			adrput(getlr(p, 'L'));
			if (ischar)
				printf(",0700000\n");
			else
				printf(",0740000\n");
		}
	} else {
		CONSZ off = p->in.right->tn.lval;

		if (BTYPE(ty) == CHAR || BTYPE(ty) == UCHAR)
			cerror("addtoptr pointer to pointer");
		if (off == 0)
			return; /* Should be taken care of in clocal() */
		printf("	addi ");
		adrput(getlr(p, 'L'));
		printf(",0%llo\n", off >> 1);
		if (off & 1) {
			printf("	ibp ");
			adrput(getlr(p, 'L'));
			printf("\n");
		}
	}
}

/*
 * Add a constant to a char pointer and return it in a scratch reg.
 */
static void     
addconandcharptr(NODE *p) 
{                   
	NODE *l = p->in.left;
	int ty = l->in.type;
	CONSZ off = p->in.right->tn.lval;

	if (BTYPE(ty) != CHAR && BTYPE(ty) != UCHAR)
		cerror("addconandcharptr != CHAR");
	if (l->tn.rval == STKREG) {
		printf("	xmovei ");
		adrput(getlr(p, '1'));
		printf(",0%llo(0%o)\n", off >> 2, l->tn.rval);
		printf("	tlo ");
		adrput(getlr(p, '1'));
		printf(",0%o0000\n", (int)(off & 3) + 070);
	} else {
		printf("	movei ");
		adrput(getlr(p, '1'));
		printf(",0%llo\n", off);
		printf("	adjbp ");
		adrput(getlr(p, '1'));
		printf(",0%o\n", l->tn.rval);
	}
}

/*
 * Multiply a register with a constant.
 */
static void     
imuli(NODE *p)
{
	NODE *r = p->in.right;

	if (r->tn.lval >= 0 && r->tn.lval <= 0777777) {
		printf("	imuli ");
		adrput(getlr(p, 'L'));
		printf(",0%llo\n", r->tn.lval);
	} else {
		printf("	imul ");
		adrput(getlr(p, 'L'));
		printf(",[ .long 0%llo ]\n", r->tn.lval & 0777777777777);
	}
}

void
zzzcode(NODE *p, int c)
{
	int m;

	switch (c) {
	case 'A':
		printf("\tZA: ");
		break;
	case 'C':
		constput(p);
		break;

	case 'D': /* Find out which type of const load insn to use */
		if (p->in.op != ICON)
			cerror("zzzcode not ICON");
		if (p->in.name[0] == '\0') {
			if ((p->tn.lval <= 0777777) && (p->tn.lval > -0777777))
				printf("movei");
			else if ((p->tn.lval & 0777777) == 0)
				printf("hrlzi");
			else
				printf("move");
		} else
			printf("move");
		break;

	case 'E': /* Print correct constant expression */
		if (p->in.name[0] == '\0') {
			if ((p->tn.lval <= 0777777) && (p->tn.lval > -0777777))
				printf("0%llo", p->tn.lval);
			else if ((p->tn.lval & 0777777) == 0)
				printf("0%llo", p->tn.lval >> 18);
			else
				printf("[ .long 0%llo]", p->tn.lval);
		} else {
			if (p->tn.lval == 0)
				printf("[ .long %s]", p->in.name);
			else
				printf("[ .long %s+0%llo]",
				    p->in.name, p->tn.lval);
		}
		break;

	case 'O': /* Print long long expression. Can be made more efficient */
		if (p->in.name[0] != '\0')
			cerror("longlong in name");
		printf("[ .long 0%llo,0%llo ]",
		    p->tn.lval & 0777777777777,
		    (p->tn.lval >> 36) & 0777777777777);
		break;

	case 'F': /* Print an "opsimp" instruction based on its const type */
		hopcode(oneinstr(p->in.right) ? 'C' : 'R', p->in.op);
		break;

	case 'G': /* Print a constant expression based on its const type */
		p = p->in.right;
		if (oneinstr(p)) {
			printf("0%llo", p->tn.lval);
		} else {
			if (p->in.name[0] == '\0') {
				printf("[ .long 0%llo ]",
				    p->tn.lval & 0777777777777ULL);
			} else {
				if (p->tn.lval == 0)
					printf("[ .long %s ]", p->in.name);
				else
					printf("[ .long %s+0%llo]", p->in.name,
					    p->tn.lval, 0777777777777ULL);
			}
		}
		break;

	case 'H': /* Print a small constant */
		p = p->in.right;
		printf("0%llo", p->tn.lval & 0777777);
		break;

	case 'I':
		p = p->in.left;
		/* FALLTHROUGH */
	case 'K':
		if (p->in.name[0] != '\0')
			putstr(p->in.name);
		if (p->tn.lval != 0) {
			putchar('+');
			printf("0%llo", p->tn.lval & 0777777);
		}
		break;

	case 'J':
		outvbyte(p);
		break;

	case 'L':
		zzzcode(p->in.left, 'T');
		break;

	case 'T':
		if (p->in.op != OREG)
			cerror("ZT");
		p->in.op = REG;
		adrput(p);
		p->in.op = OREG;
		break;

	case 'M':
		sconv( p, c == 'M' );
		break;

	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		cbgen(0, m = getlab(), 'I');
		deflab(p->bn.label);
		printf("	setz %s\n", rnames[getlr(p, '1')->tn.rval]);
		deflab(m);
		break;

	case 'Q': /* two-param long long comparisions */
		twollcomp(p);
		break;

	case 'R': /* two-param conditionals */
		twocomp(p);
		break;

	case 'S':
		emitxor(p);
		break;

	case 'U':
		emitshort(p);
		break;
		
	case 'V':
		storeshort(p);
		break;

	case 'W':
		addtoptr(p);
		break;

	case 'X':
		addcontoptr(p);
		break;

	case 'Y':
		addconandcharptr(p);
		break;

	case 'Z':
		ptrcomp(p);
		break;

	case 'a':
		imuli(p);
		break;

	default:
		cerror("zzzcode %c", c);
	}
}

#if 0
void
stasg(p)
	register NODE *p;
{
	NODE *l, *r;
	int size;

	if( p->in.op == STASG ){
		l = p->in.left;
		r = p->in.right;

		}
	else if( p->in.op == STARG ){  /* store an arg into a temporary */
		r = p->in.left;
		}
	else cerror( "STASG bad" );

	if( r->in.op == ICON ) r->in.op = NAME;
	else if( r->in.op == REG ) r->in.op = OREG;
	else if( r->in.op != OREG ) cerror( "STASG-r" );

	size = p->stn.stsize;

	if( size <= 0 || size > 65535 )
		cerror("structure size <0=0 or >65535");

	switch(size) {
		case 1:
			putstr("	movb	");
			break;
		case 2:
			putstr("	movw	");
			break;
		case 4:
			putstr("	movl	");
			break;
		case 8:
			putstr("	movq	");
			break;
		default:
			printf("	movc3	$%d,", size);
			break;
	}
	adrput(r);
	if( p->in.op == STASG ){
		putchar(',');
		adrput(l);
		putchar('\n');
		}
	else
		putstr(",(sp)\n");

	if( r->in.op == NAME ) r->in.op = ICON;
	else if( r->in.op == OREG ) r->in.op = REG;
	}

static NODE *
makearg(int ty)
{
	NODE *p, *q;

	/* build a -(sp) operand */
	p = talloc();
	p->in.op = REG;

	/* the type needn't be right, just consistent */
	p->in.type = INCREF(ty);
	p->tn.rval = SP;
	p->tn.lval = 0;

	q = talloc();
	q->in.op = ASG MINUS;
	q->in.type = INCREF(ty);
	q->in.left = p;

	p = talloc();
	p->in.op = ICON;
	p->in.type = INT;
	p->tn.name = "";
	/* size of floating argument is always 2 */
	p->tn.lval = (1 + (ty == FLOAT || ty == DOUBLE ||
	    ty == LONGLONG || ty == ULONGLONG)) * (SZINT/SZCHAR);
	q->in.right = p;

	p = talloc();
	p->in.op = UNARY MUL;
	p->in.left = q;
	return( p );
}
#endif

/*
 * Convert between two data types.
 */
void
sconv(NODE *p, int forarg)
{
#if 0
//	register NODE *l, *r;
	int m, val;

	if (x2debug)
		eprint(p, 0, &val, &val);

	switch (p->in.op) {
	case SCONV:
		r = getlr(p, 'R');
		l = getlr(p, 'L');
		break;

	default:
		cerror("sconv op %d", p->in.op);
	}
#endif
#if 0
	r = getlr(p, 'R');
	if (p->in.op == ASSIGN)
		l = getlr(p, 'L');
	else if (p->in.op == SCONV) {
		m = r->in.type;
		if (forarg)
			l = makearg( m );
		else
			l = resc;
		l->in.type = m;
		r = getlr(p, 'L');
	} else {		/* OPLTYPE */
		m = (r->in.type == FLOAT || r->in.type == DOUBLE ||
		    r->in.type == LONGLONG || r->in.type == ULONGLONG
		    ? r->in.type : INT);
		if (forarg)
			l = makearg( m );
		else
			l = resc;
		l->in.type = m;
	}
	if (r->in.op == ICON) {
		if (r->in.name[0] == '\0') {
			if (r->tn.lval == 0 &&
			    (r->in.type == DOUBLE || r->in.type == FLOAT ||
			    !forarg)) {
				if (r->in.type == FLOAT)
					r->in.type = DOUBLE;
				putstr("clr");
				prtype(l);
				putchar('\t');
				adrput(l);
				goto cleanup;
				}
			if (r->tn.lval < 0 && r->tn.lval >= -63) {
				putstr("mneg");
				prtype(l);
				r->tn.lval = -r->tn.lval;
				goto ops;
				}
			if (r->tn.lval < 0)
				r->in.type = r->tn.lval >= -128 ? CHAR
					: (r->tn.lval >= -32768 ? SHORT
					: INT);
			else if (l->in.type == FLOAT ||
			    l->in.type == DOUBLE)
				r->in.type = r->tn.lval <= 63 ? INT
					: (r->tn.lval <= 127 ? CHAR
					: (r->tn.lval <= 32767 ? SHORT
					: INT));
			else
				r->in.type = r->tn.lval <= 63 ? INT
					: (r->tn.lval <= 127 ? CHAR
					: (r->tn.lval <= 255 ? UCHAR
					: (r->tn.lval <= 32767 ? SHORT
					: (r->tn.lval <= 65535 ? USHORT
					: INT))));
			if (forarg && r->in.type == INT) {
				putstr("pushl\t");
				adrput(r);
				goto cleanup;
			}
		} else {
			if (forarg && tlen(r) == SZINT/SZCHAR) {
				putstr("pushl\t");
				adrput(r);
				goto cleanup;
			}
			putstr("moval\t");
			acon(r);
			putchar(',');
			adrput(l);
			goto cleanup;
		}
	}

	if (p->in.op == SCONV &&
	    !(l->in.type == FLOAT || l->in.type == DOUBLE) &&
	    !mixtypes(l, r)) {
		/*
		 * Because registers must always contain objects
		 * of the same width as INTs, we may have to
		 * perform two conversions to get an INT.  Can
		 * the conversions be collapsed into one?
		 */
		if ((m = collapsible(l, r)))
			r->in.type = m;
		else if (ISLONGLONG(r->in.type) || ISLONGLONG(l->in.type))
			cerror("SCONV LONGLONG conversion needed");
		else {
			/* two steps are required */
			NODE *x;

			if (forarg) {
				x = resc;
				x->in.type = l->in.type;
			} else {
				x = &resc[1];
				*x = *l;
			}

			if (tlen(x) > tlen(r) && ISUNSIGNED(r->in.type))
				putstr("movz");
			else
				putstr("cvt");
			prtype(r);
			prtype(x);
			putchar('\t');
			adrput(r);
			putchar(',');
			adrput(x);
			putchar('\n');
			putchar('\t');
			r = x;
		}
		l->in.type = (ISUNSIGNED(l->in.type) ? UNSIGNED : INT);
	} else if ((forarg || l == resc) &&
		 tlen(l) < SZINT/SZCHAR &&
		 mixtypes(l, r)) {
		/* two steps needed here too */
		NODE *x;

		if (forarg) {
			x = resc;
			x->in.type = l->in.type;
			}
		else {
			x = &resc[1];
			*x = *l;
			}
		putstr("cvt");
		prtype(r);
		prtype(x);
		putchar('\t');
		adrput(r);
		putchar(',');
		adrput(x);
		putstr("\n\t");
		r = x;
		l->in.type = (ISUNSIGNED(l->in.type) ? UNSIGNED : INT);
	} else if ((r->in.type == UNSIGNED || r->in.type == ULONG) &&
	    mixtypes(l, r)) {
		int label1;
		NODE *x = NULL;

#if defined(FORT) || defined(SPRECC)
		if (forarg)
#else
		if (forarg || l == resc)
#endif
			{
			/* compute in register, convert to double when done */
			x = l;
			l = resc;
			l->in.type = x->in.type;
			}

		label1 = getlab();

		putstr("cvtl");
		prtype(l);
		putchar('\t');
		adrput(r);
		putchar(',');
		adrput(l);
		printf("\n\tjgeq\tL%d\n\tadd", label1);
		prtype(l);
		putstr("2\t$0");
		prtype(l);
		putstr("4.294967296e9,");
		adrput(l);
		printf("\nL%d:", label1);

#if defined(FORT) || defined(SPRECC)
		if (!forarg)
#else
		if (!forarg && (l->in.type == DOUBLE || l != resc))
#endif
			goto cleanup;
		if (x == NULL)
			cerror("sconv botch");
		if (l == x) {
			r = &resc[1];
			*r = *l;
			}
		else {
			r = l;
			l = x;
			}
		l->in.type = DOUBLE;
		putstr("\n\t");
	} else if( (l->in.type == FLOAT || l->in.type == DOUBLE) &&
	    (r->in.type == UCHAR || r->in.type == USHORT) ) {
		/* skip unnecessary unsigned to floating conversion */
#if defined(FORT) || defined(SPRECC)
		if (forarg)
#else
		if (forarg || l == resc)
#endif
			l->in.type = DOUBLE;
		putstr("movz");
		prtype(r);
		putstr("l\t");
		adrput(r);
		putchar(',');
		adrput(resc);
		putstr("\n\t");
		if (l == resc) {
			r = &resc[1];
			*r = *l;
			}
		else
			r = resc;
		r->in.type = INT;
	}

#if defined(FORT) || defined(SPRECC)
	if (forarg && l->in.type == FLOAT)
#else
	if ((forarg || l == resc) && l->in.type == FLOAT)
#endif
		{
		/* perform an implicit conversion to double */
		l->in.type = DOUBLE;
		if (r->in.type != FLOAT &&
		    r->in.type != CHAR &&
		    r->in.type != SHORT) {
			/* trim bits from the mantissa */
			putstr("cvt");
			prtype(r);
			putstr("f\t");
			adrput(r);
			putchar(',');
			adrput(resc);
			putstr("\n\t");
			if (l == resc) {
				r = &resc[1];
				*r = *l;
				}
			else
				r = resc;
			r->in.type = FLOAT;
		}
	}

	if (!mixtypes(l,r)) {
		if (tlen(l) == tlen(r)) {
			if (forarg && tlen(l) == SZINT/SZCHAR) {
				putstr("pushl\t");
				adrput(r);
				goto cleanup;
			}
			putstr("mov");
#ifdef FORT
			if (Oflag)
				prtype(l);
			else {
				if (l->in.type == DOUBLE)
					putchar('q');
				else if(l->in.type == FLOAT)
					putchar('l');
				else
					prtype(l);
			}
#else
			prtype(l);
#endif FORT
			goto ops;
		} else if (ISLONGLONG(r->in.type)) {
			putstr("mov");
			prtype(l);
			goto ops;
		} else if (ISLONGLONG(l->in.type)) {
			if (ISUNSIGNED(r->in.type)) {
				putstr("clrq\t");
				adrput(l);
				putstr("\n\t");
				putstr("mov");
				prtype(r);
				goto ops;
			} else {
				putstr("ashl\t$-31,");
				adrput(r);
				putchar(',');
				adrput(l);
				putstr("\n\t");
				putstr("ashq\t$32,");
				adrput(l);
				putchar(',');
				adrput(l);
				putstr("\n\t");
				putstr("mov");
				prtype(r);
				goto ops;
			}
		} else if (tlen(l) > tlen(r) && ISUNSIGNED(r->in.type)) {
			putstr("movz");
		} else {
			putstr("cvt");
		}
	} else
		putstr("cvt");
	prtype(r);
	prtype(l);
ops:
	putchar('\t');
	adrput(r);
	putchar(',');
	adrput(l);

cleanup:
	if (forarg)
		tfree(l);
#endif
}

#if 0
/*
 * collapsible(dest, src) -- if a conversion with a register destination
 *	can be accomplished in one instruction, return the type of src
 *	that will do the job correctly; otherwise return 0.  Note that
 *	a register must always end up having type INT or UNSIGNED.
 */
int
collapsible(dest, src)
NODE *dest, *src;
{
	int st = src->in.type;
	int dt = dest->in.type;
	int newt = 0;

	/*
	 * Are there side effects of evaluating src?
	 * If the derived type will not be the same size as src,
	 * we may have to use two steps.
	 */
	if (tlen(src) > tlen(dest)) {
		if (tshape(src, STARREG))
			return (0);
		if (src->in.op == OREG && R2TEST(src->tn.rval))
			return (0);
		}

	/*
	 * Can we get an object of dest's type by punning src?
	 * Praises be to great Cthulhu for little-endian machines...
	 */
	if (st == CHAR && dt == USHORT)
		/*
		 * Special case -- we must sign-extend to 16 bits.
		 */
		return (0);

	if (tlen(src) < tlen(dest))
		newt = st;
	else
		newt = dt;

	return (newt);
	}
#endif


/*
 * Output code to move a value between two registers.
 * XXX - longlong?
 */
void
rmove(int rt, int rs, TWORD t)
{
	printf("\t%s %s,%s\n", (t == DOUBLE ? "dmove" : "move"),
	    rnames[rt], rnames[rs]);
}

struct respref respref[] = {
	{ INTAREG|INTBREG,	INTAREG|INTBREG, },
	{ INAREG|INBREG,	INAREG|INBREG|SOREG|STARREG|STARNM|SNAME|SCON,},
	{ INTEMP,	INTEMP, },
	{ FORARG,	FORARG, },
	{ INTEMP,	INTAREG|INAREG|INTBREG|INBREG|SOREG|STARREG|STARNM, },
	{ 0,	0 },
};

/* set up temporary registers */
void
setregs()
{
	fregs = 7;	/* 7 free regs on PDP10 (1-7) */
}

/*ARGSUSED*/
int
rewfld(NODE *p)
{
	return(1);
}

/*ARGSUSED*/
int
callreg(NODE *p)
{
	return(1);
}

#if 0
int
base( p ) register NODE *p; {
	register int o = p->in.op;

	if( o==ICON && p->tn.name[0] != '\0' ) return( 100 ); /* ie no base reg */
	if( o==REG ) return( p->tn.rval );
    if( (o==PLUS || o==MINUS) && p->in.left->in.op == REG && p->in.right->in.op==ICON)
		return( p->in.left->tn.rval );
    if( o==OREG && !R2TEST(p->tn.rval) && (p->in.type==INT || p->in.type==UNSIGNED || ISPTR(p->in.type)) )
		return( p->tn.rval + 0200*1 );
	if( o==INCR && p->in.left->in.op==REG ) return( p->in.left->tn.rval + 0200*2 );
	if( o==ASG MINUS && p->in.left->in.op==REG) return( p->in.left->tn.rval + 0200*4 );
	if( o==UNARY MUL && p->in.left->in.op==INCR && p->in.left->in.left->in.op==REG
	  && (p->in.type==INT || p->in.type==UNSIGNED || ISPTR(p->in.type)) )
		return( p->in.left->in.left->tn.rval + 0200*(1+2) );
	if( o==NAME ) return( 100 + 0200*1 );
	return( -1 );
	}

int
offset( p, tyl ) register NODE *p; int tyl; {

	if( tyl==1 &&
	    p->in.op==REG &&
	    (p->in.type==INT || p->in.type==UNSIGNED) )
		return( p->tn.rval );
	if( p->in.op==LS &&
	    p->in.left->in.op==REG &&
	    (p->in.left->in.type==INT || p->in.left->in.type==UNSIGNED) &&
	    p->in.right->in.op==ICON &&
	    p->in.right->in.name[0]=='\0' &&
	    (1<<p->in.right->tn.lval)==tyl)
		return( p->in.left->tn.rval );
	if( tyl==2 &&
	    p->in.op==PLUS &&
	    (p->in.left->in.type==INT || p->in.left->in.type==UNSIGNED) &&
	    p->in.left->in.op==REG &&
	    p->in.right->in.op==REG &&
	    p->in.left->tn.rval==p->in.right->tn.rval )
		return( p->in.left->tn.rval );
	return( -1 );
	}

void
makeor2( p, q, b, o) register NODE *p, *q; register int b, o; {
	register NODE *t;
	NODE *f;

	p->in.op = OREG;
	f = p->in.left; 	/* have to free this subtree later */

	/* init base */
	switch (q->in.op) {
		case ICON:
		case REG:
		case OREG:
		case NAME:
			t = q;
			break;

		case MINUS:
			q->in.right->tn.lval = -q->in.right->tn.lval;
		case PLUS:
			t = q->in.right;
			break;

		case INCR:
		case ASG MINUS:
			t = q->in.left;
			break;

		case UNARY MUL:
			t = q->in.left->in.left;
			break;

		default:
			cerror("illegal makeor2");
	}

	p->tn.lval = t->tn.lval;
	p->in.name = t->in.name;

	/* init offset */
	p->tn.rval = R2PACK( (b & 0177), o, (b>>7) );

	tfree(f);
	return;
}
#endif

int canaddr(NODE *);
int
canaddr(NODE *p)
{
	int o = p->in.op;

	if (o==NAME || o==REG || o==ICON || o==OREG ||
	    (o==UNARY MUL && shumul(p->in.left)))
		return(1);
	return(0);
}

int
flshape(NODE *p)
{
	register int o = p->in.op;

	return (o == REG || o == NAME || o == ICON ||
		(o == OREG && (!R2TEST(p->tn.rval) || tlen(p) == 1)));
}

/* INTEMP shapes must not contain any temporary registers */
int
shtemp(NODE *p)
{
	int r;

	if (p->in.op == STARG )
		p = p->in.left;

	switch (p->in.op) {
	case REG:
		return (!istreg(p->tn.rval));

	case OREG:
		r = p->tn.rval;
		if (R2TEST(r)) {
			if (istreg(R2UPK1(r)))
				return(0);
			r = R2UPK2(r);
		}
		return (!istreg(r));

	case UNARY MUL:
		p = p->in.left;
		return (p->in.op != UNARY MUL && shtemp(p));
	}

	if (optype(p->in.op) != LTYPE)
		return(0);
	return(1);
}

int
shumul(NODE *p)
{
	register int o;

	if (x2debug) {
		int val;
		printf("shumul(%p)\n", p);
		eprint(p, 0, &val, &val);
	}

	o = p->in.op;
#if 0
	if (o == NAME || (o == OREG && !R2TEST(p->tn.rval)) || o == ICON)
		return(STARNM);
#endif

	if ((o == INCR || o == ASG MINUS) &&
	    (p->in.left->in.op == REG && p->in.right->in.op == ICON) &&
	    p->in.right->in.name[0] == '\0') {
		switch (p->in.type) {
			case CHAR|PTR:
			case UCHAR|PTR:
				o = 1;
				break;

			case SHORT|PTR:
			case USHORT|PTR:
				o = 2;
				break;

			case INT|PTR:
			case UNSIGNED|PTR:
			case LONG|PTR:
			case ULONG|PTR:
			case FLOAT|PTR:
				o = 4;
				break;

			case DOUBLE|PTR:
			case LONGLONG|PTR:
			case ULONGLONG|PTR:
				o = 8;
				break;

			default:
				if (ISPTR(p->in.type) &&
				     ISPTR(DECREF(p->in.type))) {
					o = 4;
					break;
				} else
					return(0);
		}
		return( p->in.right->tn.lval == o ? STARREG : 0);
	}

	return( 0 );
}

void
adrcon(CONSZ val)
{
	cerror("adrcon: val %llo\n", val);
#if 0
	putchar('$');
	printf(CONFMT, val);
#endif
}

void
conput(NODE *p)
{
	switch (p->in.op) {
	case ICON:
		acon(p);
		if (p->in.name[0] != '\0')
			printf("+%s", p->in.name);
		return;

	case REG:
		putstr(rnames[p->tn.rval]);
		return;

	default:
		cerror("illegal conput");
	}
}

/*ARGSUSED*/
void
insput(NODE *p)
{
	cerror("insput");
}

/*
 * Write out the upper address, like the upper register of a 2-register
 * reference, or the next memory location.
 */
void
upput(NODE *p, int size)
{
	switch (p->in.op) {
	case REG:
		if (size != SZLONG)
			cerror("upput REG not SZLONG");
		putstr(rnames[p->tn.rval + 1]);
		break;

	case NAME:
	case OREG:
		p->tn.lval++;
		adrput(p);
		p->tn.lval--;
		break;

	default:
		cerror("upput bad op %d size %d", p->in.op, size);
	}
#if 0
	switch (p->in.op) {
	case REG:
		if (size != SZLONG)
			cerror("upput REG not SZLONG");
		putstr(rnames[p->tn.rval + 1]);
		break;
	case NAME:
	case OREG:
		p->tn.lval += 4;
		adrput(p);
		p->tn.lval -= 4;
		break;
	case ICON:
		putstr("$0");
		break;
	default:
		cerror("upput bad op %d size %d", p->in.op, size);
	}
#endif
}

void
adrput(NODE *p)
{
	int r;
	/* output an address, with offsets, from p */

	if (p->in.op == FLD)
		p = p->in.left;

	switch (p->in.op) {

	case NAME:
		zzzcode(p, 'K');
		return;

	case OREG:
		r = p->tn.rval;
#if 0
		if (R2TEST(r)) { /* double indexing */
			register int flags;

			flags = R2UPK3(r);
			if (flags & 1)
				putchar('*');
			if (flags & 4)
				putchar('-');
			if (p->tn.lval != 0 || p->in.name[0] != '\0')
				acon(p);
			if (R2UPK1(r) != 100)
				printf("(%s)", rnames[R2UPK1(r)]);
			if (flags & 2)
				putchar('+');
			printf("[%s]", rnames[R2UPK2(r)]);
			return;
		}
#endif
		if (R2TEST(r))
			cerror("adrput: unwanted double indexing: r %o", r);
		acon(p);
		if (p->in.name[0] != '\0')
			printf("+%s", p->in.name);
		printf("(%s)", rnames[p->tn.rval]);
		return;
	case ICON:
		/* addressable value of the constant */
		acon(p);
		if (p->in.name[0] != '\0')
			printf("+%s", p->in.name);
		return;

	case REG:
		putstr(rnames[p->tn.rval]);
		return;

#if 0
	case OREG:
		r = p->tn.rval;
		if (R2TEST(r)) { /* double indexing */
			register int flags;

			flags = R2UPK3(r);
			if (flags & 1)
				putchar('*');
			if (flags & 4)
				putchar('-');
			if (p->tn.lval != 0 || p->in.name[0] != '\0')
				acon(p);
			if (R2UPK1(r) != 100)
				printf("(%s)", rnames[R2UPK1(r)]);
			if (flags & 2)
				putchar('+');
			printf("[%s]", rnames[R2UPK2(r)]);
			return;
		}
		if (r == AP) {  /* in the argument region */
			if (p->in.name[0] != '\0')
				werror( "bad arg temp" );
			printf(CONFMT, p->tn.lval);
			putstr("(ap)");
			return;
		}
		if (p->tn.lval != 0 || p->in.name[0] != '\0')
			acon(p);
		printf("(%s)", rnames[p->tn.rval]);
		return;

	case UNARY MUL:
		/* STARNM or STARREG found */
		if (tshape(p, STARNM)) {
			putchar('*');
			adrput(p->in.left);
		} else {	/* STARREG - really auto inc or dec */
			register NODE *q;

			q = p->in.left;
			if (q->in.right->tn.lval != tlen(p))
				cerror("adrput: bad auto-increment/decrement");
			printf("%s(%s)%s", (q->in.op==INCR ? "" : "-"),
				rnames[q->in.left->tn.rval], 
				(q->in.op==INCR ? "+" : ""));
			p->in.op = OREG;
			p->tn.rval = q->in.left->tn.rval;
			p->tn.lval = (q->in.op == INCR ? -q->in.right->tn.lval : 0);
			p->in.name = "";
			tfree(q);
		}
		return;

#endif
	default:
		cerror("illegal address, op %d", p->in.op);
		return;

	}
}

/*
 * print out a constant
*/
void
acon(NODE *p)
{
	if (p->tn.lval < 0 && p->tn.lval > -0777777777777ULL)
		printf("-" CONFMT, -p->tn.lval);
	else
		printf(CONFMT, p->tn.lval);
}

int
genscall(NODE *p, int cookie)
{
	/* structure valued call */
	return(gencall(p, cookie));
}

/*
 * generate the call given by p
 */
/*ARGSUSED*/
int
gencall(NODE *p, int cookie)
{

	NODE *p1;
	int temp, temp1, m;

	temp = p->in.right ? argsize(p->in.right) : 0;

	if (p->in.op == STCALL || p->in.op == UNARY STCALL) {
		/* set aside room for structure return */

		temp1 = p->stn.stsize > temp ? p->stn.stsize : temp;
	}

	SETOFF(temp1,4);

	 /* make temp node, put offset in, and generate args */
	if (p->in.right)
		genargs(p->in.right);

	/*
	 * Verify that pushj can be emitted.
	 */
	p1 = p->in.left;
	switch (p1->in.op) {
	case ICON:
	case REG:
	case OREG:
	case NAME:
		break;
	default:
		order(p1, INAREG);
	}

	p->in.op = UNARY CALL;
	m = match(p, INTAREG|INTBREG);

	/* Remove args (if any) from stack */
	if (temp)
		printf("	subi 017,%o\n", temp);

	return(m != MDONE);
}


/*   printf conditional and unconditional branches */
void
cbgen(int o,int lab,int mode )
{
	if (o != 0 && (o < EQ || o > GT))
		cerror("bad conditional branch: %s", opst[o]);
	printf("	%s 0,L%d\n", o == 0 ? "jrst" : ccbranches[o-EQ], lab);
}

/* we have failed to match p with cookie; try another */
int
nextcook(NODE *p, int cookie)
{
	if (cookie == FORREW)
		return(0);  /* hopeless! */
	if (!(cookie&(INTAREG|INTBREG)))
		return(INTAREG|INTBREG);
	if (!(cookie&INTEMP) && asgop(p->in.op))
		return(INTEMP|INAREG|INTAREG|INTBREG|INBREG);
	return(FORREW);
}

int
lastchance(NODE *p, int cook)
{
	/* forget it! */
	return(0);
}

#if 0
void     optim2(NODE *);
void
optim2( p ) register NODE *p; {
	/* do local tree transformations and optimizations */

	int o;
	int i, mask;
	register NODE *l, *r;

	switch( o = p->in.op ) {

	case ASG PLUS:
	case ASG MINUS:
	case ASG MUL:
	case ASG OR:
		/* simple ASG OPSIMP -- reduce range of constant rhs */
		l = p->in.left;
		r = p->in.right;
		if( tlen(l) < SZINT/SZCHAR &&
		    r->in.op==ICON && r->in.name[0]==0 ){
			mask = (1 << tlen(l) * SZCHAR) - 1;
			if( r->tn.lval & (mask & ~(mask >> 1)) )
				r->tn.lval |= ~mask;
			else
				r->tn.lval &= mask;
			}
		break;

	case AND:
		/* commute L and R to eliminate complements and constants */
		if (((l = p->in.left)->in.op == ICON && l->in.name[0] == 0) ||
		    l->in.op == COMPL ) {
			p->in.left = p->in.right;
			p->in.right = l;
			}
		/* fall through */

	case ASG AND:
		/* change meaning of AND to ~R&L - bic on pdp11/vax */
		r = p->in.right;
		if( r->in.op==ICON && r->in.name[0]==0 ) {
			/* check for degenerate operations */
			l = p->in.left;
			mask = (1 << tlen(l) * SZCHAR) - 1;
			if( o == ASG AND || ISUNSIGNED(r->in.type) ) {
				i = ~r->tn.lval & mask;
				if( i == 0 ) {
					/* redundant mask */
					r->in.op = FREE;
					ncopy(p, l);
					l->in.op = FREE;
					break;
					}
				else if( i == mask )
					/* all bits masked off */
					goto zero;
				r->tn.lval = i;
				if( tlen(l) < SZINT/SZCHAR ){
					/* sign extend */
					if( r->tn.lval & (mask & ~(mask >> 1)) )
						r->tn.lval |= ~mask;
					else
						r->tn.lval &= mask;
					}
				break;
				}
			else if( r->tn.lval == mask &&
				 tlen(l) < SZINT/SZCHAR ) {
				/* use movz instead of bic */
				r->in.op = SCONV;
				r->in.left = l;
				r->in.right = 0;
				r->in.type = ENUNSIGN(l->in.type);
				r->in.su = l->in.su > 1 ? l->in.su : 1;
				ncopy(p, r);
				p->in.left = r;
				p->in.type = INT;
				break;
				}
			/* complement constant */
			r->tn.lval = ~r->tn.lval;
			}
		else if( r->in.op==COMPL ) { /* ~~A => A */
			r->in.op = FREE;
			p->in.right = r->in.left;
			}
		else { /* insert complement node */
			p->in.right = l = talloc();
			l->in.op = COMPL;
			l->in.rall = NOPREF;
			l->in.type = r->in.type;
			l->in.left = r;
			l->in.right = NULL;
			}
		break;

	case SCONV:
		l = p->in.left;
#if defined(FORT) || defined(SPRECC)
		if( p->in.type == FLOAT || p->in.type == DOUBLE ||
		    l->in.type == FLOAT || l->in.type == DOUBLE )
			return;
#else
		if( mixtypes(p, l) ) return;
#endif
		if( l->in.op == PCONV )
			return;
		if( (l->in.op == CALL || l->in.op == UNARY CALL) &&
		    l->in.type != INT && l->in.type != UNSIGNED )
			return;

		/* Only trust it to get it right if the size is the same */
		if( tlen(p) != tlen(l) )
			return;

		/* clobber conversion */
		if( l->in.op != FLD )
			l->in.type = p->in.type;
		ncopy( p, l );
		l->in.op = FREE;

		break;

	case ASSIGN:
		/*
		 * Conversions are equivalent to assignments;
		 * when the two operations are combined,
		 * we can sometimes zap the conversion.
		 */
		r = p->in.right;
		l = p->in.left;
		if ( r->in.op == SCONV &&
		     !mixtypes(l, r) &&
		     l->in.op != FLD &&
		     tlen(l) == tlen(r) ) {
				p->in.right = r->in.left;
				r->in.op = FREE;
			}
		break;

	case ULE:
	case ULT:
	case UGE:
	case UGT:
		p->in.op -= (UGE-GE);
		if( degenerate(p) )
			break;
		p->in.op += (UGE-GE);
		break;

	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:
		if( p->in.left->in.op == SCONV &&
		    p->in.right->in.op == SCONV ) {
			l = p->in.left;
			r = p->in.right;
			if( l->in.type == DOUBLE &&
			    l->in.left->in.type == FLOAT &&
			    r->in.left->in.type == FLOAT ) {
				/* nuke the conversions */
				p->in.left = l->in.left;
				p->in.right = r->in.left;
				l->in.op = FREE;
				r->in.op = FREE;
				}
			/* more? */
			}
		(void) degenerate(p);
		break;

	case DIV:
		if( p->in.right->in.op == ICON &&
		    p->in.right->tn.name[0] == '\0' &&
		    ISUNSIGNED(p->in.right->in.type) &&
		    (unsigned) p->in.right->tn.lval >= 0x80000000 ) {
			/* easy to do here, harder to do in zzzcode() */
			p->in.op = UGE;
			break;
			}
	case MOD:
	case ASG DIV:
	case ASG MOD:
		/*
		 * optimize DIV and MOD
		 *
		 * basically we spot UCHAR and USHORT and try to do them
		 * as signed ints...  apparently div+mul+sub is always
		 * faster than ediv for finding MOD on the VAX, when
		 * full unsigned MOD isn't needed.
		 *
		 * a curious fact: for MOD, cmp+sub and cmp+sub+cmp+sub
		 * are faster for unsigned dividend and a constant divisor
		 * in the right range (.5 to 1 of dividend's range for the
		 * first, .333+ to .5 for the second).  full unsigned is
		 * already done cmp+sub in the appropriate case; the
		 * other cases are less common and require more ambition.
		 */
		if( degenerate(p) )
			break;
		l = p->in.left;
		r = p->in.right;
		if( !ISUNSIGNED(r->in.type) ||
		    tlen(l) >= SZINT/SZCHAR ||
		    !(tlen(r) < SZINT/SZCHAR ||
		      (r->in.op == ICON && r->tn.name[0] == '\0')) )
			break;
		if( r->in.op == ICON )
			r->tn.type = INT;
		else {
			NODE *t = talloc();
			t->in.left = r;
			r = t;
			r->in.op = SCONV;
			r->in.type = INT;
			r->in.right = 0;
			p->in.right = r;
			}
		if( o == DIV || o == MOD ) {
			NODE *t = talloc();
			t->in.left = l;
			l = t;
			l->in.op = SCONV;
			l->in.type = INT;
			l->in.right = 0;
			p->in.left = l;
			}
		/* handle asgops in table */
		break;

	case RS:
	case ASG RS:
	case LS:
	case ASG LS:
		/* pick up degenerate shifts */
		l = p->in.left;
		r = p->in.right;
		if( !(r->in.op == ICON && r->tn.name[0] == '\0') )
			break;
		i = r->tn.lval;
		if( i < 0 ) {
			/* front end 'fixes' this? */
			if( o == LS || o == ASG LS )
				o += (RS-LS);
			else
				o += (LS-RS);
		}
		if( (o == RS || o == ASG RS) &&
		    !ISUNSIGNED(l->in.type) )
			/* can't optimize signed right shifts */
			break;
		if( o == LS ) {
			if( i < SZINT )
				break;
			}
		else {
			if( i < tlen(l) * SZCHAR )
				break;
			}
	zero:
		if( !asgop( o ) )
			if( tshape(l, SAREG|SNAME|SCON|SOREG|STARNM) ) {
				/* no side effects */
				tfree(l);
				ncopy(p, r);
				r->in.op = FREE;
				p->tn.lval = 0;
				}
			else {
				p->in.op = COMOP;
				r->tn.lval = 0;
				}
		else {
			p->in.op = ASSIGN;
			r->tn.lval = 0;
			}
		break;
		}
	}

int
degenerate(p) register NODE *p; {
	int o;
	int result, i;
	int lower, upper;
	register NODE *l, *r;

	/*
	 * try to keep degenerate comparisons with constants
	 * out of the table.
	 */
	r = p->in.right;
	l = p->in.left;
	if( r->in.op != ICON ||
	    r->tn.name[0] != '\0' ||
	    tlen(l) >= tlen(r) )
		return (0);
	switch( l->in.type ) {
	case CHAR:
		lower = -(1 << (SZCHAR - 1));
		upper = (1 << (SZCHAR - 1)) - 1;
		break;
	case UCHAR:
		lower = 0;
		upper = (1 << SZCHAR) - 1;
		break;
	case SHORT:
		lower = -(1 << (SZSHORT - 1));
		upper = (1 << (SZSHORT - 1)) - 1;
		break;
	case USHORT:
		lower = 0;
		upper = (1 << SZSHORT) - 1;
		break;
	default:
		cerror("unsupported type in degenerate()");
		}
	i = r->tn.lval;
	switch( o = p->in.op ) {
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		/* DIV and MOD work like EQ */
	case EQ:
	case NE:
		if( lower == 0 && (unsigned) i > upper )
			result = o == NE;
		else if( i < lower || i > upper )
			result = o == NE;
		else
			return (0);
		break;
	case LT:
	case GE:
		if( lower == 0 && (unsigned) i > upper )
			result = o == LT;
		else if( i <= lower )
			result = o != LT;
		else if( i > upper )
			result = o == LT;
		else
			return (0);
		break;
	case LE:
	case GT:
		if( lower == 0 && (unsigned) i >= upper )
			result = o == LE;
		else if( i < lower )
			result = o != LE;
		else if( i >= upper )
			result = o == LE;
		else
			return (0);
		break;
	default:
		cerror("unknown op in degenerate()");
		}
		
	if( o == MOD || o == ASG MOD ) {
		r->in.op = FREE;
		ncopy(p, l);
		l->in.op = FREE;
		}
	else if( o != ASG DIV && tshape(l, SAREG|SNAME|SCON|SOREG|STARNM) ) {
		/* no side effects */
		tfree(l);
		ncopy(p, r);
		r->in.op = FREE;
		p->tn.lval = result;
		}
	else {
		if( o == ASG DIV )
			p->in.op = ASSIGN;
		else {
			p->in.op = COMOP;
			r->tn.type = INT;
			}
		r->tn.lval = result;
		}
	if( logop(o) )
		p->in.type = INT;

	return (1);
	}

/* added by jwf */
struct functbl {
	int fop;
	TWORD ftype;
	char *func;
} opfunc[] = {
	{ DIV,		TANY,	"udiv", },
	{ MOD,		TANY,	"urem", },
	{ ASG DIV,	TANY,	"audiv", },
	{ ASG MOD,	TANY,	"aurem", },
	{ 0,	0,	0 },
};

static void
hardops(NODE *p)
 {
	/* change hard to do operators into function calls.  */
	NODE *q;
	struct functbl *f;
	int o;
	NODE *old,*temp;

	o = p->in.op;
	if( ! (optype(o)==BITYPE &&
	       (ISUNSIGNED(p->in.left->in.type) ||
		ISUNSIGNED(p->in.right->in.type))) )
		return;

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop ) goto convert;
		}
	return;

	convert:
	if( p->in.right->in.op == ICON && p->in.right->tn.name[0] == '\0' )
		/* 'J', 'K' in zzzcode() -- assumes DIV or MOD operations */
		/* save a subroutine call -- use at most 5 instructions */
		return;
	if( tlen(p->in.left) < SZINT/SZCHAR && tlen(p->in.right) < SZINT/SZCHAR )
		/* optim2() will modify the op into an ordinary int op */
		return;
	if( asgop( o ) ) {
		old = NIL;
		switch( p->in.left->in.op ){
		case FLD:
			q = p->in.left->in.left;
			/*
			 * rewrite (lval.fld /= rval); as
			 *  ((*temp).fld = udiv((*(temp = &lval)).fld,rval));
			 * else the compiler will evaluate lval twice.
			 */
			if( q->in.op == UNARY MUL ){
				/* first allocate a temp storage */
				temp = talloc();
				temp->in.op = OREG;
				temp->tn.rval = TMPREG;
				temp->tn.lval = BITOOR(freetemp(1));
				temp->in.type = INCREF(p->in.type);
				temp->in.name = "";
				old = q->in.left;
				q->in.left = temp;
			}
			/* fall thru ... */

		case REG:
		case NAME:
		case OREG:
			/* change ASG OP to a simple OP */
			q = talloc();
			q->in.op = NOASG p->in.op;
			q->in.rall = NOPREF;
			q->in.type = p->in.type;
			q->in.left = tcopy(p->in.left);
			q->in.right = p->in.right;
			p->in.op = ASSIGN;
			p->in.right = q;
			p = q;
			f -= 2; /* Note: this depends on the table order */
			/* on the right side only - replace *temp with
			 *(temp = &lval), build the assignment node */
			if( old ){
				temp = q->in.left->in.left; /* the "*" node */
				q = talloc();
				q->in.op = ASSIGN;
				q->in.left = temp->in.left;
				q->in.right = old;
				q->in.type = old->in.type;
				q->in.name = "";
				temp->in.left = q;
			}
			break;

		case UNARY MUL:
			/* avoid doing side effects twice */
			q = p->in.left;
			p->in.left = q->in.left;
			q->in.op = FREE;
			break;

		default:
			cerror( "hardops: can't compute & LHS" );
			}
		}

	/* build comma op for args to function */
	q = talloc();
	q->in.op = CM;
	q->in.rall = NOPREF;
	q->in.type = INT;
	q->in.left = p->in.left;
	q->in.right = p->in.right;
	p->in.op = CALL;
	p->in.right = q;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
	q->in.name = f->func;
	q->tn.lval = 0;
	q->tn.rval = 0;

	}

static void
zappost(NODE *p)
{
	/* look for ++ and -- operators and remove them */

	register int o, ty;
	register NODE *q;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case INCR:
	case DECR:
			q = p->in.left;
			p->in.right->in.op = FREE;  /* zap constant */
			ncopy( p, q );
			q->in.op = FREE;
			return;

		}

	if( ty == BITYPE ) zappost( p->in.right );
	if( ty != LTYPE ) zappost( p->in.left );
}

static void
fixpre(NODE *p)
{
	int o, ty;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case ASG PLUS:
			p->in.op = PLUS;
			break;
	case ASG MINUS:
			p->in.op = MINUS;
			break;
		}

	if( ty == BITYPE ) fixpre( p->in.right );
	if( ty != LTYPE ) fixpre( p->in.left );
}

/*ARGSUSED*/
NODE * addroreg(l) NODE *l;
				/* OREG was built in clocal()
				 * for an auto or formal parameter
				 * now its address is being taken
				 * local code must unwind it
				 * back to PLUS/MINUS REG ICON
				 * according to local conventions
				 */
{
	cerror("address of OREG taken");
	/*NOTREACHED*/
	return NULL;
}
#endif

/*
 * Do some local optimizations that must be done after optim is called.
 */
static void
optim2(NODE *p)
{
	int op = p->in.op;
	int m, ml;
	NODE *l;

	/* Remove redundant PCONV's */
	if (op == PCONV) {
		l = p->in.left;
		m = BTYPE(p->in.type);
		ml = BTYPE(l->in.type);
		if ((m == INT || m == LONG || m == LONGLONG || m == FLOAT ||
		    m == DOUBLE || m == STRTY || m == UNIONTY || m == ENUMTY ||
		    m == UNSIGNED || m == ULONG || m == ULONGLONG) &&
		    (ml == INT || ml == LONG || ml == LONGLONG || ml == FLOAT ||
		    ml == DOUBLE || ml == STRTY || ml == UNIONTY || 
		    ml == ENUMTY || ml == UNSIGNED || ml == ULONG ||
		    ml == ULONGLONG)) {
			ncopy(p, l);
			l->in.op = FREE;
			op = p->in.op;
		} else
		if (ISPTR(DECREF(p->in.type)) &&
		    (l->in.type == INCREF(STRTY))) {
			ncopy(p, l);
			l->in.op = FREE;
			op = p->in.op;
		}
	}
	/* Add constands, similar to the one in optim() */
	if (op == PLUS && p->in.right->in.op == ICON) {
		l = p->in.left;
		if (l->in.op == PLUS && l->in.right->in.op == ICON &&
		    (p->in.right->tn.name[0] == '\0' ||
		     l->in.right->tn.name[0] == '\0')) {
			l->in.right->tn.lval += p->in.right->tn.lval;
			if (l->in.right->tn.name[0] == '\0')
				l->in.right->tn.name = p->in.right->tn.name;
			p->in.right->in.op = FREE;
			ncopy(p, l);
			l->in.op = FREE;
		}
	}

	/* Convert "PTR undef" (void *) to "PTR uchar" */
	if (p->in.type == INCREF(UNDEF))
		p->in.type = INCREF(UCHAR);
}

#if 0
static void
strip(NODE *p)
{
	NODE *q;

	for (;;) {
		switch (p->in.op) {
		case PCONV:
			q = p->in.left;
			ncopy(p, q);
			q->in.op = FREE;
			break;

		default:
			return;
		}
	}
}
#endif

void
myreader(NODE *p)
{
	int e2print(NODE *p, int down, int *a, int *b);
#if 0
	strip(p);	/* strip off operations with no side effects */
	canon( p );		/* expands r-vals for fields */
	walkf( p, hardops );	/* convert ops to function calls */
#endif
	walkf(p, optim2);
	if (x2debug) {
		printf("myreader final tree:\n");
		fwalk(p, e2print, 0);
	}
}
