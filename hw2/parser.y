%{
// %prec
// https://stackoverflow.com/questions/5330541/what-does-prec-mean-here-in-yacc
// Declared section
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int yylex();
int yyerror();
char buff[100000000];

int isLog = 1;
void logger(char *p);

typedef enum
{
    text,
    scalar_decl,
    array_decl,
    func_decl,
    func_def,
    expr,
    stmt,
    none
}Tag;

struct Node
{
    Tag tag;
    char *context;
    struct Node *next, *down;
};


struct Node* initNode();
struct Node* makeStrNode(char *c);
void linkDown(struct Node *p, struct Node *dp);
void linkNext(struct Node *p, struct Node *np);
void printNode(struct Node *p);

#define EXPR_OP_EXPR(exp1, op, exp2, parent){\
    struct Node *p1 = (exp1);\
    struct Node *p2 = makeStrNode((op));\
    struct Node *p3 = (exp2);\
    linkNext(p1, p2);\
    linkNext(p2, p3);\
    (parent) = initNode();\
    (parent)->tag = expr;\
    linkDown((parent), p1);\
}

#define OP_EXPR(op, exp, parent){\
    struct Node *p1 = makeStrNode((op));\
    struct Node *p2 = (exp);\
    linkNext(p1,p2);\
    (parent)= initNode();\
    (parent)->tag = expr;\
    linkDown((parent), p1);\
}

%}

%union{
    int intVal;
    char stringVal[1000];
    float floatVal;

    struct Node *nodeVal;
}

%type <nodeVal> program programComp

%type<nodeVal> stmts stmt
%type<nodeVal> ifElseStmt switchStmt whileStmt forStmt returnStmt breakStmt continueStmt compoundStmt
%type<nodeVal> switchClauses switchClause
%type<nodeVal> forExpr

%type<nodeVal> delType delLongShort
%type<nodeVal> const

%type<nodeVal> delVarsScalar delVarsArray
%type<nodeVal> delScalars delScalar
%type<nodeVal> delArrays delArray delArrayDims
%type<nodeVal> arrayContents arrayContent
%token<stringVal> CONST SIGNED_UNSIGNED LONG SHORT
%token<stringVal> INT CHAR FLOAT DOUBLE VOID
%token<stringVal> SIZEOF

%token<typeName> STRUCT
%token<stringVal> DELIM

%type<nodeVal> delFn delFnParams delFnParam

%token<stringVal> IF ELSE SWITCH CASE DEFAULT WHILE DO FOR RETURN BREAK CONTINUE

%token<stringVal> ID
%token<intVal> CONST_INT
%token<floatVal> CONST_FLOAT
%token<stringVal> CONST_CHAR CONST_STRING

%start program

%token<stringVal> MEMBER
%token<stringVal> INCDEC UPLUSMINUS BITSHIFT RELATION EQNEQ 
%token<stringVal> BITAND BITXOR BITOR
%token<stringVal> LOGICALAND LOGICALOR
%token<stringVal> ASSIGNMENT
%type<stringVal> assignment

%left ';'
%left ID
%left ','
%right '=' ASSIGNMENT
%right TERNARY
%left LOGICALOR
%left LOGICALAND
%left '|'
%left '^'
%left '&'
%left EQNEQ
%left RELATION
%left BITSHIFT
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' PREFIXINCDEC UPLUSMINUS DEREF REF
%left '(' ')' '[' ']' POSTFIXINCDEC MEMBER
%nonassoc INCDEC

%type<nodeVal> expr
%type<nodeVal> factor mutable immutable
%type<nodeVal> args

%%

program: programComp {}
       | program programComp {}
       ;
