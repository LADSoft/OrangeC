/* calc.c */
/* Keyboard command interpreter	*/
/* by Stephen L. Moshier */

#include <stdio.h>
#if __STDC__
#include <stdlib.h>
#endif
#include <string.h>

/* This version is for C9X complex variables.  */
#include <complex.h>

#ifndef USE_READLINE
#define USE_READLINE 0
#endif
#if USE_READLINE
char *readline();
void add_history();
static char *line_read = (char *)NULL;
#endif

/* length of command line: */
#define LINLEN 128

#define XON 0x11
#define XOFF 0x13

#define SALONE 1
#define DECPDP 0
#define INTLOGIN 0
#define INTHELP 1
#ifndef TRUE
#define TRUE 1
#endif

/* Initialize squirrel printf: */
#define INIPRINTF 0

#if DECPDP
#define TRUE 1
#endif

static char idterp[] = {
"\n\nSteve Moshier's command interpreter V1.3\n"};
#define ISLOWER(c) ((c >= 'a') && (c <= 'z'))
#define ISUPPER(c) ((c >= 'A') && (c <= 'Z'))
#define ISALPHA(c) (ISLOWER(c) || ISUPPER(c))
#define ISDIGIT(c) ((c >= '0') && (c <= '9'))
#define ISATF(c) (((c >= 'a')&&(c <= 'f')) || ((c >= 'A')&&(c <= 'F')))
#define ISXDIGIT(c) (ISDIGIT(c) || ISATF(c))
#define ISOCTAL(c) ((c >= '0') && (c < '8'))
#define ISALNUM(c) (ISALPHA(c) || (ISDIGIT(c))
FILE *fopen();

#include "dccalc.h"
/* #include "ehead.h" */
#include "mconf.h"
/* int strlen(), strcmp(); */
int system();

/* space for working precision numbers */
static double complex vs[23];

/*	the symbol table of temporary variables: */

#define NTEMP 4
struct varent temp[NTEMP] = {
{"T",	OPR | TEMP, &vs[14]},
{"T",	OPR | TEMP, &vs[15]},
{"T",	OPR | TEMP, &vs[16]},
{"\0",	OPR | TEMP, &vs[17]}
};

/*	the symbol table of operators		*/
/* EOL is interpreted on null, newline, or ;	*/
struct symbol oprtbl[] = {
{"BOL",		OPR | BOL,	0},
{"EOL",		OPR | EOL,	0},
{"-",		OPR | UMINUS,	8},
/*"~",		OPR | COMP,	8,*/
{",",		OPR | EOE,	1},
{"=",		OPR | EQU,	2},
/*"|",		OPR | LOR,	3,*/
/*"^",		OPR | LXOR,	4,*/
/*"&",		OPR | LAND,	5,*/
{"+",		OPR | PLUS,	6},
{"-",		OPR | MINUS, 6},
{"*",		OPR | MULT,	7},
{"/",		OPR | DIV,	7},
/*"%",		OPR | MOD,	7,*/
{"(",		OPR | LPAREN,	11},
{")",		OPR | RPAREN,	11},
{"\0",		ILLEG, 0}
};

#define NOPR 8

/*	the symbol table of indirect variables: */
extern double PI;
double complex ZPI;
struct varent indtbl[] = {
{"i",		VAR | IND,	&vs[22]},
{"t",		VAR | IND,	&vs[21]},
{"u",		VAR | IND,	&vs[20]},	
{"v",		VAR | IND,	&vs[19]},
{"w",		VAR | IND,	&vs[18]},	
{"x",		VAR | IND,	&vs[10]},
{"y",		VAR | IND,	&vs[11]},
{"z",		VAR | IND,	&vs[12]},
{"pi",		VAR | IND,	&ZPI},
{"\0",		ILLEG,		0}
};

/*	the symbol table of constants:	*/

#define NCONST 10
struct varent contbl[NCONST] = {
{"C",CONST,&vs[0]},
{"C",CONST,&vs[1]},
{"C",CONST,&vs[2]},
{"C",CONST,&vs[3]},
{"C",CONST,&vs[4]},
{"C",CONST,&vs[5]},
{"C",CONST,&vs[6]},
{"C",CONST,&vs[7]},
{"C",CONST,&vs[8]},
{"\0",CONST,&vs[9]}
};

/* the symbol table of string variables: */

static char strngs[160] = {0};

#define NSTRNG 5
struct strent strtbl[NSTRNG] = {
{0, VAR | STRING, 0},
{0, VAR | STRING, 0},
{0, VAR | STRING, 0},
{0, VAR | STRING, 0},
{"\0",ILLEG,0},
};


