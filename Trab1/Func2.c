//
//  main.c
//  SBTeste
//
//  Created by Felipe Viberti on 22/03/17.
//  Copyright © 2017 Felipe Viberti. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int utf16_8(FILE *arq_entrada, FILE *arq_saida);
void utf8 (unsigned int ch,char **string,FILE *arqSaida);

int main (void) {
    FILE *f1,*f2,*f3,*f4;
    int result;
    f1 = fopen("/Users/felipeviberti/Desktop/SBTeste/SBTeste/utf16_peq_big.txt", "rb");
    f2 = fopen("/Users/felipeviberti/Desktop/SBTeste/SBTeste/conversao.txt","wb");
    f3 = fopen("/Users/felipeviberti/Desktop/SBTeste/SBTeste/utf16_big.txt", "rb");
    f4 = fopen("/Users/felipeviberti/Desktop/SBTeste/SBTeste/conversaoGrande.txt","wb");
    result = utf16_8(f3, f4);
    return 0;
}


int utf16_8(FILE *arq_entrada, FILE *arq_saida) {
    char *src;
    int result,result2;
    
    //Primeiro pego os dois valores iniciais do arquivo para ver o BOM.
    int first = fgetc(arq_entrada);
    int second = fgetc(arq_entrada);
    
    //Verificar se tem o BOM certo no arquivo.O BOM pode estar tanto em Big Endian como em Little Endian.Por isso precisa checar os dois.
    
    if ((first != 0xfe || second != 0xff) && (first != 0xff || second != 0xfe)) {
        fprintf(stderr, "%s", "BOM incorreto!\n");
        return -1;
    }
    
    //Começa a ler o arquivo a partir daí
    int startFile = fgetc(arq_entrada);
    
    //Esse while lê o arquivo até o final
    while (startFile != EOF) {
        int secondPart = fgetc(arq_entrada);
        //Total junta o primeiro byte com o segundo para formar o valor certo em hexa.
        int total = ((startFile << 8) | secondPart) & 0xffff;
        
        //Agora tem que ver os dois casos possíveis
        
        // 1 - Valores entre U+10000 a U+10FFFF
        
        //Se estiver nesse range existem dois code units e o primeiro deles vai estar entre 0xd800 e 0xdbff.
        if ((total >= 0xd800) && (total <= 0xdbff)) {
            
            //Pego só a parte que importa desse code unit
            result = (total - 0xd800) << 10;
            
            //Agora tem que ler os dois próximos para formar o segundo code unit
            int thirdPart = fgetc(arq_entrada);
            int fourthPart = fgetc(arq_entrada);
            
            //Da mesma forma que o total,aqui eu formo o segundo code unit.
            int secondCodeUnit = ((thirdPart << 8) | fourthPart) & 0xffff;
            
            //Agora tem que converter esse code units para o seu valor Unicode
            result2 = result | secondCodeUnit - 0xdc00;
            int unicode = result2 + 0x10000;
            
            //Agora converte para UTF8.Como esse código Unicode tem que estar entre U+10000 a U+10FFFF faço a conversão considerando 4 bytes.
            src = (char*) malloc((sizeof(char)*4) + 1);
            src[0] = 0xF0 + ((unicode & 0x1C0000)>>18);
            src[1] = 0x80 + ((unicode & 0x3F000)>>12);
            src[2] = 0x80 + ((unicode & 0xFC0)>>6);
            src[3] = 0x80 + (unicode & 0x3F);
        }
        
        //Agora tem que pegar todos os outros valores,ou seja os que estão entre U+0000 e U+FFFF e tratar cada caso especificamente.
        else {
            //Valores entre U+0000 a U+FFFF.São representados só com um byte
            if (total <= 0x7f) {
                src = (char*) malloc(sizeof(char) + 1);
                src[0] = total;
            }
            
            //Códigos entre U+0080 a U+07FF.São representados com dois bytes.
            else if (total <= 0x0007ff) {
                src = (char*) malloc((sizeof(char)*2) + 1);
                src[0] = 0xc0 | ((total >>  6) & 0x1f);
                src[1] = 0x80 | (total & 0x3f);
            }
            
            //Códigos entre U+0800 a U+FFFF.São representados com três bytes.
            else if (total <= 0x00ffff) {
                src = (char*) malloc((sizeof(char)*3) + 1);
                src[0] = 0xe0 | ((total >> 12) & 0x0f);
                src[1] = 0x80 | ((total >>  6) & 0x3f);
                src[2] = 0x80 | (total & 0x3f);
            }
        }
        //Agora faço um for na string src para imprimir cada elemento dela no arquivo de saída.
        int i = 0;
        for (i=0;src[i] != '\0';i++) {
            fputc(src[i], arq_saida);
        }
        
        //Continuar lendo o arquivo.
        startFile = fgetc(arq_entrada);
    }
    return 1;
}
