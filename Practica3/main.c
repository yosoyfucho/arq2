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
#define RAND_MAX 1
#define NUM_COEFS 5
#define NUM_CLIENTS 2

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
    sin_tarjeta,
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
    enum saldo_medio smed;
    enum tarjeta tarj;
    enum seguros seg;
    enum naturaleza nat;
    float comis_prod;
    float comis_rentab;
    float comis_total;
};

/*
Variables compartidas
*/

int update_gv = 0;
struct cuenta_corriente cc[NUM_CLIENTS];
float coefs[5];
int update_gv; //0 no hay update 1 hay update
				/*coefs[0] = hip;
                coefs[1] = smed;
                coefs[2] = tarj;
                coefs[3] = seg;
                coefs[4] = nat;*/

/*
Mutexes y Variables de condición
*/

pthread_mutex_t coefs_m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t coefs_update_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t fin_calculo_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t cuentas_cv = PTHREAD_COND_INITIALIZER;

/*
Prototipos de funciones
*/

double double_rand();
void h_update();
void h_prod();
void h_rentab();
void h_total();

/*
Funcion h_prod()
-Calcula comisiones por el concepto de productos contratados: hipoteca, tarjeta y seguros.
cuentas[i].comis_prod = 10*coefs[HIP] + 10*coefs[TARJ] + 10*coefs[SEG]
*/
void h_prod(){

	int i;

	while(1){

        #if DEBUG
            printf("Funcion h_prod dentro del while\n");
        #endif
            
       while(update_gv!=2){

            #if DEBUG
                printf("Funcion h_prod voy a dormir\n");
                printf("\n");
            #endif
            pthread_cond_wait(&coefs_update_cv, &coefs_m);
        }

        #if DEBUG
            printf("Funcion h_prod despierto\n");
        #endif
		pthread_mutex_lock(&coefs_m);

		for(i=0; i<NUM_CLIENTS; i++){
			cc[i].comis_prod = 10*coefs[0] + 10*coefs[2] + 10*coefs[3];
            #if DEBUG
                printf("h_prod -> El valor de la comision prod de %s es: %f\n", cc[i].titular, cc[i].comis_prod);
            #endif
		}

        update_gv ++;

        if(update_gv<3){
            pthread_cond_signal(&coefs_update_cv);
        }else{
            pthread_cond_signal(&cuentas_cv);
        }

		pthread_mutex_unlock(&coefs_m);
	}
}


/*
Funcion h_rentab()
-Calcula comisiones por conceptos de saldo medio y tipo de cliente.
cuentas[i].comis_rentab = 10*coefs[SMED] + 10*coefs[NAT]
*/
void h_rentab(){

	int i;

	while(1){

        #if DEBUG
            printf("Funcion h_rentab dentro del while\n");
        #endif

        pthread_mutex_lock(&coefs_m);

        while(update_gv!=1){
            #if DEBUG
                printf("Funcion h_rentab voy a dormir\n");
                printf("\n");
            #endif
            pthread_cond_wait(&coefs_update_cv, &coefs_m);
        }

        #if DEBUG
            printf("Funcion h_rentab despierto\n");
        #endif

		for(i=0; i<NUM_CLIENTS; i++){
			cc[i].comis_rentab = 10*coefs[1] + 10*coefs[4];
            #if DEBUG
                printf("h_rentab -> El valor de la comision rentab de %s es: %f\n", cc[i].titular, cc[i].comis_rentab);
            #endif
		}

        update_gv ++;

        if(update_gv<3){
            pthread_cond_signal(&coefs_update_cv);
        }else{
            pthread_cond_signal(&cuentas_cv);
        }
        
		pthread_mutex_unlock(&coefs_m);
	}
}


/*
Funcion h_total()
-Calcula la comision total cuando terminan h_prod y h_rentab.
cuentas[i].comis_total = cuentas[i].comis_rentab + cuentas[i].comis_prod
*/
void h_total(){

	int i;

	while(1){

        #if DEBUG
            printf("Funcion h_total en while\n");
        #endif

		pthread_mutex_lock(&coefs_m);

        while(update_gv!=3){
            #if DEBUG
                printf("h_total -> Voy a dormir\n");
                printf("\n");
            #endif
		    pthread_cond_wait(&cuentas_cv, &coefs_m);
        }

		for(i=0; i<NUM_CLIENTS; i++){
			printf("El valor de la comision rentab es: %f\n",cc[i].comis_rentab);
			printf("El valor de la comision prod es: %f\n",cc[i].comis_prod);

			cc[i].comis_total = cc[i].comis_rentab + cc[i].comis_prod;

			printf("El valor de la comision total es: %f\n",cc[i].comis_total);
            printf("\n");
		}

		update_gv = 0;
		
		pthread_cond_signal(&fin_calculo_cv);
		pthread_mutex_unlock(&coefs_m);
	}
}



/*
Funcion h_update()
-Actualiza las comisiones periodicamente de forma asincrona con el calculo de comisiones.
*/

