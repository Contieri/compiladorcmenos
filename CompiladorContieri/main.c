#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE


#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#include "lcode.h"
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;

int Error = FALSE;

int main(int argc, char *argv[]){
    TreeNode * syntaxTree;
    char pgm[120]; /* source code file name */
    if(argc!=2){
        fprintf(stderr,"usage: %s <filename>\n",argv[0]);
        exit(1);
    }
    strcpy(pgm,argv[1]);
    if(strchr(pgm, '.')==NULL)
        strcat(pgm,".cminus");
    source = fopen(pgm,"r");
    if(source==NULL){
        fprintf(stderr,"File %s not found\n",pgm);
        exit(1);
    }
    listing = stdout; /* send listing to screen */
    fprintf(listing,"\nC- COMPILATION: %s\n",pgm);
    #if NO_PARSE
    while(getToken()!=ENDFILE);
    #else
    syntaxTree = parse();
    if(TraceParse){
        fprintf(listing,"\nSyntax tree:\n");
        printTree(syntaxTree);
    }
    #if !NO_ANALYZE
    if(!Error){
        if(TraceAnalyze)
            fprintf(listing,"\nBuilding Symbol Table...\n");
        buildSymtab(syntaxTree);
        if(TraceAnalyze)
            fprintf(listing,"\nChecking Types...\n");
        typeCheck(syntaxTree);
        if(TraceAnalyze)
            fprintf(listing,"\nType Checking Finished\n");
    }	
	char * codefile;
    int fnlen = strcspn(pgm,".");
    codefile = (char *) calloc(fnlen+4, sizeof(char));
    strncpy(codefile,pgm,fnlen);
    strcat(codefile,"_binary.txt");
    code = fopen(codefile,"w");
	fprintf(listing,"\nCreating Intermediate Code...\n");
    codeGen(syntaxTree,codefile);
    fprintf(listing,"\nIndermediate Code Created\n");

    #endif
    #endif
    fclose(source);
    return 0;
}

