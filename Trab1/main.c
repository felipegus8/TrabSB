#include <stdlib.h>
#include "conv_utf.h"

int main () {
	int result;
	FILE *arq_entrada, *arq_saida;
	arq_entrada = fopen ("arq_entrada", "rb");
	arq_saida = fopen ("arq_saida", "wb");
	if(arq_entrada == NULL){
		fprintf(stderr, "Erro ao abrir arquivo para leitura\n");
		if(arq_saida != NULL) {
			fclose(arq_saida);
		}
		exit(1);
	}
	if(arq_saida == NULL){
		fprintf(stderr, "Erro ao abrir arquivo para gravacao\n");
		fclose(arq_entrada);
		exit(1);
	}
	result = utf8_16(arq_entrada, arq_saida);
	if(!result){
		printf("Conversao realizada com Sucesso!\n");
	}
	else {
		printf("A conversao nao pode ser realizada\n");
	}
	fclose(arq_entrada);
	fclose(arq_saida);
	return 0;
}
