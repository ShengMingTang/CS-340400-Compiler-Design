#pragma once
#include "code.h"
typedef enum{
    NODE_TYPE_CONST,
    NODE_TYPE_VAR,
    NODE_TYPE_EXPR,
    NODE_TYPE_CALL,
    NODE_TYPE_DEREF,
    NODE_TYPE_REF,
    NODE_TYPE_ARG,
    NODE_TYPE_NONE
}NodeType;
struct Node{
    NodeType type;
    char *op;
    int opflag;
    struct Node *left, *right;
};
#define EXPR_OP_DEREF "__DEREF__"
#define EXPR_OP_REF "__REF__"

#define OPFLAG_NONE (0)
#define OPFLAG_PTRADDSUB (1)

#define POP_STACK_t0(){\
    fprintf(f_asm, "  ld t0, 0(sp)\n");\
    fprintf(f_asm, "  addi sp, sp, 8 // stack pop\n");\
}
#define POP_STACK_t1(){\
    fprintf(f_asm, "  ld t1, 0(sp)\n");\
    fprintf(f_asm, "  addi sp, sp, 8 // stack pop\n");\
}

#define PUSH_STACK_t0(){\
    fprintf(f_asm, "  addi sp, sp, -8\n");\
    fprintf(f_asm, "  sd t0, 0(sp) // stack push\n");\
}

#define PUSH_STACK_t1(){\
    fprintf(f_asm, "  addi sp, sp, -8\n");\
    fprintf(f_asm, "  sd t1, 0(sp) // stack push\n");\
}

struct Node* makeNode(NodeType type, char *op);
struct Node* makeEmptyNode();
struct Node* makeExprNode(struct Node *pl, struct Node *pr, char*op);

void eval(struct Node* p);
void freeNode(struct Node* p);
struct Node* nodeAssignLR(struct Node *p, struct Node *l, struct Node *r);

// label
typedef enum{
    LABEL_TYPE_IFELSE,
    LABEL_TYPE_FORWHILE,
    LABEL_TYPE_FUNC,
    LABEL_TYPE_NONE
}LabelType;
struct Label{
    int id; // id of this label
    int offset; // offset from fp before/after this scope
    LabelType type; // for break
};
struct LabelStack{
    int top;
    // @buggy
    struct Label stack[500];
};
// scope management
struct Label pushLabel(LabelType type, int offset);
struct Label pushLabelNamed(struct Label label);
struct Label popLabel();
void leaveLocalScope(struct Label label, char *fn, int isEndOfParsing);
struct Label fetchReturnLabel();
struct Label fetchBreakLabel();

#define FUNCDEF_OFFSET (-104)
void pop_symbol_table_count(int n);
void stack_for_local_vars(char *var, char *cur_fn, int nElement, int scope);
void stack_for_args(struct Func fn, int scope);