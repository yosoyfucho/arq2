#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>


#define MAX_MESSAGE_SIZE 100
#define MAX_QUEUE_SIZE 2
#define PERM_ALL 0600
#define NUM_THREADS 4

/*Variables Globales */
  mqd_t mqd;


/* Funciones para los hilos */

void * consumidor()
{
  #if DEBUG
    printf("CONSUMIDOR -> Entrando en hilo consumidor\n");
  #endif

  int aux;
  char * buffer = (char *)malloc(sizeof(char));

    aux =1;
    while (aux > 0)
    {
      aux = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
      if(aux == -1)
      {
        #if DEBUG
          printf("MAIN -> Se ha producido un error en mq_receive\n");
          printf("  Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
        #endif
        //mq_unlink(argv[1]);
        mq_close(mqd);
        exit(-1);
      }
      printf("---Consumidor: %s\n",buffer);
      memset(buffer,0,sizeof(buffer));
    }
  free(buffer);
  mq_close(mqd);
  #if DEBUG
    printf("CONSUMIDOR -> Saliendo de hilo consumidor\n");
  #endif
  pthread_exit(NULL);
}

void * productor (char * buffer)
{
  #if DEBUG
    printf("PRODUCTOR -> Entrando en hilo productor\n");
  #endif
/*  char buff[sizeof(buffer)];
  memcpy(buff,buffer,sizeof(buffer));*/
  int i, largoCadena,aux;
  largoCadena = strlen(buffer);
  for (i = 2; i< largoCadena; i++)
  {
    aux = mq_send(mqd,buffer[i],MAX_MESSAGE_SIZE,0);
    if (aux == -1)
    {

      #if DEBUG
        printf("MAIN -> Se ha producido un error en mq_send\n");
        printf("  Oh dear, something went wrong with mq_send()! %s\n", strerror(errno));
       #endif
      mq_unlink(buffer[1]);
      mq_close(mqd);
      exit(-1);
    }
    mq_close(mqd);
    printf("Productor: %s\n",buffer[i]);
    sleep(2);
  }
pthread_exit(NULL);
}


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
  pthread_t hebras_t[NUM_THREADS];
  int flags,ret;
  //mode_t perms; tercer argumento mq_open
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


/* Trabajo con hilos */

  ret = pthread_create(&hebras_t[0], NULL, productor, argv);
  if(ret){
    printf("Main -> ERROR en pthread_create con productor\n");
     mq_unlink(argv[1]);
      mq_close(mqd);
    pthread_exit(NULL);
  }

    ret = pthread_create(&hebras_t[1], NULL, consumidor, NULL);
  if(ret){
    printf("Main -> ERROR en pthread_create consumidor\n");
     mq_unlink(argv[1]);
      mq_close(mqd);
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


  mq_unlink(argv[1]);
      pthread_exit(NULL);

    return 0;



}
