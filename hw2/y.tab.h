#define CONST 257
#define SIGNED_UNSIGNED 258
#define LONG 259
#define SHORT 260
#define INT 261
#define CHAR 262
#define FLOAT 263
#define DOUBLE 264
#define VOID 265
#define SIZEOF 266
#define STRUCT 267
#define DELIM 268
#define IF 269
#define ELSE 270
#define SWITCH 271
#define CASE 272
#define DEFAULT 273
#define WHILE 274
#define DO 275
#define FOR 276
#define RETURN 277
#define BREAK 278
#define CONTINUE 279
#define ID 280
#define CONST_INT 281
#define CONST_FLOAT 282
#define CONST_CHAR 283
#define CONST_STRING 284
#define MEMBER 285
#define INCDEC 286
#define UPLUSMINUS 287
#define BITSHIFT 288
#define RELATION 289
#define EQNEQ 290
#define BITAND 291
#define BITXOR 292
#define BITOR 293
#define LOGICALAND 294
#define LOGICALOR 295
#define ASSIGNMENT 296
#define TERNARY 297
#define PREFIXINCDEC 298
#define DEREF 299
#define REF 300
#define POSTFIXINCDEC 301
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union{
    int intVal;
    char stringVal[1000];
    float floatVal;

    struct Node *nodeVal;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
