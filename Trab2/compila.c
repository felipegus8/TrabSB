#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compila.h"

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

void retorno(FILE *myfp, int line, char *text){
  char c0;
  if (fscanf(myfp, "et%c", &c0) != 1){
    error("comando invalido", line);
  }else{
    printf("ret\n");
    text = strcat(text,"leave");
    text = strcat(text,"ret");
  }
}

void atribuicao(FILE *myfp, int line, char *text, int c){
  int idx0, idx1, idx2;
  char var0 = c, var1, var2, op;
  if (fscanf(myfp, "%d = %c%d %c %c%d", &idx0, &var1, &idx1,&op, &var2, &idx2) != 6){
    error("comando invalido", line);
  }else{
    printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
  }
}

void desvia(FILE *myfp, int line, char *text){
  char var0;
  int idx0, num;
  if (fscanf(myfp, "f %c%d %d", &var0, &idx0, &num) != 3){
    error("comando invalido", line);
  }else{
    printf("if %c%d %d\n", var0, idx0, num);
  }
}
typedef int (*funcp) ();

funcp compila (FILE *myfp){
  char *tmp_texto;
  char code[100];
  int line = 1;
  int  c;

  tmp_texto = (char*) malloc (sizeof(char*)*50*4);
  if (tmp_texto == NULL) {
   printf ("ERROR!\n");
   exit (1);
 }
  while ((c = fgetc(myfp)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        retorno(myfp,line,code);
        break;
      }
      case 'v':
      case 'p': {  /* atribuicao */
        atribuicao(myfp,line,code,c);
        break;
      }
      case 'i': { /* desvio */
        desvia(myfp,line,code);
        break;
      }
      default: error("comando desconhecido", line);
    }
    line ++;
    fscanf(myfp, " ");
  }

  tmp_texto=strcpy(tmp_texto,code);

  return (funcp)tmp_texto;
}
