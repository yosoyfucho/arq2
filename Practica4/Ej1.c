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
 gcc -Wall -lrt -g -o Ej1 Ej1.c

Línea de compilación con mensajes de depuración:
 gcc -Wall -DDEBUG -lrt -g -o Ej1 Ej1.c
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
#include <import.h>
#include <errno.h> //Lo incluyo para saber porque mq_open=-1

/* Constantes */

#define MAX_MESSAGE_SIZE 12
#define MAX_SIZE 1024
#define PERM_ALL 0777

/*Estructuras*/

/* Variables globales */
mqd_t mqd;
char nombreCola[] = "/miCola";
char * msg [MAX_MESSAGE_SIZE];

extern int errno;

/* funciones */

int cierroCola ()
{
	int aux;
	aux = mq_close(mqd);
	if (aux<0)
		{
			#if DEBUG
				printf("CierroCola -> Se ha producido un error en mq_close\n");
			#endif
		}
	else
	{
		aux = mq_unlink(nombreCola);
		if (aux <0)
		{
			#if DEBUG
				printf("CierroCola -> Se ha producido un error en mq_unlink\n");
			#endif
		}
	}
	return aux;
}

/*Funcion Consumidor() recibe mensajes de la cola*/
void consumidor(){
	#if DEBUG
		printf("CONSUMIDOR -> Acabamos de entrar en la funcion consumidor()\n");
	#endif
	char buffer[MAX_SIZE];

	if(mq_receive(mqd,buffer,MAX_SIZE,NULL)==-1)
	{
		printf("CONSUMIDOR -> Error en mq_receive()\n");
		#if DEBUG
	    printf("Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
	    #endif
	}
	else
		printf("CONSUMIDOR -> Mensaje leido en la cola: %s\n", buffer);

}

void productor(char * buffer){
	#if DEBUG
		printf("PRODUCTOR -> Acabamos de entrar en la funcion productor()\n");
	#endif

 //int mq_send(mqd_t mqdes, const char *msg_ptr,size_t msg_len, unsigned int msg_prio);
	printf("Productor escribiendo...\n");
	int rc = mq_send(mqd,buffer,sizeof(buffer)+1,1);
	#if DEBUG
		printf("PRODUCTOR -> he realizado la escritura\n");
	#endif

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


/* Función principal */

int main (int argc, char *argv[])
{
	int flags, rc,i;

	struct mq_attr attr;

	/* Defino mis atributos */
	attr.mq_maxmsg = MAX_MESSAGE_SIZE;
	attr.mq_msgsize = 2 * MAX_SIZE;
	attr.mq_curmsgs = 0;
	attr.mq_flags = 0;

	/* Defino los flags para abrir la cola */

	flags = O_CREAT|O_RDWR;

	#if DEBUG
		printf("MAIN -> Abriendo cola de mensajes\n");
	#endif

	/* Abro la cola de mensajes */
	#if DEBUG
		printf("MAIN -> Voy a usar la cola %s\n", nombreCola);
	#endif

	mqd = mq_open(nombreCola,flags,PERM_ALL,&attr);
	if (mqd <0)
	{
		#if DEBUG
		 fprintf (stderr, "%s ERROR: mq_open(): %s\n", nombreCola, strerror(errno));
     printf("ERROR en mq_open(): %d -> %s\n", errno, strerror(errno));
     #endif
     printf("MAIN -> fallo en mq_open\n");
     rc = cierroCola();
     printf("Eliminación de la cola : %d\n",rc);
     exit(EXIT_FAILURE);
  }

  #if DEBUG
  	printf("MAIN -> Cola abierta sin fallos\n");
  	printf("MAIN -> Genero mensaje a través de teclado\n");
  #endif

		if(argc<2 || strcmp(argv[1], "--help")==0)
		{
			printf("Formato incorrecto, ./ejecutable palabras_separadas_por_espacios\n");
			rc = cierroCola();
     	printf("Eliminación de la cola : %d\n",rc);

			exit(-1);
		}
		else
		{
			printf("Mensaje = { ");
			for(i=1; i<MAX_MESSAGE_SIZE; i++)
				{
					msg[i]=argv[i];
					printf("%s",msg[i]);
					if(i!=(MAX_MESSAGE_SIZE-1))
					{
						printf(", ");
					}
				}
			printf(" }\n");
		}
		#if DEBUG
			printf("MAIN -> Mensaje generado\n");
		#endif

pid_t pid, parent;

	pid = fork();
	if (pid<0)
	{
		perror("fork");
	}else if (pid == 0)
	 {

		/* Soy el primer hijo*/

		pid = getpid();
		parent = getppid();

		#if DEBUG
			printf("MAIN -> Creado primer hijo (Consumidor) (PID %d)\n", pid);
		#endif

		printf("MAIN -> Soy el Consumidor (PID %d PPID %d)\n",pid,parent);

		#if DEBUG
			printf("MAIN -> Lanzo mi funcion Consumidor (PID %d)\n",pid);
		#endif

		consumidor();

		#if DEBUG
			printf("MAIN -> He salido de mi funcion (Consumidor)(PID %d)\n",pid);
		#endif

		cierroCola();
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
		productor(*msg);
		cierroCola();
		exit(0);

	return 0;
}
