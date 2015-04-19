/*
Lucia Peñaranda Pardo, NIA: 100068468
Rafael León Miranda, NIA: 100275593
Practica 4 Ejercicio 3
Arquitectura de Sistemas 2
Linea de compilacion basica:
  gcc -Wall -lrt -pthread -g -o mainLu mainLU.c
Linea de compilacion con mensajes de depuracion:
  gcc -Wall -lrt -DDEBUG -pthread -g -o mainLu mainLU.c
Linea de ejecucion
  ./mainLu QFactura QServicio QInstalacion QMovilNoVa QAveriaPendiente
*/

/*
Librerias
*/

#include <pthread.h>
#include <mqueue.h>
#include <fcntl.h>            /* For definition of O_NONBLOCK */
#include <errno.h>
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
#include <wait.h>

/*
Constantes
*/

#define MAX_QUEUE_SIZE 6
#define MAX_MESSAGE_SIZE 100
#define NUM_THREADS 2

/*
Variables compartidas
*/

pthread_t hebras_t[NUM_THREADS];
int tam;
mqd_t mqd;
char * outgoing ="END";

/*
Prototipos de funciones
*/

void operador_uno();
void operador_dos();

// static void notifySetup(mqd_t *mdqp);

// static void /* Thread notification function */
// threadFunc(union signal sv)
// {
//   ssize_t numRead;
//   mqd_t *mqdp;
//   void * buffer;
//   struct mq_attr attr;

//   mqdp = sv.sival_ptr;

//   if (mq_getattr(*mqdp,&attr) == -1)
//   {
//     // Error
//   }

//   buffer = malloc (attr.mq_msgsize);
//   if (buffer == NULL)
//   {
//     // Error
//   }

//   notifySetup(mqdp);

//   while ((numRead = mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL)) >= 0)
//     printf("Read %ld bytes\n",(long) numRead);

//   if (errno != EAGAIN)          /* Unexpected error */
//     {
//       //Error en receive

//     }

//   free (buffer);
//   pthread_exit (NULL);
// }

// static void
// notifySetup (mqd_t *mqdp)
// {

//   struct sigevent sev;

//   sev.sigev_notify = SIGEV_THREAD;      /* Notify via thread */
//   sev.sigev_notify_function = threadFunc;
//   sev.sigev_notify_attributes = NULL;
//       /* Could be a pointer to pthread_attr_t structure */
//   sev.sigev_value.sival_ptr = mqdp;  /* Argument to threadFunc() */

//   if (mq_notify(*mqdp, &sev) == -1)
//   {
//     //Err mq_notify
//   }
// }


void operador_uno(){

  int aux, res_send;

  #if DEBUG
    printf("__DEBUG in Operador1\n");
  #endif
  ssize_t rcv =1;
  char * buffer = (char*)malloc(MAX_MESSAGE_SIZE);

  while(rcv>0)
  {
    #if DEBUG
      printf("__  __ DEBUG in while consumption Operador1\n");
    #endif
    rcv = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
    #if DEBUG
      printf("    Value of rcv en Operador1: %d \n",rcv);
    #endif
    if(rcv == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_receive() Operador1! %s\n", strerror(errno));
    }
    else
    {
      if (strncmp(outgoing,buffer,MAX_MESSAGE_SIZE)!=0)
      {
        printf("--Operador1: %s recibida. Atendiendo.\n", buffer);
        sleep(2);
        printf("--Operador1: %s servida ***\n", buffer);
        #if DEBUG
          printf("__  __ Operador1 outgoing: %s , buffer: %s\n", outgoing,buffer);
        #endif
      }else
      {
        #if DEBUG
        printf("__ __  DEBUG exit while consumption Operador1\n");
        #endif
        break;
      }
      memset(buffer,0,MAX_MESSAGE_SIZE);
      aux = rand()%3 + 2;
      sleep(aux);
    }
  }
  #if DEBUG
    printf("__  __  DEBUG out of the while consumption Operador1\n");
  #endif
  res_send = mq_send(mqd, outgoing, MAX_MESSAGE_SIZE, 0);
  if(res_send==0){
    #if DEBUG
      printf("Operador1 -> res_send = %d\n", res_send);
    #endif
  }else if(res_send==-1){
    printf("Operador1 -> Error en mq_send\n");
    perror("mq_send");
  }

  free (buffer);
  #if DEBUG
    printf("__DEBUG CONSUMER OUT Operador1\n");
  #endif 
}