/* Help messages */
#if INTHELP
static char *intmsg[] = {
"?",
"Unkown symbol",
"Expression ends in illegal operator",
"Precede ( by operator",
")( is illegal",
"Unmatched )",
"Missing )",
"Illegal left hand side",
"Missing symbol",
"Must assign to a variable",
"Divide by zero",
"Missing symbol",
"Missing operator",
"Precede quantity by operator",
"Quantity preceded by )",
"Function syntax",
"Too many function args",
"No more temps",
"Arg list"
};
#endif

/*	the symbol table of functions:	*/
#if SALONE
void exit();
double complex hex(), hexinput(), cmdh(), cmdhlp(), init();
double complex cmddm(), cmdtm(), cmdem();
double complex take(), mxit(), bits(), csys();
double complex cmddig(), prhlst(), abmac();
double complex xcmpl();
double complex zcabs(), zcreal(), zcimag(), zcarg(), zconj();
double complex cgamma (double complex);
double complex clgam (double complex);

struct funent funtbl[] = {
{"h",		OPR | FUNC, cmdh},
{"help",	OPR | FUNC, cmdhlp},
{"hex",		OPR | FUNC, hex},
{"abs",		OPR | FUNC, zcabs},
{"arg",		OPR | FUNC, zcarg},
{"re",		OPR | FUNC, zcreal},
{"im",		OPR | FUNC, zcimag},
{"conj",	OPR | FUNC, zconj},
  /* {"hexinput",		OPR | FUNC, hexinput}, */
/*"view",		OPR | FUNC, view,*/
{"exp",		OPR | FUNC, cexp},
  /* {"floor",	OPR | FUNC, floor}, */
{"log",		OPR | FUNC, clog},
{"pow",		OPR | FUNC, cpow},
{"sqrt",	OPR | FUNC, csqrt},
{"sin",		OPR | FUNC, csin},
{"cos",		OPR | FUNC, ccos},
{"tan",		OPR | FUNC, ctan},
{"cot",		OPR | FUNC, ccot},
{"asin",	OPR | FUNC, casin},
{"acos",	OPR | FUNC, cacos},
{"atan",	OPR | FUNC, catan},
{"sinh",	OPR | FUNC, csinh},
{"cosh",	OPR | FUNC, ccosh},
{"tanh",	OPR | FUNC, ctanh},
{"asinh",	OPR | FUNC, casinh},
{"acosh",	OPR | FUNC, cacosh},
{"atanh",	OPR | FUNC, catanh},
{"tanh",	OPR | FUNC, ctanh},
{"tanh",	OPR | FUNC, ctanh},
#if 1
{"gamma",	OPR | FUNC, cgamma},
{"lgam",	OPR | FUNC, clgam},
#endif
{"cmp",		OPR | FUNC, xcmpl},
{"bits",	OPR | FUNC, bits},
{"digits",	OPR | FUNC, cmddig},
{"dm",		OPR | FUNC, cmddm},
{"tm",		OPR | FUNC, cmdtm},
{"em",		OPR | FUNC, cmdem},
{"take",	OPR | FUNC | COMMAN, take},
{"system",	OPR | FUNC | COMMAN, csys},
{"exit",	OPR | FUNC, mxit},
{"\0",		OPR | FUNC,	0}
};

/*	the symbol table of key words */
struct funent keytbl[] = {
{"\0",		ILLEG,	0}
};
#endif

void zgets();

/* Number of decimals to display */
#define DEFDIS 70
static int ndigits = DEFDIS;

/* Menu stack */
struct funent *menstk[5] = {&funtbl[0], NULL, NULL, NULL, NULL};
int menptr = 0;

/* Take file stack */
FILE *takstk[10] = {0};
int takptr = -1;

/* size of the expression scan list: */
#define NSCAN 20

/* previous token, saved for syntax checking: */
struct symbol *lastok = 0;

/* Work around strong type checking. */
union
{
  struct symbol *psym;
  struct varent *pvar;
  struct funent *pfun;
  struct strent *pstr;
} pvfs;

/*	variables used by parser: */
static char str[LINLEN] = {0};
int uposs = 0;		/* possible unary operator */
static double complex qnc;
char lc[40] = { '\n' };	/*	ASCII string of token	symbol	*/
static char line[LINLEN] = { '\n','\0' };	/* input command line */
static char maclin[LINLEN] = { '\n','\0' };	/* macro command */
char *interl = line;		/* pointer into line */
extern char *interl;
static int maccnt = 0;	/* number of times to execute macro command */
static int comptr = 0;	/* comma stack pointer */
static double complex comstk[5];	/* comma argument stack */
static int narptr = 0;	/* pointer to number of args */
static int narstk[5] = {0};	/* stack of number of function args */

/*							main()		*/

/*	Entire program starts here	*/

