#include "Node.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *f_asm;
extern struct symbol_entry table[MAX_TABLE_SIZE];
struct Node* makeNode(NodeType type, char *op)
{
    struct Node *p = (struct Node*)malloc(sizeof(struct Node));
    if(!p) {perror("Make Node NULL Error");}

    p->type = type;
    p->op = (char*)malloc(sizeof(char)*strlen(op) + 1);
    strcpy(p->op, op);
    p->opflag = OPFLAG_NONE;
    p->left = p->right = NULL;
    return p;
}
struct Node* makeEmptyNode()
{
    return NULL;
}
struct Node* makeExprNode(struct Node *pl, struct Node *pr, char *op)
{
    struct Node *p = makeNode(NODE_TYPE_EXPR, op);
    p->type = NODE_TYPE_EXPR;
    p->op = (char*)malloc(sizeof(char)*(strlen(op) + 1));
    strcpy(p->op, op);
    p->opflag = OPFLAG_NONE;
    p->left = pl;
    p->right = pr;
    return p;
}

// result should be put on stack not in register
void eval(struct Node* p)
{
    if(!p) return;
    // printf("op:%s\n", p->op);
    // Terminal
    if(p->type == NODE_TYPE_CONST){
        fprintf(f_asm,"  li t0, %s\n", p->op);
        PUSH_STACK_t0();
    }
    else if(p->type == NODE_TYPE_REF){
        // @@ cannot handle arr[i] like expr
        if(p->left->type == NODE_TYPE_VAR){
            int index = look_up_symbol(p->left->op);
            fprintf(f_asm, "  addi t0, fp, %d // ref\n", table[index].offset);
            PUSH_STACK_t0();
        }
        else{
            perror("& for non-id type not handled");
            exit(1);
        }
    }
    else if(p->type == NODE_TYPE_DEREF){
        eval(p->left);
        POP_STACK_t0();
        fprintf(f_asm, "  ld t0, 0(t0) // deref\n");
        PUSH_STACK_t0();
    }
    // non-terminal
    else if(p->type == NODE_TYPE_EXPR){
        if(!strcmp(p->op, "+")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1(); // right
            POP_STACK_t0(); // left
            // @@ right array type not checked
            if(p->left->opflag & OPFLAG_PTRADDSUB){
                fprintf(f_asm, "  li t2, 8\n");
                fprintf(f_asm, "  mul t1, t1, t2 // ArrAddSub\n");
                p->opflag |= OPFLAG_PTRADDSUB;
            }
            fprintf(f_asm, "  add t0, t0, t1\n");
            PUSH_STACK_t0();
            fprintf(f_asm, "  // end add\n");
        }
        else if(!strcmp(p->op, "-")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1();
            POP_STACK_t0();
            // @@ right array type not checked
            if(p->left->opflag & OPFLAG_PTRADDSUB){
                fprintf(f_asm, "  li t2, 8\n");
                fprintf(f_asm, "  mul t1, t1, t2 // ArrAddSub\n");
                p->opflag |= OPFLAG_PTRADDSUB;
            }

            fprintf(f_asm, "  sub t0, t0, t1\n");
            PUSH_STACK_t0();
            fprintf(f_asm, "  // end sub\n");
        }
        else if(!strcmp(p->op, "*")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1();
            POP_STACK_t0();
            fprintf(f_asm, "  mul t0, t0, t1\n");
            PUSH_STACK_t0();
            fprintf(f_asm, "  // end mul\n");
        }
        else if(!strcmp(p->op, "/")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1();
            POP_STACK_t0();
            fprintf(f_asm, "  div t0, t0, t1\n");
            PUSH_STACK_t0();
            fprintf(f_asm, "  // end sub\n");
        }
        else if(!strcmp(p->op, "=")){
            // var = expr
            if(p->left->type == NODE_TYPE_VAR){
                eval(p->right);
                POP_STACK_t0();
                int index = look_up_symbol(p->left->op);
                fprintf(f_asm, "  sd t0, %d(fp)\n", table[index].offset);
                PUSH_STACK_t0();
            }
            // *var = expr
            // p->left '*'
            // p->left->left->op: var name
            else if(p->left->type == NODE_TYPE_DEREF && p->left->left->type == NODE_TYPE_VAR){
                eval(p->right);
                POP_STACK_t0();
                int index = look_up_symbol(p->left->left->op);
                fprintf(f_asm, "  ld t1, %d(fp)\n", table[index].offset);
                // t1 has value of that pointer
                // t0 has rhs value;
                fprintf(f_asm, "  sd t0, 0(t1)\n");
                PUSH_STACK_t0();
            }
            // arr[i] = expr
            else if (p->left->type == NODE_TYPE_EXPR){
                eval(p->right);
                eval(p->left);
                POP_STACK_t1(); // t1 has lval
                POP_STACK_t0(); // t0 has rval
                fprintf(f_asm, "  sd t0, 0(t1)\n");
                PUSH_STACK_t0();
            }
            // *(expr) = expr
            // p->left '*'
            // p->left->left: expr
            else if(p->left->type == NODE_TYPE_DEREF && p->left->left->type == NODE_TYPE_EXPR){
                eval(p->right);
                eval(p->left->left);
                POP_STACK_t1(); // t1 has lval
                POP_STACK_t0(); // t0 has rval
                fprintf(f_asm, "  sd t0, 0(t1)\n");
                PUSH_STACK_t0();
            }
            else{
                perror("= LHS expr not handled");
                exit(1);
            }
        }
        else if(!strcmp(p->op, "<")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1();
            POP_STACK_t0();
            fprintf(f_asm, "  slt t0, t0, t1 // '<' \n");
            PUSH_STACK_t0();
        }
        else if(!strcmp(p->op, "!=")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1();
            POP_STACK_t0();
            fprintf(f_asm, "  sub t0, t0, t1\n");
            fprintf(f_asm, "  snez t0, t0 // '!=' \n");
            PUSH_STACK_t0();
        }
        else if(!strcmp(p->op, "==")){
            eval(p->left);
            eval(p->right);
            POP_STACK_t1();
            POP_STACK_t0();
            fprintf(f_asm, "  sub t0, t0, t1\n");
            fprintf(f_asm, "  seqz t0, t0 // '!=' \n");
            PUSH_STACK_t0();
        }
        else{
            perror("OP not handled");
        }
    }
    else if(p->type == NODE_TYPE_VAR){
        int index = look_up_symbol(p->op);
        // @@ how to resolve
        // int arr[5], *p;
        // int *pa = p, *pb = arr;
        // raise PTRADDSUB

        if(table[index].type & T_ARR){
            fprintf(f_asm, "  addi t0, fp, %d\n", table[index].offset);
            PUSH_STACK_t0();
        }
        else{
            fprintf(f_asm, "  ld t0, %d(fp)\n", table[index].offset);
            PUSH_STACK_t0();
        }
        if(table[index].type & T_PTR){
            p->opflag |= OPFLAG_PTRADDSUB;
        }
    }
    else if(p->type == NODE_TYPE_CALL){
        // @@ only ID(args) are handled
        // fprintf(f_asm, "  jal ra, %s\n", p->op);
        // // return value is on stack top
        // gen_func_caller_restored(p->op);

        // new version
        // evaluate args
        // @@ arg arrangement
        gen_func_caller_saved(p->op); 
        struct Node *arg = p->left;
        int argCount = 0, argPut = 0;
        while(arg){
            eval(arg->right);
            // on stack
            arg = arg->left;
            argCount++;
        }
        // return is on stack (bottom) alast ... a0 (top)
        while(argPut < argCount){
            POP_STACK_t0();
            fprintf(f_asm, "  addi a%d, t0, 0\n", argPut);
            argPut++;
        }
        fprintf(f_asm, "  jal ra, %s\n", p->op);
        gen_func_caller_restored(p->op);
    }
    else{
        fprintf(f_asm, "type:%d ,op:%s not handled\n", p->type, p->op);
        perror("Some Eval not handled");
        exit(1);
    }
}
void freeNode(struct Node* p)
{
    if(!p) return;
    freeNode(p->left);
    freeNode(p->right);
    if(p->op) free(p->op);
}
struct Node* nodeAssignLR(struct Node *p, struct Node *l, struct Node *r)
{
    p->left = l;
    p->right = r;
}

