#include <stdio.h>
#include <stdlib.h>
#include "solidos.h"

float volume_cone(cone c){
float v;
v = 1.0/3.0 * c.alt * PI * c.raio * c.raio;
return v;
}

float volume_cilin(cilindro c){
float v;
v = c.alt * PI * c.raio * c.raio;
return v;
}

void novo_cone(cone *c){
printf("Insira os dados do cone:\n");
printf("Raio: ");
scanf("%f", &(c->raio));
printf("Altura: ");
scanf("%f", &(c->alt));
}