int main()
{

/*	the scan table:			*/

/*	array of pointers to symbols which have been parsed:	*/
struct symbol *ascsym[NSCAN];

/*	current place in ascsym:			*/
register struct symbol **as;

/*	array of attributes of operators parsed:		*/
int ascopr[NSCAN];

/*	current place in ascopr:			*/
register int *ao;

#if LARGEMEM
/*	array of precedence levels of operators:		*/
long asclev[NSCAN];
/*	current place in asclev:			*/
long *al;
long symval;	/* value of symbol just parsed */
#else
int asclev[NSCAN];
int *al;
int symval;
#endif

double complex acc;	/* the accumulator, for arithmetic */
int accflg;	/* flags accumulator in use	*/
double complex val;	/* value to be combined into accumulator */
register struct symbol *psym;	/* pointer to symbol just parsed */
struct varent *pvar;	/* pointer to an indirect variable symbol */
struct funent *pfun;	/* pointer to a function symbol */
struct strent *pstr;	/* pointer to a string symbol */
int att;	/* attributes of symbol just parsed */
int i;		/* counter	*/
int offset;	/* parenthesis level */
int lhsflg;	/* kluge to detect illegal assignments */
struct symbol *parser();	/* parser returns pointer to symbol */
int errcod;	/* for syntax error printout */


/* Perform general initialization */

init();

menstk[0] = &funtbl[0];
menptr = 0;
cmdhlp();		/* print out list of symbols */


/*	Return here to get next command line to execute	*/
getcmd:

/* initialize registers and mutable symbols */

accflg = 0;	/* Accumulator not in use				*/
acc = 0.0;	/* Clear the accumulator				*/
offset = 0;	/* Parenthesis level zero				*/
comptr = 0;	/* Start of comma stack					*/
narptr = -1;	/* Start of function arg counter stack	*/
/* Initialize i = imaginary unit */
creal(vs[22]) = 0.0;
cimag(vs[22]) = 1.0;
/* Initialize pi */
creal(ZPI) = PI;
cimag(ZPI) = 0.0;
/* psym = (struct symbol *)&contbl[0]; */
pvfs.pvar = &contbl[0];
psym = pvfs.psym;
for( i=0; i<NCONST; i++ )
	{
	psym->attrib = CONST;	/* clearing the busy bit */
	++psym;
	}
/* psym = (struct symbol *)&temp[0]; */
pvfs.pvar = &temp[0];
psym = pvfs.psym;
for( i=0; i<NTEMP; i++ )
	{
	psym->attrib = VAR | TEMP;	/* clearing the busy bit */
	++psym;
	}

pstr = &strtbl[0];
for( i=0; i<NSTRNG; i++ )
	{
	pstr->spel = &strngs[ 40*i ];
	pstr->attrib = STRING | VAR;
	pstr->string = &strngs[ 40*i ];
	++pstr;
	}

/*	List of scanned symbols is empty:	*/
as = &ascsym[0];
*as = 0;
--as;
/*	First item in scan list is Beginning of Line operator	*/
ao = &ascopr[0];
*ao = oprtbl[0].attrib & 0xf;	/* BOL */
/*	value of first item: */
al = &asclev[0];
*al = oprtbl[0].sym;

lhsflg = 0;		/* illegal left hand side flag */
psym = &oprtbl[0];	/* pointer to current token */

/*	get next token from input string	*/

gettok:
lastok = psym;		/* last token = current token */
psym = parser();	/* get a new current token */
/*printf( "%s attrib %7o value %7o\n", psym->spel, psym->attrib & 0xffff,
		psym->sym );*/

/* Examine attributes of the symbol returned by the parser	*/
att = psym->attrib;
if( att == ILLEG )
	{
	errcod = 1;
	goto synerr;
	}

/*	Push functions onto scan list without analyzing further */
if( att & FUNC )
	{
	/* A command is a function whose argument is
	 * a pointer to the rest of the input line.
	 * A second argument is also passed: the address
	 * of the last token parsed.
	 */
	if( att & COMMAN )
		{
		pfun = (struct funent *)psym;
		( *(pfun->fun))( interl, lastok );
		abmac();	/* scrub the input line */
		goto getcmd;	/* and ask for more input */
		}
	++narptr;	/* offset to number of args */
	narstk[narptr] = 0;
	i = lastok->attrib & 0xffff; /* attrib=short, i=int */
	if( ((i & OPR) == 0)
			|| (i == (OPR | RPAREN))
			|| (i == (OPR | FUNC)) )
		{
		errcod = 15;
		goto synerr;
		}

	++lhsflg;
	++as;
	*as = psym;
	++ao;
	*ao = FUNC;
	++al;
	*al = offset + UMINUS;
	goto gettok;
	}

/* deal with operators */
if( att & OPR )
	{
	att &= 0xf;
	/* expression cannot end with an operator other than
	 * (, ), BOL, or a function
	 */
	if( (att == RPAREN) || (att == EOL) || (att == EOE))
		{
		i = lastok->attrib & 0xffff; /* attrib=short, i=int */
		if( (i & OPR) 
			&& (i != (OPR | RPAREN))
			&& (i != (OPR | LPAREN))
			&& (i != (OPR | FUNC))
			&& (i != (OPR | BOL)) )
				{
				errcod = 2;
				goto synerr;
				}
		}
	++lhsflg;	/* any operator but ( and = is not a legal lhs */

/*	operator processing, continued */

	switch( att )
		{
 	case EOE:
		lhsflg = 0;
		break; 
	case LPAREN:
		/* ( must be preceded by an operator of some sort. */
		if( ((lastok->attrib & OPR) == 0) )
			{
			errcod = 3;
			goto synerr;
			}
		/* also, a preceding ) is illegal */
		if( (unsigned short )lastok->attrib == (OPR|RPAREN))
			{
			errcod = 4;
			goto synerr;
			}
		/* Begin looking for illegal left hand sides: */
		lhsflg = 0;
		offset += RPAREN;	/* new parenthesis level */
		goto gettok;
	case RPAREN:
		offset -= RPAREN;	/* parenthesis level */
		if( offset < 0 )
			{
			errcod = 5;	/* parenthesis error */
			goto synerr;
			}
		goto gettok;
	case EOL:
		if( offset != 0 )
			{
			errcod = 6;	/* parenthesis error */
			goto synerr;
			}
		break;
	case EQU:
		if( --lhsflg )	/* was incremented before switch{} */
			{
			errcod = 7;
			goto synerr;
			}
	case UMINUS:
	case COMP:
		goto pshopr;	/* evaluate right to left */
	default:	;
		}


/*	evaluate expression whenever precedence is not increasing	*/

symval = psym->sym + offset;

while( symval <= *al )
	{
	/* if just starting, must fill accumulator with last
	 * thing on the line
	 */
	if( (accflg == 0) && (as >= ascsym) && (((*as)->attrib & FUNC) == 0 ))
		{
		pvar = (struct varent *)*as;
/*
		if( pvar->attrib & STRING )
			strcpy( (char *)&acc, (char *)pvar->value );
		else
*/
			acc = *pvar->value;
		--as;
		accflg = 1;
		}

/* handle beginning of line type cases, where the symbol
 * list ascsym[] may be empty.
 */
	switch( *ao )
		{
	case BOL:	
	        if (cimag (acc) < 0)
		  printf( "%.16e - %.16e*I\n", creal(acc), -cimag(acc) );
		else
		  printf( "%.16e + %.16e*I\n", creal(acc), cimag(acc) );
#if 0
#if NE == 6
		e64toasc( &acc, str, 100 );
#else
		e113toasc( &acc, str, 100 );
#endif
#endif
		printf( "%s\n", str );
		goto getcmd;	/* all finished */
	case UMINUS:
		acc = -acc;
		goto nochg;
/*
	case COMP:
		acc = ~acc;
		goto nochg;
*/
	default:	;
		}
/* Now it is illegal for symbol list to be empty,
 * because we are going to need a symbol below.
 */
	if( as < &ascsym[0] )
		{
		errcod = 8;
		goto synerr;
		}
/* get attributes and value of current symbol */
	att = (*as)->attrib;
	pvar = (struct varent *)*as;
	if( att & FUNC )
		val = 0.0;
	else
		{
/*
		if( att & STRING )
			strcpy( (char *)&val, (char *)pvar->value );
		else
*/
			val = *pvar->value;
		}

/* Expression evaluation, continued. */

	switch( *ao )
		{
	case FUNC:
		pfun = (struct funent *)*as;
	/* Call the function with appropriate number of args */
	i = narstk[ narptr ];
	--narptr;
	switch(i)
			{
			case 0:
			acc = ( *(pfun->fun) )(acc);
			break;
			case 1:
			acc = ( *(pfun->fun) )(acc, comstk[comptr-1]);
			break;
			case 2:
			acc = ( *(pfun->fun) )(acc, comstk[comptr-2],
				comstk[comptr-1]);
			break;
			case 3:
			acc = ( *(pfun->fun) )(acc, comstk[comptr-3],
				comstk[comptr-2], comstk[comptr-1]);
			break;
			default:
			errcod = 16;
			goto synerr;
			}
		comptr -= i;
		accflg = 1;	/* in case at end of line */
		break;
	case EQU:
		if( ( att & TEMP) || ((att & VAR) == 0) || (att & STRING) )
			{
			errcod = 9;
			goto synerr;	/* can only assign to a variable */
			}
		pvar = (struct varent *)*as;
		*pvar->value = acc;
		break;
	case PLUS:
		acc = acc + val;	break;
	case MINUS:
		acc = val - acc;	break;
	case MULT:
		acc = acc * val;	break;
	case DIV:
		if( acc == 0.0 )
			{
/*
divzer:
*/
			errcod = 10;
			goto synerr;
			}
		acc = val / acc;	break;
/*
	case MOD:
		if( acc == 0 )
			goto divzer;
		acc = val % acc;	break;
	case LOR:
		acc |= val;		break;
	case LXOR:
		acc ^= val;		break;
	case LAND:
		acc &= val;		break;
*/
	case EOE:
		if( narptr < 0 )
			{
			errcod = 18;
			goto synerr;
			}
		narstk[narptr] += 1;
		comstk[comptr++] = acc;
/*	printf( "\ncomptr: %d narptr: %d %d\n", comptr, narptr, acc );*/
		acc = val;
		break;
		}


/*	expression evaluation, continued		*/

/* Pop evaluated tokens from scan list:		*/
	/* make temporary variable not busy	*/
	if( att & TEMP )
		(*as)->attrib &= ~BUSY;
	if( as < &ascsym[0] )	/* can this happen? */
		{
		errcod = 11;
		goto synerr;
		}
	--as;
nochg:
	--ao;
	--al;
	if( ao < &ascopr[0] )	/* can this happen? */
		{
		errcod = 12;
		goto synerr;
		}
/* If precedence level will now increase, then			*/
/* save accumulator in a temporary location			*/
	if( symval > *al )
		{
		/* find a free temp location */
		pvar = &temp[0];
		for( i=0; i<NTEMP; i++ )
			{
			if( (pvar->attrib & BUSY) == 0)
				goto temfnd;
			++pvar;
			}
		errcod = 17;
		printf( "no more temps\n" );
		pvar = &temp[0];
		goto synerr;

	temfnd:
		pvar->attrib |= BUSY;
		*pvar->value = acc;
		/*printf( "temp %d\n", acc );*/
		accflg = 0;
		++as;	/* push the temp onto the scan list */
		*as = (struct symbol *)pvar;
		}
	}	/* End of evaluation loop */


/*	Push operator onto scan list when precedence increases	*/

pshopr:
	++ao;
	*ao = psym->attrib & 0xf;
	++al;
	*al = psym->sym + offset;
	goto gettok;
	}	/* end of OPR processing */


/* Token was not an operator.  Push symbol onto scan list.	*/
if( (lastok->attrib & OPR) == 0 )
	{
	errcod = 13;
	goto synerr;	/* quantities must be preceded by an operator */
	}
if( (unsigned short )lastok->attrib == (OPR | RPAREN) )	/* ...but not by ) */
	{
	errcod = 14;
	goto synerr;
	}
++as;
*as = psym;
goto gettok;

synerr:

#if INTHELP
printf( "%s ", intmsg[errcod] );
#endif
printf( " error %d\n", errcod );
abmac();	/* flush the command line */
goto getcmd;
}	/* end of program */

