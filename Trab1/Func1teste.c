#include <stdio.h>
#include <stdlib.h>

// void dump (void *p, int n) {
//   unsigned char *p1 = p;
//   while (n--) {
//     printf("%p - %02x\n", p1, *p1);
//     p1++;
//   }
// }


int utf8_16(FILE *arq_entrada, FILE *arq_saida) {

  int n = fseek (arq_entrada, 0, SEEK_END);

  if (n != 0 ) {
    fprintf(stderr, "Erro ao calcular tamanho do arquivo\n");
    return -1;
  }

  int tamanho = ftell(arq_entrada);
  rewind (arq_entrada);
  unsigned char FE = 0xFE, FF = 0xFF;
  n = fwrite(&FE, 1, 1, arq_saida);
  if(n!=1){
    fprintf(stderr,"Erro de Gravacao!\n");
    return -1;
  }

  n = fwrite(&FF, 1, 1, arq_saida);
  if(n!=1){
    fprintf(stderr,"Erro de Gravacao!\n");
    return -1;
  }


  int i = 0;
  unsigned char v[4];

  while (i < tamanho ){

    unsigned int masc, temp, binario;

    unsigned short unit_1, unit_2,tmp_unit_1,tmp_unit_2;
    unsigned int tmp;

    n = fread (v, 1, 1, arq_entrada);
    if(n!=1){
      fprintf(stderr,"Erro de Leitura 1!\n");
      return -1;
    }

    if(((v[0]&0x80)>>7)==0){
      // PARA 1 BYTE
      i++;

      // dump(&v[1],1);
      masc = 0x7F;
			binario = v[0] & masc;

      //converte unit para utf16

      unit_1 = binario;
      unit_2 = 0x0;

    }else if(((v[0]&0x20)>>5)==0){
      //PARA 2 BYTES

      n = fread ((v+1), 1, 1, arq_entrada);
      // dump(&v[1],1);

      masc = 0x1F;
      temp = v[0] & masc;
      temp = temp << 6;
      masc = 0x3F;
      binario = v[1] & masc;
      binario = binario + temp;

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 2!\n");
        return -1;
      }
      i = i + 2;

      //converte unit para utf16

      unit_1 = binario;
      unit_2 = 0x0;

    }else if(((v[0]&0x10)>>4)==0){
      //PARA 3 BYTES
      n = fread ((v+1), 1, 1, arq_entrada);
      // dump(&v[1],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 3!\n");
        return -1;
      }
      n = fread ((v+2), 1, 1, arq_entrada);
      // dump(&v[2],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 4!\n");
        return -1;
      }
      i = i + 3;

      masc = 0xF;
			temp = v[0] & masc;
			temp = temp << 12;
			masc = 0x3F;
			binario = v[1] & masc;
			binario = binario << 6;
			temp = temp + binario;
			binario = v[2] & masc;
			binario = temp + binario;

      //converte unit para utf16

      unit_1 = binario;
      unit_2 = 0x0;

    }else if(((v[0]&0x8)>> 3)==0){
      //PARA 4 BYTES
      n = fread ((v+1), 1, 1, arq_entrada);
      // dump(&v[1],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 5!\n");
        return -1;
      }

      n = fread ((v+2), 1, 1, arq_entrada);
      // dump(&v[2],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 6!\n");
        return -1;
      }

      n = fread ((v+3), 1, 1, arq_entrada);
      // dump(&v[3],1);

      if(n!=1){
        fprintf(stderr,"Erro de Leitura 7!\n");
        return -1;
      }

      i = i + 4;

      //converte de utf8 para unicode

      masc = 0x7;
      temp = v[0] & masc;
      temp = temp << 18;
      masc = 0x3F;
      binario = v[1] & masc;
      binario = binario << 12;
      temp = temp + binario;
      binario = v[2] & masc;
      binario = binario << 6;
      temp = temp + binario;
      binario = v[3] & masc;
      binario = temp + binario;

      //converte unit para utf16


      tmp = binario - 0x10000;
      tmp_unit_1 = (tmp & 0xFFC00) >> 10;
      tmp_unit_1 = unit_1 + 0xD800;
      tmp_unit_2 = tmp & 0x3FF;
      tmp_unit_2 = unit_2 + 0xDC00;

      tmp = (unit_2 & 0xFF) << 8;
      unit_2= (unit_2 & 0xFF00) >> 8;
      unit_2= unit_2 + tmp;

      unit_1 = tmp_unit_1;
      unit_2 = tmp_unit_2;

    }else{

      printf ("Arquivo com erro de codificacao na posicao %d\n", i);
      return -1;

    }

    tmp = (unit_1 & 0xFF) << 8;
		unit_1 = (unit_1 & 0xFF00) >> 8;
		unit_1 = unit_1 + tmp;

    unit_1= unit_1>>8;

    n = fwrite (&unit_1, 1, 1, arq_saida);
    if(n!=1){
      fprintf(stderr,"Erro de Gravacao!\n");
      return -1;
    }
    if(unit_2 != 0x0){

  		n = fwrite (&unit_2,1, 1, arq_saida);

      if(n!=1){
        fprintf(stderr,"Erro de Gravacao!\n");
        return -1;
      }
  	}
  }
  return 0;
}
