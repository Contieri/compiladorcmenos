%{
    #define YYPARSER    /* distinguishes Yacc output from other code files */

    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    #include "parse.h"

    #define YYSTYPE TreeNode *
    static TreeNode * savedTree;    /* stores syntax tree for later return */
    static int yylex(void);
	int yyerror(char *msg);

%}

%token IF ELSE WHILE INT VOID RETURN
%token NUM ID
%token ASSIGN EQ NEQ LT LTE GT GTE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACKET RBRACKET LKEY RKEY COMMA SEMI
%token ERROR ENDFILE

%% /* GRAMATICA PARA A LINGUAGEM C MINUS */
/* $$, FUNCIONA COMO UM MARCADOR NA ARVORE, $2 , $S3 , ETC.. POSICAO NOS*/




programa            :   declaracao_lista
                            { savedTree = $1;}
                    ;
declaracao_lista    :   declaracao_lista declaracao
                            { YYSTYPE t = $1;
                                if(t != NULL){     while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $2;
                                    $$ = $1;
                                }
                                else
                                    $$ = $2;
                            }
                    |   declaracao
                            { $$ = $1;}
                    ;
declaracao          :   var_declaracao
                            { $$ = $1;}
                    |   fun_declaracao
                            { $$ = $1;}
                    ;
var_declaracao      :   INT ident SEMI
                            {
                               
									$$ = newExpNode(TypeK);
                                    $$->type = Integer;
									$$->attr.name = "integer";
                                    $$->child[0] = $2;
                                    $2->nodekind = StmtK;
                                    $2->kind.stmt = VarK;
                                    $2->type = Integer;
                            }
                    |   INT ident LBRACKET num RBRACKET SEMI
                            {     $$ = newExpNode(TypeK);
                                    $$->type = Integer;
									$$->attr.name = "integer";
                                    $$->child[0] = $2;
                                    $2->nodekind = StmtK;
                                    $2->kind.stmt = VecK;
                                    $2->attr.val = $4->attr.val;
                                    $2->type = Integer;
                            }
                    ;
fun_declaracao      :  INT ident LPAREN params RPAREN composto_decl
                            { $$ = newExpNode(TypeK);
                            	$$->type = Integer;
                           	$$->attr.name = "integer";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = FuncK;
                                $2->child[0] = $4;
                                $2->child[1] = $6;
				$2->type= Integer;
				aggScope($2->child[0], $2->attr.name);
				aggScope($2->child[1], $2->attr.name);
                            }
		    | VOID ident LPAREN params RPAREN composto_decl
                            { $$ = newExpNode(TypeK);
                            	$$->type = Void;
                           	$$->attr.name = "VOID";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = FuncK;
                                $2->child[0] = $4;
                                $2->child[1] = $6;
				$2->type= Void;
				aggScope($2->child[0], $2->attr.name);
				aggScope($2->child[1], $2->attr.name);
                            }
                    ;

		    

params              :   param_lista
                            { $$ = $1;}
                    |   VOID
                            {
							}
                    ;
param_lista         :   param_lista COMMA param
                            { YYSTYPE t = $1;
                                if(t != NULL){     while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $3;
                                    $$ = $1;
                                }
                                else
                                    $$ = $3;
                            }
                    |   param
                            { $$ = $1;}
                    ;
param               :   INT ident
                            {
				    $$ = newExpNode(TypeK);
                                    $$->type = Integer;
									$$->attr.name = "integer";
                                    $$->child[0] = $2;
                                    $2->nodekind = StmtK;
                                    $2->kind.stmt = VarK;
                                    $2->type = Integer;
                            }
                    |   INT ident LBRACKET RBRACKET
                            {     $$ = newExpNode(TypeK);
                                    $$->type = Integer;
									$$->attr.name = "integer";
                                    $$->child[0] = $2;
                                    $2->nodekind = StmtK;
                                    $2->kind.stmt = VecK;
                                    $2->type = Integer;
                            } 		
                    ;
composto_decl       :   LKEY local_declaracoes statement_lista RKEY
                            { YYSTYPE t = $2;
                                if(t != NULL){     while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $3;
                                    $$ = $2;
                                }
                                else
                                    $$ = $3;
                            }
                    |   LKEY local_declaracoes RKEY
                            { $$ = $2;}
                    |   LKEY statement_lista RKEY
                            { $$ = $2;}
                    |   LKEY RKEY
                            {}
                    ;
local_declaracoes   :   local_declaracoes var_declaracao
                            { YYSTYPE t = $1;
                                if(t != NULL){     while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $2;
                                    $$ = $1;
                                }
                                else
                                    $$ = $2;
                            }
                    |   var_declaracao
                            { $$ = $1;}
                    ;
statement_lista     :   statement_lista statement
                            { YYSTYPE t = $1;
                                if(t != NULL){     while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $2;
                                    $$ = $1;
                                }
                                else
                                    $$ = $2;
                            }
                    |   statement
                            { $$ = $1;}
                    ;
statement           :   expressao_decl
                            { $$ = $1;}
                    |   composto_decl
                            { $$ = $1;}
                    |   selecao_decl
                            { $$ = $1;}
                    |   iteracao_decl
                            { $$ = $1;}
                    |   retorno_decl
                            { $$ = $1;}
                    ;
