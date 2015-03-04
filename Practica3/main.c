/*
Lucia Peñaranda Pardo, NIA: 100068468
Rafael León Miranda, NIA: 100275593
Practica 3 Ejercicio 4
Arquitectura de Sistemas 2
 */

/*
Constantes
*/

#define NUM_THREADS 4

/*
Bibliotecas
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/*
Estructuras
*/

enum hipoteca{
    no_tiene,
    unica,
    varias
};
enum saldo_medio{
    descubierto,
    menor_mil,
    mayor_mil
};
enum tarjeta{
    no_tiene,
    debito,
    debito_y_credito
};
enum seguros{
    hogar,
    hogar_vivienda,
    hogar_vivienda_vida
};
enum naturaleza{
    persona_fisica,
    autonomo,
    juridica
};


struct cuenta_corriente 
{
    char * titular;
    long saldo;
    enum hipoteca hip;
    enum saldoMedio smed;
    enum tarjeta tarj;
    enum seguros seg;
    enum naturaleza nat;
    long comis_prod;
    long comis_rentab;
};

/*
Variables globales
*/

struct cuenta_corriente cc[2];
long comis_total = 0;

/*
coefs[0] = hip;
coefs[1] = smed;
coefs[2] = tarj;
coefs[3] = seg;
coefs[4] = nat;
*/
long coefs[5];

/*
Mutexes y Variables de condición
*/



/*
Prototipos de funciones
*/



/*
Funcion h_prod()
-Calcula comisiones por el concepto de productos contratados: hipoteca, tarjeta y seguros.
cuentas[i].comis_prod = 10*coefs[HIP] + 10*coefs[TARJ] + 10*coefs[SEG]
*/



/*
Funcion h_rentab()
-Calcula comisiones por conceptos de saldo medio y tipo de cliente.
cuentas[i].comis_rentab = 10*coefs[SMED] + 10*coefs[NAT]
*/



/*
Funcion h_total()
-Calcula la comision total cuando terminan h_prod y h_rentab.
cuentas[i].comis_total = cuentas[i].comis_rentab + cuentas[i].comis_prod
*/



/*
Funcion h_update()
-Actualiza las comisiones periodicamente de forma asincrona con el calculo de comisiones.
*/



/*
Programa principal
*/

int main(){

    pthread_t hebras[NUM_THREADS];
    int ret;

    cc[0].titular = (char *)malloc(sizeof(char));
    strcpy(cc[0].titular,"Lucia Penaranda");
    cc[0].saldo = 1000;

    cc[1].titular = (char *)malloc(sizeof(char));
    strcpy(cc[1].titular,"Rafael Leon");
    cc[1].saldo = 1000;

    #if DEBUG
        printf("cc[0].titular = %s\n",cc[0].titular);
        printf("cc[0].saldo Lucia = %ld\n", cc[0].saldo);
        printf("cc[1].titular = %s\n",cc[1].titular);
        printf("cc[1].saldo Rafa = %ld\n", cc[1].saldo);
    #endif

    /*Inicializacion de los mutexes*/
    pthread_mutex_init(&, NULL);

    #if DEDUG
        printf("Se acaba de inicializar el mutex\n");
    #endif

    /*Creacion de los hilos*/

    ret = pthread_create(&hebras[0], NULL, (void *)h_update, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    }   
    ret = pthread_create(&hebras[1], NULL, (void *)h_prod, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    } 
    ret = pthread_create(&hebras[2], NULL, (void *)h_rentab, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    } 
    ret = pthread_create(&hebras[3], NULL, (void *)h_total, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    } 
    
    
    for(i=0;i<NUM_THREADS;i++){

        ret = pthread_join(hebras[i], NULL);

        #if DEBUG
            printf("Ha terminado el hilo %d\n",i+1);
        #endif

        if(ret){
            printf("ERROR en pthread_join\n");
            pthread_exit(NULL);
        }   
    }

    free(cc[0].titular);
    free(cc[1].titular);

    pthread_exit(NULL);

    return 0;
}