/*						parser()	*/

/* Get token from input string and identify it.		*/


static char number[128];

struct symbol *parser( )
{
register struct symbol *psym;
register char *pline;
struct varent *pvar;
struct strent *pstr;
char *cp, *plc, *pn;
long lnc;
int i;
double complex tem;

/* reference for old Whitesmiths compiler: */
/*
 *extern FILE *stdout;
 */

pline = interl;		/* get current location in command string	*/


/*	If at beginning of string, must ask for more input	*/
if( pline == line )
	{

	if( maccnt > 0 )
		{
		--maccnt;
		cp = maclin;
		plc = pline;
		while( (*plc++ = *cp++) != 0 )
			;
		goto mstart;
		}
	if( takptr < 0 )
		{	/* no take file active: prompt keyboard input */
		printf("* ");
		}
/* 	Various ways of typing in a command line. */

/*
 * Old Whitesmiths call to print "*" immediately
 * use RT11 .GTLIN to get command string
 * from command file or terminal
 */

/*
 *	fflush(stdout);
 *	gtlin(line);
 */

 
#if USE_READLINE
	if (takptr < 0)
	  {
	    if (line_read)
	      {
		free (line_read);
		line_read = (char *)NULL;
	      }
	    /* Get a line from the user. */
	    line_read = readline ("");
	    /* If the line has any text in it, save it on the history. */
	    if (line_read && *line_read)
	      add_history (line_read);
	    /* Copy to local buffer. */
	    strcpy(line,line_read);
	  }
	else
#endif
	zgets( line, TRUE );	/* keyboard input for other systems: */


mstart:
	uposs = 1;	/* unary operators possible at start of line */
	}

ignore:
/* Skip over spaces */
while( *pline == ' ' )
	++pline;

/* unary minus after operator */
if( uposs && (*pline == '-') )
	{
	psym = &oprtbl[2];	/* UMINUS */
	++pline;
	goto pdon3;
	}
	/* COMP */
/*
if( uposs && (*pline == '~') )
	{
	psym = &oprtbl[3];
	++pline;
	goto pdon3;
	}
*/
if( uposs && (*pline == '+') )	/* ignore leading plus sign */
	{
	++pline;
	goto ignore;
	}

/* end of null terminated input */
if( (*pline == '\n') || (*pline == '\0') || (*pline == '\r') )
	{
	pline = line;
	goto endlin;
	}
if( *pline == ';' )
	{
	++pline;
endlin:
	psym = &oprtbl[1];	/* EOL */
	goto pdon2;
	}


/*						parser()	*/


/* Test for numeric input */
if( (ISDIGIT(*pline)) || (*pline == '.') )
	{
	lnc = 0;	/* initialize numeric input to zero */
	qnc = 0.0;
	if( *pline == '0' )
		{ /* leading "0" may mean octal or hex radix */
		++pline;
		if( *pline == '.' )
			goto decimal; /* 0.ddd */
		/* leading "0x" means hexadecimal radix */
		if( (*pline == 'x') || (*pline == 'X') )
			{
			++pline;
			while( ISXDIGIT(*pline) )
				{
				i = *pline++ & 0xff;
				if( i >= 'a' )
					i -= 047;
				if( i >= 'A' )
					i -= 07;
				i -= 060;
				lnc = (lnc << 4) + i;
				qnc = lnc;
				}
			goto numdon;
			}
		else
			{
			while( ISOCTAL( *pline ) )
				{
				i = ((*pline++) & 0xff) - 060;
				lnc = (lnc << 3) + i;
				qnc = lnc;
				}
			goto numdon;
			}
		}
	else
		{
		/* no leading "0" means decimal radix */
/******/
decimal:
		pn = number;
		while( (ISDIGIT(*pline)) || (*pline == '.') )
			*pn++ = *pline++;
/* get possible exponent field */
		if( (*pline == 'e') || (*pline == 'E') )
			*pn++ = *pline++;
		else
			goto numcvt;
		if( (*pline == '-') || (*pline == '+') )
			*pn++ = *pline++;
		while( ISDIGIT(*pline) )
			*pn++ = *pline++;
numcvt:
		*pn++ = ' ';
		*pn++ = 0;
#if 0
#if NE == 6
		asctoe64( number, &qnc );
#else
		asctoe113( number, &qnc );
#endif
#endif
		sscanf( number, "%le", (double *) &qnc );
		}
/* output the number	*/
numdon:
	/* search the symbol table of constants 	*/
	pvar = &contbl[0];
	for( i=0; i<NCONST; i++ )
		{
		if( (pvar->attrib & BUSY) == 0 )
			goto confnd;
		tem = *pvar->value;
		if( tem == qnc )
			{
			psym = (struct symbol *)pvar;
			goto pdon2;
			}
		++pvar;
		}
	printf( "no room for constant\n" );
	/* psym = (struct symbol *)&contbl[0]; */
	pvfs.pvar = &contbl[0];
	psym = pvfs.psym;
	goto pdon2;

confnd:
	pvar->spel= contbl[0].spel;
	pvar->attrib = CONST | BUSY;
	*pvar->value = qnc;
	psym = (struct symbol *)pvar;
	goto pdon2;
	}

/* check for operators */
psym = &oprtbl[3];
for( i=0; i<NOPR; i++ )
	{
	if( *pline == *(psym->spel) )
		goto pdon1;
	++psym;
	}

/* if quoted, it is a string variable */
if( *pline == '"' )
	{
	/* find an empty slot for the string */
	pstr = strtbl;	/* string table	*/
	for( i=0; i<NSTRNG-1; i++ ) 
		{
		if( (pstr->attrib & BUSY) == 0 )
			goto fndstr;
		++pstr;
		}
	printf( "No room for string\n" );
	pstr->attrib |= ILLEG;
	psym = (struct symbol *)pstr;
	goto pdon0;

fndstr:
	pstr->attrib |= BUSY;
	plc = pstr->string;
	++pline;
	for( i=0; i<39; i++ )
		{
		*plc++ = *pline;
		if( (*pline == '\n') || (*pline == '\0') || (*pline == '\r') )
			{
illstr:
			pstr = &strtbl[NSTRNG-1];
			pstr->attrib |= ILLEG;
			printf( "Missing string terminator\n" );
			psym = (struct symbol *)pstr;
			goto pdon0;
			}
		if( *pline++ == '"' )
			goto finstr;
		}

	goto illstr;	/* no terminator found */

finstr:
	--plc;
	*plc = '\0';
	psym = (struct symbol *)pstr;
	goto pdon2;
	}
/* If none of the above, search function and symbol tables:	*/

/* copy character string to array lc[] */
plc = &lc[0];
while( ISALPHA(*pline) )
	{
	/* convert to lower case characters */
	if( ISUPPER( *pline ) )
		*pline += 040;
	*plc++ = *pline++;
	}
*plc = 0;	/* Null terminate the output string */

/*						parser()	*/

psym = (struct symbol *)menstk[menptr];	/* function table	*/
plc = &lc[0];
cp = psym->spel;
do
	{
	if( strcmp( plc, cp ) == 0 )
		goto pdon3;	/* following unary minus is possible */
	++psym;
	cp = psym->spel;
	}
while( *cp != '\0' );

/* psym = (struct symbol *)&indtbl[0]; */	/* indirect symbol table */
pvfs.pvar = &indtbl[0];
psym = pvfs.psym;
plc = &lc[0];
cp = psym->spel;
do
	{
	if( strcmp( plc, cp ) == 0 )
		goto pdon2;
	++psym;
	cp = psym->spel;
	}
while( *cp != '\0' );

pdon0:
pline = line;	/* scrub line if illegal symbol */
goto pdon2;

pdon1:
++pline;
if( (psym->attrib & 0xf) == RPAREN )
pdon2:	uposs = 0;
else
pdon3:	uposs = 1;

interl = pline;
return( psym );
}		/* end of parser */

