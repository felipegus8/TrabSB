#include <stdio.h>
#include <stdlib.h>
#include "compila.h"
typedef int (*funcp) ();

int main(void){
  funcp f;
  FILE *myfp;

  if ((myfp = fopen ("programa.txt", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }

  return 0;
}
