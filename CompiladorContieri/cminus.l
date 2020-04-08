/****************************************************/
/* File: cminus.l                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/




%{

    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    /* lexeme of identifier or keyword */
    char tokenString[MAXTOKENLEN+1];
    static int yylex(void);
%}

/* setting standards */
digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%option noyywrap
%%

"/*"            {
                    char c;
                    int a = 2;
                    while(a){
                        c = input();
                        if(c == EOF) break;
                        else if(c == '\n') lineno++;
                        else{
                            if(c == '*') a = 1;
                            else if(a == 1 && c == '/') a = 0;
                            else a = 2;
                        }
                    }
                }

"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"int"           {return INT;}
"void"          {return VOID;}
"return"        {return RETURN;}
"="             {return ASSIGN;}
"=="            {return EQ;}
"!="            {return NEQ;}
"<"             {return LT;}
"<="            {return LTE;}
">"             {return GT;}
">="            {return GTE;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}
"{"             {return LKEY;}
"}"             {return RKEY;}
","             {return COMMA;}
";"             {return SEMI;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace*/}

.               {return ERROR;}

%%

TokenType getToken(void){
    static int firstTime = TRUE;
    TokenType currentToken;
    if(firstTime){
        firstTime = FALSE;
        lineno++;
        yyin = source;
        yyout = listing;
    }
    currentToken = yylex();
    strncpy(tokenString,yytext,MAXTOKENLEN);
    if(TraceScan){
        fprintf(listing,"\t%d: ",lineno);
        printToken(currentToken,tokenString);
    }
    return currentToken;
}