expressao_decl      :   expressao SEMI 
                            { $$ = $1;}
                    |   SEMI
                            {}
                    ;
selecao_decl        :   IF LPAREN expressao RPAREN statement 
                            { $$ = newStmtNode(IfK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                            }
                    |   IF LPAREN expressao RPAREN statement ELSE statement
                            { $$ = newStmtNode(IfK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                                $$->child[2] = $7;
                            }
                    ;
iteracao_decl       :   WHILE LPAREN expressao RPAREN statement
                            { $$ = newStmtNode(WhileK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                            }
                    ;
retorno_decl        :   RETURN SEMI
                            { $$ = newStmtNode(ReturnK);
                            }
                    |   RETURN expressao SEMI
                            { $$ = newStmtNode(ReturnK);
                                $$->child[0] = $2;
                            }
                    ;
expressao           :   var ASSIGN expressao
                            { $$ = newStmtNode(AssignK);
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   simples_expressao
                            { $$ = $1;}
                    ;
var                 :   ident
                            { $$ = $1;
				$$->type=Integer;
                            }
                    |   ident LBRACKET expressao RBRACKET
                            { $$ = $1;
                                $$->kind.exp = VecIndexK;
                                $$->child[0] = $3;
				$$->type=Integer;
                            }
                    ;
simples_expressao   :   soma_expressao relacional soma_expressao
                            { $$ = $2;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   soma_expressao
                            { $$ = $1;
                            }
                    ;
relacional          :   EQ
                            { $$ = newExpNode(OpK);
                                $$->attr.op = EQ; 
				$$->type=Boolean;
				$$->attr.name = "==";                           
                            }
                    |   NEQ
                            { $$ = newExpNode(OpK);
                                $$->attr.op = NEQ;  
				$$->type=Boolean;
				$$->attr.name = "!=";                           
                            }
                    |   LT
                            { $$ = newExpNode(OpK);
                                $$->attr.op = LT; 
				$$->type=Boolean;
				$$->attr.name = "<";                            
                            }
                    |   LTE
                            { $$ = newExpNode(OpK);
                                $$->attr.op = LTE; 
				$$->type=Boolean; 
				$$->attr.name = "<=";                           
                            }
                    |   GT
                            { $$ = newExpNode(OpK);
                                $$->attr.op = GT;  
				$$->type=Boolean; 
				$$->attr.name = ">";                          
                            }
                    |   GTE
                            { $$ = newExpNode(OpK);
                                $$->attr.op = GTE;  
				$$->type=Boolean;
				$$->attr.name = ">=";                           
                            }
                    ;
soma_expressao      :   soma_expressao soma termo
                            { $$ = $2;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   termo
                            { $$ = $1;}
                    ;
soma                :   PLUS
                            { $$ = newExpNode(OpK);
                                $$->attr.op = PLUS; 
				$$->type=Integer;  
				$$->attr.name = "+";                          
                            }
                    |   MINUS
                            { $$ = newExpNode(OpK);
                                $$->attr.op = MINUS;   
				$$->type=Integer; 
				$$->attr.name = "-";                        
                            }
                    ;
termo               :   termo mult fator
                            { $$ = $2;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   fator
                            { $$ = $1;}
                    ;
mult                :   TIMES
                            {     $$ = newExpNode(OpK);
                                    $$->attr.op = TIMES;
                                    $$->attr.name = "*";
                                    $$->type = Integer;                         
                            }
                    |   OVER
                            {     $$ = newExpNode(OpK);
                                    $$->attr.op = OVER;
                                    $$->attr.name = "/";
                                    $$->type = Integer;                          
                            }
                    ;
fator               :   LPAREN expressao RPAREN
                            { $$ = $2;}
                    |   var
                            { $$ = $1;}
                    |   ativacao
                            { $$ = $1;}
                    |   num
                            { $$ = $1;}
                    ;
ativacao            :   ident LPAREN arg_lista RPAREN
                            { $$ = $1;
                                $$->child[0] = $3;
                                $$->kind.stmt = CallK;
                            }
                    |   ident LPAREN RPAREN
                            { $$ = $1;
                                $$->kind.stmt = CallK;
                            }
                    ;
arg_lista           :   arg_lista COMMA expressao
                            { YYSTYPE t = $1;
                                if(t != NULL){     while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $3;
                                    $$ = $1;
                                }
                                else
                                    $$ = $3;
                            }
                    |   expressao
                            { $$ = $1;}
                    ;
ident               :   ID
                            { $$ = newExpNode(IdK);
                                $$->attr.name = copyString(tokenString);
                            }
                    ;
num                 :   NUM
                            { $$ = newExpNode(ConstK);
                              $$->attr.val = atoi(tokenString);
				$$->type=Integer;
                            }
                    ;

%%

int yyerror(char* message){
    fprintf(listing,"Erro de SINTAXE na linha %d: %s\n",lineno,message);
    fprintf(listing,"TOKEN: ");
    printToken(yychar,tokenString);
    Error = TRUE;
    return 0;
}

static int yylex(void){
    return getToken();
}

TreeNode * parse(void){
    yyparse();
    return savedTree;
}
