#include <stdio.h>
#include <math.h>
#include "funcoes.h"
#include "menus.h"

int main(){

int a, b, maior, n;
float m, med, v[50];
char opcao;

n = menu_qtosnums();

menu_leNvalores(n, v);

opcao = menu_escolha();	

if (opcao == 'E'){
med = mediaVec(n,v);
printf("Media = %.2f", med);
}

if (opcao == 'A'){
m = maiordovetor(n, v);
printf("Maior número = %.2f", m);
}

return 0;
}
