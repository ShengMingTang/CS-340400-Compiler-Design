idStart [A-Za-z_]
%{
#include <stdio.h>
#include <string.h>
int num_lines = 0, num_chars = 0;
int is_pragma_source_on = 1;
int is_pragma_token_on = 1;
char buff[600] = {0};
char str[600] = {0};

void addToBuffer(char* p1, char *p2);
void clearBuffer(char *p);
void printToken(char *format, char *text);
void printLineInfo();

// added
#include "code.h"
#include "y.tab.h"
extern YYSTYPE yylval;
%}

%x CMNT_MULTILINE
%x CMNT_ONELINE
%x STRING_CONSTANT
%x PRAGMA

%option yylineno
%%

"//"[^\n]* {/*strcpy(yylval.stringVal,  yytext); return CMNT;*/}
"/*" {BEGIN CMNT_MULTILINE; addToBuffer(buff, yytext);}
<CMNT_MULTILINE>"*/" {BEGIN INITIAL; addToBuffer(buff, yytext); strcpy(yylval.stringVal, buff); clearBuffer(buff); /*return CMNT;*/}
<CMNT_MULTILINE>\n {++num_lines; addToBuffer(buff, yytext);}
<CMNT_MULTILINE>. {addToBuffer(buff, yytext);}

\n {BEGIN INITIAL; ++num_lines;}

"#pragma" {BEGIN PRAGMA; addToBuffer(buff, yytext);}
<PRAGMA>[ \t]+"source"[ \t]+"on" {BEGIN INITIAL; addToBuffer(buff, yytext); is_pragma_source_on = 1;}
<PRAGMA>[ \t]+"source"[ \t]+"off" {BEGIN INITIAL; addToBuffer(buff, yytext); is_pragma_source_on = 0;}
<PRAGMA>[ \t]+"token"[ \t]+"on" {BEGIN INITIAL; addToBuffer(buff, yytext); is_pragma_token_on = 1;}
<PRAGMA>[ \t]+"token"[ \t]+"off" {BEGIN INITIAL; addToBuffer(buff, yytext);  is_pragma_token_on = 0;}
<PRAGMA>\n {BEGIN INITIAL; ++num_lines; addToBuffer(buff, yytext); strcpy(yylval.stringVal, buff); clearBuffer(buff); /*return MYPRAGMA;*/}
<PRAGMA>. {addToBuffer(buff, yytext);}

"struct" {strcpy(yylval.stringVal, yytext); return STRUCT;}
"for" {strcpy(yylval.stringVal, yytext); return FOR;}
"do" {strcpy(yylval.stringVal, yytext); return DO;}
"while" {strcpy(yylval.stringVal, yytext); return WHILE;}
"break" {strcpy(yylval.stringVal, yytext); return BREAK;}
"continue" {strcpy(yylval.stringVal, yytext); return CONTINUE;}
"if" {strcpy(yylval.stringVal, yytext); return IF;}
"else" {strcpy(yylval.stringVal, yytext); return ELSE;}
"return" {strcpy(yylval.stringVal, yytext); return RETURN;}
"switch" {strcpy(yylval.stringVal, yytext); return SWITCH;}
"case" {strcpy(yylval.stringVal, yytext); return CASE;}
"default" {strcpy(yylval.stringVal, yytext); return DEFAULT;}
"void" {strcpy(yylval.stringVal, yytext); return VOID;}
"int" {strcpy(yylval.stringVal, yytext); return INT;}
"double" {strcpy(yylval.stringVal, yytext); return DOUBLE;}
"float" {strcpy(yylval.stringVal, yytext); return FLOAT;}
"char" {strcpy(yylval.stringVal, yytext); return CHAR;}
"const" {strcpy(yylval.stringVal, yytext); return CONST;}
"signed"|"unsigned" {strcpy(yylval.stringVal, yytext); return SIGNED_UNSIGNED;}
"short" {strcpy(yylval.stringVal, yytext); return SHORT;}
"long" {strcpy(yylval.stringVal, yytext); return LONG;}

