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
void printToken(char *format, char *text);
void printLineInfo();
%}

%x CMNT_MULTILINE
%x CMNT_ONELINE
%x STRING_CONSTANT
%x PRAGMA

%option yylineno
%%

"//"[^\n]* {addToBuffer(buff, yytext);}
"/*" {BEGIN CMNT_MULTILINE; addToBuffer(buff, yytext);}
<CMNT_MULTILINE>"*/" {BEGIN INITIAL; addToBuffer(buff, yytext);}
<CMNT_MULTILINE>\n {++num_lines; addToBuffer(buff, yytext); printLineInfo();}
<CMNT_MULTILINE>. {addToBuffer(buff, yytext);}

\n {BEGIN INITIAL; ++num_lines; addToBuffer(buff, yytext); printLineInfo(); }

"#pragma" {BEGIN PRAGMA; addToBuffer(buff, yytext);}
<PRAGMA>[ \t]+"source"[ \t]+"on" {BEGIN INITIAL; addToBuffer(buff, yytext); is_pragma_source_on = 1;}
<PRAGMA>[ \t]+"source"[ \t]+"off" {BEGIN INITIAL; addToBuffer(buff, yytext); is_pragma_source_on = 0;}
<PRAGMA>[ \t]+"token"[ \t]+"on" {BEGIN INITIAL; addToBuffer(buff, yytext); is_pragma_token_on = 1;}
<PRAGMA>[ \t]+"token"[ \t]+"off" {BEGIN INITIAL; addToBuffer(buff, yytext);  is_pragma_token_on = 0;}
<PRAGMA>\n {BEGIN INITIAL; ++num_lines; addToBuffer(buff, yytext); printLineInfo(); }
<PRAGMA>. {addToBuffer(buff, yytext);}

"for"|"do"|"while"|"break"|"continue"|"if"|"else"|"return"|"struct"|"switch"|"case"|"default"|"void"|"int"|"double"|"float"|"char"|"const"|"signed"|"unsigned"|"short"|"long" {addToBuffer(buff, yytext); printToken("#key:%s\n", yytext);}
"NULL"|"__COUNTER__"|"__LINE__"|"INT_MAX"|"INT_MIN"|"CHAR_MAX"|"CHAR_MIN"|"MAX"|"MIN" {addToBuffer(buff, yytext); printToken("#macro:%s\n", yytext); }
[A-Za-z_][A-Za-z0-9_]* {addToBuffer(buff, yytext); printToken("#id:%s\n", yytext);}
"+"|"-"|"*"|"/"|"++"|"\%"|"++"|"--"|"<"|"<="|">"|">="|"=="|"!="|"="|"&&"|"||"|"!"|"&"|"|" {addToBuffer(buff, yytext); printToken("#op:%s\n", yytext);}
":"|";"|","|"\."|"["|"]"|"("|")"|"{"|"}" {addToBuffer(buff, yytext); printToken("#punc:%s\n", yytext);}

("+"|"-")?[0-9]+"."[0-9]* {addToBuffer(buff, yytext); printToken("#float:%s\n", yytext);}
("+"|"-")?[0-9]*"."[0-9]+ {addToBuffer(buff, yytext); printToken("#float:%s\n", yytext);}
("+"|"-")?[0-9]+ {addToBuffer(buff, yytext); printToken("#integer:%s\n", yytext);}


"\"" {BEGIN STRING_CONSTANT; addToBuffer(buff, yytext); addToBuffer(str, yytext);}
<STRING_CONSTANT>"\\". {addToBuffer(buff, yytext); addToBuffer(str, yytext);}
<STRING_CONSTANT>"\"" {BEGIN INITIAL; addToBuffer(buff, yytext); addToBuffer(str, yytext); printToken("#string:%s\n", str);}
<STRING_CONSTANT>. {addToBuffer(buff, yytext); addToBuffer(str, yytext);}

'(("\\"[aebfnrtv"\\""'""?"])|("\\"[01234567]{1,3})|("\\x"[0123456789aAbBcCdDeEfF]+)|("\\u"[0123456789aAbBcCdDeEfF]{4})|("\\U"[0123456789aAbBcCdDeEfF]{8}))' {addToBuffer(buff, yytext); addToBuffer(str, yytext); printToken("#char:%s\n", yytext);}
'.' {addToBuffer(buff, yytext); addToBuffer(str, yytext); printToken("#char:%s\n", yytext);}

. {addToBuffer(buff, yytext);}
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
int main(int argc, char *argv[])
{
    yylex();
    // printf("line:%d char:%d\n", yylineno, num_chars);
    return 0;
}

// '("\\")?.' {addToBuffer(buff, yytext); addToBuffer(str, yytext); printToken("#char:%s\n", yytext);}