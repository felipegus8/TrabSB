#include <stdio.h>
#include <stdlib.h>
#include "compila.h"
typedef int (*funcp) ();

int main(void){
  FILE *myfp;
  funcp retorno;
  int i;

  if ((myfp = fopen ("programa.txt", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }
  retorno = compila(myfp);
  i = (*retorno)(3);
  printf("\ni = %d\n", i);
  return 0;
}
