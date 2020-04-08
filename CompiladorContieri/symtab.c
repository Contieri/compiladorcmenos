#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

//DEFININDO O TAMANHO DA TABELA DE SIMBOLOS
#define SIZE 307

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

//FUNCAO HASH
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}


 //LISTA DE LINHAS EM QUE UMA VARIAVEL E REFERENCIADA NO CODIGO FONTE
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;


 
 //A STRUCT BUCKETLIST ARMAZENA UMA LISTA PARA CADA VARIAVEL
 // CONTENDO NOME, POSIÇÃO NA MEMORIA, E A LISTA DE LINHAS
 // EM QUE A VARIAVEL APARECE NO CODIGO FONTE
typedef struct BucketListRec
   { 
    char * name;
    char * scope;
    IdType id;
    DataType data;
     LineList lines;
     int memloc ; //LOCALIZACAO DA VARIAVEL NA MEMORIA
     struct BucketListRec * next; // PONTEIRO PARA A PROXIMA VARIAVEL
   } * BucketList;

/* the hash table */
static BucketList hashTable[SIZE];

void st_initialize(){
    int i;
    for(i = 0; i < SIZE; ++i)
        hashTable[i] = NULL;
}


 
 
//A FUNCAO st_insert INSERE O NUMERO DE LINHAS E A LOCALIZACAO NA MEMORIA DENTRO DA TABELA DE SIMBOLOS
// loc = LOCALIZACAO DA MEMORIA E INSERIDA APENAS UMA VEZ E DEPOIS E IGNORADA
void st_insert( char * name, char * scope, IdType id, DataType data, int lineno, int loc)
{ 
	

int h = hash(name), first = 1;
    BucketList l =  hashTable[h], p = NULL;
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        p = l;
        l = l->next;
        first = 0;
    }
    if(l == NULL){ //CASO A VARIAVEL AINDA NAO ESTEJA NA TABELA
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->scope = scope;
        l->id = id;
        l->data = data;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = loc;
        l->lines->next = NULL;
        l->next = NULL;
        if(first)
            hashTable[h] = l;
        else{
            p->next = l; 
        }
    }
    else{ //CASO JA ESTEJA NA TABELA ACRESCENTA UMA NOVA LINHA A LISTA DE LINHAS
        LineList t = l->lines;
        while(t->next != NULL)
            t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
} //FIM DA FUNCAO 


 //FUNCAO st_lookup RETORNA A LOCALIZACAO NA MEMORIA DE UMA VARIAVEL OU -1 CASO NAO SEJA ENCONTRADA
 //FUNCAO MUITO UTILIZADA NA ANALISE SEMANTICA analyze.c
 //BUSCA NO HASHTABLE RETORNA -1 OU O ENDERECO
int st_lookup ( char * name, char * scope )
{ 	
    int h = hash(name);
    BucketList l =  hashTable[h];
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        l = l->next;
    }
    if(l == NULL)
        return -1;
    else
        return l->memloc;
}



int st_getDataType(char * name, char * scope){
    int h = hash(name);
    BucketList l =  hashTable[h];
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        l = l->next;
    }
    if(l == NULL)
        return -1;
    else
        return l->data;
}

//FUNCAO QUE PESQUISA O TIPO DE DADO, UTILIZADA NA ANALISE SEMANTICA
//RECEBE COMO PARAMETROS O NOME E O ESCOPO DA VARIAVEL
//FAZ A BUSCA E RETORNA -1 CASO NAO ENCONTRADO OU O ENDERECO CASO ENC
int st_getIdType(char * name, char * scope){
    int h = hash(name);
    BucketList l =  hashTable[h];
    while(l != NULL){
        if((strcmp(name, l->name) == 0) && (strcmp(scope, l->scope) == 0))
            break;
        l = l->next;
    }
    if(l == NULL)
        return -1;
    else
        return l->id;
}



 
 //A FUNCAO printSymTab PRINTA UMA VERSAO FORMATADA DA TABELA DE SIMBOLOS

void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"VARIAVEL        ESCOPO               TIPO          TIPO DE DADO        NUMERO LINHAS  \n");
  fprintf(listing,"--------         -----             --------         ----------          -----------\n");
  
for(i = 0; i < SIZE; ++i){
        if(hashTable[i] != NULL){
            BucketList l = hashTable[i];
            while(l != NULL){
                LineList t = l->lines;
                fprintf(listing, "%-16s ", l->name);
                fprintf(listing, "%-16s ", l->scope);
                switch(l->id){
                    case variableT:
                        fprintf(listing, "%-16s ", "variavel");
                        break;
                    case vectorT:
                        fprintf(listing, "%-16s ", "vetor");
                        break;
                    case functionT:
                        fprintf(listing, "%-16s ", "funcao");
                        break;
                }
                switch(l->data){
                    case voidT:
                        fprintf(listing, "%-16s ", "void");
                        break;
                    case integerT:
                        fprintf(listing, "%-16s ", "inteiro");
                        break;
                }
                while (t != NULL){
                    fprintf(listing, "%4d ", t->lineno);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
} /* printSymTab */
