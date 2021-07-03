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
extern YYSTYPE yylval;
