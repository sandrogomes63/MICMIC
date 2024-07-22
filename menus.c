int menu_qtosnums (){
int n;
printf("Quantos números serão inseridos?\n");
scanf("%d",&n);
return n;
}

void menu_leNvalores ( int n, float* vec){
for (int i = 0; i < n; i++){
printf("Insira o %d número:\n", i+1);
scanf("%f", &vec[i]);
}
}

char menu_escolha(){
char  opc;
printf("Para calcular a media digite E\nPara mostrar o maior número digite A\n");
printf("Opção que deseja: ");
scanf(" %c", &opc);
while( opc != 'A' && opc!= 'E'){
printf("Opção Inválida!");
printf("Para calcular a media digite E\n Para mostrar o maior número digite A\n");
scanf("%c", &opc);}
return opc;
}
