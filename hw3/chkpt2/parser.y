%{
// %prec
// https://stackoverflow.com/questions/5330541/what-does-prec-mean-here-in-yacc
// Declared section
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "code.h"

int yylex();
int yyerror();
char buff[100000000];

FILE *f_asm = NULL;
int isInstallingSymbol = 0;
extern struct symbol_entry table[MAX_TABLE_SIZE];
char cur_fn[500];
%}

%union{
    int intVal;
    char stringVal[1000];
    float floatVal;
    struct Func fnVal;
}

%type <stringVal> program programComp

%type<stringVal> stmts stmt
%type<stringVal> ifElseStmt switchStmt whileStmt forStmt returnStmt breakStmt continueStmt compoundStmt
%type<stringVal> switchClauses switchClause
%type<stringVal> forExpr

%type<stringVal> delType delLongShort
%type<stringVal> const

%type<stringVal> delVarsScalar delVarsArray
%type<intVal> delScalars 
%type<stringVal> delScalar
%type<stringVal> delArrays delArray delArrayDims
%type<stringVal> arrayContents arrayContent
%token<stringVal> CONST SIGNED_UNSIGNED LONG SHORT
%token<stringVal> INT CHAR FLOAT DOUBLE VOID
%token<stringVal> SIZEOF

%token<typeName> STRUCT
%token<stringVal> DELIM

// %type<stringVal> delFn delFnParams delFnParam
// :gen:
%type<fnVal> delFn
// count of params
%type<intVal> delFnParams delFnParam

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

// %type<stringVal> expr
// :gen:
%type<stringVal> expr
%type<stringVal> factor mutable immutable
%type<intVal> args

%%

program: programComp {EMPTY_STR_ASSIGN($$);}
       | program programComp {EMPTY_STR_ASSIGN($$);}
       ;
programComp: delVarsScalar {EMPTY_STR_ASSIGN($$);}
| delVarsArray {EMPTY_STR_ASSIGN($$);}
// pop up extra installed symbols, symbols are used only in func_def
| delFn ';' {
    gen_func_del($1.name);
    set_global_vars($1.name);
    pop_symbol_table_count($1.numArgs);
    EMPTY_STR_ASSIGN($$);
    install_symbol($1.name);
}

