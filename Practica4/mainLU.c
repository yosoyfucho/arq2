/*
Sin DEBUG
gcc -Wall -lrt -pthread -g -o mainLu mainLU.c
Con DEBUG
gcc -Wall -lrt -DDBUG -pthread -g -o mainLu mainLU.c
Ejecucion
./mainLu QFactura QServicio QInstalacion QMovilNoVa QAveriaPendiente

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


#define MAX_QUEUE_SIZE 6
#define MAX_MESSAGE_SIZE 100
#define NUM_THREADS 2

void hilos();
void operador_uno();
void operador_dos();

pthread_t hebras_t[NUM_THREADS];
int tam;
mqd_t mqd;
char * outgoing ="END";

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
  int aux;

  #if DEBUG
    printf("__DEBUG in Operador1\n");
  #endif
  ssize_t rcv =1;
  char * buffer = (char*)malloc(MAX_MESSAGE_SIZE);

  while(rcv>0)
  {
    #if DEBUG
      printf("__  __ DEBUG in while consumption\n");
    #endif
    rcv = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
    #if DEBUG
      printf("    Value of rcv: %d \n",rcv);
    #endif
    if(rcv == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
    }
    else
    {
      printf("--Operador1: %s recibida. Atendiendo.\n", buffer);
      sleep(2);
      printf("--Operador1: %s servida ***\n", buffer);
      #if DEBUG
      printf("__  __ outgoing: %s , buffer: %s\n", outgoing,buffer);
      #endif
      if (strncmp(outgoing,buffer,MAX_MESSAGE_SIZE)==0)
      {
        #if DEBUG
        printf("__ __  DEBUG exit while consumption\n");
        #endif
        break;
      }
      memset(buffer,0,MAX_MESSAGE_SIZE);
      aux = rand()%3 + 2;
      sleep(aux);
    }
  }
  #if DEBUG
    printf("__  __  DEBUG out of the while consumption\n");
  #endif
  free (buffer);
  #if DEBUG
    printf("__DEBUG CONSUMER OUT\n");
  #endif
  //pthread_exit(NULL);
}

void operador_dos(){
  
  int aux;

  #if DEBUG
    printf("__DEBUG in Operador2\n");
  #endif
  ssize_t rcv =1;
  char * buffer = (char*)malloc(MAX_MESSAGE_SIZE);

  while(rcv>0)
  {
    #if DEBUG
      printf("__  __ DEBUG in while consumption 2\n");
    #endif
    rcv = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
    #if DEBUG
      printf("    Value of rcv: %d \n",rcv);
    #endif
    if(rcv == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
    }
    else
    {
      printf("--Operador2: %s recibida. Atendiendo.\n", buffer);
      sleep(2);
      printf("--Operador2: %s servida ***\n", buffer);
      #if DEBUG
      printf("__  __ outgoing: %s , buffer: %s\n", outgoing,buffer);
      #endif
      if (strncmp(outgoing,buffer,MAX_MESSAGE_SIZE)==0)
      {
        #if DEBUG
        printf("__ __  DEBUG exit while consumption\n");
        #endif
        break;
      }
      memset(buffer,0,MAX_MESSAGE_SIZE);
      aux = rand()%3 + 2;
      sleep(aux);
    }
  }
  #if DEBUG
    printf("__  __  DEBUG out of the while consumption\n");
  #endif
  free (buffer);
  #if DEBUG
    printf("__DEBUG CONSUMER OUT\n");
  #endif
  //pthread_exit(NULL);
}

int
main (int argc, char *argv[])
{
  
  struct mq_attr attr;
  char * nombre = "/quejas";
  int i, res_send, flags;
  //int bytes_rec = 1;
  pid_t pid;
  pid_t p_hijo = 0;

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

    p_hijo = getpid();

    //Consumidor

    #if DEBUG
      printf("MAIN -> Proceso hijo creado = Consumidor, pid: %d , ppid: %d\n", p_hijo, getppid());
    #endif

      #if DEBUG
    printf("DEBUG   Make ready for the threads\n");
  #endif

  //pthread_t threads_t[THREADS];
  pthread_attr_t atributos;
  pthread_attr_init(&atributos);
  pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);

  int rc;

  /* Creating the threads */
  rc = pthread_create(&(hebras_t[0]),&atributos,(void *)operador_uno, NULL);
  if (rc!=0)
  {
    errno = rc;
    printf("XXXX  Oh dear, something went wrong with pthread_create() with publisher! %s\n", strerror(errno));
    printf("      %d\n",rc);
    if (mq_close(mqd)== (mqd_t)-1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_close()! %s\n", strerror(errno));
    }
    if (mq_unlink(argv[1])== -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_unlink()! %s\n", strerror(errno));
    }
    exit(-1);
  }

  rc = pthread_create(&(hebras_t[1]),&atributos, (void *)operador_dos,NULL);
  if (rc>0)
  {
    errno = rc;
    printf("XXXX  Oh dear, something went wrong with pthread_create() with consumer! %s\n", strerror(rc));
    printf("      %d\n",rc);
    if (mq_close(mqd)== (mqd_t)-1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_close()! %s\n", strerror(errno));
    }
    if (mq_unlink(argv[1])== -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_unlink()! %s\n", strerror(errno));
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
    printf("XXXX  Oh dear, something went wrong with mq_close()! %s\n", strerror(errno));
  }
  if (mq_unlink(argv[1])== -1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_unlink()! %s\n", strerror(errno));
  }

  pthread_exit(NULL);
  
  /*  char * buffer = (char *)malloc(MAX_MESSAGE_SIZE); 

    while (bytes_rec > 0){
      #if DEBUG
        printf("Entro en el while del consumidor\n");
      #endif

      bytes_rec = mq_receive(mqd, buffer, MAX_MESSAGE_SIZE, 0);
    
      #if DEBUG
        printf("bytes_rec = %d\n",bytes_rec);
      #endif
      if(bytes_rec == -1){
        printf("Error en mq_receive\n");
        perror("mq_receive");
        mq_unlink(argv[1]);
        mq_close(mqd);
        exit(-1);
      }
      //Aqui tengo que llamar a la funcion que me cree los hilos
      //para atender las peticiones.
      hilos();
      printf("--OperadorX: %s recibida. Atendiendo.\n", buffer);
      sleep(1);
      printf("--OperadorX: %s servida ***\n", buffer);
    }

    free(buffer);
    mq_close(mqd);*/
    
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
        sleep(3);
      }else if(res_send==-1){
        printf("PRODUCTOR -> Error en mq_send\n");
        perror("mq_send");
      }
    }
    mq_unlink(nombre);
    mq_close(mqd);
  }
  
 // notifySetup(&mqd);
 // pause(); /* Wait for notifications via thread function */
  #if DEBUG
    printf("Voy a matar el proceso hijo\n");
  #endif
  kill(p_hijo, SIGKILL);
  return 0;
}
