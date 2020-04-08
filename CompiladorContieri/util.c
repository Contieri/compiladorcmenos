#include "globals.h"
#include "util.h"

void printToken(TokenType token, const char* tokenString){

    switch(token){
	    case IF:
	    case INT:
	    case RETURN:
	    case VOID:
	    case ELSE:
	    case WHILE:
	      fprintf(listing,
		 "reserved word: %s\n",tokenString);
	      break;
           
        case ASSIGN: fprintf(listing,"Special Character: =\n"); break;
        case LT: fprintf(listing,"Special Character: <\n"); break;
        case LTE: fprintf(listing,"Special Character: <=\n"); break;
        case GT: fprintf(listing,"Special Character: > \n"); break;
        case GTE: fprintf(listing,"Special Character: >=\n"); break;
        case EQ: fprintf(listing,"Special Character: ==\n"); break;
        case NEQ: fprintf(listing,"Special Character: != \n"); break;
        case LPAREN: fprintf(listing,"Special Character: ( \n"); break;
        case RPAREN: fprintf(listing,"Special Character: )\n"); break;
        case LBRACKET: fprintf(listing,"Special Character: [\n"); break;
        case RBRACKET: fprintf(listing,"Special Character: ]\n"); break;
        case LKEY: fprintf(listing,"Special Character: { \n"); break;
        case RKEY: fprintf(listing,"Special Character: }\n"); break;
        case SEMI: fprintf(listing,"Special Character: ;\n"); break;
        case COMMA: fprintf(listing,"Special Character: ,\n"); break;
        case PLUS: fprintf(listing,"Special Character: +\n"); break;
        case MINUS: fprintf(listing,"Special Character: - \n"); break;
        case TIMES: fprintf(listing,"Special Character: *\n"); break;
        case OVER: fprintf(listing,"Special Character: /\n"); break;
        case ENDFILE: fprintf(listing,"Fim do arquivo\n"); break;
        case NUM: fprintf(listing, "NUM, val: %s\n",tokenString); break;
        case ID: fprintf(listing, "ID, name: %s\n",  tokenString); break;
        case ERROR: fprintf(listing, "ERROR: %s\n", tokenString); break;
        default: fprintf(listing,"Unknown Token: %d\n",token);
    }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind){
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if(t==NULL)
        fprintf(listing,"ERRO: sem memoria no local/linha %d\n",lineno);
    else{
        for(i=0;i<MAXCHILDREN;i++)
            t->child[i] = NULL;

        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
	t->scope = "global";
    }
    return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind){

    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if(t==NULL)
        fprintf(listing,"ERRO: sem memoria no local/linha %d\n",lineno);
    else{
        for(i=0;i<MAXCHILDREN;i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
	t->scope = "global";
    }
    return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s){

    int n;
    char * t;
    
    if(s==NULL)
        return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if(t==NULL)
        fprintf(listing,"ERRO: sem memoria no local/linha %d\n",lineno);
    else
        strcpy(t,s);
    return t;
}

void aggScope(TreeNode* tree, char* scope){
	int i;
	while(tree != NULL){
        tree->scope = copyString(scope);
		for(i=0; i<MAXCHILDREN; ++i){
			aggScope(tree->child[i], scope);
		}
		tree = tree->sibling;
	}
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=6
#define UNINDENT indentno-=6

/* printSpaces indents by printing spaces */
static void printSpaces(void){
    int i;
    for(i=0;i<indentno;i++)
        fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode * tree){ 

    int i;
    INDENT;
    
    while(tree!=NULL){
        printSpaces();
        if(tree->nodekind==StmtK){
            switch (tree->kind.stmt){
                case IfK:
				
                    if(tree->child[2] != NULL)
                        fprintf(listing,"IfElse: \n");
                    else
                        fprintf(listing,"If: \n");
                    break;
                case WhileK:
                    fprintf(listing,"While: \n");
                    break;
                case AssignK:
                    fprintf(listing,"Assign to: \n");
                    break;
                case ReturnK:
                    if(tree->child[0] != NULL)
                        fprintf(listing,"Return: \n");
                    else
                        fprintf(listing,"Return: \n");
                    break;
                case VarK:
                    fprintf(listing,"Declaracao de variavel: %s\n", tree->attr.name);
                    break;
                case VecK:
                    fprintf(listing,"Declaracao de vetor: %s[%d]\n", tree->attr.name, tree->attr.val);
                    break;
                case FuncK:
                    if(tree->child[0] != NULL){
                        
                        fprintf(listing,"Declaracao de funcao: %s()\n",
                                tree->attr.name);
                    }
                    else
                        fprintf(listing,"Declaracao de funcao: %s(void)\n",tree->attr.name);
                    break;
                case CallK:
                    if(tree->child[0] != NULL){
                        
                        fprintf(listing,"Chamada de funcao: %s\n", tree->attr.name);
                    }
                    else
                        fprintf(listing,"Chamada de funcao: %s\n", tree->attr.name);
                    break;
                default:
                    fprintf(listing,"ExpNode tipo nao reconhecido\n");
                    break;
            }
        }
        else if(tree->nodekind==ExpK){
            switch(tree->kind.exp){
                case OpK:
                    fprintf(listing,"Operador: ");
                    printToken(tree->attr.op,"\0");
                    break;
                case ConstK:
                    fprintf(listing,"Constante: %d\n", tree->attr.val);
                    break;
                case IdK:
                    fprintf(listing,"Identificador: %s\n", tree->attr.name);
                    break;
                case VecIndexK:
                    fprintf(listing,"Indice do vetor: %s[]\n", tree->attr.name);
                    break;
                case TypeK:
                    fprintf(listing,"Tipo: %s\n", tree->attr.name);
                    break;
                default:
                    fprintf(listing,"Unknown ExpNode kind\n");
                    break;
            }
        }
        else
            fprintf(listing,"Tipo no nao reconhecido\n");
        for(i=0;i<MAXCHILDREN;i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
