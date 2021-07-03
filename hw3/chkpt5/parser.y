%{
// %prec
// https://stackoverflow.com/questions/5330541/what-does-prec-mean-here-in-yacc
// Declared section
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "code.h"
#include "Node.h"

int yylex();
int yyerror();
char buff[100000000];

FILE *f_asm = NULL;
extern struct symbol_entry table[MAX_TABLE_SIZE];
// @@ buggy
char cur_fn[500];

%}

%union{
    int intVal;
    char stringVal[1000];
    float floatVal;
    struct Func fnVal;
    struct Node *nodeVal;
}

%type <stringVal> program programComp

// # expr
%type<nodeVal> expr
%type<nodeVal> factor
%token<stringVal> MEMBER
%token<stringVal> INCDEC UPLUSMINUS BITSHIFT RELATION EQNEQ 
%token<stringVal> BITAND BITXOR BITOR
%token<stringVal> LOGICALAND LOGICALOR
%token<stringVal> ASSIGNMENT
%type<stringVal> assignment
// # stmt
%type<stringVal> stmts stmt
%type<stringVal> ifElseStmt switchStmt whileStmt forStmt returnStmt breakStmt continueStmt
// %type<stringVal> compoundStmt
%type<stringVal> ifStmt
%type<stringVal> switchClauses switchClause
%type<nodeVal> forExpr

// # declare func
%type<fnVal> delFn
// %type<intVal> delFnParams delFnParam // count of params
%type<fnVal> delFnParams 
%type<stringVal> delFnParam // count of params
%type<nodeVal> args
// # declare vars
%type<stringVal> delType delLongShort
%type<stringVal> delVarsScalar delVarsArray
%type<intVal> delScalars 
%type<stringVal> delScalar
%type<intVal> delArrays
%type<intVal> delArrayDims // the number of elements
%type<stringVal> delArray

// # type token
%type<nodeVal> const
%type<stringVal> arrayContents arrayContent
%token<stringVal> CONST SIGNED_UNSIGNED LONG SHORT
%token<stringVal> INT CHAR FLOAT DOUBLE VOID
%token<stringVal> SIZEOF
%token<stringVal> ID
%token<intVal> CONST_INT
%token<floatVal> CONST_FLOAT
%token<stringVal> CONST_CHAR CONST_STRING
// # kw token
%token<typeName> STRUCT
%token<stringVal> DELIM
// # control token
%token<stringVal> IF ELSE SWITCH CASE DEFAULT WHILE DO FOR RETURN BREAK CONTINUE

%start program

// # prec
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
%left FUNCCALL
%left '(' ')' '[' ']' POSTFIXINCDEC MEMBER ARRAYACCESS
%nonassoc INCDEC

%%

program: programComp {EMPTY_STR_ASSIGN($$);}
       | program programComp {EMPTY_STR_ASSIGN($$);}
       ;
programComp: delVarsScalar {EMPTY_STR_ASSIGN($$);}
| delVarsArray {EMPTY_STR_ASSIGN($$);}
| delFn ';' {
    gen_func_del($1.name);
    print_fn($1);
    // pop up extra installed symbols
    // symbols are re-installed in func_def
    pop_symbol_table_count($1.numArgs);

    int index = install_symbol($1.name);
    table[index].type = T_FUNCTION;
    table[index].offset = FUNCDEF_OFFSET; // stack offset of args + local vars
    table[index].scope = GLOBAL_SCOPE;
    
    fprintf(f_asm, "// ***** Table after declare a func *****\n");
    print_symbol_table();
    fprintf(f_asm, "// ***** .......................... *****\n");
    EMPTY_STR_ASSIGN($$);
}