void operador_dos(){
  
  int aux, res_send;

  #if DEBUG
    printf("__DEBUG in Operador2\n");
  #endif
  ssize_t rcv =1;
  char * buffer = (char*)malloc(MAX_MESSAGE_SIZE);

  while(rcv>0)
  {
    #if DEBUG
      printf("__  __ DEBUG in while consumption Operador2\n");
    #endif
    rcv = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
    #if DEBUG
      printf("    Value of rcv Operador2: %d \n",rcv);
    #endif
    if(rcv == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_receive() Operador2! %s\n", strerror(errno));
    }
    else
    {
      if(strncmp(outgoing,buffer,MAX_MESSAGE_SIZE)!=0){
        printf("--Operador2: %s recibida. Atendiendo.\n", buffer);
        sleep(2);
        printf("--Operador2: %s servida ***\n", buffer);
        #if DEBUG
          printf("__  __ Operador2 outgoing: %s , buffer: %s\n", outgoing,buffer);
        #endif
      }else
      {
        #if DEBUG
        printf("__ __  DEBUG exit while consumption Operador2\n");
        #endif
        break;
      }
      memset(buffer,0,MAX_MESSAGE_SIZE);
      aux = rand()%3 + 2;
      sleep(aux);
    }
  }
  #if DEBUG
    printf("__  __  DEBUG out of the while consumption Operador2\n");
  #endif
  res_send = mq_send(mqd, outgoing, MAX_MESSAGE_SIZE, 0);
  if(res_send==0){
    #if DEBUG
      printf("Operador2 -> res_send = %d\n", res_send);
    #endif
  }else if(res_send==-1){
    printf("Operador2 -> Error en mq_send\n");
    perror("mq_send");
  }

  free (buffer);
  #if DEBUG
    printf("__DEBUG CONSUMER OUT Operador2\n");
  #endif
}

