
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>

#define MAX_MESSAGE_SIZE 100
#define MAX_QUEUE_SIZE 42

int main(int argc, char *argv[]) 
{
	int i,flags,rc;
	mqd_t mqd;
	flags = O_RDWR|O_CREAT;
	char * nombreCola = "/estaCola2";
			char * buffer = (char *)malloc(MAX_MESSAGE_SIZE);


#if DEBUG	
printf("El valor de Argc es :%d \n",argc);
printf("El nombre de la cola es %s:\n",nombreCola);
#endif

if(argc<2)
{
	printf("El Formato de ejecución es /nombreCola mensaje_con_espacios\n");
	exit(-1);
}


	printf("Cola a usar: %s\n", nombreCola);
	printf(" ");



	printf("MENSAJE = {");
	for (i = 1; i<argc;i++)
	{
		printf("%s, ", argv[i]);
	}
	printf("}\n");

#if DEBUG
	printf("MAIN-> Voy a abrir la cola %s\n",nombreCola);
#endif

	mqd = mq_open(nombreCola,flags,0777,NULL);

	if (mqd <0)
	{
		printf("Error en mq_open\n");	
		mq_unlink(nombreCola);
		mq_close(mqd);

	}

	#if DEBUG
		printf("MAIN -> Cola creada sin errores\n");
	#endif
	int pid, ppid;
	pid = fork();
	if (pid == 0)
	{
		pid = getpid();
		ppid = getppid();
		#if DEBUG
			printf("Soy el proceso Consumidor pid: %d y mi padre es :%d \n",pid,ppid );
		#endif

		/*Soy el hijo */
  	int recibidos = 1;     
    while (recibidos > 0)
    {
    	//hijo, consumidor
    	
    	recibidos = mq_receive(mqd, buffer, MAX_MESSAGE_SIZE, 0);
    	#if DEBUG
    		printf("---Consumidor: Hemos recibido %d bytes\n",recibidos);
    	#endif
    	printf("---Consumidor: %s\n", buffer);
    	sleep(1);
    	memset(buffer, 0, sizeof(buffer));
    }
		#if DEBUG
			printf("Consumidor -> Cierro mi descriptor de la cola\n");
		#endif
		mq_unlink(nombreCola);
		mq_close(mqd);
		free(buffer);
		
	}
	else
	{
		pid = getpid();
		#if DEBUG
			printf("Soy el proceso productor y mi pid es %d\n",pid );
		#endif
		for (i = 1; i < argc; i++)
		{
			rc = mq_send(mqd,argv[i],MAX_MESSAGE_SIZE,0);
			if (rc<0)
			{
				printf("Productor -> Error en mq_send()\n");
				mq_unlink(nombreCola);
				mq_close(mqd);
				exit(-1);
			}
			#if DEBUG
				printf("Productor: Envio con exito\n");
			#endif
			printf("Productor: %s\n", argv[i]);
			sleep(1);
		}
		#if DEBUG
			printf("Productor -> Cierro mi descriptor de la cola\n");
		#endif
		mq_unlink(nombreCola);
		mq_close(mqd);
	}

	
  return 0;
} 

