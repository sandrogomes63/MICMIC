#include <stdio.h>
#include <stdlib.h>
#include "solidos.h"
#include <string.h>

int main(int argc, char *argv[]){
float volumecone;
float alt = atof(argv[2]);
float raio = atof(argv[3]);
cone c = {alt,raio};

float volumecilin;
float alt1 = atof(argv[2]);
float raio1 = atof(argv[3]);
cilindro d = {alt1,raio1};

if (strcmp(argv[1],"-cone")== 0){
volumecone = volume_cone(c);
printf("Volume = %.2f\n", volumecone);
}

if (strcmp(argv[1],"-cilindro") == 0){
volumecilin = volume_cilin(d);
printf("Volume = %.2f\n", volumecilin);
}
return 0;
}