int
main (int argc, char *argv[])
{
  
  struct mq_attr attr;
  char * nombre = "/quejas";
  int i, res_send, flags;// dead;
  pid_t pid;
 // pid_t p_hijo = 0;

  attr.mq_maxmsg = MAX_QUEUE_SIZE;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  flags = O_RDWR | O_CREAT;

  if (argc < 2 || strcmp(argv[1], "--help")==0){
      printf("MAIN -> Formato incorrecto, ./ejecutable quejas_separadas_por_espacios\n");
      exit(-1);
  }else{
    printf("\n");
    printf("Quejas = { ");
    for(i=1; i<argc-1; i++){
      printf("%s, ",argv[i]);
    }
    printf("%s }\n", argv[argc-1]);
    printf("\n");
  }
  tam = argc;
  #if DEBUG
    printf("tam = %d\n",tam);
  #endif
  mqd = mq_open(nombre,flags, 0777, &attr);

  if (mqd == -1){
      // Err en mq_open
      printf("MAIN-> Error al crear la cola \n");
      perror("mq_open");
      exit(-1);
  }
  
  pid = fork();

  if(pid == 0){

    //p_hijo = getpid();

    //Consumidor

    #if DEBUG
      printf("MAIN -> Proceso hijo creado = Consumidor, pid: %d , ppid: %d\n", getpid(), getppid());
    #endif

    #if DEBUG
      printf("DEBUG   Make ready for the threads\n");
    #endif

    pthread_attr_t atributos;
    pthread_attr_init(&atributos);
    pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);

    int rc;

    /* Creating the threads */
    rc = pthread_create(&(hebras_t[0]),&atributos,(void *)operador_uno, NULL);
    if (rc!=0)
    {
      errno = rc;
      printf("XXXX  Oh dear, something went wrong with pthread_create() with Operador1! %s\n", strerror(errno));
      printf("      %d\n",rc);
      if (mq_close(mqd)== (mqd_t)-1)
      {
        printf("XXXX  Oh dear, something went wrong with mq_close()! in MAIN thread Operador1 %s\n", strerror(errno));
      }
      if (mq_unlink(nombre)== -1)
      {
        printf("XXXX  Oh dear, something went wrong with mq_unlink()! in MAIN thread Operador1 %s\n", strerror(errno));
      }
      exit(-1);
    }

    rc = pthread_create(&(hebras_t[1]),&atributos, (void *)operador_dos,NULL);
    if (rc>0)
    {
      errno = rc;
      printf("XXXX  Oh dear, something went wrong with pthread_create() with Operador2! %s\n", strerror(rc));
      printf("      %d\n",rc);
      if (mq_close(mqd)== (mqd_t)-1)
      {
        printf("XXXX  Oh dear, something went wrong with mq_close()! in MAIN thread Operador2 %s\n", strerror(errno));
      }
      if (mq_unlink(nombre)== -1)
      {
        printf("XXXX  Oh dear, something went wrong with mq_unlink()! in MAIN thread Operador2 %s\n", strerror(errno));
      }
      exit(-1);
    }


    #if DEBUG
      printf("DEBUG   threads created\n");
    #endif

    /*Ending*/

    pthread_attr_destroy(&atributos);
    void * status;
    for (i = 0; i < NUM_THREADS; i++)
    {
      pthread_join(hebras_t[i],&status);
    }
    if (mq_close(mqd)== (mqd_t)-1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_close()! in MAIN %s\n", strerror(errno));
    }
    if (mq_unlink(nombre)== -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_unlink()! in MAIN %s\n", strerror(errno));
    }

    pthread_exit(NULL);
    printf("Fin del proceso hijo\n");
    exit(0);
    
  }else{

    //Productor
    #if DEBUG
      printf("MAIN -> Proceso padre = Productor, pid: %d \n", getpid());
    #endif

    printf("Cliente en espera... 5 segundos...\n");
    printf("\n");
    sleep(3);

    for (i = 1; i < argc; i++){
      res_send = mq_send(mqd, argv[i], MAX_MESSAGE_SIZE, 0);
      if(res_send==0){
        #if DEBUG
          printf("PRODUCTOR -> res_send = %d\n", res_send);
        #endif
        printf("Cliente: %s\n", argv[i]);
        sleep(1);
      }else if(res_send==-1){
        printf("PRODUCTOR -> Error en mq_send\n");
        perror("mq_send");
      }
    }

    res_send = mq_send(mqd, outgoing, MAX_MESSAGE_SIZE, 0);
    if(res_send==0){
      #if DEBUG
        printf("PRODUCTOR -> He mandado el outgoing res_send = %d\n", res_send);
      #endif
    }else if(res_send==-1){
      #if DEBUG
        printf("PRODUCTOR -> Error en mq_send cuando mando outgoing\n");
        perror("mq_send");
      #endif
    }
    wait(NULL);
    mq_unlink(nombre);
    mq_close(mqd);
  }
  
 // notifySetup(&mqd);
 // pause(); /* Wait for notifications via thread function */
 /* #if DEBUG
    printf("Voy a matar el proceso hijo\n");
  #endif
  dead = kill(p_hijo, SIGKILL);
  if(dead == 0){
    #if DEBUG
      printf("Proceso hijo finalizado\n");
    #endif
  }else{
    #if DEBUG
      printf("Proceso hijo sigue vivo\n");
    #endif
  }*/
  return 0;
}
