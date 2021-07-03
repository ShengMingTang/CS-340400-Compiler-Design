/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20140715

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 7 "y2.y"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "code.h"

int yylex();
void err(char *s);

extern int lineno;
extern FILE *f_asm;
int    errcnt=0;
int    errline=0;
char   *install_symbol();
#line 23 "y2.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union { 
         int       token ;
         char      charv ;
         char      *ident;
       } YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 84 "y2.y"
/* external function is defined here */
int TRACEON = 100;
#line 51 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define IDENTIFIER 257
#define INTEGER 258
#define FLOAT 259
#define SCSPEC 260
#define TYPESPEC 261
#define ENUM 262
#define STRUCT 263
#define UNION 264
#define TYPEMOD 265
#define CONSTANT 266
#define STRING 267
#define SIZEOF 268
#define IF 269
#define ELSE 270
#define WHILE 271
#define DO 272
#define FOR 273
#define SWITCH 274
#define CASE 275
#define DEFAULT_TOKEN 276
#define BREAK 277
#define CONTINUE 278
#define RETURN 279
#define GOTO 280
#define ASM 281
#define ASSIGN 282
#define OROR 283
#define ANDAND 284
#define EQCOMPARE 285
#define ARITHCOMPARE 286
#define LSHIFT 287
#define RSHIFT 288
#define UNARY 289
#define PLUSPLUS 290
#define MINUSMINUS 291
#define HYPERUNARY 292
#define POINTSAT 293
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    0,    4,    4,    5,    6,    9,   10,    5,    5,
    5,    3,    3,    3,    3,    3,    2,    2,    2,    2,
    1,    1,   11,   11,   12,   12,   13,    8,    8,   14,
    7,    7,   15,   15,   16,
};
static const YYINT yylen[] = {                            2,
    0,    1,    1,    2,    3,    0,    0,    0,    8,    2,
    1,    1,    3,    3,    3,    3,    1,    1,    1,    2,
    4,    1,    0,    1,    1,    3,    2,    1,    2,    2,
    0,    1,    1,    2,    3,
};
static const YYINT yydefred[] = {                         0,
    0,   22,    0,   11,    0,    0,    0,    3,   10,    0,
    0,    0,    4,    5,    0,    0,    0,   25,    0,    0,
   21,    0,    0,    7,    0,   33,   26,    0,    0,   34,
   35,   17,   18,   19,    0,    0,    0,   28,   20,   30,
    0,    0,    0,    0,    0,   29,    0,    0,    0,   15,
    9,
};
static const YYINT yydgoto[] = {                          5,
    6,   35,   36,    7,    8,   12,   24,   37,   29,   45,
   16,   17,   18,   38,   25,   26,
};
static const YYINT yysindex[] = {                       -57,
  -46,    0, -243,    0,    0,   -7,  -57,    0,    0,  -29,
 -227,  -79,    0,    0, -243,    4,    3,    0, -213,   -7,
    0, -227, -243,    0, -213,    0,    0,  -28, -239,    0,
    0,    0,    0,    0, -241,  -42, -239,    0,    0,    0,
 -239, -239, -239, -239,  -75,    0,  -37,  -27,    9,    0,
    0,
};
static const YYINT yyrindex[] = {                        52,
    0,    0,    0,    0,    0,  -70,   54,    0,    0,    0,
   14,    0,    0,    0,    0,    0,   15,    0, -231,    2,
    0,    0,    0,    0, -225,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -39,    0,  -68,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   -1,  -51,  -36,    0,
    0,
};
static const YYINT yygindex[] = {                         0,
    6,    0,   -4,    0,   53,    0,    0,    0,    0,    0,
    0,    0,   37,   24,    0,   38,
};
#define YYTABLESIZE 250
static const YYINT yytable[] = {                         44,
   43,    4,   12,   12,   44,   43,   13,   16,   10,   16,
   11,   11,    9,    2,   44,   43,   40,   32,   41,   12,
   20,   12,   13,   41,   13,   31,   33,   34,   28,   14,
   31,   32,   11,   15,   31,   31,   47,   48,   49,   50,
   32,   32,   27,   19,   21,   27,   22,   23,   39,   51,
   44,    1,    6,    2,   23,   24,    8,   14,   27,   13,
   46,    0,   30,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    1,    2,
    0,    0,    0,    3,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   16,    0,    0,    0,    0,    0,
    0,    0,    0,   42,    0,    0,   12,    0,   42,   13,
};
static const YYINT yycheck[] = {                         42,
   43,   59,   42,   43,   42,   43,   43,   59,    3,   61,
   40,   40,   59,  257,   42,   43,   59,  257,   61,   59,
   15,   61,   59,   61,   61,  257,  266,  267,   23,   59,
   59,  257,   40,  261,  266,  267,   41,   42,   43,   44,
  266,  267,   41,  123,   41,   44,   44,  261,  290,  125,
   42,    0,  123,    0,   41,   41,  125,   59,   22,    7,
   37,   -1,   25,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  256,  257,
   -1,   -1,   -1,  261,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  286,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  286,   -1,   -1,  286,   -1,  286,  286,
};
#define YYFINAL 5
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 293
#define YYUNDFTOKEN 312
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,0,0,0,
0,0,0,0,"':'","';'","'<'","'='","'>'","'?'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'{'","'|'","'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"IDENTIFIER","INTEGER","FLOAT",
"SCSPEC","TYPESPEC","ENUM","STRUCT","UNION","TYPEMOD","CONSTANT","STRING",
"SIZEOF","IF","ELSE","WHILE","DO","FOR","SWITCH","CASE","DEFAULT_TOKEN","BREAK",
"CONTINUE","RETURN","GOTO","ASM","ASSIGN","OROR","ANDAND","EQCOMPARE",
"ARITHCOMPARE","LSHIFT","RSHIFT","UNARY","PLUSPLUS","MINUSMINUS","HYPERUNARY",
"POINTSAT",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : program",
"program :",
"program : extdefs",
"extdefs : extdef",
"extdefs : extdefs extdef",
"extdef : TYPESPEC notype_declarator ';'",
"$$1 :",
"$$2 :",
"$$3 :",
"extdef : notype_declarator $$1 '{' xdecls $$2 stmts $$3 '}'",
"extdef : error ';'",
"extdef : ';'",
"expr_no_commas : primary",
"expr_no_commas : expr_no_commas '+' expr_no_commas",
"expr_no_commas : expr_no_commas '=' expr_no_commas",
"expr_no_commas : expr_no_commas '*' expr_no_commas",
"expr_no_commas : expr_no_commas ARITHCOMPARE expr_no_commas",
"primary : IDENTIFIER",
"primary : CONSTANT",
"primary : STRING",
"primary : primary PLUSPLUS",
"notype_declarator : notype_declarator '(' parmlist ')'",
"notype_declarator : IDENTIFIER",
"parmlist :",
"parmlist : parms",
"parms : parm",
"parms : parms ',' parm",
"parm : TYPESPEC notype_declarator",
"stmts : stmt",
"stmts : stmts stmt",
"stmt : expr_no_commas ';'",
"xdecls :",
"xdecls : decls",
"decls : decl",
"decls : decls decl",
"decl : TYPESPEC notype_declarator ';'",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 339 "y2.y"


