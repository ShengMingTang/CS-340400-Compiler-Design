#pragma once
// my version
struct Func
{
    // @@ buggy
   char name[500];
   int numArgs;
   char args[10][500];
};

#define GLOBAL_SCOPE (1)

#define NOT_IMPLMENTED {\
    perror("not implemented");\
    exit(1);\
}

#define EMPTY_STR_ASSIGN(s) strcpy(s, "");

#define PTR_TYPE_STR "_ptr"
#define PTR_TYPE_STR_ASSIGN(s) strcpy(s, PTR_TYPE_STR);

#define ARR_TYPE_STR "_arr"
#define ARR_TYPE_STR_ASSIGN(s) strcpy(s, ARR_TYPE_STR);

#define CHECK_ARR_TYPE(s, ss) {\
    if(!strcmp(s, PTR_TYPE_STR)){\
        fprintf(f_asm, "  li t2, 8 // Left hand arr type\n");\
        fprintf(f_asm, "  mul t1, t1, t2 // Left hand arr type\n");\
        EMPTY_STR_ASSIGN(ss);\
    }\
    else{\
        EMPTY_STR_ASSIGN(ss);\
    }\
}



#define LOAD_IF_PTR_TYPE_t0(s){\
    if(!strcmp(s, PTR_TYPE_STR)){\
        fprintf(f_asm, "  ld t0, 0(t0) // load t0\n");\
    }\
}

#define LOAD_IF_PTR_TYPE_t1(s){\
    if(!strcmp(s, PTR_TYPE_STR)){\
        fprintf(f_asm, "  ld t1, 0(t1) // load t1\n");\
    }\
}

void print_symbol_table();
void print_fn(struct Func fn);
void gen_func_del(char *fn);
void gen_func_def_label(char *fn);
void gen_func_caller_saved(char *fn);
void gen_func_caller_restored(char *fn);
void gen_func_callee_epilogue(char *fn);
void gen_func_callee_prologue(char *fn);


int push_label();
int push_label_named(int label);
int pop_label();

void enter_local_scope(char *fn);
void leave_local_scope(char *fn);

#define MAX_TABLE_SIZE 5000

void init_symbol_table();
int install_symbol(char *s);
int look_up_symbol(char *s);
void pop_up_symbol(int scope);
// void set_scope_and_offset_of_param(char *s);
// void set_local_vars(char *functor);
// void set_global_vars(char *s);
// void code_gen_func_header(char *functor);
// void code_gen_global_vars();
// void code_gen_at_end_of_function_body(char *functor);
char * copyn(register int n, register char *s);
char * copys(char *s);

typedef struct symbol_entry *PTR_SYMB;
struct symbol_entry {
   char *name;
   int scope;

// for function: offset of the current stack, addi -8 before assign new one
// for var     : offset from fp
   int offset;

   int id;
   int variant;
   int type;
   int total_args;
   int total_locals;
   int mode;
   int functor_index; /* add for risc-v, 2020 by Jenq-Kuen Lee */
// }  table[MAX_TABLE_SIZE];
};

#define T_FUNCTION (1)
#define T_PTR (2)
#define T_VAR (4)
#define T_ARR (8)

#define OP_NONE ""

#define ARGUMENT_MODE   (2)
#define LOCAL_MODE      (4)
#define GLOBAL_MODE     (8)

extern int cur_scope;
extern int cur_counter;
