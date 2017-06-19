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

typedef int (*funcp) ();

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
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
void retorno(FILE *myfp, int line, int c, Memory *block, int *code_line){
  char c0;
  if (fscanf(myfp, "et%c", &c0) != 1){
    error("comando invalido", line);
  }else{
    code_line[line]=block->nextFree;
  //  printf("ret\n");
    // mov -X(%ebp),%eax ou x(%ebp)
    //Sempre tem que retornar o que está em v1
    unsigned char local_pilha = 0xFC;
    block->code[block->nextFree] = 0x8B;
    block->nextFree ++;
    block->code[block->nextFree] = 0x45;
    block->nextFree ++;
    block->code[block->nextFree] = local_pilha;
    block->nextFree ++;

    //Chama a função para finalizar o código com as instruções padrão
    end_code(block);
  }
}

void atribuicao(FILE *myfp, int line, int c,Memory *block, int *code_line){
  int idx0, idx1, idx2;
  char var0 = c, var1, var2, op;
  if (fscanf(myfp, "%d = %c%d %c %c%d", &idx0, &var1, &idx1,&op, &var2, &idx2) != 6){
    error("comando invalido", line);
  }else{
    code_line[line]=block->nextFree;
  //  printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
    switch (var1) {
      case 'p':
      block->code[block->nextFree] = 0x41;
      block->nextFree ++;
      block->code[block->nextFree] = 0x89;
      block->nextFree ++;
      if (idx1 == 1) //Parâmetro está no %edi
      //movl %edi,%r12d
      block->code[block->nextFree] = 0xFC;
      else  //Parâmetro está no %esi
      //movl %esi,%r12d
      block->code[block->nextFree] = 0xF4;

      block->nextFree ++;
      break;
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
      unsigned char local_pilha = 0xFC - ((idx1 - 1) * 4);
      block->code[block->nextFree] = local_pilha;
      block->nextFree ++;

      break;


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
      break;

    }

    switch (var2) {
      case 'p':
      block->code[block->nextFree] = 0x41;
      block->nextFree ++;
      block->code[block->nextFree] = 0x89;
      block->nextFree ++;
      if (idx2 == 1)  //Parâmetro está no %edi
      //movl %edi,%r13d
      block->code[block->nextFree] = 0xFD;
      else  //Parâmetro está no %esi
      //movl %esi,%r13d
      block->code[block->nextFree] = 0xF5;


      block->nextFree ++;
      break;
      case 'v':
      //Mesmo caso do primeiro switch.Só muda aqui é que está movendo para %r13d.
      block->code[block->nextFree] = 0x44;
      block->nextFree ++;
      block->code[block->nextFree] = 0x8B;
      block->nextFree ++;
      block->code[block->nextFree] = 0x6D;
      block->nextFree ++;

      //O quarto muda de acordo com o número da variável,que indica onde ela está na pilha.
      unsigned char local_pilha = 0xFC - ((idx2 - 1) * 4);
      block->code[block->nextFree] = local_pilha;
      block->nextFree ++;
      break;

      case '$':
      //movl $const,%r13d
      //41 BC (Próximos 4 bytes correpondem ao número em si em hexa.Por isso faço os shifts)
      block->code[block->nextFree] = 0x41;
      block->nextFree ++;
      block->code[block->nextFree] = 0xBD;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx2;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx2 >> 8;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx2 >> 16;
      block->nextFree ++;
      block->code[block->nextFree] = (char) idx2 >> 24;
      block->nextFree ++;
      break;
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
      break;

      case '-':
      //45 29 EC   sub %r13d,%r12d
      block->code[block->nextFree] = 0x45;
      block->nextFree ++;
      block->code[block->nextFree] = 0x29;
      block->nextFree ++;
      block->code[block->nextFree] = 0xEC;
      block->nextFree ++;
      break;

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
      break;
    }
  }

  switch (var0) {
    case 'v':
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
    unsigned char local_pilha = 0xFC - ((idx0 - 1) * 4);
    block->code[block->nextFree] = local_pilha;
    block->nextFree ++;
    break;

    case 'p':
    block->code[block->nextFree] = 0x44;
    block->nextFree ++;
    block->code[block->nextFree] = 0x89;
    block->nextFree ++;
    if (idx0 == 1) //mover para %edi
    block->code[block->nextFree] = 0xE7;
    else   //mover para %esi
    block->code[block->nextFree] = 0xE6;

    block->nextFree ++;
    break;
  }
}

