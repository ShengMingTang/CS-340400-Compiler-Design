
/*
   This is a very simple c compiler written by Prof. Jenq Kuen Lee,
   Department of Computer Science, National Tsing-Hua Univ., Taiwan,
   Fall 1995.

   This is used in compiler class.
   This file contains Symbol Table Handling.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include "code.h"

extern FILE *f_asm;
int cur_counter = 0;
int cur_scope = 1;
char *copys();
struct symbol_entry table[MAX_TABLE_SIZE];

// my version
void gen_func_del(char *fn)
{
    fprintf(f_asm, ".global %s\n", fn);
}
void gen_func_def_label(char *fn)
{
    fprintf(f_asm, "%s:\n", fn);
}
void gen_func_caller_saved(char *fn)
{
    // for ret value
    fprintf(f_asm, "  // reserved for ret val, -8(sp) // BEGIN CALLER=%s SAVED\n", fn);
    fprintf(f_asm, "  sd ra, -16(sp)\n");
    fprintf(f_asm, "  sd t0, -24(sp)\n");
    fprintf(f_asm, "  sd t1, -32(sp)\n");
    fprintf(f_asm, "  sd t2, -40(sp)\n");
    fprintf(f_asm, "  sd a0, -48(sp)\n");
    fprintf(f_asm, "  sd a1, -56(sp)\n");
    fprintf(f_asm, "  sd a2, -64(sp)\n");
    fprintf(f_asm, "  sd a3, -72(sp)\n");
    fprintf(f_asm, "  sd a4, -80(sp)\n");
    fprintf(f_asm, "  sd a5, -88(sp)\n");
    fprintf(f_asm, "  sd a6, -96(sp)\n");
    fprintf(f_asm, "  sd a7, -104(sp)\n");
    fprintf(f_asm, "  sd t3, -112(sp)\n");
    fprintf(f_asm, "  sd t4, -120(sp)\n");
    fprintf(f_asm, "  sd t5, -128(sp)\n");
    fprintf(f_asm, "  sd t6, -136(sp)\n");
    fprintf(f_asm, "  addi sp, sp, -136 // END CALLER=%s SAVED\n", fn);

    // without ret value version
    // fprintf(f_asm, "  sd ra, -8(sp) // BEGIN CALLER=%s SAVED\n", fn);
    // fprintf(f_asm, "  sd t0, -16(sp)\n");
    // fprintf(f_asm, "  sd t1, -24(sp)\n");
    // fprintf(f_asm, "  sd t2, -32(sp)\n");
    // fprintf(f_asm, "  sd a0, -40(sp)\n");
    // fprintf(f_asm, "  sd a1, -48(sp)\n");
    // fprintf(f_asm, "  sd a2, -56(sp)\n");
    // fprintf(f_asm, "  sd a3, -64(sp)\n");
    // fprintf(f_asm, "  sd a4, -72(sp)\n");
    // fprintf(f_asm, "  sd a5, -80(sp)\n");
    // fprintf(f_asm, "  sd a6, -88(sp)\n");
    // fprintf(f_asm, "  sd a7, -96(sp)\n");
    // fprintf(f_asm, "  sd t3, -104(sp)\n");
    // fprintf(f_asm, "  sd t4, -112(sp)\n");
    // fprintf(f_asm, "  sd t5, -120(sp)\n");
    // fprintf(f_asm, "  sd t6, -128(sp)\n");
    // fprintf(f_asm, "  addi sp, sp, -128 // END CALLER=%s SAVED\n", fn);
}
void gen_func_caller_restored(char *fn)
{
    // for ret value
    fprintf(f_asm, "  addi sp, sp, 136 // BEGIN CALLER=%s RESTORED\n", fn);
    fprintf(f_asm, "  sd a0, -8(sp)\n");
    fprintf(f_asm, "  ld t6, -136(sp)\n");
    fprintf(f_asm, "  ld t5, -128(sp)\n");
    fprintf(f_asm, "  ld t4, -120(sp)\n");
    fprintf(f_asm, "  ld t3, -112(sp)\n");
    fprintf(f_asm, "  ld a7, -104(sp)\n");
    fprintf(f_asm, "  ld a6, -96(sp)\n");
    fprintf(f_asm, "  ld a5, -88(sp)\n");
    fprintf(f_asm, "  ld a4, -80(sp)\n");
    fprintf(f_asm, "  ld a3, -72(sp)\n");
    fprintf(f_asm, "  ld a2, -64(sp)\n");
    fprintf(f_asm, "  ld a1, -56(sp)\n");
    fprintf(f_asm, "  ld a0, -48(sp)\n");
    fprintf(f_asm, "  ld t2, -40(sp)\n");
    fprintf(f_asm, "  ld t1, -32(sp)\n");
    fprintf(f_asm, "  ld t0, -24(sp)\n");
    fprintf(f_asm, "  ld ra, -16(sp) // END CALLER=%s RESTORED\n", fn);
    fprintf(f_asm, "  addi sp, sp, -8 // to keep stack top always the evaluated value\n");

    // without ret value
    // fprintf(f_asm, "  addi sp, sp, 128 // BEGIN CALLER=%s RESTORED\n", fn);
    // fprintf(f_asm, "  ld t6, -128(sp)\n");
    // fprintf(f_asm, "  ld t5, -120(sp)\n");
    // fprintf(f_asm, "  ld t4, -112(sp)\n");
    // fprintf(f_asm, "  ld t3, -104(sp)\n");
    // fprintf(f_asm, "  ld a7, -96(sp)\n");
    // fprintf(f_asm, "  ld a6, -88(sp)\n");
    // fprintf(f_asm, "  ld a5, -80(sp)\n");
    // fprintf(f_asm, "  ld a4, -72(sp)\n");
    // fprintf(f_asm, "  ld a3, -64(sp)\n");
    // fprintf(f_asm, "  ld a2, -56(sp)\n");
    // fprintf(f_asm, "  ld a1, -48(sp)\n");
    // fprintf(f_asm, "  ld a0, -40(sp)\n");
    // fprintf(f_asm, "  ld t2, -32(sp)\n");
    // fprintf(f_asm, "  ld t1, -24(sp)\n");
    // fprintf(f_asm, "  ld t0, -16(sp)\n");
    // fprintf(f_asm, "  ld ra, -8(sp) // END CALLER=%s RESTORED\n", fn);
}
void gen_func_callee_prologue(char *fn)
{
    fprintf(f_asm, "\n  addi sp, sp, -104 // BEGIN %s PROLOGUE\n", fn);
    fprintf(f_asm, "  sd sp, 96(sp)\n");
    fprintf(f_asm, "  sd s0, 88(sp)\n");
    fprintf(f_asm, "  sd s1, 80(sp)\n");
    fprintf(f_asm, "  sd s2, 72(sp)\n");
    fprintf(f_asm, "  sd s3, 64(sp)\n");
    fprintf(f_asm, "  sd s4, 56(sp)\n");
    fprintf(f_asm, "  sd s5, 48(sp)\n");
    fprintf(f_asm, "  sd s6, 40(sp)\n");
    fprintf(f_asm, "  sd s7, 32(sp)\n");
    fprintf(f_asm, "  sd s8, 24(sp)\n");
    fprintf(f_asm, "  sd s9, 16(sp)\n");
    fprintf(f_asm, "  sd s10, 8(sp)\n");
    fprintf(f_asm, "  sd s11, 0(sp)\n");
    fprintf(f_asm, "  addi s0, sp, 104 // END %s PROLOGUE\n", fn);
}
void gen_func_callee_epilogue(char *fn)
{
    fprintf(f_asm, "  ld sp, 96(sp) // BEGIN %s EPILOGUE\n", fn);
    fprintf(f_asm, "  ld s0, 88(sp)\n");
    fprintf(f_asm, "  ld s1, 80(sp)\n");
    fprintf(f_asm, "  ld s2, 72(sp)\n");
    fprintf(f_asm, "  ld s3, 64(sp)\n");
    fprintf(f_asm, "  ld s4, 56(sp)\n");
    fprintf(f_asm, "  ld s5, 48(sp)\n");
    fprintf(f_asm, "  ld s6, 40(sp)\n");
    fprintf(f_asm, "  ld s7, 32(sp)\n");
    fprintf(f_asm, "  ld s8, 24(sp)\n");
    fprintf(f_asm, "  ld s9, 16(sp)\n");
    fprintf(f_asm, "  ld s10, 8(sp)\n");
    fprintf(f_asm, "  ld s11, 0(sp)\n");
    fprintf(f_asm, "  addi sp, sp, 104\n");
    fprintf(f_asm, "  jalr zero, 0(ra) // END %s EPILOGUE\n", fn);
}
// @param n: number of terms to pop
void pop_symbol_table_count(int n)
{
    fprintf(f_asm, "// pop %d\n", n);
    for(int i = 0; i < n; i++){
        fprintf(f_asm, "  // pop symbol=%s count\n", table[cur_counter-1-i].name);
    }
    cur_counter -= n;
    if(cur_counter < 0){
        fprintf(stderr, "cur_counter=%d\n", cur_counter);
        exit(1);
    }
}
void stack_for_local_vars(char *var, char *cur_fn)
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
    table[i].scope = cur_scope;
    table[i].offset = -104 - 8*(i-fnIndex);
    table[i].mode = LOCAL_MODE;
    fprintf(f_asm, "  addi sp, sp, -8 // stack local=\"%s\" parent=%s, offset=%d\n", var, table[fnIndex].name, table[i].offset);
}
void stack_for_args(char* fn)
{
    int i, j, index;
    int total_args;

    index = look_up_symbol(fn);
    if (index < 0){
        perror("Error in function header");
        exit(1);
    }
    else{
        table[index].type = T_FUNCTION;
        total_args = cur_counter - index - 1;
        table[index].total_args = total_args;
        for(j = 1; j <= total_args; j++){
            table[index+j].scope = cur_scope;
            table[index+j].offset = -104 - 8*j;
            table[index+j].mode = ARGUMENT_MODE;
            table[index+j].functor_index = index;
            fprintf(f_asm, "  addi sp, sp, -8 // offset from fp = %d \n", table[index+j].offset);
            fprintf(f_asm, "  sd a%d (0)sp // stack arg=%s\n", j-1, table[index+j].name);
        }
    }
}
void unstack_for_local_vars_and_args(char *fn)
{
    int fnIndex = look_up_symbol(fn);
    if(fnIndex < 0){
        perror("Cannot find function at func_def end");
        exit(1);
    }
    fprintf(f_asm, "  addi sp, sp, %d // unstack for locals(%d) and args(%d)\n", 
        8*(table[fnIndex].total_locals + table[fnIndex].total_args),
        table[fnIndex].total_locals,
        table[fnIndex].total_args
    );
}
/*

  init_symbol_table();

*/
void init_symbol_table()
{

    bzero(&table[0], sizeof(struct symbol_entry) * MAX_TABLE_SIZE);
}

