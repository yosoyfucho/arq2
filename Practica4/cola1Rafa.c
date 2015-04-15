/*
Grupo 6
Lucía Peñaranda Pardo
Rafael León Miranda
Práctica 4
Ejercicio 1
Uso de colas de mensajes para comunicación entre procesos
Arquitectura de Sistemas II
Esquema Padre(Productor) - Hijo(Consumidor)

*/
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
#define MAX_QUEUE_SIZE 2
#define PERM_ALL 0600

/*
con Debug gcc -Wall -DDEBUG -lrt -g -o new cola1Rafa.c
sin Debug  gcc -Wall -lrt -g -o new cola1Rafa.c

Al ejecutar el primer parametro es el nombre de la cola

./new /nombreCola Oye como va mi ritmo bueno pa
*/


int main (int argc, char * argv[])
{
  if (argc <= 2 || strcmp(argv[1], "--help") == 0)
      {
        #if DEBUG
          printf("MAIN -> O no hay parametros suficientes o piden ayuda para iniciar\n");
        #endif
        printf("main4_1.c El formato de ejecucion es:\n");
        printf("  <./Ejecutable> <cola que abriremos con /> <mensaje compuesto por variables palabras>\n");
        exit(-1);
      }

  int flags;
  //mode_t perms; tercer argumento mq_open
  mqd_t mqd;
  struct mq_attr attr;
  //char * name = "/cola12";

  attr.mq_maxmsg = MAX_QUEUE_SIZE;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  flags = O_RDWR | O_CREAT;

  mqd = mq_open(argv[1], flags, 0777, &attr);

#if DEBUG
  printf("MAIN -> Voy a abrir la cola %s\n", argv[1]);
#endif
 // mqd = mq_open(argv[1],O_CREAT|O_RDWR,PERM_ALL, &atributos);
 if (mqd == (mqd_t)-1)
  {
    #if DEBUG
      printf("MAIN -> Se ha producido un error en mq_open\n");
      printf("	Oh dear, something went wrong with mq_open()! %s\n", strerror(errno));
    #endif
    exit(-1);
  }
  int i;
  #if DEBUG
    printf("MAIN -> Cola %s creada", argv[1]);
    printf("MAIN -> Mensaje a enviar:\n");
    for (i=1;i<argc;i++)
    {
      printf("%s \n",argv[i]);
    }
  #endif
  printf("QUEUE = %s\n", argv[1]);
  printf("PALABRAS = { ");
  for (i=2; i<argc-1;i++)
  {
    printf("%s ,",argv[i]);
  }
  printf("%s }\n\n",argv[argc-1]);


  int pid = fork();
  if (pid == (pid_t) 0)
  {
    /* Soy el hijo - Consumer*/
    #if DEBUG
      printf("Consumidor pid %d y padre con pid %d\n",getpid(),getppid());
    #endif
    int aux = 1;
    char * buffer = (char*)malloc(sizeof(MAX_MESSAGE_SIZE));
    while (aux > 0)
    {
      aux = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
      if(aux == -1)
      {
        #if DEBUG
          printf("MAIN -> Se ha producido un error en mq_receive\n");
          printf("	Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
        #endif
        mq_unlink(argv[1]);
        mq_close(mqd);
        exit(-1);
      }
      printf("---Consumidor: %s\n",buffer);
      memset(buffer,0,sizeof(buffer));
    }
    free(buffer);
    mq_close(mqd);
  }
  else
  {
    /* Soy el padre - Publisher*/
    #if DEBUG
      printf("Productor pid %d \n",getpid());
    #endif
    int env;
    for (i=2;i<argc;i++)
    {
    	#if DEBUG
    		printf("	El valor del tamaño de %s es %d\n",argv[i],sizeof(argv[i]));
    	#endif

      env = mq_send(mqd,argv[i],MAX_MESSAGE_SIZE,0);

      if (env == -1)
      {
        #if DEBUG
          printf("MAIN -> Se ha producido un error en mq_send\n");
          printf("	Oh dear, something went wrong with mq_send()! %s\n", strerror(errno));
        #endif
        mq_unlink(argv[1]);
        mq_close(mqd);
        exit(-1);
      }
      printf("Productor: %s\n",argv[i]);
      sleep(2);
    }
    mq_unlink(argv[1]);
    mq_close(mqd);


  }



    return 0;



}