"HIGH" {yylval.intVal = 1; return CONST_INT;}
"LOW" {yylval.intVal = 0; return CONST_INT;}

"MAX"|"MIN" {addToBuffer(buff, yytext); printToken("#macro:%s\n", yytext);}
"NULL" {yylval.intVal = 0; return CONST_INT;}
"__COUNTER__"|"__LINE__"|"INT_MAX"|"INT_MIN"|"CHAR_MAX"|"CHAR_MIN" {yylval.intVal = atoi(yytext); return CONST_INT;}

[A-Za-z_][A-Za-z0-9_]* {strcpy(yylval.stringVal, yytext); return ID;}


"->"|"." {strcpy(yylval.stringVal, yytext); return MEMBER;}
"+="|"-="|"*/"|"/="|"%="|"<<="|">>="|"&="|"^="|"*="|"/="|"*="|"|=" {strcpy(yylval.stringVal, yytext); return ASSIGNMENT;}

"?"|":" {strcpy(yylval.stringVal, yytext); return yytext[0];}
"!"|"&"|"|"|"!"|"~"|"^" {strcpy(yylval.stringVal, yytext); return yytext[0];}
">>"|"<<" {strcpy(yylval.stringVal, yytext); return BITSHIFT;}
"&&" {strcpy(yylval.stringVal, yytext); return LOGICALAND;}
"||" {strcpy(yylval.stringVal, yytext); return LOGICALOR;}
"<"|"<="|">"|">=" {strcpy(yylval.stringVal, yytext); return RELATION;}
"=="|"!=" {strcpy(yylval.stringVal, yytext); return EQNEQ;}
"++"|"--" {strcpy(yylval.stringVal, yytext); return INCDEC;}
"="|"+"|"-"|"\%"|"/"|"*"|":"|";"|","|"\."|"["|"]"|"("|")"|"{"|"}" {return yytext[0];}

[0-9]+"."[0-9]* {yylval.floatVal = atof(yytext); return CONST_FLOAT;}
[0-9]*"."[0-9]+ {yylval.floatVal = atof(yytext); return CONST_FLOAT;}
[0-9]+ {yylval.intVal = atoi(yytext); return CONST_INT;}


"\"" {BEGIN STRING_CONSTANT; addToBuffer(str, yytext);}
<STRING_CONSTANT>"\\". {addToBuffer(str, yytext);}
<STRING_CONSTANT>"\"" {BEGIN INITIAL; addToBuffer(str, yytext); strcpy(yylval.stringVal, str); clearBuffer(str); return CONST_STRING;}
<STRING_CONSTANT>. {addToBuffer(str, yytext);}

'(("\\"[aebfnrtv"\\""'""?"])|("\\"[01234567]{1,3})|("\\x"[0123456789aAbBcCdDeEfF]+)|("\\u"[0123456789aAbBcCdDeEfF]{4})|("\\U"[0123456789aAbBcCdDeEfF]{8}))' {strcpy(yylval.stringVal, yytext); return CONST_CHAR;}
'.' {strcpy(yylval.stringVal, yytext); return CONST_CHAR;}

. {}
%%
void printLineInfo()
{
    if(is_pragma_source_on){
        printf("%d:%s", num_lines, buff);
    }
    buff[0] = '\0';
}
void printToken(char *format, char *text)
{
    if(is_pragma_token_on){
        printf(format, text);
    }
    text[0] = 0;
}
void addToBuffer(char* p1, char *p2)
{
    strcat(p1, p2);
    num_chars += strlen(p2);
}
void clearBuffer(char *p)
{
    p[0] = 0;
}

/*
"sizeof" {strcpy(yylval.stringVal, yytext); return SIZEOF;}

const parsing
("+"|"-")?[0-9]+"."[0-9]* {yylval.floatVal = atof(yytext); return CONST_FLOAT;}
("+"|"-")?[0-9]*"."[0-9]+ {yylval.floatVal = atof(yytext); return CONST_FLOAT;}
("+"|"-")?[0-9]+ {yylval.intVal = atoi(yytext); return CONST_INT;}
*/