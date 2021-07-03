# flex scanner.l
# gcc -o scanner lex.yy.c -lfl

CC = gcc
FILES = lex.yy.c y.tab.c
OUT_EXE = parser
OPT = -lfl 
 
 
build:
	flex scanner.l
	byacc -d -v parser.y 
	$(CC) -o $(OUT_EXE) $(FILES) $(OPT)
 
clean:
	rm -f *.o core
	rm scanner lex.yy.c -f
	rm mytest.xml goldentest.xml

run:
	@echo ===========================================;
	./parser < sample_input.c
	@echo; echo ===========================================;
	golden_parser < sample_input.c
	@echo

test:
	./parser < sample_input.c > mytest.xml
	golden_parser < sample_input.c > goldentest.xml
	# python3 pretty.py ./mytest ./goldentest
	diff mytest.xml goldentest.xml

rebuild: clean build