all:
	flex ss2.source
	byacc -d -v y2.y
	gcc -o codegen lex.yy.c y.tab.c code.c -lfl
clean:
	rm -f codegen lex.yy.c y.output y.tab.c y.tab.h