/*	exit from current menu */

double complex cmdex()
{

if( menptr == 0 )
	{
	printf( "Main menu is active.\n" );
	}
else
	--menptr;

cmdh();
return(0.0);
}


/*			gets()		*/

void zgets( gline, echo )
char *gline;
int echo;
{
register char *pline;
register int i;


scrub:
pline = gline;
getsl:
	if( (pline - gline) >= LINLEN )
		{
		printf( "\nLine too long\n *" );
		goto scrub;
		}
	if( takptr < 0 )
		{	/* get character from keyboard */
/*
if DECPDP
		gtlin( gline );
		return(0);
else
*/
		*pline = getchar();
/*endif*/
		}
	else
		{	/* get a character from take file */
		i = fgetc( takstk[takptr] );
		if( i == -1 )
			{	/* end of take file */
			if( takptr >= 0 )
				{	/* close file and bump take stack */
				fclose( takstk[takptr] );
				takptr -= 1;
				}
			if( takptr < 0 )	/* no more take files:   */
				printf( "*" ); /* prompt keyboard input */
			goto scrub;	/* start a new input line */
			}
		*pline = i;
		}

	*pline &= 0x7f;
	/* xon or xoff characters need filtering out. */
	if ( *pline == XON || *pline == XOFF )
		goto getsl;

	/*	control U or control C	*/
	if( (*pline == 025) || (*pline == 03) )
		{
		printf( "\n" );
		goto scrub;
		}

	/*  Backspace or rubout */
	if( (*pline == 010) || (*pline == 0177) )
		{
		pline -= 1;
		if( pline >= gline )
			{
			if ( echo )
				printf( "\010\040\010" );
			goto getsl;
			}
		else
			goto scrub;
		}
	if ( echo )
		printf( "%c", *pline );
	if( (*pline != '\n') && (*pline != '\r') )
		{
		++pline;
		goto getsl;
		}
	*pline = 0;
	if ( echo )
		printf( "%c", '\n' );	/* \r already echoed */
}


