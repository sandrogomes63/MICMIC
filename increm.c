#include <stdio.h>
#include <math.h>
#include "funcoes.h"

int main(){

int e = 10, f = 10;

incrementa ( e, &f);
printf("Valor de e = %d\n", e);
printf("Valor de f = %d\n", f);
}
