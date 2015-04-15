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
 gcc -Wall -lrt -g -o p4_1 p4ej1RA.c

Línea de compilación con mensajes de depuración:
 gcc -Wall -DDEBUG -lrt -g -o p4_1 p4ej1RA.c
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
#define MAX_SIZE 1024
#define PERM_ALL 0777

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

 //int mq_send(mqd_t mqdes, const char *msg_ptr,size_t msg_len, unsigned int msg_prio);
	printf("Productor escribiendo...\n");
	int rc = mq_send(mqd_q,buffer,sizeof(buffer)+1,1);
	#if DEBUG
		printf("PRODUCTOR -> he realizado la escritura\n");
	#endif

	//if(mq_send(mqd_q,buffer,MAX_SIZE,0)==-1)
	if(rc<0)
	{
		printf("PRODUCTOR -> Error en mq_send\n");
		#if DEBUG
	    printf("Oh dear, something went wrong with mq_send()!: %d -> %s\n", errno, strerror(errno));
	    #endif
	}
	else
		printf("PRODUCTOR -> Mensaje enviado a la cola: %s\n", buffer);


}

/*Funcion Consumidor() recibe mensajes de la cola*/
void consumidor(){
	#if DEBUG
		printf("CONSUMIDOR -> Acabamos de entrar en la funcion consumidor()\n");
	#endif
	char buffer[MAX_SIZE];

	if(mq_receive(mqd_q,buffer,MAX_SIZE,NULL)==-1)
	{
		printf("CONSUMIDOR -> Error en mq_receive()\n");
		#if DEBUG
	    printf("Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
	    #endif
	}
	else
		printf("CONSUMIDOR -> Mensaje enviado a la cola: %s\n", buffer);

}


/* Main */
int main (int argc, char *argv[]){

	struct mq_attr attr;
	char * palabras[MAX_MESSAGE_SIZE];
	int i;

	/* Definiendo los atributos de la estructura */
	attr.mq_flags = 0;
  	attr.mq_maxmsg = MAX_MESSAGE_SIZE;
  	attr.mq_msgsize = 10*MAX_SIZE;
  	attr.mq_curmsgs = 0;
  

	mqd_q = mq_open("/newQueue",O_RDWR | O_CREAT, PERM_ALL, &attr);

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
