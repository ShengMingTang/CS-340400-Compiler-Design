# CS 340400 Compiler Design
CS 340400 Compiler Design Term Project

---

## HW1 Lexer
See hw1/HW1.pdf
```shell
$ cd hw1
$ make
$ make sample # to run sample_testcase
```
---

## HW2 Grammar
See hw2/HW2.pdf\
Implement a subset of C grammar in YACC
```shell
$ cd hw2
$ make
$ make run
```

---

## HW3 Code generation
See hw3/HW3.pdf\
Extend HW2 and must be run in RISCV-64 simulator\
There are 5 checkpoints, chkpt5_demo is ready to run.\
chekpt1-4 directories contain different version of code from bottom-up code generation to top-down code generation (Syntax tree node structure).
```shell
$ cd hw3
$ cd chkpt5_demo
$ make
$ make basic # to test chkpt1
$ make arith # to test chkpt2
$ make Pointer # to test chkpt3
$ make Jump # to test chkpt4
$ make Function # to test chkpt5
```