// func_def
// @@ This will let cases like int f(int){} pass
// @@ delFn may be constrained to the condition that parameter must be named
| delFn 
'{' {
    strcpy(cur_fn, $1.name);
    gen_func_def_label($1.name);
    gen_func_callee_prologue($1.name);
    
    int index = look_up_symbol($1.name);
    pushLabel(LABEL_TYPE_FUNC, table[index].offset);
    cur_scope++;
    stack_for_args($1, cur_scope);
    strcpy($$, $1.name);    
}
stmts '}' {
    struct Label label = popLabel();
    leaveLocalScope(label, cur_fn, 1);
    gen_func_callee_epilogue(cur_fn);
    
    fprintf(f_asm, "// ***** Before leaving local scope, table *****\n");
    print_symbol_table();
    fprintf(f_asm, "// ***** After  leaving local scope, table *****\n");
    pop_up_symbol(cur_scope);
    print_symbol_table();
    cur_scope--;
    strcpy(cur_fn, "");
}
;
// expr
expr:
'(' expr ')' {$$ = $2;}
// binary
| expr assignment expr %prec ASSIGNMENT {$$ = makeExprNode($1, $3, $2);}
| expr '+' expr {$$ = makeExprNode($1, $3, "+");}
| expr '-' expr {$$ = makeExprNode($1, $3, "-");}
| expr '*' expr {$$ = makeExprNode($1, $3, "*");}
| expr '/' expr {$$ = makeExprNode($1, $3, "/");}
| expr '%' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr '|' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr '&' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr '^' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr LOGICALOR expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr LOGICALAND expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr EQNEQ expr %prec EQNEQ {$$ = makeExprNode($1, $3, $2);}
| expr RELATION expr %prec RELATION {$$ = makeExprNode($1, $3, $2);}
| expr BITSHIFT expr {perror("OP not handled"); $$ = makeEmptyNode();}
// unary
| '+' expr %prec UPLUSMINUS {$$ = $2;}
| '-' expr %prec UPLUSMINUS {
    // 0 - expr
    $$ = makeNode(NODE_TYPE_EXPR, "-");
    nodeAssignLR($$, makeNode(NODE_TYPE_CONST, "0"), $2);
}
| '&' expr %prec REF {$$ = makeNode(NODE_TYPE_REF, EXPR_OP_REF); nodeAssignLR($$, $2, NULL);}
// convert (address of a pointer) to (address where it is pointing to)
| '*' expr %prec DEREF {$$ = makeNode(NODE_TYPE_DEREF, EXPR_OP_DEREF); nodeAssignLR($$, $2, NULL);}
| '!' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| '~' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| INCDEC expr %prec PREFIXINCDEC {perror("++/-- not handled"); $$ = makeEmptyNode();}
| expr INCDEC %prec POSTFIXINCDEC {perror("++/-- not handled"); $$ = makeEmptyNode();}
| factor {$$ = $1;}

// @@ return value not restored
// # func call @@ use ID (args)
// | expr 
| ID '(' args ')' %prec FUNCCALL { // args are prepared
    $$ = makeNode(NODE_TYPE_CALL, $1);
    nodeAssignLR($$, $3, NULL);
}
// end func invocation

| '(' delType ')' expr {perror("OP not handled"); $$ = makeEmptyNode();}
| expr MEMBER ID %prec MEMBER {perror("OP not handled"); $$ = makeEmptyNode();}

// @@ ternary
// | expr '?' expr ':' expr %prec TERNARY {}
;

assignment: 
'=' {strcpy($$, "=");}
// | ASSIGNMENT {strcpy($$, $1);}
;

factor:
const {$$ = $1;}
| ID {$$ = makeNode(NODE_TYPE_VAR, $1);}
| ID '[' expr ']' %prec ARRAYACCESS {
    // *(ID + expr)
    struct Node *nid = makeNode(NODE_TYPE_VAR, $1);
    struct Node *inner = makeExprNode(nid, $3, "+");
    $$ = makeNode(NODE_TYPE_DEREF, EXPR_OP_DEREF);
    nodeAssignLR($$, inner, NULL);
}
;

