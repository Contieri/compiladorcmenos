#include "globals.h"
#include "symtab.h"

#include "lcode.h"

static int tmpOffset = 0;



QuadList head = NULL;

int location = 0;
int mainLocation;

int nlabel = 0;
int ntemp = 0;

Address aux;
Address var;
Address offset;
Address empty;

void emitComment( char * c ) { 
  if (TraceCode) fprintf(code,"// %s\n",c);
  printf("%s\n", c);
}


const char * OpKindNames[] =  { "add", "sub", "mult", "div", "lt", "lte", "gt", "gte", "eq", "or", "assign", "alloc", "immed", "load", "store",
                              "vec", "goto", "if_f", "ret", "fun", "end", "param", "call", "arg", "lab", "hlt" };



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
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

void quad_insert (OpKind op, Address addr1, Address addr2, Address addr3) {
  Quad quad;
  quad.op = op;
  quad.addr1 = addr1;
  quad.addr2 = addr2;
  quad.addr3 = addr3;
  QuadList new = (QuadList) malloc(sizeof(struct QuadListRec));
  new->location = location;
  new->quad = quad;
  new->next = NULL;
  if (head == NULL) {
    head = new;
  }
  else {
    QuadList q = head;
    while (q->next != NULL) q = q->next;
    q->next = new;
  }
  location ++;
}

int quad_update(int loc, Address addr1, Address addr2, Address addr3) {
  QuadList q = head;
  while (q != NULL) {
    if (q->location == loc) break;
    q = q->next;
  }
  if (q == NULL) 
    return 0;
  else {
    q->quad.addr1 = addr1;
    q->quad.addr2 = addr2;
    q->quad.addr3 = addr3;
    return 1;
  }
}

char * newLabel() {
  //char label[nlabel_size + 2];
  char * label = (char *) malloc((nlabel_size + 3) * sizeof(char));
  sprintf(label, "L%d", nlabel);
  nlabel++;
  return label;
}

char * newTemp() {
  //char temp[ntemp_size + 2];
  char * temp = (char *) malloc((ntemp_size + 3) * sizeof(char));
  sprintf(temp, "$t%d", ntemp);
  ntemp = (ntemp + 1) % 16;
  return temp;
}

Address addr_createEmpty() {
  Address addr;
  addr.kind = Empty;
  addr.contents.var.name = NULL;
  addr.contents.var.scope = NULL;
  return addr;
}

Address addr_createIntConst(int val) {
  Address addr;
  addr.kind = IntConst;
  addr.contents.val = val;
  return addr;
}

Address addr_createString(char * name, char * scope) {
  Address addr;
  addr.kind = String;
  addr.contents.var.name = (char *) malloc(strlen(name) * sizeof(char));
  strcpy(addr.contents.var.name, name);
  addr.contents.var.scope = (char *) malloc(strlen(scope) * sizeof(char));
  strcpy(addr.contents.var.scope, scope);
  return addr;
}