/*
   To install a symbol in the symbol table 

*/
char *install_symbol(char *s)
{

    if (cur_counter >= MAX_TABLE_SIZE)
        perror("Symbol Table Full");
    else
    {
        table[cur_counter].scope = cur_scope;
        table[cur_counter].name = copys(s);
        cur_counter++;
        fprintf(f_asm, "// symbol=%s installed, counter=%d\n", s, cur_counter);
    }
    return (s);
}

/*
   To return an integer as an index of the symbol table

*/
int look_up_symbol(char *s)
{
    int i;

    if (cur_counter == 0)
        return (-1);
    for (i = cur_counter - 1; i >= 0; i--){
        if (!strcmp(s, table[i].name))
            return (i);
    }
    return (-1);
}

/*
   Pop up symbols of the given scope from the symbol table upon the
   exit of a given scope.

*/
void pop_up_symbol(int scope)
{
    int i;
    if (cur_counter == 0)
        return;
    for (i = cur_counter - 1; i >= 0; i--){
        if (table[i].scope != scope)
            break;
    }
    if (i < 0)
        cur_counter = 0;
    cur_counter = i + 1;
}

/*
   Set up parameter scope and offset

*/
void set_scope_and_offset_of_param(char *s)
{

    int i, j, index;
    int total_args;

    index = look_up_symbol(s);
    if (index < 0){
        perror("Error in function header");
        exit(1);
    }
    else{
        table[index].type = T_FUNCTION;
        total_args = cur_counter - index - 1;
        table[index].total_args = total_args;
        for (j = total_args, i = cur_counter - 1; i > index; i--, j--){
            table[i].scope = cur_scope;
            table[i].offset = j;
            table[i].mode = ARGUMENT_MODE;
            table[i].functor_index = index;
        }
    }
}