/*		help function  */
double complex cmdhlp()
{

printf( "%s", idterp );
printf( "\nFunctions:\n" );
prhlst( &funtbl[0] );
printf( "\nVariables:\n" );
prhlst( &indtbl[0] );
printf( "\nOperators:\n" );
prhlst( &oprtbl[2] );
printf("\n");
return(0.0);
}


double complex cmdh()
{

prhlst( menstk[menptr] );
printf( "\n" );
return(0.0);
}

/* print keyword spellings */

double complex prhlst(ps)
register struct symbol *ps;
{
register int j, k;
int m;

j = 0;
while( *(ps->spel) != '\0' )
	{
	k = strlen( ps->spel )  -  1;
/* size of a tab field is 2**3 chars */
	m = ((k >> 3) + 1) << 3;
	j += m;
	if( j > 72 )
		{
		printf( "\n" );
		j = m;
		}
	printf( "%s\t", ps->spel );
	++ps;
	}
return(0.0);
}


#if SALONE
double complex init()
{
int dprec();
/* Set coprocessor to double precision. */
dprec();
return 0.0;
}
#endif


/*	macro commands */

/*	define macro */
double complex cmddm()
{

zgets( maclin, TRUE );
return(0.0);
}

/*	type (i.e., display) macro */
double complex cmdtm()
{

printf( "%s\n", maclin );
return 0.0;
}