// load into a0-7 registers
// args means the ith register should be filled
args:
expr {
    $$ = makeNode(NODE_TYPE_ARG, "");
    nodeAssignLR($$, NULL, $1);
}
| args ',' expr {
    // left skewed
    $$ = makeNode(NODE_TYPE_ARG, "");
    nodeAssignLR($$, $1, $3);
}
| {$$ = NULL;}
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
const: CONST_INT {sprintf(buff, "%d", $1); $$ = makeNode(NODE_TYPE_CONST, buff);}
     | CONST_FLOAT {perror("CONST not handled"); $$ = makeEmptyNode();}
     | CONST_CHAR {perror("CONST not handled"); $$ = makeEmptyNode();}
     | CONST_STRING {perror("CONST not handled"); $$ = makeEmptyNode();}
;


// # declare scalar
delVarsScalar: 
delType delScalars ';' {}
;
delScalars: 
    delScalar {}
    | delScalars ',' delScalar {}
;
delScalar:
ID {
    int index = install_symbol($1);
    table[index].type = T_VAR;
    if(cur_scope == GLOBAL_SCOPE) {perror("Global Var not handled");}
    else{
        stack_for_local_vars($1, cur_fn, 1, cur_scope);
    }
    strcpy($$, $1);
}
| ID '=' expr {
    eval($3);
    freeNode($3);
    POP_STACK_t0();

    int index = install_symbol($1);
    if(cur_scope == GLOBAL_SCOPE) {perror("Global Var not handled");}
    else{
        stack_for_local_vars($1, cur_fn, 1, cur_scope);
        fprintf(f_asm, "  sd t0, %d(fp) // local var \"%s\" assigned\n", table[index].offset, $1);
    }
    strcpy($$, $1);
}
| '*' ID {
    int index = install_symbol($2);
    table[index].type = T_PTR;
    if(cur_scope == GLOBAL_SCOPE) {perror("Global Var not handled");}
    else{
        stack_for_local_vars($2, cur_fn, 1, cur_scope);
    }
    strcpy($$, $2);
}
| '*' ID '=' expr {
    eval($4);
    freeNode($4);
    POP_STACK_t0();

    int index = install_symbol($2);
    table[index].type = T_PTR;
    if(cur_scope == GLOBAL_SCOPE) {perror("Global Var not handled");}
    else{
        stack_for_local_vars($2, cur_fn, 1, cur_scope);
        fprintf(f_asm, "  sd t0, %d(fp) // local var ptr\"%s\" assigned\n", table[index].offset, $2);
    }
    strcpy($$, $2);
}
;

// # declare array
delVarsArray: 
delType delArrays ';' {}
;
delArrays: 
delArray {}
| delArrays ',' delArray {}
;
delArray: 
ID delArrayDims {
    int index = install_symbol($1);
    table[index].type = T_PTR;
    table[index].type |= T_ARR;
    if(cur_scope == GLOBAL_SCOPE) {perror("Global Var not handled");}
    else{
        stack_for_local_vars($1, cur_fn, $2, cur_scope);
    }
    strcpy($$, $1);
}
| ID delArrayDims '=' '{' arrayContents '}' {perror("Array init not implemented"); exit(1);}
;
// delArrayDims: 
// @@ old
// '[' expr ']' {}
// | delArrayDims '[' expr ']' {}
// @@ old
delArrayDims: 
'[' CONST_INT ']' {$$ = $2;}
| delArrayDims '[' CONST_INT ']' {$$ = $1 * $3;}
;
arrayContents: 
arrayContent {}
| arrayContents ',' arrayContent {}
;
arrayContent:
'{' arrayContents '}' {}
| expr {}
;

// # declare func
delFn: 
delType ID '(' delFnParams ')' { // func is not installed here
    $$ = $4;
    strcpy($$.name, $2);
}
| delType '*' ID '(' delFnParams ')' {perror("Func decl with return type* not handled"); exit(1);}
;
delFnParams: 
delFnParam {
    int index = look_up_symbol($1);
    $$.numArgs = 1;
    strcpy($$.args[0], $1);
}
| delFnParams ',' delFnParam {
    $$.numArgs = $1.numArgs + 1;
    strcpy($$.args[$1.numArgs], $3);
}
| {$$.numArgs = 0;}
;
delFnParam: 
delType {fprintf(stderr, "delFnParam without var name\n"); exit(1);}
| delType '*' {fprintf(stderr, "delFnParam without var name\n"); exit(1);}
| delType ID {
    int index = install_symbol($2);
    table[index].type = T_VAR;
    strcpy($$, $2);
}
| delType '*' ID {
    int index = install_symbol($3);
    table[index].type = T_PTR;
    strcpy($$, $3);
}
;

