/* Felipe Gustavo Pereira Viberti 1510384 Turma 3 WB */
/* Matheus Matricula Turma 3 WB */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "conv_utf.h"

//Função de conversão de UTF-8 para UTF-16

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
        
        unsigned int unit_1, unit_2,tmp_unit_1,tmp_unit_2;
        unsigned int tmp;
        ;
        
        n = fread (v, 1, 1, arq_entrada);
        if(n!=1){
            fprintf(stderr,"Erro de Leitura 1!\n");
            return -1;
        }
        
        if(((v[0]&0x80)>>7)==0){
            // PARA 1 BYTE
            i++;
            
            //converte unit para utf16
            
            unit_1 = v[0];
            unit_2 = 0x0;
            
        }else if(((v[0]&0x20)>>5)==0){
            //PARA 2 BYTES
            
            n = fread ((v+1), 1, 1, arq_entrada);
            // dump(&v[1],1);
            
            masc = 0x1F;
            v[0] = v[0] & masc;
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
            //Até aqui OK
        }else if(((v[0]&0x8)>> 3)==0){
            //PARA 4 BYTES
            n = fread ((v+1), 1, 1, arq_entrada);
            
            if(n!=1){
                fprintf(stderr,"Erro de Leitura 5!\n");
                return -1;
            }
            
            n = fread ((v+2), 1, 1, arq_entrada);
            
            if(n!=1){
                fprintf(stderr,"Erro de Leitura 6!\n");
                return -1;
            }
            
            n = fread ((v+3), 1, 1, arq_entrada);
            
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
            tmp_unit_1 = tmp_unit_1 + 0xD800;
            tmp_unit_2 = tmp & 0x3FF;
            tmp_unit_2 = tmp_unit_2 + 0xDC00;
            
            unit_1 = tmp_unit_1;
            unit_2 = tmp_unit_2;
        }else{
            printf ("Arquivo com erro de codificacao na posicao %d\n", i);
            return -1;
        }
        
        //Se o valor de unit1 for menor que 00FF tem que colocar um 00 no arquivo antes de colocar o unit1.
        if (unit_1 <= 0X00FF) {
            fputc(0x00, arq_saida);
            fputc(unit_1, arq_saida);
        }
        //Se não for tem que separar o código em hex e printar cada um individualmente.
        else {
            int tot1 = unit_1 >> 8;
            fputc(tot1, arq_saida);
            int tot2 = unit_1  & 0x0ff;
            fputc(tot2, arq_saida);
        }
        
        if(unit_2 != 0x0){
            //Mesma lógica de printar cada um individualmente.
            int tot1 = unit_2 >> 8;
            fputc(tot1, arq_saida);
            int tot2 = unit_2  & 0x0ff;
            fputc(tot2, arq_saida);
        }
    }
    return 0;
}








//Função de conversão de UTF-16 para UTF-8



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
            result2 = result | (secondCodeUnit - 0xdc00);
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
    return 0;
}
