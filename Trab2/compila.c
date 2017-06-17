/* Felipe Gustavo Pereira Viberti 1510384 Turma 3 WB */
/* Matheus Rodrigues de Oliveira Leal 1511316 Turma 3 WB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compila.h"

struct memory {
  int nextFree; // próximo índice que está livre
  unsigned char *code; //código de máquina
};
typedef struct memory Memory;

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

void atribuicao(FILE *myfp, int line, int c,Memory *block){
  int idx0, idx1, idx2;
  char var0 = c, var1, var2, op;
  if (fscanf(myfp, "%d = %c%d %c %c%d", &idx0, &var1, &idx1,&op, &var2, &idx2) != 6){
    error("comando invalido", line);
  }else{
    printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
    switch var1: {
      case 'p':
      if (idx1 == 1) { //Parâmetro está no %edi
      //movl %edi,%r12d
      block->code[block->nextFree] = 0x41;
      block->nextFree ++;
      block->code[block->nextFree] = 0x89;
      block->nextFree ++;
      block->code[block->nextFree] = 0xFC;
      block->nextFree ++;
      }
      else { //Parâmetro está no %esi
       //movl %esi,%r12d
       block->code[block->nextFree] = 0x41;
       block->nextFree ++;
       block->code[block->nextFree] = 0x89;
       block->nextFree ++;
       block->code[block->nextFree] = 0xF4;
       block->nextFree ++;
      }
      case 'v':

      case '$':
      //movl $const,%r12d
      //41 BC (Próximos 4 bytes correpondem ao número em si em hexa.Por isso faço os shifts)
      block->code[block->nextFree] = 0x41;
      block->nextFree ++;
      block->code[block->nextFree] = 0xBC;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx1;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx1 >> 8;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx1 >> 16;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx1 >> 24;
      block->nextFree ++;

    }
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

void init_func (Memory *block) {
  unsigned char init[8];
  //0:	55                   	push   %rbp
  init[0] = 0x55;

  //1:	48 89 e5             	mov    %rsp,%rbp
  init[1] = 0x48;
  init[2] = 0x89;
  init[3] = 0xE5;

  //4:	48 83 ec 20          	sub    $0x20,%rsp
  init[4] = 0x48;
  init[5] = 0x83;
  init[6] = 0xEC;
  init[7] = 0x20;

  for (i=0;i<8;i++) {
    block->code[block->nextFree] = init[i];
    nextFree ++;
  }
}

Memory *init_memory() {
  Memory *init;
  init = (Memory*)malloc (sizeof(Memory));
  init->nextFree = 0;
  init->code = (unsigned char*) malloc (sizeof(unsigned char*)*50*4);
  if (init->code == NULL) {
   printf ("ERROR!\n");
   exit (1);
 }
 return init;
}
funcp compila (FILE *myfp){
  char *tmp_texto;
  char code[100];
  int line = 1;
  int  c;
  Memory *block = init_memory();
init_func(block);
  while ((c = fgetc(myfp)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        retorno(myfp,line,code);
        break;
      }
      case 'v':
      case 'p': {  /* atribuicao */
        atribuicao(myfp,line,c,block);
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