/*	execute macro # times */
double complex cmdem( arg )
double complex arg;
{
double f;
long n;
double floor();

f = floor(creal(arg));
n = f;
if( n <= 0 )
	n = 1;
maccnt = n;
return(0.0);
}


/* open a take file */

double complex take( fname )
char *fname;
{
FILE *f;

while( *fname == ' ' )
	fname += 1;
f = fopen( fname, "r" );

if( f == 0 )
	{
	printf( "Can't open take file %s\n", fname );
	takptr = -1;	/* terminate all take file input */
	return 0.0;
	}
takptr += 1;
takstk[ takptr ]  =  f;
printf( "Running %s\n", fname );
return(0.0);
}


/*	abort macro execution */
double complex abmac()
{

maccnt = 0;
interl = line;
return(0.0);
}


/* display integer part in hex, octal, and decimal
 */
double complex hex(qx)
double complex qx;
{
double f;
long z;
double floor();

f = floor(creal(qx));
z = f;
printf( "0%lo  0x%lx  %ld.\n", z, z, z );

f = floor(cimag(qx));
z = f;
printf( "0%lo  0x%lx  %ld.\n", z, z, z );
return(qx);
}

#define NASC 16

double complex bits( x )
double complex x;
{
union
  {
    double d;
    short i[4];
  } du;
union
  {
    float f;
    short i[2];
  } df;
int i;

du.d = creal(x);
printf( "double: " );
for( i=0; i<4; i++ )
	printf( "0x%04x,", du.i[i] & 0xffff );
printf( "\n" );
du.d = cimag(x);
printf( "double: " );
for( i=0; i<4; i++ )
	printf( "0x%04x,", du.i[i] & 0xffff );
printf( "\n" );

df.f = (float) (creal(x));
printf( "float: " );
for( i=0; i<2; i++ )
	printf( "0x%04x,", df.i[i] & 0xffff );
printf( "\n" );
df.f = (float) (cimag(x));
printf( "float: " );
for( i=0; i<2; i++ )
	printf( "0x%04x,", df.i[i] & 0xffff );
printf( "\n" );
return(x);
}