/*
   Set up local var offset

*/
void set_local_vars(char *functor)
{

    int i, j, index, index1;
    int total_locals;

    index = look_up_symbol(functor);
    index1 = index + table[index].total_args;
    total_locals = cur_counter - index1 - 1;
    if (total_locals < 0)
        perror("Error in number of local variables");
    table[index].total_locals = total_locals;
    for (j = total_locals, i = cur_counter - 1; j > 0; i--, j--){
        table[i].scope = cur_scope;
        table[i].offset = j;
        table[i].mode = LOCAL_MODE;
    }
}

/*
  Set GLOBAL_MODE to global variables

*/

void set_global_vars(char *s)
{
    int index;
    index = look_up_symbol(s);
    table[index].mode = GLOBAL_MODE;
    table[index].scope = 1;
}

/*

To generate house-keeping work at the beginning of the function

*/

void code_gen_func_header(char *functor)
{

    // fprintf(f_asm, "   #  %s\n", functor);
    // fprintf(f_asm, "      .globl main                    # -- Begin function %s\n", functor);
    // fprintf(f_asm, "      .p2align 2 \n");
    // fprintf(f_asm, "      .type %s,@function \n", functor);
    // fprintf(f_asm, "   #    \n");
    // fprintf(f_asm, "%s:\n", functor);

    // fprintf(f_asm, "        addi sp,sp,-48 \n");
    // fprintf(f_asm, "        sd   ra,40(sp) \n");
    // fprintf(f_asm, "        sd   fp,32(sp) \n");
    // fprintf(f_asm, "        addi fp,sp,48 \n");
    // fprintf(f_asm, "         \n");
}