/*
 *	  s - the error message to be printed
 */
void yyerror(char *s)
{
	err(s);
}


void err(char *s)
{
	if (! errcnt++)
		errline = lineno;
         fprintf(stderr,"Error on line %d \n",lineno);
	
	exit(1);
}




#line 344 "y.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 92 "y2.y"
	{ if (TRACEON) printf("1\n ");}
break;
case 2:
#line 94 "y2.y"
	{ if (TRACEON) printf("2\n ");}
break;
case 3:
#line 99 "y2.y"
	{ if (TRACEON) printf("3\n ");}
break;
case 4:
#line 101 "y2.y"
	{ if (TRACEON) printf("4\n ");}
break;
case 5:
#line 106 "y2.y"
	{ if (TRACEON) printf("7 ");
            set_global_vars(yystack.l_mark[-1].ident);
          }
break;
case 6:
#line 110 "y2.y"
	{ if (TRACEON) printf("10 ");
                    cur_scope++;
                    set_scope_and_offset_of_param(yystack.l_mark[0].ident);
		    code_gen_func_header(yystack.l_mark[0].ident);
                  }
break;
case 7:
#line 116 "y2.y"
	{ if (TRACEON) printf("10.5 ");
                    set_local_vars(yystack.l_mark[-3].ident);
                  }
