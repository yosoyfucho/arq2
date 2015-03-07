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
#define DESCUENTO_MAX 10
#define COMISION_DEFECTO 50

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

int rent_ok;
int prod_ok;
int update_ok;
struct cuenta_corriente cc[NUM_CLIENTS];
float coefs[5];	/*coefs[0] = hip;
                coefs[1] = smed;
                coefs[2] = tarj;
                coefs[3] = seg;
                coefs[4] = nat;*/

/*
Mutexes y Variables de condición
*/

pthread_mutex_t coefs_m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fin_m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cuentas_m = PTHREAD_MUTEX_INITIALIZER;
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
    float com_hip, desc_hip, com_tar, desc_tar, com_seg, desc_seg;

	while(1){

        sleep(3);

        #if DEBUG
            printf("h_prod -> Dentro del while\n");
        #endif
        
        pthread_mutex_lock(&coefs_m);    

        while(update_ok!=1){
            #if DEBUG
                printf("h_prod -> update_ok = %d\n",update_ok);
                printf("h_prod -> Voy a dormir\n");
                printf("\n");
            #endif
        
            pthread_cond_wait(&coefs_update_cv, &coefs_m);

            #if DEBUG
                printf("h_update -> update_ok = %d\n",update_ok);
            #endif
        }

        #if DEBUG
            printf("Funcion h_prod despierto\n");
        #endif

		for(i=0; i<NUM_CLIENTS; i++){
            switch (cc[i].hip){
                case 0:
                    desc_hip = 0;
                   // com_hip = COMISION_DEFECTO - desc_hip;
                    break;
                case 1:
                    desc_hip = coefs[0]*0.5*DESCUENTO_MAX;
                   // com_hip = COMISION_DEFECTO - desc_hip;
                    break;
                case 2:
                    desc_hip = coefs[0]*DESCUENTO_MAX;
                    //com_hip = COMISION_DEFECTO - desc_hip;
                    break;
            }
            switch (cc[i].tarj){
                case 0:
                    desc_tar = 0;
                    //com_tar = COMISION_DEFECTO - desc_tar;
                    break;
                case 1:
                    desc_tar = coefs[2]*0.5*DESCUENTO_MAX;
                    //com_tar = COMISION_DEFECTO - desc_tar;
                    break;
                case 2:
                    desc_tar = coefs[2]*DESCUENTO_MAX;
                   // com_tar = COMISION_DEFECTO - desc_tar;
                    break;
            }
            switch (cc[i].seg){
                case 0:
                    desc_seg = 0;
                    //com_seg = COMISION_DEFECTO - desc_seg;
                    break;
                case 1:
                    desc_seg = coefs[3]*0.5*DESCUENTO_MAX;
                    //com_seg = COMISION_DEFECTO - desc_seg;
                    break;
                case 2:
                    desc_seg = coefs[3]*DESCUENTO_MAX;
                    //com_seg = COMISION_DEFECTO - desc_seg;
                    break;
            }

			cc[i].comis_prod = desc_hip + desc_tar + desc_seg;
            printf("h_prod -> El descuento en la comision prod de %s es: %.2f euros\n", cc[i].titular, cc[i].comis_prod);
		}

        prod_ok = 1;

        if((rent_ok==1)&&(prod_ok==1)){
            update_ok = 0;
            pthread_cond_signal(&cuentas_cv);
            #if DEBUG
                printf("h_rentab ->Envia signal con cuentas_cv\n");
            #endif
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

	int i, com_smed, desc_smed, com_nat, desc_nat;

	while(1){

        sleep(3);

        #if DEBUG
            printf("h_rentab -> Dentro del while\n");
        #endif

        pthread_mutex_lock(&coefs_m);

        while(update_ok!=1){
            #if DEBUG
                printf("h_rentab -> update_ok = %d\n",update_ok);
                printf("h_rentab -> Voy a dormir\n");
                printf("\n");
            #endif
        
            pthread_cond_wait(&coefs_update_cv, &coefs_m);

            #if DEBUG
                printf("h_update -> update_ok = %d\n",update_ok);
            #endif
        }

        #if DEBUG
            printf("h_rentab -> Despierto\n");
        #endif

		for(i=0; i<NUM_CLIENTS; i++){
            switch (cc[i].smed){
                case 0:
                    desc_smed = 0;
                    //com_smed = COMISION_DEFECTO - desc_smed;
                    break;
                case 1:
                    desc_smed = coefs[2]*0.5*DESCUENTO_MAX;
                    //com_smed = COMISION_DEFECTO - desc_smed;
                    break;
                case 2:
                    desc_smed = coefs[2]*DESCUENTO_MAX;
                    //com_smed = COMISION_DEFECTO - desc_smed;
                    break;
            }
            switch (cc[i].nat){
                case 0:
                    desc_nat = 0;
                    //com_nat = COMISION_DEFECTO - desc_nat;
                    break;
                case 1:
                    desc_nat = coefs[3]*0.5*DESCUENTO_MAX;
                    //com_nat = COMISION_DEFECTO - desc_nat;
                    break;
                case 2:
                    desc_nat = coefs[3]*DESCUENTO_MAX;
                    //com_nat = COMISION_DEFECTO - desc_nat;
                    break;
            }
			cc[i].comis_rentab = desc_smed + desc_nat;
            printf("h_rentab -> El descuento en la comision rentab de %s es: %.2f euros\n", cc[i].titular, cc[i].comis_rentab);
		}

        rent_ok = 1;

        if((rent_ok==1)&&(prod_ok==1)){
            update_ok = 0;
            pthread_cond_signal(&cuentas_cv);
            #if DEBUG
                printf("h_rentab ->Envia signal con cuentas_cv\n");
            #endif
        }
        
		pthread_mutex_unlock(&coefs_m);
        #if DEBUG
            printf("h_rentab ->Mutex desbloqueado\n");
        #endif
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

        sleep(3);

        #if DEBUG
            printf("h_total -> En while\n");
        #endif

		pthread_mutex_lock(&cuentas_m);
		
        #if DEBUG
            printf("h_total -> Voy a dormir\n");
            printf("\n");
        #endif
        while((rent_ok!=1)&&(prod_ok!=1)){
		  pthread_cond_wait(&cuentas_cv, &cuentas_m);
        }

        rent_ok = 0;
        prod_ok = 0;

        pthread_mutex_lock(&fin_m);

		for(i=0; i<NUM_CLIENTS; i++){

            #if DEBUG
			    printf("El valor de la comision rentab es: %f\n",cc[i].comis_rentab);
			    printf("El valor de la comision prod es: %f\n",cc[i].comis_prod);
            #endif

			cc[i].comis_total = COMISION_DEFECTO - (cc[i].comis_rentab + cc[i].comis_prod);

			printf("h_total -> El valor de la comision total de %s es: %.2f euros\n",cc[i].titular, cc[i].comis_total);
		}
        printf("\n");

		pthread_cond_signal(&fin_calculo_cv);
		pthread_mutex_unlock(&fin_m);
		pthread_mutex_unlock(&cuentas_m);
	}
}

/*
Funcion h_update()
-Actualiza las comisiones periodicamente de forma asincrona con el calculo de comisiones.
*/

void h_update(){

    int i;
    double valor;

    while(1){

        sleep(5);

    	pthread_mutex_lock(&coefs_m);
        #if DEBUG
            printf("h_update -> Dentro del while\n");
        #endif

        valor = double_rand();

        printf("h_update -> El valor en el vector de coefs es: {");

        for(i=0; i<NUM_COEFS; i++){
            coefs[i]=valor;
            if(i==NUM_COEFS-1){
                printf("%.2f}\n", valor);
            }else{
                printf("%.2f, ", valor);
            }
        }

        update_ok = 1;

        #if DEBUG
            printf("h_update -> update_ok = %d\n",update_ok);
        #endif

        #if DEBUG
            printf("h_update -> broadcast con coefs_update_cv\n");
            printf("\n");
        #endif

	    pthread_cond_broadcast(&coefs_update_cv);

        pthread_mutex_unlock(&coefs_m);        

        pthread_mutex_lock(&fin_m);

        #if DEBUG
            printf("h_update -> Se va a dormir\n");
            printf("\n");
        #endif

        pthread_cond_wait(&fin_calculo_cv,&fin_m);

	    pthread_mutex_unlock(&fin_m);
        #if DEBUG
            printf("h_update -> Se despierta\n");
            printf("\n");
        #endif

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
    cc[0].smed = 0;
    cc[0].tarj = 0;
    cc[0].seg = 0;
    cc[0].nat = 0;
    cc[0].comis_rentab = 0;
    cc[0].comis_prod = 0;
    cc[0].comis_total = 0;


    cc[1].titular = (char *)malloc(sizeof(char));
    strcpy(cc[1].titular,"Rafael Leon");
    cc[1].saldo = 1000;
    cc[1].hip = 2;
    cc[1].smed = 2;
    cc[1].tarj = 2;
    cc[1].seg = 2;
    cc[1].nat = 2;
    cc[1].comis_rentab = 0;
    cc[1].comis_prod = 0;
    cc[1].comis_total = 0;

    printf("Main:\n");
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
    

    /*Inicializacion de los mutexes*/
    pthread_mutex_init(&coefs_m, NULL);
    pthread_mutex_init(&fin_m, NULL);
    pthread_mutex_init(&cuentas_m, NULL);
    pthread_cond_init(&coefs_update_cv, NULL);
    pthread_cond_init(&cuentas_cv, NULL);
    pthread_cond_init(&fin_calculo_cv, NULL);


    #if DEDUG
        printf("Main -> Se acaban de inicializar los mutexes y cv\n");
    #endif

    /*Creacion de los hilos*/

    ret = pthread_create(&hebras_t[0], NULL, (void *)h_update, NULL);
    if(ret){
        printf("Main -> ERROR en pthread_create\n");
        pthread_exit(NULL);
    }
    ret = pthread_create(&hebras_t[1], NULL, (void *)h_prod, NULL);
    if(ret){
        printf("Main -> ERROR en pthread_create\n");
        pthread_exit(NULL);
    }
    ret = pthread_create(&hebras_t[2], NULL, (void *)h_rentab, NULL);
    if(ret){
        printf("Main -> ERROR en pthread_create\n");
        pthread_exit(NULL);
    }
    ret = pthread_create(&hebras_t[3], NULL, (void *)h_total, NULL);
    if(ret){
        printf("Main -> ERROR en pthread_create\n");
        pthread_exit(NULL);
    }

    for(i=0;i<NUM_THREADS;i++){

        ret = pthread_join(hebras_t[i], NULL);

        #if DEBUG
            printf("Main -> Ha terminado el hilo %d\n",i+1);
        #endif

        if(ret){
            printf("Main -> ERROR en pthread_join\n");
            pthread_exit(NULL);
        }
    }

    free(cc[0].titular);
    free(cc[1].titular);

    pthread_exit(NULL);

    return 0;
}