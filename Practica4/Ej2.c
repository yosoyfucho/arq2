#include <fcntl.h>      /* Defines O_* constants */
#include <sys/stat.h>   /* Defines mode constants */
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*
How to use:

Debug:
 gcc -Wall -DDEBUG -lpthread -lrt -g -o main Ej2.c

Without Debug:
 gcc -Wall -lpthread -lrt -g -o main Ej2.c


Execution:

./main </nameQueue> <FirstWord> ... <LastWord>
 */

/* Definitions */

#define MAX_MESSAGE_SIZE 100
#define MAX_QUEUE_SIZE 5
#define PERM_ALL 0777
#define THREADS 2


/*Global Variables*/
extern int errno;
mqd_t mqd;
int tam;
char * outgoing ="END";


/* pthreads functions prototype */

static void * h_consumer();
static void * h_publisher (void * arg);

/* Main Function */
int main(int argc, char* argv [])
{
  if (argc <=2 || strcmp(argv[1],"--help")==0)
  {
    printf("MAIN -> Something it's wrong with the line, the syntax is:\n");
    printf("  <./exec> </nameQueue> <FirstWord> <SecondWord> ... <LastWord>\n");
    exit(-1);
  }

  int flags,i;
  struct mq_attr attr, *attrp;
  srand(time(NULL));

  /* Defining attr */
  attrp = NULL;
  attr.mq_maxmsg = MAX_QUEUE_SIZE;    // default 10
  attr.mq_msgsize = MAX_MESSAGE_SIZE; // default 8192
  flags = O_CREAT|O_RDWR;

  attrp = &attr;

  mqd = mq_open(argv[1],flags,PERM_ALL,attrp);
  if(mqd == (mqd_t)-1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_open()! %s\n", strerror(errno));
    exit(-1);
  }
  #if DEBUG
    printf("DEBUG   queue %s created\n",argv[1]);
  #endif

  printf("Using %s Queue\n\n",argv[1]);

  tam = argc;


  /*Make ready for the threads */

  #if DEBUG
    printf("DEBUG   Make ready for the threads\n");
  #endif

  pthread_t threads_t[THREADS];
  pthread_attr_t atributos;
  pthread_attr_init(&atributos);
  pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);

  int rc;

  /* Creating the threads */
  rc = pthread_create(&(threads_t[0]),&atributos, h_publisher, argv);
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

  rc = pthread_create(&(threads_t[1]),&atributos, h_consumer,NULL);
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
  for (i = 0; i < THREADS; i++)
  {
    pthread_join(threads_t[i],&status);
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
  return 0;
}


/* threads functions */
static void * h_publisher (void * arg)
{
  #if DEBUG
    printf("__DEBUG in PUBLISHER\n");
  #endif
  int aux;
  char ** argv = (char **) arg;
  // struct packet_args * args = (struct packet_args *) arg;
  int i,se;
  #if DEBUG
    printf("  View of the packet:\n");
    printf("  -- Value of num: %d\n", tam);
    for (i = 0; i<tam;i++)
    {
      printf(" -- %d -- %s\n", i,argv[i]);
    }
  #endif

  for (i = 2; i< tam; i++)
  {
    se = mq_send(mqd,argv[i],MAX_MESSAGE_SIZE,0);
    if(se == -1)
    {
      errno = se;
      printf("XXXX  Oh dear, something went wrong with mq_send()! %d ... %s\n", errno, strerror(errno));
      pthread_exit(NULL);
    }
    #if DEBUG
      printf("__DEBUG in PUBLISHER\n");
      printf("    The args.msg[%d] : '%s' has been send\n", i, argv[i]);
    #endif
    printf("Publisher: %s\n",argv[i]);
    aux = rand()%3 + 2;
    sleep(aux);
  }
  printf("Publisher: %s\n",outgoing);

  se = mq_send(mqd,outgoing,MAX_MESSAGE_SIZE,0);
    if(se == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_send()! %d ... %s\n", errno, strerror(errno));
      pthread_exit(NULL);
    }
  #if DEBUG
    printf("__DEBUG Publisher OUT\n");
  #endif
  pthread_exit(NULL);

}

static void * h_consumer()
{
  int aux;

  #if DEBUG
    printf("__DEBUG in CONSUMER\n");
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
      printf("--- Consumer %s\n",buffer);
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
  pthread_exit(NULL);
}