break;
case 8:
#line 120 "y2.y"
	{
                  if (TRACEON) printf("11 ");
                  pop_up_symbol(cur_scope);
                  cur_scope--;
                  code_gen_at_end_of_function_body(yystack.l_mark[-5].ident);
                }
break;
case 10:
#line 128 "y2.y"
	{ if (TRACEON) printf("8 "); }
break;
case 11:
#line 130 "y2.y"
	{ if (TRACEON) printf("9 "); }
break;
case 12:
#line 139 "y2.y"
	{ if (TRACEON) printf("15 ") ;
 	     yyval.ident= yystack.l_mark[0].ident;
           }
break;
case 13:
#line 143 "y2.y"
	{ 
                  if (TRACEON) printf("16 ") ; 

		  fprintf(f_asm,"        lw t0, 0(sp)\n");
		  fprintf(f_asm,"        addi sp, sp, 4\n");
		  fprintf(f_asm,"        lw t1, 0(sp)\n");
		  fprintf(f_asm,"        addi sp, sp, 4\n");
		  fprintf(f_asm,"        add  t0, t0, t1\n");
		  fprintf(f_asm,"        addi sp, sp, -4\n");
		  fprintf(f_asm,"        sw t0, 0(sp)\n");

		  yyval.ident= NULL;
                }
break;
case 14:
#line 157 "y2.y"
	{ char *s;
		  int index;

		  if (TRACEON) printf("17 ") ;
		  s= yystack.l_mark[-2].ident;
		  if (!s) perror("improper expression at LHS");
		  index = look_up_symbol(s);
		  

		  fprintf(f_asm,"        lw  t0, 0(fp) \n");
		  fprintf(f_asm,"        addi sp, sp, 4\n");
		  fprintf(f_asm,"        lw  t1, 0(fp) \n");
		  fprintf(f_asm,"        addi sp, sp, 4\n");
		  
		  switch(table[index].mode) {
                  case ARGUMENT_MODE:

		    fprintf(f_asm,"        sw  t0, %d(fp) \n", table[table[index].functor_index].total_locals *(-4)-16 +table[index].offset*(-4)  +(-4));
		    fprintf(f_asm,"        addi sp, sp, -4\n");
		    fprintf(f_asm,"        sw t0, 0(sp)\n");

                    break;
		  case LOCAL_MODE:

		    fprintf(f_asm,"        sw  t0, %d(fp) \n", table[index].offset*4*(-1)-16);
		    fprintf(f_asm,"        addi sp, sp, -4\n");
		    fprintf(f_asm,"        sw t0, 0(sp)\n");
                    break;
		  default: /* Global Vars */

		    fprintf(f_asm,"        lui     t2,%%hi(%s)\n", table[index].name);
		    fprintf(f_asm,"        sw     t0,%%lo(%s)(t2)\n", table[index].name);
		    fprintf(f_asm,"        addi sp, sp, -4\n");
		    fprintf(f_asm,"        sw t0, 0(sp)\n");

		  }
                }
break;
case 15:
#line 195 "y2.y"
	{ if (TRACEON) printf("18 ") ;

		  fprintf(f_asm,"        lw t0, 0(sp)\n");
		  fprintf(f_asm,"        addi sp, sp, 4\n");
		  fprintf(f_asm,"        lw t1, 0(sp)\n");
		  fprintf(f_asm,"        addi sp, sp, 4\n");
		  fprintf(f_asm,"        mul  t0, t0, t1\n");
		  fprintf(f_asm,"        addi sp, sp, -4\n");
		  fprintf(f_asm,"        sw t0, 0(sp)\n");
		  
		  yyval.ident= NULL;
                }