programComp: delVarsScalar {printNode($1);}
| delVarsArray {printNode($1);}
// func_decl
| delFn ';' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(";");
    linkNext(p1, p2);
    $$ = initNode();
    $$->tag = func_decl;
    linkDown($$, p1);
    printNode($$);
}
// func_def
// @@ This will let cases like int f(int){} pass
// delFn may be constrained to the condition that parameter must be named
| delFn compoundStmt {
    struct Node *p1 = $1;
    struct Node *p2 = $2;
    linkNext(p1, p2);
    $$ = initNode();
    $$->tag = func_def;
    linkDown($$, p1);
    printNode($$);
}
;
// expr
expr:
'(' expr ')' {
    struct Node *p1 = makeStrNode("(");
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode(")");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    $$->tag = expr;
    linkDown($$, p1);
}
// @@ all assignment swich
// | expr '=' expr {EXPR_OP_EXPR($1, "=", $3, $$);}
| expr assignment expr %prec ASSIGNMENT {EXPR_OP_EXPR($1, $2, $3, $$);}

| expr '+' expr {EXPR_OP_EXPR($1, "+", $3, $$);}
| expr '-' expr {EXPR_OP_EXPR($1, "-", $3, $$);}
| expr '*' expr {EXPR_OP_EXPR($1, "*", $3, $$);}
| expr '/' expr {EXPR_OP_EXPR($1, "/", $3, $$);}
| expr '%' expr {EXPR_OP_EXPR($1, "%", $3, $$);}
| expr '|' expr {EXPR_OP_EXPR($1, "|", $3, $$);}
| expr '&' expr {EXPR_OP_EXPR($1, "&", $3, $$);}
| expr '^' expr {EXPR_OP_EXPR($1, "^", $3, $$);}
| expr LOGICALOR expr {EXPR_OP_EXPR($1, $2, $3, $$);}
| expr LOGICALAND expr {EXPR_OP_EXPR($1, $2, $3, $$);}
| expr EQNEQ expr {EXPR_OP_EXPR($1, $2, $3, $$);}
| expr RELATION expr {EXPR_OP_EXPR($1, $2, $3, $$);}
| expr BITSHIFT expr {EXPR_OP_EXPR($1, $2, $3, $$);}
// unary
| '+' expr %prec UPLUSMINUS {OP_EXPR("+", $2, $$);}
| '-' expr %prec UPLUSMINUS {OP_EXPR("-", $2, $$);}
| '&' expr %prec REF {OP_EXPR("&", $2, $$);}
| '*' expr %prec DEREF {OP_EXPR("*", $2, $$);}
| '!' expr {OP_EXPR("!", $2, $$);}
| '~' expr {OP_EXPR("~", $2, $$);}
| INCDEC expr %prec PREFIXINCDEC {OP_EXPR($1, $2, $$);}
| expr INCDEC %prec POSTFIXINCDEC {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode($2);
    linkNext(p1,p2);
    $$ = initNode();
    $$->tag = expr;
    linkDown($$, p1);
}
| factor {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = expr;
    linkDown($$, p1);
}
| expr '(' args ')' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(")");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    $$ = initNode();\
    $$->tag = expr;\
    linkDown($$, p1);\
}
| '(' delType ')' expr {
    struct Node *p1 = makeStrNode("(");
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode(")");
    struct Node *p4 = $4;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    $$ = initNode();\
    $$->tag = expr;\
    linkDown($$, p1);\
}
| expr MEMBER ID %prec MEMBER {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode($2);
    struct Node *p3 = makeStrNode($3);
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    $$->tag = expr;
    linkDown($$, p1);
}
// @@ ternary
// | expr '?' expr ':' expr %prec TERNARY {
//     struct Node *p1 = $1;
//     struct Node *p2 = makeStrNode("?");
//     struct Node *p3 = $3;
//     struct Node *p4 = makeStrNode(":");
//     struct Node *p5 = $5;
//     linkNext(p1, p2);
//     linkNext(p2, p3);
//     linkNext(p3, p4);
//     linkNext(p4, p5);
//     $$ = initNode();
//     $$->tag = expr;
//     linkDown($$, p1);
// }
;