static struct LabelStack labels;
struct Label pushLabel(LabelType type, int offset)
{
    static int count = 0;
    labels.stack[labels.top].id = count;
    count++;
    labels.stack[labels.top].offset = offset;
    labels.stack[labels.top].type = type;
    labels.top++;
    
    return labels.stack[labels.top - 1];
}
struct Label pushLabelNamed(struct Label label)
{
    labels.stack[labels.top] = label;
    labels.top++;
    return label;
}
struct Label popLabel()
{
    labels.top--;
    return labels.stack[labels.top];
}

// restore sp
void leaveLocalScope(struct Label label, char *fn, int isEndOfParsing)
{
    int index = look_up_symbol(fn);
    fprintf(f_asm, "  addi sp, fp, %d // restore sp\n", label.offset);
    if(isEndOfParsing){
        table[index].offset = label.offset; // offset restored
    }
}
struct Label fetchReturnLabel()
{
    struct Label label;
    int i;
    for(i = labels.top - 1; i >= 0; i--){
        if(labels.stack[i].type == LABEL_TYPE_FUNC){
            break;
        }
    }
    if(i < 0){
        perror("return w/o a valid label, labels exhausted");
        exit(1);
    }
    // i as the label index
    return labels.stack[i];
}
struct Label fetchBreakLabel()
{
    struct Label label;
    int i;
    for(i = labels.top - 1; i >= 0; i--){
        if(labels.stack[i].type == LABEL_TYPE_FORWHILE){
            break;
        }
    }
    if(i < 0){
        perror("break w/o a valid label, labels exhausted");
        for(int j = 0; j < labels.top; j++){
            printf("labels[%d] id:%d type:%d\n", j, labels.stack[j].id, labels.stack[j].type);
        }
        exit(1);
    }
    // i as the label index
    return labels.stack[i];
}

