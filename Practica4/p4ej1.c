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

/* Constantes */

#define MAX_MESSAGE_SIZE 8


/* Variables globales */

/* Prototipos de funciones */


/* Main */
int main (int argc, char *argv[]){

	mqd_t mqd;

	if(argc!=2 || strcmp(argv[1], "--help")==0)
		usageErr("%s mq-name\n", argv[0]);
	

	pid_t pid, parent;
	
	pid = fork();
	if (pid<0){
		perror("fork");
	}else{
		
		if (pid == 0){
			/* Soy el primer hijo*/
			#if DEBUG
				printf("Creado el primer hijo (Productor) (PID %d)\n",getpid());
			#endif
			pid = getpid();
			parent = getppid();
			printf("Soy el Productor (PID %d PPID %d)\n",pid,parent);

			#if DEBUG
				printf("Lanzo mi funcion Productor (PID %d)\n",pid);
			#endif
				productor();

			#if DEBUG
				printf("El productor ha salido de su función (PID %d)\n",pid);
			#endif

			exit(0);
		}else{

			#if DEBUG
			printf("Estoy en el padre y voy a crear al segundo hijo\n");
			#endif

			pid = fork();

			if (pid <0){
				perror("fork");
			}else{
				if (pid == 0){
				/* Soy el segundo hijo - Consumidor */
				#if DEBUG
					printf("Creado el segundo hijo (Consumidor) (PID %d)\n", getpid());
				#endif
				pid = getpid();
				parent = getppid();
				printf("Soy el Consumidor (PID %d PPID %d)\n",pid,parent);

				#if DEBUG
					printf("Lanzo mi funcion Consumidor (PID %d)\n",pid);
				#endif

				consumidor();

				#if DEBUG
					printf("He salido de mi funcion (Consumidor)(PID %d)\n",pid);
				#endif
				exit(0);

				}else{
			
					/* Soy el padre */
					#if DEBUG
						printf("Soy el padre y ya he creado a todos mis hijos (PID %d)\n", getpid());
					#endif
						
					pid = getpid();
					printf("Soy el padre (PID %d)\n",pid );
					padre();
					wait(NULL);
					exit(0);
				}
			}
		}

	}
return 0;
}
