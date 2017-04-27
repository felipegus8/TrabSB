
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
    result = utf16_8(f1, f2);
    return 0;
}

void utf8 (unsigned int ch,char **string,FILE *arqSaida) {
    char *src;
    
    if (*string == NULL) {
        return;
    }
    
    src = *string;
    
    //Códigos de U+0000 a U+007F
    if (ch <= 0x7f) {
        src[0] = ch;
        *string	+= 1;
        printf("StringResult1:%s\n",src);
    }
    //Códigos entre U+0080 a U+07FF
    else if (ch <= 0x0007ff) {
        src[0] = 0xc0 | ((ch >>  6) & 0x1f);
        src[1] = 0x80 | (ch & 0x3f);
        *string	+= 2;
        printf("StringResult2:%s\n",src);
    }
    //Códigos entre U+0800 a U+FFFF
    else if (ch <= 0x00ffff) {
        src[0] = 0xe0 | ((ch >> 12) & 0x0f);
        src[1] = 0x80 | ((ch >>  6) & 0x3f);
        src[2] = 0x80 | (ch & 0x3f);
        *string	+= 3;
        printf("StringResult3:%s\n",src);
    }
    //Códigos entre U+10000 a U+10FFFF
    else if (ch <= 0x10ffff) {
        src[0] = 0xf0 | ((ch >> 18) & 0x07);
        src[1] = 0x80 | ((ch >> 12) & 0x3f);
        src[2] = 0x80 | ((ch >>  6) & 0x3f);
        src[3] = 0x80 | (ch & 0x3f);
        *string	+= 4;
        printf("StringResult4:%s\n",src);
    }
    //Escrever no arquivo o resultado obtido
    fwrite(src, 1, sizeof(src), arqSaida);
    
}


int utf16_8(FILE *arq_entrada, FILE *arq_saida) {
    static char buffer[4];
    char *pointerToBuffer;
    int result;
    int n = fseek(arq_entrada, 0, SEEK_END);
    if (n!=0) {
        fprintf(stderr, "Erro no tamanho dp arquivo");
        return -1;
    }
    rewind(arq_entrada);
    pointerToBuffer = buffer;
    int first = fgetc(arq_entrada);
    int second = fgetc(arq_entrada);
    printf("Primeiro valor do BOM:%x\nSegundo valor do BOM:%x\n",first,second);
    //Verificar se tem o BOM certo no arquivo.O BOM pode estar tanto em Big Endian como em Little Endian.Por isso precisa checar os dois.
    if ((first != 0xfe || second != 0xff) && (first != 0xff || second != 0xfe)) {
        fprintf(stderr, "%s", "BOM incorreto!\n");
        return -1;
    }
    
    int startFile = fgetc(arq_entrada);
    
    //Esse while lê o arquivo até o final
    while (startFile != 0xffffffff) {
        int secondPart = fgetc(arq_entrada);
        
        //Total junta o primeiro byte com o segundo para formar o valor certo em hexa.
        int total = ((startFile << 8) | secondPart) & 0xffff;
        
        //Agora tem que ver os dois casos possíveis
        
        // 1 - Valores entre U+10000 a U+10FFFF
        
        //Primeiro code unit
        if ((total >= 0xd800) && (total <= 0xdbff)) {
            result = (total - 0xd800) << 10;
            printf("Result1:%d\n",result);
            fwrite(&result, 1, 1, arq_saida);
        }
        
        //Segundo code unit
        
        else if ((total >= 0xdc00) && (total <= 0xdfff)) {
            result = result | total - 0xdc00;
            utf8(result + 0x10000, &pointerToBuffer,arq_saida);
            printf("Result2:%d\n",result);
        }
        //Valores entre U+0000 a U+FFFF
        else {
            utf8(total, &pointerToBuffer, arq_saida);
            printf("Result3:%d\n",result);
        }
        
        //Ver o próximo byte
        startFile = fgetc(arq_entrada);
    }
    
    *pointerToBuffer = 0;
    return 1;
}
