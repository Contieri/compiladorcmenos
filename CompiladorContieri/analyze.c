#include "globals.h"
#include "symtab.h"
#include "analyze.h"





static int location = 0; //CONTADOR PARA LOCALIZACAO NA MEMORIA


static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"	Erro SEMÂNTICO NA LINHA %d: %s\n",t->lineno,message);
  Error = TRUE;
}


 //FUNCAO traverse E UMA ARVORE GENRICA RECURSIVA
 // PERCORRIMENTO EM PRE ORDEM , PONTEIRO t
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}



 //ANALISE SEMANTICA
 //insertNode INSERE OS IDENTIFICADORES GUARDADOS EM T NA TABELA DE SIMBOLOS
static void insertNode( TreeNode * t)

{ switch (t->nodekind)

  { case StmtK:

      switch (t->kind.stmt)

      {


				//CASO VARIAVEL
        case VarK:
                    if(st_lookup(t->attr.name, "global") >= 0){
                        switch(st_getIdType(t->attr.name, "global")){
                            case variableT:
                                typeError(t, "ERRO : A variavel ja foi declarada previamente");
                                break;
                            //case vectorT:
                               // typeError(t, "ERRO : Variavel com identificador de vetor");
                                break;
                            case functionT:
                                typeError(t, "ERRO : Variavel com identificador de funcao");
                                break;
                        }
                    }
                    else if(st_lookup(t->attr.name, t->scope) >= 0){
                        switch(st_getIdType(t->attr.name, t->scope)){
                            case variableT:
                                typeError(t, "ERRO : Variavel ja declarada");
                                break;
                            //case vectorT:
                                //typeError(t, "ERRO : Variavel com identificador de vetor");
                                break;
                            case functionT:
                                typeError(t, "ERRO : Variavel com identificador de funcao");
                                break;
                        }
                    }
                    else
                        st_insert(t->attr.name, t->scope, variableT, integerT, t->lineno, location++);
                    break;




			//CASO VETOR
        case VecK:
                    if(st_lookup(t->attr.name, "global") >= 0){
                        switch(st_getIdType(t->attr.name, "global")){
                            case variableT:
                                typeError(t, "ERRO : Vetor com identificador de variavel");
                                break;
                            case vectorT:
                                typeError(t, "ERRO : Vetor ja declarado");
                                break;
                            case functionT:
                                typeError(t, "ERRO : Vetor com identificador de funcao");
                                break;
                        }
                    }
                    else if(st_lookup(t->attr.name, t->scope) >= 0){
                        switch(st_getIdType(t->attr.name, t->scope)){
                            case variableT:
                                typeError(t, "ERRO : Vetor com identificador de variavel");
                                break;
                            case vectorT:
                                typeError(t, "ERRO : Vetor ja declarado ");
                                break;
                            case functionT:
                                typeError(t, "ERRO : Vetor com identificador de funcao");
                                break;
                        }
                    }
                    else
                        st_insert(t->attr.name, t->scope, vectorT, integerT, t->lineno, location++);
                    break;
	    case ReturnK:
      		break;



			//CASO DE UMA FUNCAO
    	case FuncK:
                    if(st_lookup(t->attr.name, "global") >= 0){
                        switch(st_getIdType(t->attr.name, "global")){
                            case variableT:
                                typeError(t, "ERRO SEMÂNTICO : Funcao com identificador de variavel");
                                break;
                            case vectorT:
                                typeError(t, "ERRO : Funcao com identificador de vetor");
                                break;
                            case functionT:
                                typeError(t, "ERRO : A funcao ja foi declarada");
                                break;
                        }
                    }
                    else if(st_lookup(t->attr.name, t->scope) >= 0){
                        switch(st_getIdType(t->attr.name, t->scope)){
                            case variableT:
                                typeError(t, "ERRO : Funcao com identificador de variavel");
                                break;
                            case vectorT:
                                typeError(t, "ERRO : Funcao com identificador de vetor");
                                break;
                            case functionT:
                                typeError(t, "ERRO : A funcao ja foi declarada");
                                break;
                        }
                    }
                    else{
                        if(t->type == Integer)
                            st_insert(t->attr.name, t->scope, functionT, integerT, t->lineno, location++);
                        else if(t->type == Void)
                            st_insert(t->attr.name, t->scope, functionT, voidT, t->lineno, location++);
                    }
                    break;
                default:
                    break;
     }
     break;



		//CASO NAO SEJAM DECLARADOS
	case ExpK:

             switch(t->kind.exp){

					//CASO SEJA UMA VARIAVEL ELE PROCURA NA
					//FUNCAO st_lookup RETORNA A LOCALIZACAO NA MEMORIA DE UMA VARIAVEL OU -1 CASO NAO SEJA ENCONTRADA

                case IdK:
                    if((st_lookup(t->attr.name, "global") == -1) && (st_lookup(t->attr.name, t->scope) == -1))
                        typeError(t, "ERRO : Uso de variavel nao declarada previamente");

                    else if(st_lookup(t->attr.name, "global") >= 0)
                        st_insert(t->attr.name, "global", 0, 0, t->lineno, 0);

                    else if(st_lookup(t->attr.name, t->scope) >= 0)
                        st_insert(t->attr.name, t->scope, 0, 0, t->lineno, 0);

                    break;
                case VecIndexK:
                    if((st_lookup(t->attr.name, "global") == -1) && (st_lookup(t->attr.name, t->scope) == -1))
                        typeError(t, "ERRO : Uso da posicao de um vetor que nao foi previamente declarado");

                    else if(st_lookup(t->attr.name, "global") >= 0)
                        st_insert(t->attr.name, "global", 0, 0, t->lineno, 0);

                    else if(st_lookup(t->attr.name, t->scope) >= 0)
                        st_insert(t->attr.name, t->scope, 0, 0, t->lineno, 0);
                    break;

                case CallK:
						//EXCETO AS FUNCOES MAIN, INPUT E OUTPUT
                    if((st_lookup(t->attr.name, "global") == -1) && (st_lookup(t->attr.name, t->scope) == -1)&& (strcmp(t->attr.name,"input")!=0)&& (strcmp(t->attr.name,"output")!=0) )
                        typeError(t, "ERRO : Chamada de funcao que nao foi previamente declarada");

                    else if(st_lookup(t->attr.name, "global") >= 0)
                        st_insert(t->attr.name, "global", 0, 0, t->lineno, 0);

                    else if(st_lookup(t->attr.name, t->scope) >= 0)
                        st_insert(t->attr.name, t->scope, 0, 0, t->lineno, 0);
                    break;
                default:
                    break;
            }
            break;
    default:
      break;
  }
}