static void insertNode( TreeNode * t)
{ TreeNode * p1, * p2, * p3;
  Address addr1, addr2, addr3;
  Address aux1, aux2;
  int loc1, loc2, loc3;
  char * label;
  char * temp;
  char * s = "";
  
  switch (t->nodekind)
  { case StmtK:
  switch (t->kind.stmt) {
    case IfK:
      if (TraceCode) emitComment("-> if");
      p1 = t->child[0] ;
      p2 = t->child[1] ;
      p3 = t->child[2] ;
      // condicao if
      if(p1!=NULL)
      insertNode(p1);
      addr1 = aux;
      // se condição falsa?
      loc1 = location;
      quad_insert(opIFF, addr1, empty, empty);
      // i
      if(p2!=NULL)
      insertNode(p2);
      // vai para o final?
      loc2 = location;
      quad_insert(opGOTO, empty, empty, empty);

      label = newLabel();
      quad_insert(opLAB, addr_createString(label, t->scope), empty, empty);
      // se condição falsa vem pra cá
      quad_update(loc1, addr1, addr_createString(label, t->scope), empty);
      // else
      if(p3!=NULL)
      insertNode(p3);
      if (p3 != NULL) {
        // vai para o final?
        loc3 = location;
        quad_insert(opGOTO, empty, empty, empty);
      }
      label = newLabel();
      // final
      quad_insert(opLAB, addr_createString(label, t->scope), empty, empty);
      quad_update(loc2, addr_createString(label, t->scope), empty, empty);
      if (p3 != NULL)
        quad_update(loc3, addr_createString(label, t->scope), empty, empty);
      if (TraceCode)  emitComment("<- if");
      break;

    case WhileK:
      if (TraceCode) emitComment("-> while");
      p1 = t->child[0] ;
      p2 = t->child[1] ;
      // inicio do while
      label = newLabel();
      quad_insert(opLAB, addr_createString(label, t->scope), empty, empty);
      // condicao while
      if(p1!=NULL)
      insertNode(p1);
      addr1 = aux;
      // se condição falsa?
      loc1 = location;
      quad_insert(opIFF, addr1, empty, empty);
      // while
      if(p2!=NULL)
      insertNode(p2);
      loc3 = location;
      quad_insert(opGOTO, addr_createString(label, t->scope), empty, empty);
      // final
      label = newLabel();
      quad_insert(opLAB, addr_createString(label, t->scope), empty, empty);
      // se condição falsa vem pra cá
      quad_update(loc1, addr1, addr_createString(label, t->scope), empty);
      if (TraceCode)  emitComment("<- while");
      break;

	case FuncK:
      if (TraceCode) emitComment("-> Fun");
      // ver se é a main
      if (strcmp(t->attr.name, "main") == 0)
        mainLocation = location;
      if ((strcmp(t->attr.name, "input") != 0) && (strcmp(t->attr.name, "output") != 0)) {
        quad_insert(opFUN, addr_createString(t->attr.name, t->scope), empty, empty);
       p1 = t->child[0];
       if(p1!=NULL)
        insertNode(p1);
        // declaracoes e expressoes
        p2 = t->child[1];
        if(p2!=NULL)
        insertNode(p2);
        quad_insert(opEND, addr_createString(t->attr.name, t->scope), empty, empty);
      }
      if (TraceCode) emitComment("<- Fun");
      break; 

	case CallK:
      if (TraceCode) emitComment("-> Call");
      p1 = t->child[0];
      while (p1 != NULL) {
        insertNode(p1);
        quad_insert(opPARAM, aux, empty, empty);
        p1 = p1->sibling;
      }
      temp = newTemp();
      aux = addr_createString(temp, t->scope);
      quad_insert(opCALL, aux, addr_createString(t->attr.name, t->scope),empty);
      
      if (TraceCode) emitComment("<- Call");
      break;
	
    case VarK:
      if (TraceCode) emitComment("-> Var");
        quad_insert(opALLOC, addr_createString(t->attr.name, t->scope), addr_createIntConst(1), addr_createString(t->scope, t->scope));
      if (TraceCode) emitComment("<- Var");
      break;	
	  
    	
    case VecK:
      if (TraceCode) emitComment("-> Vet");
        quad_insert(opALLOC, addr_createString(t->attr.name, t->scope), addr_createIntConst(t->attr.val), addr_createString(t->scope, t->scope));
      if (TraceCode) emitComment("<- Vet");
      break;
	  
    case AssignK:
      if (TraceCode) emitComment("-> assign");
      p1 = t->child[0];
      p2 = t->child[1];
      // var
      if(p1!=NULL)
      insertNode(p1);
      addr1 = aux;
      aux1 = var;
      aux2 = offset;
      // ex
      if(p2!=NULL)
      insertNode(p2);
      addr2 = aux;
      quad_insert(opASSIGN, addr1, addr2, empty);
      quad_insert(opSTORE, aux1, aux2, addr1);
      if (TraceCode)  emitComment("<- assign");
      break;

    case ReturnK:
      if (TraceCode) emitComment("-> return");
      p1 = t->child[0];
      insertNode(p1);
      if (p1 != NULL)
        addr1 = aux;
      else
        addr1 = empty;
      quad_insert(opRET, addr1, empty, empty);
      if (TraceCode) emitComment("<- return");
      break;

    default:
      break;
      
  }
      break;
    
    case  ExpK:
       switch (t->kind.exp) {
    case ConstK:
      if (TraceCode) emitComment("-> Const");
      addr1 = addr_createIntConst(t->attr.val);
      temp = newTemp();
      aux = addr_createString(temp, s);
      quad_insert(opIMMED, aux, addr1, empty);
      if (TraceCode)  emitComment("<- Const");
      break;
    
    case IdK:
      if (TraceCode) emitComment("-> Id");
      //temp = newTemp();
      aux = addr_createString(t->attr.name, t->scope);
      p1 = t->child[0];
      if (p1 != NULL) {
        temp = newTemp();
        addr1 = addr_createString(temp, t->scope);
        addr2 = aux;
        insertNode(p1);
        quad_insert(opVEC, addr1, addr2, aux);
        var = addr2;
        offset = aux;
        aux = addr1;
      }
      else {
        temp = newTemp();
        addr1 = addr_createString(temp, t->scope);
        quad_insert(opLOAD, addr1, aux, empty);
        var = aux;
        offset = empty;
        aux = addr1;
      }
      if (TraceCode)  emitComment("<- Id");
      break;

    case TypeK:
      break;
   
    case OpK:
      if (TraceCode) emitComment("-> Op");
      p1 = t->child[0];
      p2 = t->child[1];
      if(p1!=NULL)
      insertNode(p1);
      addr1 = aux;
      if(p2!=NULL)
      insertNode(p2);
      addr2 = aux;
      temp = newTemp();
      aux = addr_createString(temp, t->scope);
      switch (t->attr.op) {
        case PLUS:
          quad_insert(opADD, addr1, addr2, aux);
          break;
        case MINUS:
          quad_insert(opSUB ,addr1, addr2, aux);
          break;
        case TIMES:
          quad_insert(opMULT, addr1, addr2, aux);
          break;
        case OVER:
          quad_insert(opDIV, addr1, addr2, aux);
          break;
        case LT:
          quad_insert(opLT, addr1, addr2, aux);
          break;
        case LTE:
          quad_insert(opLET, addr1, addr2, aux);
          break;
        case GT:
          quad_insert(opGT, addr1, addr2, aux);
          break;
        case GTE:
          quad_insert(opGET, addr1, addr2, aux);
          break;
        case EQ:
          quad_insert(opAND, addr1, addr2, aux);
          break;
        case NEQ:
          quad_insert(opOR, addr1, addr2, aux);
          break;
        default:
          emitComment("BUG: Unknown operator");
          break;
      }
      if (TraceCode)  emitComment("<- Op");
      break;

    default:
      break;
       }
      break;
  }
} 




