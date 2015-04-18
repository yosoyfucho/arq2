#include <fcntl.h>      /* Defines O_* constants */
#include <sys/stat.h>   /* Defines mode constants */
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


/* Definitions */

#define MAX_MESSAGE_SIZE 1024
#define MAX_QUEUE_SIZE 10
#define PERM_ALL 0777
#define THREADS 2


/*Global Variables*/
extern int errno;
mqd_t mqd;

struct packet_args
{
  int num;
  char * msg [MAX_QUEUE_SIZE];
};

/* pthreads functions */

static void * consumer();
static void * publisher (void * arg);

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

  /* Make ready the packet for the publisher */

  struct packet_args arguments;
  arguments.num = argc;
  for (i = 0; i<argc;i++)
  {
    arguments.msg[i] = (char *)malloc(MAX_MESSAGE_SIZE);
    strncpy(arguments.msg[i], argv[i],MAX_MESSAGE_SIZE);
  }

  #if DEBUG
    printf("DEBUG   MSG in the packet\n");
    for (i=0; i<arguments.num;i++)
    {
      printf("        -- %d -- %s\n", i,arguments.msg[i]);
    }
  #endif

  /*Make ready for the threads */

  #if DEBUG
    printf("DEBUG   Make ready for the threads\n");
  #endif

  pthread_t threads_t[THREADS];
  pthread_attr_t atributos;
  pthread_attr_init(&atributos);
  pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);

  int rc;

  rc = pthread_create(&threads_t[0],&atributos, &publisher, &arguments);
  if (rc==0)
  {
    printf("XXXX  Oh dear, something went wrong with pthread_create() with publisher! %s\n", strerror(rc));
    printf("      %d\n",rc);
    for (i = 0; i<argc;i++)
    {
      free(arguments.msg[i]);
    }
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

  rc = pthread_create(&threads_t[1],&atributos, consumer,NULL);
  if (rc==0)
  {
    printf("XXXX  Oh dear, something went wrong with pthread_create() with consumer! %s\n", strerror(rc));
    printf("      %d\n",rc);
    for (i = 0; i<argc;i++)
    {
      free(arguments.msg[i]);
    }
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

  for (i = 0; i<argc;i++)
  {
    free(arguments.msg[i]);
  }
  if (mq_close(mqd)== (mqd_t)-1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_close()! %s\n", strerror(errno));
  }
  if (mq_unlink(argv[1])== -1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_unlink()! %s\n", strerror(errno));
  }


  return 0;
}

static void * publisher (void * arg)
{
  #if DEBUG
    printf("__DEBUG in PUBLISHER\n");
  #endif
  struct packet_args * args = (struct packet_args *) arg;
  int i,se;
  #if DEBUG
    printf("  View of the packet:\n");
    printf("  -- Value of num: %d", args->num);
    for (i = 0; i<args->num;i++)
    {
      printf(" -- %d -- %s", i,args->msg[i]);
    }
  #endif

  for (i = 2; i< args->num; i++)
  {
    se = mq_send(mqd,args->msg[i],MAX_MESSAGE_SIZE,0);
    if(se == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_send()! %s\n", strerror(errno));
      pthread_exit(NULL);
    }
    #if DEBUG
      printf("__DEBUG in PUBLISHER\n");
      printf("    The args.msg[%d] : %s has been send\n", args->num, args->msg[i]);
    #endif
    printf("Publisher: %s\n",args->msg[i]);
    sleep(1);
  }
  pthread_exit(NULL);

}

static void * consumer()
{
  #if DEBUG
    printf("__DEBUG in CONSUMER\n");
  #endif
  ssize_t rcv =1;
  char * buffer = (char*)malloc(MAX_MESSAGE_SIZE);

  while(rcv>0)
  {
    #if DEBUG
      printf("__DEBUG in while consumption\n");
    #endif
    rcv = mq_receive(mqd,buffer,MAX_MESSAGE_SIZE,0);
    if(rcv == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_receive()! %s\n", strerror(errno));
    }
    memset(buffer,0,MAX_MESSAGE_SIZE);
    sleep(1);
  }
  #if DEBUG
    printf("__DEBUG out of the while consumption\n");
  #endif
  free (buffer);
  pthread_exit(NULL);

}
