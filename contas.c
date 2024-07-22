#include <stdio.h>
#include <math.h>
#include "funcoes.h"

int main(){

int a, b, maior;
float med;

printf("Digite o primeiro número:\n");
scanf("%d",&a);
printf("Digite o segundo número:\n");
scanf("%d",&b);

maior = omaior(a,b);

med = media(a,b);

if (maior == 1){
printf("Os números são iguais!");
printf("Media = %.2f\n", med);
}
else
printf("O maior número é:%d\n", maior);
printf("Media = %.2f\n", med);
return 0;
}