void desvia(FILE *myfp, int *line, int c, Memory *block,int *code_line){
  char var0;
  int idx0, num;
  int temp = *line;
  if (fscanf(myfp, "f %c%d %d", &var0, &idx0, &num) != 3){
    error("comando invalido", temp);
  }else{
    code_line[temp]=block->nextFree;
      block->code[block->nextFree] = 0x83;
      block->nextFree ++;
      switch (var0) {
        case 'p':
        if(idx0==1) {
          block->code[block->nextFree] = 0xFF;
        }
        else {
        block->code[block->nextFree] = 0xFE;
        }
        block->nextFree ++;
        block->code[block->nextFree] = 0x00;
        block->nextFree ++;
        break;

        case 'v':
        block->code[block->nextFree] = 0x7D;
        block->nextFree ++;
        block->code[block->nextFree] = 0xFC - ((idx0 - 1) * 4);
        block->nextFree ++;
        block->code[block->nextFree] = 0x00;
        block->nextFree ++;
        break;
      }
    //  printf("if %c%d %d\n", var0, idx0, num);

      if(temp < num){
        int nxtfree = block->nextFree;
        block->nextFree +=2;
        temp++;
        fscanf(myfp, " ");
        while (temp < num) {
          if((c = fgetc(myfp)) != EOF){
            switch (c) {
              case 'r': { /* retorno */
                retorno(myfp,temp,c,block,code_line);
                break;
              }
              case 'v':
              case 'p': {  /* atribuicao */
                atribuicao(myfp,temp,c,block,code_line);
                break;
              }
              case 'i': { /* desvio */
                desvia(myfp,&temp,c,block,code_line);
                break;
              }
              default: error("comando desconhecido", temp);
            }
            temp ++;
            fscanf(myfp, " ");
          }else{
            error("Numero de linha para JUMP inexistente", temp);
            exit (1);
          }
        }
        // jne linhaX
        //Sempre usamos Jump Not Equal depois de ter sido feita uma comparação com zero.
        block->code[nxtfree] = 0x75;
        block->code[nxtfree + 1] = &block->code[code_line[num - 1]] - &block->code[nxtfree + 2];

      }
      else {
      // jne linhaX
      block->code[block->nextFree] = 0x75;
      block->nextFree ++;
      block->code[block->nextFree] = &block->code[code_line[num - 1]] - &block->code[block->nextFree + 1];
      block->nextFree ++;
    }
      *line = temp;
  }
}


void init_func (Memory *block) {
  //Função para inicializar o código com o prólogo padrão de Assembly.
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


Memory *init_memory() {
  //Função para alocar o espaço de memória necessário onde será inserido o código de máquina.
  Memory *init;
  init = (Memory*)malloc (sizeof(Memory));
  init->nextFree = 0;
  init->code = (unsigned char*) malloc (sizeof(unsigned char*)*1600);
  if (init->code == NULL) {
    printf ("ERROR!\n");
    exit (1);
  }
  return init;
}


funcp compila (FILE *myfp){
  int code_line[50];
  int line = 0;
  int  c;
  Memory *block = init_memory();
  init_func(block);
  while ((c = fgetc(myfp)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        retorno(myfp,line,c,block,code_line);
        break;
      }
      case 'v':
      case 'p': {  /* atribuicao */
        atribuicao(myfp,line,c,block,code_line);
        break;
      }
      case 'i': { /* desvio */
        desvia(myfp,&line,c,block,code_line);
        break;
      }
      default: error("Comando Desconhecido", line);
    }
    line ++;
    fscanf(myfp, " ");
  }
  //Descomentar essa parte se quiser ver o código de máquina
  // int i;
  // for(i=0;i<block->nextFree;i++)
  // printf("Char:%x\n",block->code[i]);

  return (funcp)(&block->code[0]);
}