// func_def
// @@ original
// | delFn compoundStmt
// @@ This will let cases like int f(int){} pass
// delFn may be constrained to the condition that parameter must be named
| delFn 
'{' {
    strcpy(cur_fn, $1.name);
    cur_scope++;
    gen_func_def_label($1.name);
    gen_func_callee_prologue($1.name);
    stack_for_args($1.name);
    strcpy($$, $1.name);
    
    // set_scope_and_offset_of_param($1.name);
    // code_gen_func_header($1.name);
}
stmts '}' {
    strcpy(cur_fn, "");
    // delte the extra allocated space for local vars
    unstack_for_local_vars_and_args($1.name);
    gen_func_callee_epilogue($1.name);
    pop_up_symbol(cur_scope);
    cur_scope--;
}
;
// expr
expr:
'(' expr ')' {
    // PUSH_STACK_t0();
    strcpy($$, $2);
}
// @@ all assignment swich
// | expr '=' expr {EXPR_OP_EXPR($1, "=", $3, $$);}
| expr assignment expr %prec ASSIGNMENT {
    if (!strcmp($1, "")) perror("improper expression at LHS");
    int index = look_up_symbol($1);
    fprintf(f_asm, "  // %s assigned\n", $1);
    if(($1)[0] == '*'){ // @@ error prone
        POP_STACK_t0(); // *var (address it is pointing to)
        POP_STACK_t1(); // rhs
        fprintf(f_asm, "  // %s reference assignment\n", $1);
        fprintf(f_asm, "  sd t1, 0(t0)\n");
        PUSH_STACK_t1();
    }
    else{
        fprintf(f_asm, "  // symbol %s at index %d\n", $1, index);
        POP_STACK_t0();
        POP_STACK_t1();
        fprintf(f_asm, "  sd t0, %d(fp)\n", table[index].offset);
        // PUSH_STACK_t0();
        PUSH_STACK_t1();
    }
    fprintf(f_asm, "  // end %s assigned\n", $1);
    strcpy($$, $1);
}
| expr '+' expr {
    POP_STACK_t1();
    POP_STACK_t0();
    fprintf(f_asm, "  add t0, t0, t1\n");
    PUSH_STACK_t0();
    EMPTY_STR_ASSIGN($$);
    fprintf(f_asm, "  // end add\n");
}
| expr '-' expr {
    POP_STACK_t1();
    POP_STACK_t0();
    fprintf(f_asm, "  sub t0, t0, t1\n");
    PUSH_STACK_t0();
    EMPTY_STR_ASSIGN($$);
    fprintf(f_asm, "  // end sub\n");
}
| expr '*' expr {
    POP_STACK_t1();
    POP_STACK_t0();
    fprintf(f_asm, "  mul t0, t0, t1\n");
    PUSH_STACK_t0();
    EMPTY_STR_ASSIGN($$);
    fprintf(f_asm, "  // end mul\n");
}
| expr '/' expr {
    POP_STACK_t1();
    POP_STACK_t0();
    fprintf(f_asm, "  div t0, t0, t1\n");
    PUSH_STACK_t0();
    EMPTY_STR_ASSIGN($$);
    fprintf(f_asm, "  // end div\n");
}
| expr '%' expr {EMPTY_STR_ASSIGN($$);}
| expr '|' expr {EMPTY_STR_ASSIGN($$);}
| expr '&' expr {EMPTY_STR_ASSIGN($$);}
| expr '^' expr {EMPTY_STR_ASSIGN($$);}
| expr LOGICALOR expr {EMPTY_STR_ASSIGN($$);}
| expr LOGICALAND expr {EMPTY_STR_ASSIGN($$);}
| expr EQNEQ expr {EMPTY_STR_ASSIGN($$);}
| expr RELATION expr {EMPTY_STR_ASSIGN($$);}
| expr BITSHIFT expr {EMPTY_STR_ASSIGN($$);}
// unary
| '+' expr %prec UPLUSMINUS {
    fprintf(f_asm, "  // UPLUS\n");
    strcpy($$, $2);
}
| '-' expr %prec UPLUSMINUS {
    POP_STACK_t0();
    fprintf(f_asm, "  sub t0, zero, t0 // UMINUS\n");
    PUSH_STACK_t0();
    strcpy($$, $2);
}
| '&' expr %prec REF {
    fprintf(f_asm, "  // evalute &%s\n", $2);
    POP_STACK_t0();
    int index = look_up_symbol($2);
    fprintf(f_asm, "  addi t0, fp, %d // deref\n", table[index].offset);
    PUSH_STACK_t0();
    fprintf(f_asm, "  // end evalute &%s\n", $2);
    strcpy($$, $2);
}
| '*' expr %prec DEREF { // evaluate *var
    int index = look_up_symbol($2);
    fprintf(f_asm, "  // evalute *%s\n", $2);
    POP_STACK_t0();
    fprintf(f_asm, "  addi t0, fp, %d\n", table[index].offset);
    fprintf(f_asm, "  ld t0, 0(t0)\n");
    PUSH_STACK_t0();
    fprintf(f_asm, "  // end evalute *%s\n", $2);
    strcpy($$, "*");
    strcat($$, $2);
}
| '!' expr {EMPTY_STR_ASSIGN($$);}
| '~' expr {EMPTY_STR_ASSIGN($$);}
| INCDEC expr %prec PREFIXINCDEC {EMPTY_STR_ASSIGN($$);}
| expr INCDEC %prec POSTFIXINCDEC {EMPTY_STR_ASSIGN($$);}
| factor {}

// func invocation
| expr 
'(' {
    POP_STACK_t0();
    fprintf(f_asm, "  // pop for func id\n");
    gen_func_caller_saved($1);
}
args ')' { // args are prepared
    strcpy($$, $1);
    fprintf(f_asm, "  jal ra, %s\n", $1);
    gen_func_caller_restored($1);
    // ret is on stack top
}
// end func invocation

| '(' delType ')' expr {EMPTY_STR_ASSIGN($$);}
| expr MEMBER ID %prec MEMBER {EMPTY_STR_ASSIGN($$);}