/*

  To generate global symbol vars

*/
void code_gen_global_vars()
{
    int i;

    for (i = 0; i < cur_counter; i++){
        if (table[i].mode == GLOBAL_MODE){
            fprintf(f_asm, "        .type   %s,@object\n", table[i].name);
            fprintf(f_asm, "        .comm   %s,4,4\n", table[i].name);
        }
    }

    fprintf(f_asm, " \n");
    fprintf(f_asm, "        .ident \"NTHU Compiler Class Code Generator for RISC-V\"\n");
    fprintf(f_asm, "        .section \"note.stack\",\"\",@progbits\n");
}

/*

 To geenrate house-keeping work at the end of a function

*/

void code_gen_at_end_of_function_body(char *functor)
{
    int i;

    // fprintf(f_asm, "   #    \n");

    // fprintf(f_asm, "        ld ra,40(sp) # old ra\n");
    // fprintf(f_asm, "        ld fp,32(sp) # old fp\n");
    // fprintf(f_asm, "        addi sp,sp,48# pop activiation record\n");
    // fprintf(f_asm, "        ret\n");

    fprintf(f_asm, ".Lfunc_%s_end0:\n", functor);
    fprintf(f_asm, "        .size      %s, .Lfunc_%s_end0-%s \n", functor, functor, functor);
    fprintf(f_asm, "       \n");
}

/*******************Utility Functions ********************/
/*
 * copyn -- makes a copy of a string with known length
 *
 * input:
 *	  n - lenght of the string "s"
 *	  s - the string to be copied
 *
 * output:
 *	  pointer to the new string
 */

char *copyn(register int n, register char *s)
{
    register char *p, *q;

    p = q = calloc(1, n);
    while (--n >= 0)
        *q++ = *s++;
    return (p);
}

/*
 * copys -- makes a copy of a string
 *
 * input:
 *	  s - string to be copied
 *
 * output:
 *	  pointer to the new string
 */
char *copys(char *s)
{
    return (copyn(strlen(s) + 1, s));
}
