#pragma once

typedef enum{
    NODE_TYPE_CONST,
    NODE_TYPE_VAR,
    NODE_TYPE_EXPR,
    NODE_TYPE_CALL,
    NODE_TYPE_DEREF,
    NODE_TYPE_REF,
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