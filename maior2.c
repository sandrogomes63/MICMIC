#include <stdio.h>
#include <math.h>
#include "funcoes.h"

int main(){

int a, b, maior;

printf("Digite o primeiro número:\n");
scanf("%d",&a);
printf("Digite o segundo número:\n");
scanf("%d",&b);

maior = omaior(a,b);

if (maior == 1){
printf("Os números são iguais!");
}
else
printf("O maior número é:%d\n", maior);
return 0;
}
