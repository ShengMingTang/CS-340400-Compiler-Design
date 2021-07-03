      .text
      .file "(null)"
   #  main
      .globl main                    # -- Begin function main
      .p2align 2 
      .type main,@function 
   #    
main:
        addi sp,sp,-48 
        sd   ra,40(sp) 
        sd   fp,32(sp) 
        addi fp,sp,48 
         
        lui     t0,%hi(a)
        lw     t1, %lo(a)(t0)
        addi sp, sp, -4
        sw t1, 0(sp)
        lui     t0,%hi(a)
        lw     t1, %lo(a)(t0)
        addi sp, sp, -4
        sw t1, 0(sp)
        li t0,   3
        addi sp, sp, -4
        sw t0, 0(sp)
        lw t0, 0(sp)
        addi sp, sp, 4
        lw t1, 0(sp)
        addi sp, sp, 4
        add  t0, t0, t1
        addi sp, sp, -4
        sw t0, 0(sp)
        lw  t0, 0(fp) 
        addi sp, sp, 4
        lw  t1, 0(fp) 
        addi sp, sp, 4
        lui     t2,%hi(a)
        sw     t0,%lo(a)(t2)
        addi sp, sp, -4
        sw t0, 0(sp)
        addi sp, sp, 4
   
   #    
        ld ra,40(sp) # old ra
        ld fp,32(sp) # old fp
        addi sp,sp,48# pop activiation record
        ret
.Lfunc_main_end0:
        .size      main, .Lfunc_main_end0-main 
       
        .type   a,@object
        .comm   a,4,4
        .type   b,@object
        .comm   b,4,4
 
        .ident "NTHU Compiler Class Code Generator for RISC-V"
        .section "note.stack","",@progbits
