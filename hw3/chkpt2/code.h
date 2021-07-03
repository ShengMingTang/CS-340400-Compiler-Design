#pragma once
// my version
#define GLOBAL_SCOPE (1)

#define EMPTY_STR_ASSIGN(s) strcpy(s, "");

#define POP_STACK_t0(){\
    fprintf(f_asm, "  ld t0, 0(sp)\n");\
    fprintf(f_asm, "  addi sp, sp, 8 // stack pop\n");\
}
#define POP_STACK_t1(){\
    fprintf(f_asm, "  ld t1, 0(sp)\n");\
    fprintf(f_asm, "  addi sp, sp, 8 // stack pop\n");\
}

#define PUSH_STACK_t0(){\
    fprintf(f_asm,"  addi sp, sp, -8\n");\
    fprintf(f_asm,"  sd t0, 0(sp) // stack push\n");\
}

#define PUSH_STACK_t1(){\
    fprintf(f_asm,"  addi sp, sp, -8\n");\
    fprintf(f_asm,"  sd t1, 0(sp) // stack push\n");\
}


struct Func
{
   char name[500];
   int numArgs;
};
void gen_func_del(char *fn);
void gen_func_def_label(char *fn);
void gen_func_caller_saved(char *fn);
void gen_func_caller_restored(char *fn);
void gen_func_callee_epilogue(char *fn);
void gen_func_callee_prologue(char *fn);
void pop_symbol_table_count(int n);
void stack_for_local_vars(char *var, char *cur_fn);
void stack_for_args(char* fn);
void unstack_for_local_vars_and_args(char *fn);
#define MAX_TABLE_SIZE 5000

void init_symbol_table();
char * install_symbol(char *s);
int look_up_symbol(char *s);
void pop_up_symbol(int scope);
void set_scope_and_offset_of_param(char *s);
void set_local_vars(char *functor);
void set_global_vars(char *s);
void code_gen_func_header(char *functor);
void code_gen_global_vars();
void code_gen_at_end_of_function_body(char *functor);
char * copyn(register int n, register char *s);
char * copys(char *s);

typedef struct symbol_entry *PTR_SYMB;
struct symbol_entry {
   char *name;
   int scope;
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

#define T_FUNCTION 1
#define ARGUMENT_MODE   2
#define LOCAL_MODE      4
#define GLOBAL_MODE     8

extern int cur_scope;
extern int cur_counter;