void printCode() {
  QuadList q = head;
  Address a1, a2, a3;
  while (q != NULL) {
    a1 = q->quad.addr1;
    a2 = q->quad.addr2;
    a3 = q->quad.addr3;
    printf("(%s, ", OpKindNames[q->quad.op]);
    switch (a1.kind) {
      case Empty:
        printf("-");
        break;
      case IntConst:
        printf("%d", a1.contents.val);
        break;
      case String:
        //printf("%s.%s", a1.contents.var.scope, a1.contents.var.name);
        printf("%s", a1.contents.var.name);
        break;
      default:
      break;
    }
    printf(", ");
    switch (a2.kind) {
      case Empty:
        printf("-");
        break;
      case IntConst:
        printf("%d", a2.contents.val);
        break;
      case String:
        //printf("%s.%s", a2.contents.var.scope, a2.contents.var.name);
        printf("%s", a2.contents.var.name);
        break;
      default:
        break;
    }
    printf(", ");
    switch (a3.kind) {
      case Empty:
        printf("-");
        break;
      case IntConst:
        printf("%d", a3.contents.val);
        break;
      case String:
        //printf("%s.%s", a3.contents.var.scope, a3.contents.var.name);
        printf("%s", a3.contents.var.name);
        break;
      default:
        break;
    }
    printf(")\n");
    q = q->next;
  }
}

void codeGen(TreeNode * syntaxTree, char * codefile)
{  char * s = malloc(strlen(codefile)+7);
   strcpy(s,"File: ");
   strcat(s,codefile);
   emitComment("\nC- Intermediate Code");
   emitComment(s);
   empty = addr_createEmpty();
   traverse(syntaxTree,insertNode,nullProc);
   printCode();
   emitComment("End of execution");
}

QuadList getIntermediate() {
    return head; 
}