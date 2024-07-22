#include <stdio.h>
#include <math.h>

int omaior(  int a, int b){
	if(a == b){
	return 1;
	}
	if(a < b){
		return b;
	}
	else {return a;
	}
}
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
printf("O maior número é:%d", maior);
return 0;
}
