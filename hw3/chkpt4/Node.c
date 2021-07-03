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
        // @@ handled in LR parser
    }
    else{
        fprintf(f_asm, "type:%d ,op:%s not handled\n", p->type, p->op);
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