/* Exit to monitor. */
double complex mxit()
{

exit(0);
return(0.0);
}


double complex cmddig( x )
double complex x;
{
double f;
long lx;

f = floor(creal(x));
lx = f;
ndigits = lx;
if( ndigits <= 0 )
	ndigits = DEFDIS;
return(f);
}


double complex csys(x)
char *x;
{

system( x+1 );
cmdh();
return(0.0);
}


/*
double ifrac(x)
double x;
{
unsigned long lx;
long double y, z;

z = floor(x);
lx = z;
y = x - z;
printf( " int = %lx\n", lx );
return(y);
}
*/

double complex xcmpl(x,y)
double complex x,y;
{
double complex ans;

ans = -2.0;
if( x == y )
	{
	printf( "x == y " );
	ans = 0.0;
	}
if( x != y )
	{
	printf( "x != y " );
	ans = 1.0;
	}
return( ans );
}

#if 0
extern double INFINITY, NAN;

double complex makenan(x)
double complex x;
{
return(NAN);
}

double makeinfinity(x)
double x;
{
return(INFINITY);
}

double zfrexp(x)
double x;
{
double y;
int e;
y = frexp(x, &e);
printf("exponent = %d, significand = ", e );
return(y);
}

double zldexp(x,e)
double x, e;
{
double y;
int i;

i = e;
y = ldexp(x,i);
return(y);
}

double hexinput(a, b)
double a,b;
{
union
  {
    double d;
    unsigned short i[4];
  } u;
unsigned long l;

#ifdef IBMPC
l = a;
u.i[3] = l >> 16;
u.i[2] = l;
l = b;
u.i[1] = l >> 16;
u.i[0] = l;
#endif
#ifdef DEC
l = a;
u.i[3] = l >> 16;
u.i[2] = l;
l = b;
u.i[1] = l >> 16;
u.i[0] = l;
#endif
#ifdef MIEEE
l = a;
u.i[0] = l >> 16;
u.i[1] = l;
l = b;
u.i[2] = l >> 16;
u.i[3] = l;
#endif
return(u.d);
}
#endif

double complex zcabs (x)
double complex x;
{
  double c;
  double complex zc;
  c = cabs (x);
  creal(zc) = c;
  cimag(zc) = 0.0;
  return zc;
}

double complex zcreal (x)
double complex x;
{
  double d;
  double complex y;

  d = creal(x);
  y = d;
  return y;
}

double complex zcimag (x)
double complex x;
{
  double d;
  double complex y;

  d = cimag(x);
  y = d;
  return y;
}

double complex zcarg (x)
double complex x;
{
  double d;
  complex double y;

  d = carg (x);
  y = d;
  return y;
}

double complex zconj (x)
double complex x;
{
  complex double y;

  y = conj (x);
  return y;
}
