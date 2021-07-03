# flex scanner.l
# gcc -o scanner lex.yy.c -lfl

CC = gcc
FILES = lex.yy.c y.tab.c code.c
OUT_EXE = codegen
OPT = -lfl 
 
 
build:
	flex scanner.l
	byacc -d -v parser.y 
	$(CC) -o $(OUT_EXE) $(FILES) $(OPT)
 
clean:
	rm -f *.o core
	rm scanner lex.yy.c y.tab.c y.tab.h y.output -f
	rm codegen codegen.S a.out

run:
	echo no

basic:
	./codegen < ./Testcase/Basic/0.c
	riscv64-unknown-elf-gcc ./Spike/assembly/main.c codegen.S
	spike pk a.out
	
arith:
	echo "0.c"
	./codegen < ./Testcase/ArithmeticExpression/0.c
	riscv64-unknown-elf-gcc ./Spike/assembly/main.c codegen.S
	spike pk a.out
	echo "1.c"
	./codegen < ./Testcase/ArithmeticExpression/1.c
	riscv64-unknown-elf-gcc ./Spike/assembly/main.c codegen.S
	spike pk a.out

Pointer:
	echo "0.c"
	./codegen < ./Testcase/Pointer/0.c
	riscv64-unknown-elf-gcc ./Spike/assembly/main.c codegen.S
	spike pk a.out
	echo "1.c"
	./codegen < ./Testcase/Pointer/1.c
	riscv64-unknown-elf-gcc ./Spike/assembly/main.c codegen.S
	spike pk a.out
test:
	./codegen < ./test.c
	riscv64-unknown-elf-gcc ./Spike/assembly/main.c codegen.S
	spike pk a.out
rebuild: clean build