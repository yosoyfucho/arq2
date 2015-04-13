/*
Grupo 6
Lucía Peñaranda Pardo
Rafael León Miranda
Práctica 4
Ejercicio 1
Uso de colas de mensajes para comunicación entre procesos
Arquitectura de Sistemas II
Esquema Padre(Productor) - Hijo(Consumidor)
Mensajes de depuración disponibles con -DDEBUG

Línea de compilacion sin mensajes de depuración:
 gcc -Wall -lrt -g -o p4_1 p4ej1.c

Línea de compilación con mensajes de depuración:
 gcc -Wall -DDEBUG -lrt -g -o p4_1 p4ej1.c
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
#include <errno.h> //Lo incluyo para saber porque mq_open=-1

/* Constantes */

#define MAX_MESSAGE_SIZE 12

/*Estructuras*/

/* Variables globales */
mqd_t mqd_q;
extern int errno;
	
/* Prototipos de funciones */
void productor(char * buffer, mqd_t mqd_q);
void consumidor();

/*Funcion Productor() envia mensajes a la cola*/
void productor(char * buffer, mqd_t mqd_q){
	#if DEBUG
		printf("PRODUCTOR -> Acabamos de entrar en la funcion productor()\n");
	#endif

	if(mq_send(mqd_q,buffer,20*sizeof(char),0)==-1)
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

	struct mq_attr attr;
	char * palabras[MAX_MESSAGE_SIZE];
	int i;


	attr.mq_flags = 0;
  	attr.mq_maxmsg = 10;
  	attr.mq_msgsize = 1024;
  	attr.mq_curmsgs = 0;
  

	mqd_q = mq_open("/newQueue",O_RDWR | O_CREAT, 0600, &attr);

  if (mqd_q == -1)
  {
     fprintf (stderr, "%s ERROR: mq_open(): %s\n", argv[0], strerror(errno));
     exit(EXIT_FAILURE);
  }

	#if DEBUG
		printf("MAIN -> Se ha creado la cola de mensajes de nombre comq\n");
	#endif

	if(argc<2 || strcmp(argv[1], "--help")==0){
		printf("Formato incorrecto, ./ejecutable palabras_separadas_por_espacios\n");
		exit(-1);
	}else{
		printf("PALABRAS = { ");
		for(i=1; i<MAX_MESSAGE_SIZE; i++){
			palabras[i]=argv[i];
			printf("%s",palabras[i]);
			if(i!=(MAX_MESSAGE_SIZE-1)){
				printf(", ");
			}
		}
		printf(" }\n");
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
			printf("MAIN -> Soy el padre (Productor) y ya he creado a mi hijo (PID %d)\n", getpid());
		#endif
					
		pid = getpid();
		printf("MAIN -> Soy el Productor (PID %d)\n",pid);

		#if DEBUG
			printf("MAIN -> Lanzo mi funcion Productor (PID %d)\n",pid);
		#endif
		productor(*palabras, mqd_q);
		exit(0);	
	
	return 0;
}