void h_update(){

    int aleatorio;
    int i;
    double valor;

    #if DEBUG
        printf("Funcion h_update 1a vez\n");
    #endif

    pthread_mutex_lock(&coefs_m);
    valor = double_rand();

    #if DEBUG      
        printf("El valor en el vector de coefs es: %f \n", valor);
    #endif

    for(i=0; i<NUM_COEFS; i++){
        coefs[i]=valor;
    }

    update_gv++;

    #if DEBUG
        printf("El valor de update_gv es : %d \n", update_gv);
    #endif

    pthread_cond_signal(&coefs_update_cv);

    #if DEBUG
        printf("h_update-> he mandado el signal\n");
        printf("h_update-> desbloqueo coefs_m\n");
    #endif

    pthread_mutex_unlock(&coefs_m);

    while(1){

        aleatorio = rand()%3;
        sleep(aleatorio);

        pthread_mutex_lock(&coefs_m);

    	if(update_gv!=0){
            printf("h_update se va a dormir\n");
            printf("\n");
    		pthread_cond_wait(&fin_calculo_cv,&coefs_m);
    	}
    	
        #if DEBUG
            printf("Funcion h_update dentro del while\n");
        #endif

        valor = double_rand();

        #if DEBUG
            printf("El valor en el vector de coefs es: %f \n", valor);
        #endif

        for(i=0; i<NUM_COEFS; i++){
            coefs[i]=valor;
        }
        
        update_gv++;

        pthread_cond_signal(&coefs_update_cv);
        pthread_mutex_unlock(&coefs_m);
    }
}

/*
Funcion double_rand()
-Devuelve un numero aleatorio entre [0,RAND_MAX]
*/

double double_rand() {
    return (rand() / (double)RAND_MAX);
}

/*
Programa principal
*/

int main(){

	srand(time(NULL));
    pthread_t hebras_t[NUM_THREADS];
    int ret,i;

    cc[0].titular = (char *)malloc(sizeof(char));
    strcpy(cc[0].titular,"Lucia Penaranda");
    cc[0].saldo = 1000;
    cc[0].hip = 0;
    cc[0].smed = 1;
    cc[0].tarj = 2;
    cc[0].seg = 0;
    cc[0].nat = 0;
    cc[0].comis_rentab = 0;
    cc[0].comis_prod = 0;
    cc[0].comis_total = 0;


    cc[1].titular = (char *)malloc(sizeof(char));
    strcpy(cc[1].titular,"Rafael Leon");
    cc[1].saldo = 1000;
    cc[1].hip = 2;
    cc[1].smed = 0;
    cc[1].tarj = 1;
    cc[1].seg = 1;
    cc[1].nat = 1;
    cc[1].comis_rentab = 0;
    cc[1].comis_prod = 0;
    cc[1].comis_total = 0;

    #if DEBUG
        printf("cc[0].titular = %s\n",cc[0].titular);
        printf("cc[0].saldo Lucia = %ld\n", cc[0].saldo);
        printf("cc[0].hip Lucia = %d\n", cc[0].hip);
        printf("cc[0].smed Lucia = %d\n", cc[0].smed);
        printf("cc[0].tarj Lucia = %d\n", cc[0].tarj);
        printf("cc[0].seg Lucia = %d\n", cc[0].seg);
        printf("cc[0].nat Lucia = %d\n", cc[0].nat);
        printf("cc[0].comis_rentab Lucia = %f\n", cc[0].comis_rentab);
        printf("cc[0].comis_prod Lucia = %f\n", cc[0].comis_prod);
        printf("cc[0].comis_total Lucia = %f\n", cc[0].comis_total);
        printf("\n");
        printf("cc[1].titular = %s\n",cc[1].titular);
        printf("cc[1].saldo Rafa = %ld\n", cc[1].saldo);
        printf("cc[1].hip Rafa = %d\n", cc[1].hip);
        printf("cc[1].smed Rafa = %d\n", cc[1].smed);
        printf("cc[1].tarj Rafa = %d\n", cc[1].tarj);
        printf("cc[1].seg Rafa = %d\n", cc[1].seg);
        printf("cc[1].nat Rafa = %d\n", cc[1].nat);
        printf("cc[1].comis_rentab Rafa = %f\n", cc[1].comis_rentab);
        printf("cc[1].comis_prod Rafa = %f\n", cc[1].comis_prod);
        printf("cc[1].comis_total Rafa = %f\n", cc[1].comis_total);
        printf("\n");
    #endif

    /*Inicializacion de los mutexes*/
    pthread_mutex_init(&coefs_m, NULL);
    pthread_cond_init(&coefs_update_cv, NULL);
    pthread_cond_init(&cuentas_cv, NULL);
    pthread_cond_init(&fin_calculo_cv, NULL);


    #if DEDUG
        printf("Se acaba de inicializar el mutex\n");
    #endif

    /*Creacion de los hilos*/

    ret = pthread_create(&hebras_t[0], NULL, (void *)h_update, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    }
    ret = pthread_create(&hebras_t[1], NULL, (void *)h_prod, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    }
    ret = pthread_create(&hebras_t[2], NULL, (void *)h_rentab, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    }
    ret = pthread_create(&hebras_t[3], NULL, (void *)h_total, NULL);
    if(ret){
        printf("ERROR en pthread_create\n");
        pthread_exit(NULL);
    }

    for(i=0;i<NUM_THREADS;i++){

        ret = pthread_join(hebras_t[i], NULL);

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