

#include <stdio.h>
int utf16_8(FILE *arq_entrada, FILE *arq_saida) {
    int first = fgetc(arq_entrada);
    int second = fgetc(arq_entrada);
    if (first != 0xfe || second != 0xff) {
        fprintf(stderr, "%s", "BOM incorreto!\n");
        return -1;
    }
    int startFile = fgetc(arq_entrada);
    while (startFile != 0xffffffff) {
        printf("firstbyte = %x\n",(unsigned)startFile);
        startFile = fgetc(arq_entrada);
    }
    return 1;
}

