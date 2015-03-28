/*
Grupo 6
Lucía Peñaranda Pardo
Rafael León Miranda
Práctica 4
Ejercicio 1
Uso de colas de mensajes para comunicación entre procesos
Arquitectura de Sistemas II
Esquema Padre - Hijo - Hijo -Hijo
Mensajes de depuración disponibles con -DDEBUG
*/

/* Librerias */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <mqueue.h>

/* Constantes */

#define MAX_MESSAGE_SIZE 11

/*Estructuras*/

/* Variables globales */

/* Prototipos de funciones */
void productor();
void consumidor();

/*Funcion Productor() envia mensajes a la cola*/
void productor(char buffer[MAX_MESSAGE_SIZE]){
	#if DEBUG
		printf("PRODUCTOR -> Acabamos de entrar en la funcion productor()\n");
	#endif

	if(mq_send(mqd,buffer,sizeof(20*char),0)==-1)
		printf("PRODUCTOR -> Error en mq_send\n");

}

/*Funcion Consumidor() recibe mensajes de la cola*/
void consumidor(){
	#if DEBUG
		printf("CONSUMIDOR -> Acabamos de entrar en la funcion consumidor()\n");
	#endif
}


/* Main */
int main (int argc, char *argv[]){

	mqd_t mqd;
	struct mq_attr atributos;
	char palabras[MAX_MESSAGE_SIZE];
	int i;

	atributos.mq_maxmsg = MAX_MESSAGE_SIZE;
	atributos.mq_msgsize = sizeof(20*char);

	if ((mqd = mq_open("comq",O_CREAT, 0777, &atributos))==-1){
		printf("MAIN ->Error en mq_open\n");
		exit(-1);
	}

	#if DEBUG
		printf("MAIN -> Se ha creado la cola de mensajes de nombre comq\n");
	#endif

	if(argc!=2 || strcmp(argv[1], "--help")==0){
		usageErr("%s mq-name\n", argv[0]);
	}else{
		printf("PALABRAS = { ");
		for(i=0; i<MAX_MESSAGE_SIZE; i++){
			palabras[i]=argv[i];
			printf(palabras[i]);
			if(i!=(MAX_MESSAGE_SIZE-1)){
				printf(",");
			}
		}
		printf("}\n");
	}

	pid_t pid, parent;
	
	pid = fork();
	if (pid<0){
		perror("fork");
	}else if (pid == 0){
		
			/* Soy el primer hijo*/
	
			pid = getpid();
			parent = getppid();

			#if DEBUG
				printf("MAIN -> Creado primer hijo (Consumidor) (PID %d)\n", getpid());
			#endif

			pid = getpid();
			parent = getppid();
			printf("MAIN -> Soy el Consumidor (PID %d PPID %d)\n",pid,parent);

			#if DEBUG
				printf("MAIN -> Lanzo mi funcion Consumidor (PID %d)\n",pid);
			#endif

			consumidor();

			#if DEBUG
				printf("MAIN -> He salido de mi funcion (Consumidor)(PID %d)\n",pid);
			#endif
		
			exit(0);	
		}
			
		/* Soy el padre */
		#if DEBUG
			printf("MAIN -> Soy el padre y ya he creado a mi hijo (PID %d)\n", getpid());
		#endif
					
		pid = getpid();
		printf("MAIN -> Soy el padre (Productor)(PID %d)\n",pid );
		productor(palabras);
		exit(0);	
	}	
	return 0;
}
