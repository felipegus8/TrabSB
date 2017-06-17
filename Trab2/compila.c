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

void retorno(FILE *myfp, int line, int c,Memory *block){
  char c0;
  if (fscanf(myfp, "et%c", &c0) != 1){
    error("comando invalido", line);
  }else{
    printf("ret\n");
    // mov -X(%ebp),%eax ou x(%ebp)
    block->code[block->nextFree] = 0x8B;
    block->nextFree ++;
    block->code[block->nextFree] = 0x45;
    block->nextFree ++;
    block->code[block->nextFree] = 0x00;
    block->nextFree ++;
    // leave ret
    block->code[block->nextFree] = 0xC9;
    block->nextFree ++;
    block->code[block->nextFree] = 0xC3;
    block->nextFree ++;
  }
}

void atribuicao(FILE *myfp, int line, int c,Memory *block){
  int idx0, idx1, idx2;
  char var0 = c, var1, var2, op;
  if (fscanf(myfp, "%d = %c%d %c %c%d", &idx0, &var1, &idx1,&op, &var2, &idx2) != 6){
    error("comando invalido", line);
  }else{
    printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
    switch (var1) {
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
      //No caso de uma variável local,ela tem que estar na pilha.Só tem que descobrir em que posição.
      //Os três primeiros códigos são iguais.
      //Aqui está movendo um valor da pilha para %r12d.
      block->code[block->nextFree] = 0x44;
      block->nextFree ++;
      block->code[block->nextFree] = 0x8B;
      block->nextFree ++;
      block->code[block->nextFree] = 0x65;
      block->nextFree ++;

      //O quarto muda de acordo com o número da variável,que indica onde ela está na pilha.
      unsigned char local_pilha = 0xFC - (var1 * 4);
      block->code[block->nextFree] = local_pilha;
      block->nextFree ++;


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

    switch (var2) {
    case 'p':
    if (idx1 == 1) { //Parâmetro está no %edi
    //movl %edi,%r13d
    block->code[block->nextFree] = 0x41;
    block->nextFree ++;
    block->code[block->nextFree] = 0x89;
    block->nextFree ++;
    block->code[block->nextFree] = 0xFD;
    block->nextFree ++;
    }
    else { //Parâmetro está no %esi
     //movl %esi,%r13d
     block->code[block->nextFree] = 0x41;
     block->nextFree ++;
     block->code[block->nextFree] = 0x89;
     block->nextFree ++;
     block->code[block->nextFree] = 0xF5;
     block->nextFree ++;
    }
    case 'v':
    //Mesmo caso do primeiro switch.Só muda aqui é que está movendo para %r13d.
    block->code[block->nextFree] = 0x44;
    block->nextFree ++;
    block->code[block->nextFree] = 0x8B;
    block->nextFree ++;
    block->code[block->nextFree] = 0x6D;
    block->nextFree ++;

    //O quarto muda de acordo com o número da variável,que indica onde ela está na pilha.
    unsigned char local_pilha = 0xFC - (var2 * 4);
    block->code[block->nextFree] = local_pilha;
    block->nextFree ++;

    case '$':
    //movl $const,%r13d
    //41 BC (Próximos 4 bytes correpondem ao número em si em hexa.Por isso faço os shifts)
    block->code[block->nextFree] = 0x41;
    block->nextFree ++;
    block->code[block->nextFree] = 0xBD;
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

  switch (op) {
    //Três casos.Add,Sub e Mul,todos de %r13d em %r12d
    case '+':
      // 45 01 ec     add %r13d,%r12d
      block->code[block->nextFree] = 0x45;
      block->nextFree ++;
      block->code[block->nextFree] = 0x01;
      block->nextFree ++;
      block->code[block->nextFree] = 0xEC;
      block->nextFree ++;

    case '-':
      //45 29 EC   sub %r13d,%r12d
      block->code[block->nextFree] = 0x45;
      block->nextFree ++;
      block->code[block->nextFree] = 0x29;
      block->nextFree ++;
      block->code[block->nextFree] = 0xEC;
      block->nextFree ++;

    case '*':
      //45 of af e5 imul %r13d,%r12d
      block->code[block->nextFree] = 0x45;
      block->nextFree ++;
      block->code[block->nextFree] = 0x0F;
      block->nextFree ++;
      block->code[block->nextFree] = 0xAF;
      block->nextFree ++;
      block->code[block->nextFree] = 0xE5;
      block->nextFree ++;
  }
  }

  //Agora tem que colocar o que está em %r12d no lugar certo na pilha.
  //Só o que muda no código de máquina de uma posição da pilha para outra são os dois últimos dígitos que variam de acordo com o lugar onde a variavel está sendo colocada.
  //O início sempre é 44 89 65

  block->code[block->nextFree] = 0x44;
  block->nextFree ++;
  block->code[block->nextFree] = 0x89;
  block->nextFree ++;
  block->code[block->nextFree] = 0x65;
  block->nextFree ++;

  //O maior código de máquina é o da primeira posição.(0xFC).Ele diminui de 4 a cada próxima posição.
  unsigned char local_pilha = 0xFC - (var0 * 4);
  block->code[block->nextFree] = local_pilha;
  block->nextFree ++;

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
  int i;
  //0:	55                   	push   %rbp
  init[0] = 0x55;


  //1:	48 89 e5             	mov    %rsp,%rbp
  init[1] = 0x48;
  init[2] = 0x89;
  init[3] = 0xE5;

  //4:	48 83 ec 10          	sub    $0x10,%rsp
  //Precisa de 16 na pilha para armazenar as 4 variáveis locais.
  init[4] = 0x48;
  init[5] = 0x83;
  init[6] = 0xEC;
  init[7] = 0x10;

  for (i=0;i<8;i++) {
    block->code[block->nextFree] = init[i];
    block->nextFree ++;
  }
}

void end_code (Memory *block) {
  //Instruções para finalizar o programa
  //mov %rbp,%rsp   48 89 EC
  //pop %rbo   5D
  //retq  C3
  block->code[block->nextFree] = 0x48;
  block->nextFree ++;
  block->code[block->nextFree] = 0x89;
  block->nextFree ++;
  block->code[block->nextFree] = 0xEC;
  block->nextFree ++;
  block->code[block->nextFree] = 0x5D;
  block->nextFree ++;
  block->code[block->nextFree] = 0xC3;
  block->nextFree ++;

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
  int line = 8;
  int  c;
  Memory *block = init_memory();
  init_func(block);
  while ((c = fgetc(myfp)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        retorno(myfp,line,c,block);
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
