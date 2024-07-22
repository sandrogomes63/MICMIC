#include <stdio.h>

void incrementa ( int a, int *p){
a++;
(*p)++;
printf("Valor de a = %d\n", a);
printf("Valor de p = %d\n", *p);
}

int multip ( int a, int* vect){
int total = vect[0];
for( int i = 1; i < a; i++){
total *= vect[i];
}
printf("Resultado = %d\n", total);
return total;
}