stmts: 
stmts stmt {}
| {}
;

// stmt
stmt:
expr ';' {
    eval($1);
    freeNode($1);
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
// | compoundStmt {}
| delVarsScalar {}
| delVarsArray {}
;
ifStmt:
IF '(' expr ')' {
    eval($3);
    freeNode($3);
    POP_STACK_t0();

    int index = look_up_symbol(cur_fn);
    struct Label label = pushLabel(LABEL_TYPE_IFELSE, table[index].offset);
    fprintf(f_asm, "  // begin if %d, before offset=%d\n", label.id, table[index].offset);
    fprintf(f_asm, "  beqz t0, ._L_if_false_%d\n", label.id);
    cur_scope++;
    EMPTY_STR_ASSIGN($$);
}
'{' stmts '}' { // label not popped, may be followed by an ELSE
    struct Label label = popLabel();
    label = pushLabelNamed(label);
    leaveLocalScope(label, cur_fn, 1);
    pop_up_symbol(cur_scope);
    cur_scope--;
}
;
ifElseStmt:
ifStmt {
    struct Label label = popLabel();
    fprintf(f_asm, "._L_if_false_%d:\n", label.id);
    EMPTY_STR_ASSIGN($$);
}
| ifStmt ELSE {
    struct Label label = popLabel();
    label = pushLabelNamed(label);
    fprintf(f_asm, "  j ._L_if_end_%d // else-begin \n", label.id);
    fprintf(f_asm, "._L_if_false_%d:\n", label.id);
    cur_scope++;
    EMPTY_STR_ASSIGN($$);
}
'{' stmts '}' { // _if_end
    struct Label label = popLabel();
    pop_up_symbol(cur_scope);
    cur_scope--;
    leaveLocalScope(label, cur_fn, 1);
    fprintf(f_asm, "._L_if_end_%d:\n", label.id);
    EMPTY_STR_ASSIGN($$);
}

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
WHILE {
    int index = look_up_symbol(cur_fn);
    struct Label label = pushLabel(LABEL_TYPE_FORWHILE, table[index].offset);
    cur_scope++;

    fprintf(f_asm, "  // begin while %d, before offset=%d\n", label.id, table[index].offset);
    fprintf(f_asm, "._L_while_cond_%d:\n", label.id);
    EMPTY_STR_ASSIGN($$);
} 
'(' expr ')' {
    eval($4);
    freeNode($4);
    POP_STACK_t0();
    
    struct Label label = popLabel();
    label = pushLabelNamed(label);
    
    // fprintf(f_asm, "  beqz t0, ._L_while_false_%d\n", label.id);
    fprintf(f_asm, "  beqz t0, .L_%d\n", label.id);
    EMPTY_STR_ASSIGN($$);
}
'{' stmts '}' {
    struct Label label = popLabel();
    leaveLocalScope(label, cur_fn, 1);
    pop_up_symbol(cur_scope);
    cur_scope--;
    fprintf(f_asm, "  j ._L_while_cond_%d\n", label.id);
    // fprintf(f_asm, "._L_while_false_%d:\n", label.id);
    fprintf(f_asm, ".L_%d:\n", label.id);
}
| DO {
    int index = look_up_symbol(cur_fn);
    struct Label label = pushLabel(LABEL_TYPE_FORWHILE, table[index].offset);
    cur_scope++;

    fprintf(f_asm, "  // begin dowhile %d \n", label.id);
    fprintf(f_asm, "._L_dowhile_%d:\n", label.id);
    EMPTY_STR_ASSIGN($$);
}
'{' stmts '}' WHILE '(' expr ')' ';' {
    struct Label label = popLabel();
    pop_up_symbol(cur_scope);
    cur_scope--;
    leaveLocalScope(label, cur_fn, 1);
    eval($8);
    freeNode($8);
    POP_STACK_t0();
    fprintf(f_asm, "  bnez t0, ._L_dowhile_%d\n", label.id);
    EMPTY_STR_ASSIGN($$);

    fprintf(f_asm, ".L_%d:\n", label.id);
}
;
forStmt: 
FOR {
    int index = look_up_symbol(cur_fn);
    struct Label label = pushLabel(LABEL_TYPE_FORWHILE, table[index].offset);
    cur_scope++;
    fprintf(f_asm, "  // begin for %d \n", label.id);
} 
'(' forExpr ';' {
    eval($4);
    freeNode($4);
    POP_STACK_t0();

    struct Label label = popLabel();
    label = pushLabelNamed(label);
    fprintf(f_asm, ".L_for_cond_%d:\n", label.id);
}
forExpr ';' {
    // cond
    eval($7);
    freeNode($7);
    POP_STACK_t0();

    struct Label label = popLabel();
    label = pushLabelNamed(label);
    fprintf(f_asm, "  bnez t0, .L_for_body_%d\n", label.id);
    // fprintf(f_asm, "  beqz t0, .L_for_end_%d\n", label.id);
    fprintf(f_asm, "  beqz t0, .L_%d\n", label.id);

    // tail
    fprintf(f_asm, ".L_for_tail_%d:\n", label.id);
} 
forExpr ')' {
    struct Label label = popLabel();
    label = pushLabelNamed(label);
    
    // tail
    eval($10);
    freeNode($10);
    POP_STACK_t0();
    fprintf(f_asm, "  j .L_for_cond_%d\n", label.id);
    fprintf(f_asm, ".L_for_body_%d:\n", label.id);
} 
'{' stmts '}' {
    struct Label label = popLabel();
    leaveLocalScope(label, cur_fn, 1);
    pop_up_symbol(cur_scope);
    cur_scope--;
    fprintf(f_asm, "  j .L_for_tail_%d\n", label.id);
    // fprintf(f_asm, ".L_for_end_%d:\n", label.id);
    fprintf(f_asm, ".L_%d:\n", label.id);
}
;
forExpr: 
// result should be on stack
expr {$$ = $1;}
| {perror("Empty forexpr not handled");}
;
// @@ return not handled
returnStmt: 
RETURN expr ';' {
    eval($2);
    freeNode($2);
    POP_STACK_t0();
    fprintf(f_asm, "  addi a0, t0, 0 // return\n");
    struct Label label = fetchReturnLabel();
    
    // same as at the end of func def
    leaveLocalScope(label, cur_fn, 0);
    gen_func_callee_epilogue(cur_fn);
}
| RETURN ';' {
    struct Label label = fetchReturnLabel();
    // same as at the end of func def
    leaveLocalScope(label, cur_fn, 0);
    gen_func_callee_epilogue(cur_fn);
    fprintf(f_asm, "// ***** Before leaving local scope, table *****\n");
    print_symbol_table();
    fprintf(f_asm, "// ***** After  leaving local scope, table *****\n");
    pop_up_symbol(cur_scope);
    print_symbol_table();
    cur_scope--;
    strcpy(cur_fn, "");
}
;
// @@ focus
breakStmt: 
BREAK ';' {
    struct Label label = fetchBreakLabel();
    leaveLocalScope(label, cur_fn, 0);
    fprintf(f_asm, "j .L_%d // break \n", label.id);
}
;
continueStmt:
CONTINUE ';' {}
;

// compoundStmt:
// '{' stmts '}' {}
;
%%
// C code section

int main()
{
    int index;
    f_asm = fopen("codegen.S", "w");
    init_symbol_table();
    index = install_symbol("delay");
    table[index].type = T_FUNCTION;
    table[index].scope = GLOBAL_SCOPE;

    index = install_symbol("digitalWrite");
    table[index].type = T_FUNCTION;
    table[index].scope = GLOBAL_SCOPE;

    yyparse();
    fclose(f_asm);
    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}