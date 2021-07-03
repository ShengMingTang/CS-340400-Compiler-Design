# flex scanner.l
# gcc -o scanner lex.yy.c -lfl

CC = gcc
FILES = lex.yy.c
OUT_EXE = scanner
OPT = -lfl 
 
 
build:
	flex scanner.l
	$(CC) -o $(OUT_EXE) $(FILES) $(OPT)
 
clean:
	rm -f *.o core
	rm scanner lex.yy.c -f
	rm golden.txt testt.txt diff.txt -f
	rm sample_mytest.out -f

run:
	./scanner < testt.c > testt.txt
	golden_scanner < testt.c > golden.txt
	diff testt.txt golden.txt

sample:
	./scanner < sample_testcase.txt > sample_mytest.out
	diff sample_mytest.out sample_testcase.out

rebuild: clean build