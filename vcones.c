#include <stdio.h>
#include <stdlib.h>
#include "solidos.h"
#include <string.h>

int main(int argc, char *argv[]){

if ( argc != 2){
	printf("Erro!\nDeve inserir o nome do programa+espaço+nº de cones\n");
	printf("Exemplo: ./vcone 4\n");
exit(1);
}

int n = atoi(argv[1]);
cone *vcones = (cone*)malloc( n * sizeof(cone));

for ( int j = 0; j < n; j++){
novo_cone(&vcones[j]);
}

printf("Cones: %d\n", n);
for(int i = 0; i < n; i++){
printf("%d raio:\t%.2f\taltura:%.2f\n",i+1,vcones[i].raio,vcones[i].alt);
}
free(vcones);
return 0;
}