// scope
// @param n: number of terms to pop
void pop_symbol_table_count(int n)
{
    fprintf(f_asm, "// pop_symbol_table_count %d\n", n);
    for(int i = 0; i < n; i++){
        fprintf(f_asm, "  // pop symbol=%s count\n", table[cur_counter-1-i].name);
    }
    cur_counter -= n;
    fprintf(f_asm, "// cur_counter=%d\n", cur_counter);
    if(cur_counter < 0){
        fprintf(stderr, "cur_counter=%d\n", cur_counter);
        exit(1);
    }
    fprintf(f_asm, "// pop_symbol_table_count %d\n", n);
}
void stack_for_local_vars(char *var, char *cur_fn, int nElement, int scope)
{
    int i = look_up_symbol(var), fnIndex;
    // find index for parent function
    for(fnIndex = i; fnIndex >= 0; fnIndex--){
        if (table[fnIndex].type == T_FUNCTION && !strcmp(table[fnIndex].name, cur_fn)){
            break;
        }
    }
    if(fnIndex < 0){
        perror("Cannot find parent function of a local var");
        exit(1);
    }
    table[fnIndex].total_locals += 1;
    table[i].name = copys(var);
    table[i].scope = scope;
    table[i].offset = table[fnIndex].offset - 8*nElement;
    table[fnIndex].offset -= 8*nElement; // offset is the next position for next local var
    fprintf(f_asm, "  addi sp, sp, %d // stack local(type%d)=\"%s\" parent=%s, offset=%d\n", -8*nElement, table[i].type, var, table[fnIndex].name, table[i].offset);
}
void stack_for_args(struct Func fn, int scope)
{
    int i, j, index;
    int total_args;

    index = look_up_symbol(fn.name);
    if (index < 0){
        perror("Error in function header");
        exit(1);
    }
    else{
        table[index].type = T_FUNCTION;
        table[index].total_args = fn.numArgs;
        for(int i = 0; i < table[index].total_args; i++){
            // the ith arg
            int idx = look_up_symbol(fn.args[i]);
            if(table[idx].type & T_ARR){
                perror("Arr as arg is not supported");
                exit(1);
            }
            // @@ variable arg size is not supported
            table[idx].scope = scope;
            // table[idx].offset = table[index].offset - 8*i;
            table[idx].offset = table[index].offset - 8;
            table[index].offset -= 8;
            fprintf(f_asm, "  addi sp, sp, -8 // offset from fp = %d \n", table[idx].offset);
            fprintf(f_asm, "  sd a%d, 0(sp) // stack arg=%s\n", i, table[idx].name);
            fprintf(f_asm, "  // %s stack=%d\n", fn.name, table[index].offset);
        }
    }
}