break;
case 16:
#line 208 "y2.y"
	{ if (TRACEON) printf("19 ") ; }
break;
case 17:
#line 215 "y2.y"
	{    	  int index;

                  if (TRACEON) printf("20 ") ;
                  
		  index =look_up_symbol(yystack.l_mark[0].ident);
		  switch(table[index].mode) {
                  case ARGUMENT_MODE:
		    fprintf(f_asm,"        lw  t0, %d(fp) \n",
			    table[table[index].functor_index].total_locals *(-4)-16 +table[index].offset*(-4)  +(-4));
		    fprintf(f_asm,"        addi sp, sp, -4\n");
		    fprintf(f_asm,"        sw t0, 0(sp)\n");

                    break;
		  case LOCAL_MODE:

		    fprintf(f_asm,"        lw  t0, %d(fp) \n",table[index].offset*4*(-1)-16);
		    fprintf(f_asm,"        addi sp, sp, -4\n");
		    fprintf(f_asm,"        sw t0, 0(sp)\n");

                    break;
		  default: /* Global Vars */

		    fprintf(f_asm,"        lui     t0,%%hi(%s)\n", table[index].name);
		    fprintf(f_asm,"        lw     t1, %%lo(%s)(t0)\n", table[index].name);
		    fprintf(f_asm,"        addi sp, sp, -4\n");
		    fprintf(f_asm,"        sw t1, 0(sp)\n");

		  }
		  yyval.ident=yystack.l_mark[0].ident;
         }
break;
case 18:
#line 246 "y2.y"
	{ if (TRACEON) printf("21 ") ;
		  fprintf(f_asm,"        li t0,   %d\n",yystack.l_mark[0].token);
		  fprintf(f_asm,"        addi sp, sp, -4\n");
		  fprintf(f_asm,"        sw t0, 0(sp)\n");
                }
break;
case 19:
#line 252 "y2.y"
	{ 
		  if (TRACEON) printf("22 ") ;
                }
break;
case 20:
#line 256 "y2.y"
	{ 
		  if (TRACEON) printf("23 ") ;
                }
break;
case 21:
#line 264 "y2.y"
	{   if (TRACEON) printf("24 ") ;
		    yyval.ident=yystack.l_mark[-3].ident;
                }
break;
case 22:
#line 268 "y2.y"
	{   if (TRACEON) printf("25 ") ;
                    yyval.ident=install_symbol(yystack.l_mark[0].ident);
                }
break;
case 23:
#line 276 "y2.y"
	{ if (TRACEON) printf("26 ") ; }
break;
case 24:
#line 279 "y2.y"
	{ if (TRACEON) printf("27 ") ;  }
break;
case 25:
#line 286 "y2.y"
	{ if (TRACEON) printf("28 ") ;  }
break;
case 26:
#line 288 "y2.y"
	{ if (TRACEON) printf("29 ") ;  }
break;
case 27:
#line 293 "y2.y"
	{ if (TRACEON) printf("30 ") ;  }
break;
case 28:
#line 303 "y2.y"
	{ if (TRACEON) printf("31 ") ;  }
break;
case 29:
#line 305 "y2.y"
	{ if (TRACEON) printf("32 ") ;  }
break;
case 30:
#line 312 "y2.y"
	{
	  fprintf(f_asm,"        addi sp, sp, 4\n");
	  fprintf(f_asm,"   \n");
        }
break;
case 31:
#line 321 "y2.y"
	{ if (TRACEON) printf("102 ") ; }
break;
case 32:
#line 323 "y2.y"
	{ if (TRACEON) printf("103 ") ; }
break;
case 33:
#line 328 "y2.y"
	{ if (TRACEON) printf("104 ") ;
           }
break;
case 34:
#line 331 "y2.y"
	{ if (TRACEON) printf("106 ") ;
           }
break;
case 35:
#line 336 "y2.y"
	{ if (TRACEON) printf("110 ") ;}
break;
#line 802 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