assignment: '=' {strcpy($$, "=");}
          | ASSIGNMENT {strcpy($$, $1);}
;

factor: 
mutable {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| immutable {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
;
mutable: 
ID {$$ = makeStrNode($1);}
| ID delArrayDims {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = $2;
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
;
immutable:
const {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
;
args:
expr {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| args ',' expr {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(",");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
| {
    $$ = initNode();
}
;


// stringVal to nodeVal
delType: INT {$$ = makeStrNode($1);}
       | delLongShort INT {sprintf(buff, "%s%s", $1->context, $2); $$ = makeStrNode(buff);}
       | SIGNED_UNSIGNED INT {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | SIGNED_UNSIGNED delLongShort INT {sprintf(buff, "%s%s%s", $1, $2->context, $3); $$ = makeStrNode(buff);}
       | CONST INT {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | CONST delLongShort INT {sprintf(buff, "%s%s%s", $1, $2->context, $3); $$ = makeStrNode(buff);}
       | CONST SIGNED_UNSIGNED INT {sprintf(buff, "%s%s%s", $1, $2, $3); $$ = makeStrNode(buff);}
       | CONST SIGNED_UNSIGNED delLongShort INT {sprintf(buff, "%s%s%s%s", $1, $2, $3->context, $4); $$ = makeStrNode(buff);}
       
       | CHAR {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
       | delLongShort {sprintf(buff, "%s", $1->context); $$ = makeStrNode(buff);}
       | SIGNED_UNSIGNED CHAR {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | SIGNED_UNSIGNED delLongShort {sprintf(buff, "%s%s", $1, $2->context); $$ = makeStrNode(buff);}
       | CONST CHAR {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | CONST delLongShort {sprintf(buff, "%s%s", $1, $2->context); $$ = makeStrNode(buff);}
       | CONST SIGNED_UNSIGNED CHAR {sprintf(buff, "%s%s%s", $1, $2, $3); $$ = makeStrNode(buff);}
       | CONST SIGNED_UNSIGNED delLongShort {sprintf(buff, "%s%s%s", $1, $2, $3->context); $$ = makeStrNode(buff);}
       
       | SIGNED_UNSIGNED {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
       | FLOAT {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
       | DOUBLE {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
       | VOID {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
       | CONST SIGNED_UNSIGNED {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | CONST FLOAT {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | CONST DOUBLE {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       | CONST VOID {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
       
       | CONST {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
;
delLongShort: LONG {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
            | LONG LONG {sprintf(buff, "%s%s", $1, $2); $$ = makeStrNode(buff);}
            | SHORT {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
;
const: CONST_INT {sprintf(buff, "%d", $1); $$ = makeStrNode(buff);}
     | CONST_FLOAT {sprintf(buff, "%f", $1); $$ = makeStrNode(buff);}
     | CONST_CHAR {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
     | CONST_STRING {sprintf(buff, "%s", $1); $$ = makeStrNode(buff);}
;
// scalar_decl
delVarsScalar: 
delType delScalars ';' {
    struct Node *p1 = $1;
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode(";");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    $$->tag = scalar_decl;
    linkDown($$, p1);
}
;
// array_decl
delVarsArray: 
delType delArrays ';' {
    struct Node *p1 = $1;
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode(";");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    $$->tag = array_decl;
    linkDown($$, p1);
}
;


delScalars: 
delScalar {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| delScalars ',' delScalar {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(",");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
;
delScalar:
ID {
    $$ = makeStrNode($1);
}
| ID '=' expr {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("=");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);  
    $$ = initNode();
    linkDown($$, p1);
}
| '*' ID {
    struct Node *p1 = makeStrNode("*");
    struct Node *p2 = makeStrNode($2);
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
| '*' ID '=' expr {
    struct Node *p1 = makeStrNode("*");
    struct Node *p2 = makeStrNode($2);
    struct Node *p3 = makeStrNode("=");
    struct Node *p4 = $4;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    $$ = initNode();
    linkDown($$, p1);
}
;

delArrays: 
delArray {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| delArrays ',' delArray {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(",");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
;
delArray: 
ID delArrayDims {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = $2;
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
| ID delArrayDims '=' '{' arrayContents '}' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode("=");
    struct Node *p4 = makeStrNode("{");
    struct Node *p5 = $5;
    struct Node *p6 = makeStrNode("}");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    $$ = initNode();
    linkDown($$, p1);
}
;
delArrayDims: 
'[' expr ']' {
    struct Node *p1 = makeStrNode("[");
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode("]");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
| delArrayDims '[' expr ']' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode("[");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode("]");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    $$ = initNode();
    linkDown($$, p1);
}
;

arrayContents: 
arrayContent {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| arrayContents ',' arrayContent {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(",");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
;
arrayContent:
'{' arrayContents '}' {
    struct Node *p1 = makeStrNode("{");
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode("}");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
| expr {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
;

delFn: 
delType ID '(' delFnParams ')' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode($2);
    struct Node *p3 = makeStrNode("(");
    struct Node *p4 = $4;
    struct Node *p5 = makeStrNode(")");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    $$ = initNode();
    linkDown($$, p1);
}
| delType '*' ID '(' delFnParams ')' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode("*");
    struct Node *p3 = makeStrNode($3);
    struct Node *p4 = makeStrNode("(");
    struct Node *p5 = $5;
    struct Node *p6 = makeStrNode(")");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    $$ = initNode();
    linkDown($$, p1);
}
;
delFnParams: 
delFnParam {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| delFnParams ',' delFnParam {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(",");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
| {
    $$ = initNode();
}
;
delFnParam: 
delType {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| delType '*' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode("*");
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
| delType ID {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode($2);
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);    
}
| delType '*' ID {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode("*");
    struct Node *p3 = makeStrNode($3);
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
;

stmts: 
stmts stmt {
    struct Node *p1 = $1;
    struct Node *p2 = $2;
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);    
}
| {
    $$ = initNode();
}
;

// stmt
stmt:
expr ';' {
    struct Node *p1 = $1;
    struct Node *p2 = makeStrNode(";");
    linkNext(p1, p2);
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);    
}
| ifElseStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| switchStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| whileStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| forStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| returnStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| breakStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| continueStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| compoundStmt {
    struct Node *p1 = $1;
    $$ = initNode();
    $$->tag = stmt;
    linkDown($$, p1);
}
| delVarsScalar {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| delVarsArray {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
;
ifElseStmt: 
IF '(' expr')' compoundStmt {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(")");
    struct Node *p5 = $5;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    $$ = initNode();
    linkDown($$, p1);
}
| IF '(' expr ')' compoundStmt ELSE compoundStmt {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(")");
    struct Node *p5 = $5;
    struct Node *p6 = makeStrNode($6);
    struct Node *p7 = $7;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    linkNext(p6, p7);
    $$ = initNode();
    linkDown($$, p1);
}
;
switchStmt: 
SWITCH '(' expr ')' '{' switchClauses '}' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(")");
    struct Node *p5 = makeStrNode("{");
    struct Node *p6 = $6;
    struct Node *p7 = makeStrNode("}");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    linkNext(p6, p7);
    $$ = initNode();
    linkDown($$, p1);
}
| SWITCH '(' expr ')' '{' '}' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(")");
    struct Node *p5 = makeStrNode("{");
    struct Node *p6 = makeStrNode("}");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    $$ = initNode();
    linkDown($$, p1);
}
;
switchClauses: 
switchClause {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| switchClauses switchClause {
    struct Node *p1 = $1;
    struct Node *p2 = $2;
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
;
switchClause: 
CASE expr ':' stmts {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode(":");
    struct Node *p4 = $4;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    $$ = initNode();
    linkDown($$, p1);
}
| DEFAULT ':' stmts {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode(":");
    struct Node *p3 = $3;
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
;
whileStmt: 
WHILE '(' expr ')' stmt {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(")");
    struct Node *p5 = $5;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    $$ = initNode();
    linkDown($$, p1);
}
| DO stmt WHILE '(' expr ')' ';' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode($3);
    struct Node *p4 = makeStrNode("(");
    struct Node *p5 = $5;
    struct Node *p6 = makeStrNode(")");
    struct Node *p7 = makeStrNode(";");
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    linkNext(p6, p7);
    $$ = initNode();
    linkDown($$, p1);
}
;
forStmt: 
FOR '(' forExpr ';' forExpr ';' forExpr ')' stmt {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode("(");
    struct Node *p3 = $3;
    struct Node *p4 = makeStrNode(";");
    struct Node *p5 = $5;
    struct Node *p6 = makeStrNode(";");
    struct Node *p7 = $7;
    struct Node *p8 = makeStrNode(")");
    struct Node *p9 = $9;
    linkNext(p1, p2);
    linkNext(p2, p3);
    linkNext(p3, p4);
    linkNext(p4, p5);
    linkNext(p5, p6);
    linkNext(p6, p7);
    linkNext(p7, p8);
    linkNext(p8, p9);
    $$ = initNode();
    linkDown($$, p1);
}
;
forExpr: 
expr {
    struct Node *p1 = $1;
    $$ = initNode();
    linkDown($$, p1);
}
| {
    $$ = initNode();
}
;
returnStmt: 
RETURN expr ';' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode(";");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
| RETURN ';' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode(";");
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
;
breakStmt: 
BREAK ';' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode(";");
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
;
continueStmt:
CONTINUE ';' {
    struct Node *p1 = makeStrNode($1);
    struct Node *p2 = makeStrNode(";");
    linkNext(p1, p2);
    $$ = initNode();
    linkDown($$, p1);
}
;

compoundStmt:
'{' stmts '}' {
    struct Node *p1 = makeStrNode("{");
    struct Node *p2 = $2;
    struct Node *p3 = makeStrNode("}");
    linkNext(p1, p2);
    linkNext(p2, p3);
    $$ = initNode();
    linkDown($$, p1);
}
;
%%
// C code section

int main()
{
    yyparse();
    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}

void logger(char *p)
{
    // printf("%s\n", p);
}

struct Node* initNode()
{
    struct Node *p = (struct Node*)malloc(sizeof(struct Node));
    p->tag = none;
    p->next = p->down = NULL;
    p->context = NULL;
    return p;
}
struct Node* makeStrNode(char *c)
{
    struct Node *p = initNode();
    p->tag = text;
    p->context = (char*)malloc(strlen(c)+1);
    strcpy(p->context, c);
    return p;
}
void linkDown(struct Node *p, struct Node *dp)
{
    p->down = dp;
}
void linkNext(struct Node *p, struct Node *np)
{
    p->next = np;
}
void printNode(struct Node *p)
{
    if(!p) return;
    // simply no[
    // if(p->tag == none) printf("Got None");

    if(p->tag == text && p->context) printf("%s", p->context);
    else if(p->tag == scalar_decl) printf("<scalar_decl>");
    else if(p->tag == array_decl) printf("<array_decl>");
    else if(p->tag == func_decl) printf("<func_decl>");
    else if(p->tag == func_def) printf("<func_def>");
    else if(p->tag == expr) printf("<expr>");
    else if(p->tag == stmt) printf("<stmt>");
    
    if(p->down) printNode(p->down);

    if(p->tag == scalar_decl) printf("</scalar_decl>");
    else if(p->tag == array_decl) printf("</array_decl>");
    else if(p->tag == func_decl) printf("</func_decl>");
    else if(p->tag == func_def) printf("</func_def>");
    else if(p->tag == expr) printf("</expr>");
    else if(p->tag == stmt) printf("</stmt>");

    printNode(p->next);

    free(p);
}