
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
int cur_scope   = 1;
char *copys();



/*

  init_symbol_table();

*/
void init_symbol_table()
{

  bzero(&table[0], sizeof(struct symbol_entry)*MAX_TABLE_SIZE);

}

/*
   To install a symbol in the symbol table 

*/
char * install_symbol(char *s)
{

   if (cur_counter >= MAX_TABLE_SIZE)
     perror("Symbol Table Full");
   else {
    table[cur_counter].scope = cur_scope;
    table[cur_counter].name = copys(s);
    cur_counter++;
  }
   return(s);
}



/*
   To return an integer as an index of the symbol table

*/
int look_up_symbol(char *s)
{
   int i;

   if (cur_counter==0) return(-1);
   for (i=cur_counter-1;i>=0; i--)
     {
       if (!strcmp(s,table[i].name))
	 return(i);
     }
   return(-1);
 }


/*
   Pop up symbols of the given scope from the symbol table upon the
   exit of a given scope.

*/
void pop_up_symbol(int scope)
{
   int i;
   if (cur_counter==0) return;
   for (i=cur_counter-1;i>=0; i--)
     {
       if (table[i].scope !=scope) break;
     }
   if (i<0) cur_counter = 0;
   cur_counter = i+1;
  
}



/*
   Set up parameter scope and offset

*/
void set_scope_and_offset_of_param(char *s)
{

  int i,j,index;
  int total_args;

   index = look_up_symbol(s);
   if (index<0) perror("Error in function header");
   else {
      table[index].type = T_FUNCTION;
      total_args = cur_counter -index -1;
      table[index].total_args=total_args;
      for (j=total_args, i=cur_counter-1;i>index; i--,j--)
        {
          table[i].scope= cur_scope;
          table[i].offset= j;
          table[i].mode  = ARGUMENT_MODE;
          table[i].functor_index  = index;
        }
   }

}



/*
   Set up local var offset

*/
void set_local_vars(char *functor)
{

  int i,j,index,index1;
  int total_locals;

  index = look_up_symbol(functor);
  index1 = index + table[index].total_args;
  total_locals= cur_counter -index1 -1;
  if (total_locals <0)
     perror("Error in number of local variables");
  table[index].total_locals=total_locals;
  for (j=total_locals, i=cur_counter-1;j>0; i--,j--)
        {
          table[i].scope= cur_scope;
          table[i].offset= j;
          table[i].mode  = LOCAL_MODE;
        }

}



/*
  Set GLOBAL_MODE to global variables

*/

void set_global_vars(char *s)
{
  int index;
  index =look_up_symbol(s);
  table[index].mode = GLOBAL_MODE;
  table[index].scope =1;
}


/*

To generate house-keeping work at the beginning of the function

*/

void code_gen_func_header(char *functor)
{

fprintf(f_asm,"   #  %s\n",functor);
fprintf(f_asm,"      .globl main                    # -- Begin function %s\n",functor);
fprintf(f_asm,"      .p2align 2 \n");
fprintf(f_asm,"      .type %s,@function \n",functor);   
fprintf(f_asm,"   #    \n");
fprintf(f_asm,"%s:\n",functor);
 
fprintf(f_asm,"        addi sp,sp,-48 \n"); 
fprintf(f_asm,"        sd   ra,40(sp) \n");         
fprintf(f_asm,"        sd   fp,32(sp) \n");         
fprintf(f_asm,"        addi fp,sp,48 \n");         
fprintf(f_asm,"         \n");         



}

/*

  To generate global symbol vars

*/
void code_gen_global_vars()
{
  int i;


  for (i=0; i<cur_counter; i++)
     {
       if (table[i].mode == GLOBAL_MODE)
	 {
            fprintf(f_asm,"        .type   %s,@object\n",table[i].name);
            fprintf(f_asm,"        .comm   %s,4,4\n",table[i].name);
         }
     }

  fprintf(f_asm," \n");
  fprintf(f_asm,"        .ident \"NTHU Compiler Class Code Generator for RISC-V\"\n");  
  fprintf(f_asm,"        .section \"note.stack\",\"\",@progbits\n");  
}


/*

 To geenrate house-keeping work at the end of a function

*/

void code_gen_at_end_of_function_body(char *functor)
{
  int i;

  fprintf(f_asm,"   #    \n");
    
  fprintf(f_asm,"        ld ra,40(sp) # old ra\n");
  fprintf(f_asm,"        ld fp,32(sp) # old fp\n");
  fprintf(f_asm,"        addi sp,sp,48# pop activiation record\n");
  fprintf(f_asm,"        ret\n");

  fprintf(f_asm,".Lfunc_%s_end0:\n",functor);
  fprintf(f_asm,"        .size      %s, .Lfunc_%s_end0-%s \n",functor,functor,functor);
  fprintf(f_asm,"       \n");
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

char * copyn(register int n, register char *s)
{
	register char *p, *q;

	p = q = calloc(1,n);
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
char * copys(char *s)
{
	return (copyn(strlen(s) + 1, s));
}