/* Procedure checkNode performs
 * type checking at a single tree node
 */

 //checkNode VERIFICA SE OS TIPOS ESTAO COERENTES
//ATRAVES DE PEQUENAS SUB ARVORES
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  {


	case ExpK:
      switch(t->kind.exp){
                case TypeK:
                    break;
                case OpK:
                    if((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
                        typeError(t, "ERRO: Operador aritmetico aplicado a entidades nao inteiras");
                    break;
                case ConstK:
                    break;
                case IdK:
                    break;
		        case CallK:
                    break;
                case VecIndexK:
                    break;
                default:
                    break;
            }



      break;
    case StmtK:
      switch(t->kind.stmt){
                case IfK:
                    if(t->child[0]->type != Boolean)
                        typeError(t->child[0], "ERRO: Condicao IF nao booleano");
                break;
                case WhileK:
                    if(t->child[0]->type != Boolean)
                        typeError(t->child[0], "ERRO: Condicao WHILE nao booleano");
                    break;
                case AssignK:
                    if(t->child[1]->nodekind == ExpK && t->child[1]->kind.exp == CallK){
                        if((st_getDataType(t->child[1]->attr.name, t->child[1]->scope) != integerT) && (st_getDataType(t->child[1]->attr.name, "global") != integerT)&& (strcmp(t->child[1]->attr.name,"input")!=0))
                            typeError(t->child[1], "ERRO : Atribuicao invalida para variavel");
                    }
                    else if(t->child[1]->type != Integer)
                        typeError(t->child[1], "ERRO : Atribuicao invalida para variavel ");
                    break;
                case ReturnK:
                    break;
                case VarK:
                    break;
                case VecK:
                    break;
                case FuncK:
                    break;
                default:
                    break;
            }


      break;
    default:
      break;

  }
}



 //A FUNCAO buildSymTab CONTROI A TABELA DE SIMBOLOS ATRAVES DA LEITURA PRE ORDEM DA ARVORE SINTATICA

void buildSymtab(TreeNode * syntaxTree)
{
	st_initialize();
	traverse(syntaxTree,insertNode,nullProc);
	if(st_lookup("main","global") == -1){
		fprintf(listing, "       ERRO : A Funcao MAIN nao existe ");
		Error = TRUE;
	}


  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}


/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