// @@ ternary
// | expr '?' expr ':' expr %prec TERNARY {}
;

assignment: 
'=' {strcpy($$, "=");}
| ASSIGNMENT {strcpy($$, $1);}
;

factor: 
mutable {strcpy($$, $1);}
| immutable {strcpy($$, $1);}
;
mutable: 
ID {
    int index;
    index =look_up_symbol($1);
    fprintf(f_asm, "  ld t0, %d(fp)\n", table[index].offset);
    PUSH_STACK_t0();
    strcpy($$, $1);
}
// ! Not handled
| ID delArrayDims {perror("Not handled array in mutable"); strcpy($$, $1);}
;

immutable:
const {
    strcpy($$, $1);
    fprintf(f_asm,"  li t0, %s\n", $1);
    PUSH_STACK_t0();
}
;

// load into a0-7 registers
// args means the ith register should be filled
args:
expr {
    POP_STACK_t0();
    fprintf(f_asm, "  addi a0, t0, 0\n");
    $$ = 1;
}
| args ',' expr {
    POP_STACK_t0();
    fprintf(f_asm, "  addi a%d, t0, 0\n", $1);
    $$ = $1 + 1;
}
| {$$ = 0;}
;


// stringVal to stringVal
delType: INT {strcpy($$, $1);}
       | delLongShort INT {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | SIGNED_UNSIGNED INT {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | SIGNED_UNSIGNED delLongShort INT {sprintf(buff, "%s%s%s", $1, $2, $3); strcpy($$, buff);}
       | CONST INT {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST delLongShort INT {sprintf(buff, "%s%s%s", $1, $2, $3); strcpy($$, buff);}
       | CONST SIGNED_UNSIGNED INT {sprintf(buff, "%s%s%s", $1, $2, $3); strcpy($$, buff);}
       | CONST SIGNED_UNSIGNED delLongShort INT {sprintf(buff, "%s%s%s%s", $1, $2, $3, $4); strcpy($$, buff);}
       
       | CHAR {sprintf(buff, "%s", $1); strcpy($$, buff);}
       | delLongShort {sprintf(buff, "%s", $1); strcpy($$, buff);}
       | SIGNED_UNSIGNED CHAR {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | SIGNED_UNSIGNED delLongShort {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST CHAR {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST delLongShort {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST SIGNED_UNSIGNED CHAR {sprintf(buff, "%s%s%s", $1, $2, $3); strcpy($$, buff);}
       | CONST SIGNED_UNSIGNED delLongShort {sprintf(buff, "%s%s%s", $1, $2, $3); strcpy($$, buff);}
       
       | SIGNED_UNSIGNED {sprintf(buff, "%s", $1); strcpy($$, buff);}
       | FLOAT {sprintf(buff, "%s", $1); strcpy($$, buff);}
       | DOUBLE {sprintf(buff, "%s", $1); strcpy($$, buff);}
       | VOID {sprintf(buff, "%s", $1); strcpy($$, buff);}
       | CONST SIGNED_UNSIGNED {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST FLOAT {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST DOUBLE {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       | CONST VOID {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
       
       | CONST {sprintf(buff, "%s", $1); strcpy($$, buff);}
;
delLongShort: LONG {sprintf(buff, "%s", $1); strcpy($$, buff);}
            | LONG LONG {sprintf(buff, "%s%s", $1, $2); strcpy($$, buff);}
            | SHORT {sprintf(buff, "%s", $1); strcpy($$, buff);}
;
const: CONST_INT {sprintf($$, "%d", $1);}
     | CONST_FLOAT {sprintf($$, "%f", $1);}
     | CONST_CHAR {strcpy($$, $1);}
     | CONST_STRING {strcpy($$, $1);}
;
// scalar_decl
delVarsScalar: 
delType delScalars ';' {}
;
// array_decl
delVarsArray: 
delType delArrays ';' {}
;


delScalars: 
delScalar {}
| delScalars ',' delScalar {
}
;
delScalar:
ID {
    install_symbol($1);
    if(cur_scope == GLOBAL_SCOPE){
        set_global_vars($1);
    }
    else{
        // set_local_vars($1);
        stack_for_local_vars($1, cur_fn);
    }
    strcpy($$, $1);
}
| ID '=' expr {
    POP_STACK_t0();
    install_symbol($1);
    if(cur_scope == GLOBAL_SCOPE){
        set_global_vars($1);
    }
    else{
        int index = look_up_symbol($1);
        stack_for_local_vars($1, cur_fn);
        fprintf(f_asm, "  sd t0, %d(fp) // local var \"%s\" assigned\n", table[index].offset, $1);
    }
    strcpy($$, $1);
}
| '*' ID {
    int index = install_symbol($2);
    // table[index].type = T_PTR;
    if(cur_scope == GLOBAL_SCOPE){
        set_global_vars($2);
    }
    else{
        stack_for_local_vars($2, cur_fn);
    }
    strcpy($$, $2);
}
| '*' ID '=' expr {
    POP_STACK_t0();
    int index = install_symbol($2);
    // table[index].type = T_PTR;
    if(cur_scope == GLOBAL_SCOPE){
        set_global_vars($2);
        perror("gloval var assignment not handled");
    }
    else{
        stack_for_local_vars($2, cur_fn);
        fprintf(f_asm, "  sd t0, %d(fp) // local var ptr\"%s\" assigned\n", table[index].offset, $2);
    }
    strcpy($$, $2);
}
;

delArrays: 
delArray {}
| delArrays ',' delArray {}
;
delArray: 
ID delArrayDims {}
| ID delArrayDims '=' '{' arrayContents '}' {}
;
delArrayDims: 
'[' expr ']' {}
| delArrayDims '[' expr ']' {}
;

arrayContents: 
arrayContent {}
| arrayContents ',' arrayContent {}
;
arrayContent:
'{' arrayContents '}' {}
| expr {}
;

// func_decl
delFn: 
delType ID '(' delFnParams ')' {
    strcpy($$.name, $2);
    $$.numArgs = $4;
}
// ! not handled
| delType '*' ID '(' delFnParams ')' {fprintf(stderr, "delType\n"); exit(1);}
;

delFnParams: 
delFnParam {$$=1;}
| delFnParams ',' delFnParam {$$=$1+1;}
| {}
;
delFnParam: 
// ! not handled
  delType {fprintf(stderr, "delFnParam without var name\n"); exit(1);}
// ! not handled
| delType '*' {fprintf(stderr, "delFnParam without var name\n"); exit(1);}
// :gen:
| delType ID {install_symbol($2); $$=1;}
// :gen:
| delType '*' ID {install_symbol($3); $$=1;}
;

stmts: 
stmts stmt {}
| {}
;

// stmt
stmt:
expr ';' {
    POP_STACK_t0();
    fprintf(f_asm, "  // pop for end of expr\n");
}
| ifElseStmt {}
| switchStmt {}
| whileStmt {}
| forStmt {}
| returnStmt {}
| breakStmt {}
| continueStmt {}
| compoundStmt {}
| delVarsScalar {}
| delVarsArray {}
;
ifElseStmt: 
IF '(' expr')' compoundStmt {}
;
switchStmt: 
SWITCH '(' expr ')' '{' switchClauses '}' {}
| SWITCH '(' expr ')' '{' '}' {}
;
switchClauses: 
switchClause {}
| switchClauses switchClause {}
;
switchClause: 
CASE expr ':' stmts {}
| DEFAULT ':' stmts {}
;
whileStmt: 
WHILE '(' expr ')' stmt {}
| DO stmt WHILE '(' expr ')' ';' {}
;
forStmt: 
FOR '(' forExpr ';' forExpr ';' forExpr ')' stmt {}
;
forExpr: 
expr {}
| {}
;
returnStmt: 
RETURN expr ';' {}
| RETURN ';' {}
;
breakStmt: 
BREAK ';' {}
;
continueStmt:
CONTINUE ';' {}
;

compoundStmt:
'{' stmts '}' {}
;
%%
// C code section

int main()
{
    f_asm = fopen("codegen.S", "w");
    init_symbol_table();
    yyparse();
    fclose(f_asm);
    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}