#ifndef _SYMTAB_H_
#define _SYMTAB_H_

typedef enum {variableT, vectorT, functionT} IdType;
typedef enum {voidT, integerT} DataType;

/* zera a tabela de simbolos
 */
void st_initialize();

/* numero da linha e locais de memoria na tabela de simbolos
 * loc = posicao de memoria inserida apenas uma vez , em  outros casos e ignorada
 */
void st_insert(char * name, char * scope, IdType id, DataType data, int lineno, int loc);

/* retorna o local da memoria de uma variavel ou -1 se nao for encontrado
 */
int st_lookup(char * name, char * scope);

/*retorna  o tipo de dado de uma variavel ou -1
 */
int st_getDataType(char * name, char * scope);

/*Retorna o id da variavel ou -1 
 */
int st_getIdType(char * name, char * scope);

/*printar tabela de simbolos
 */
void printSymTab(FILE * listing);

#endif

