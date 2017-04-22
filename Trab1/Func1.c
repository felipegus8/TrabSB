#include <stdio.h>

void dump (void *p, int n) {
  unsigned char *p1 = p;
  while (n--) {
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}


int utf8_16(FILE *arq_entrada, FILE *arq_saida) {

  int n = fseek (arq_entrada, 0, SEEK_END);

  if (n != 0 ) {
    fprintf(stderr, "Erro ao calcular tamanho do arquivo\n");
    return -1;
  }

  int tamanho = ftell(arq_entrada);
  rewind (arq_entrada);
  unsigned char FE = 0xFE, FF = 0xFF;
  n = fwrite(&FF, 1, 1, arq_saida);
  if(n!=1){
    fprintf(stderr,"Erro de Gravacao!\n");
    return -1;
  }

  n = fwrite(&FE, 1, 1, arq_saida);
  if(n!=1){
    fprintf(stderr,"Erro de Gravacao!\n");
    return -1;
  }


  int i = 0;
  unsigned char v[4];

  while (i < tamanho ){
    n = fread (v, 1, 1, arq_entrada);
    if(n!=1){
      fprintf(stderr,"Erro de Leitura 1!\n");
      return -1;
    }

    if(((v[0]&0x80)>>7)==0){
      i++;
      dump(&v[1],1);

    }else if(((v[0]&0x20)>>5)==0){

      n = fread ((v+1), 1, 1, arq_entrada);
      dump(&v[1],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 2!\n");
        return -1;
      }
      i = i + 2;

    }else if(((v[0]&0x10)>>4)==0){
      n = fread ((v+1), 1, 1, arq_entrada);
      dump(&v[1],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 3!\n");
        return -1;
      }
      n = fread ((v+2), 1, 1, arq_entrada);
      dump(&v[2],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 4!\n");
        return -1;
      }
      i = i + 3;

    }else if(((v[0]&0x8)>> 3)==0){

      n = fread ((v+1), 1, 1, arq_entrada);
      dump(&v[1],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 5!\n");
        return -1;
      }

      n = fread ((v+2), 1, 1, arq_entrada);
      dump(&v[2],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 6!\n");
        return -1;
      }

      n = fread ((v+3), 1, 1, arq_entrada);
      dump(&v[3],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 7!\n");
        return -1;
      }

      i = i + 4;

    }else{

      printf ("Arquivo com erro de codificacao na posicao %d\n", i);
      return -1;

    }
  }
  return 0;
}

