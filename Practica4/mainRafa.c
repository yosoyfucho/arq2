/*
Lucia Peñaranda Pardo, NIA: 100068468
Rafael León Miranda, NIA: 100275593
Practica 4 Ejercicio 3
Arquitectura de Sistemas 2

Linea de compilacion basica:
  gcc -Wall -lrt -pthread -g -o main main.c

Linea de compilacion con mensajes de depuracion:
  gcc -Wall -lrt -DDEBUG -pthread -g -o main main.c

Linea de ejecucion
  ./main QFactura QServicio QInstalacion QMovilNoVa QAveriaPendiente
*/

/* Time for the includes */
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
#include <linux/unistd.h>

/* Time for the constants */

#define MAX_QUEUE_SIZE 6
#define MAX_MESSAGE_SIZE 100
#define PERM_ALL 0777
#define THREADS 2

/* Global Variables */

int size;
mqd_t mqd;
char * ending = "END";
char * queueName = "/quejasRALUq";


/* Function Prototypes */

static void
notifySetup(mqd_t *mqdp);
static void
threadFunc (union sigval sv);
static void
publisher (void * buffer);

/* Main Function */

int
main (int argc, char * argv[])
{
	struct mq_attr attr;
  int i, flags;
  pid_t pid;

  /* Definition of queue atributes */
  attr.mq_maxmsg = MAX_QUEUE_SIZE;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  flags = O_RDWR | O_CREAT;

  if (argc < 2 || strcmp(argv[1], "--help")==0)
  {
    printf("MAIN -> Formato incorrecto, ./ejecutable quejas_separadas_por_espacios\n");
    exit(-1);
  }
  else
  {
  	#if DEBUG
  		printf("MAIN -> Formato correcto\n");
  	#endif
    printf("\n");
    printf("Quejas = { ");
    for(i=1; i<argc-1; i++)
    {
      printf("%s, ",argv[i]);
    }
    printf("%s }\n", argv[argc-1]);
    printf("\n");
  }

  mqd = mq_open(queueName,flags,PERM_ALL,&attr);
  if (mqd == (mqd_t)-1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_open()! %s\n", strerror(errno));
    exit(-1);
  }
  #if DEBUG
  	printf("The %s Queue has been created\n",queueName);
  #endif

  size = argc;

  /* Time for the fork */

  pid = fork();

  if (pid == 0)
  {
  	/* This is the son's code - I'm the consumer */

  	/* Make ready for the threads */

		 #if DEBUG
		  //printf("DEBUG   Make ready for the threads\n");
      printf("DEBUG in son\n");
		 #endif

     notifySetup(&mqd);
     pause();
     #if DEBUG
      printf("DEBUG in son -> Ready to die\n");
     #endif
     mq_unlink(queueName);
     mq_close(mqd);

  }
  else
  {

  	/* This is the father's code - I'm the publisher */

  	int duerme = 3;
  	#if DEBUG
  		printf("DEBUG in father -> making a sleep(%d) to let the consumer notify first\n",duerme);
  	#endif
  	sleep(duerme);
  	publisher((void *) argv);
    #if DEBUG
      printf("DEBUG in father -> Ready to die\n");
    #endif
  	mq_unlink(queueName);
 		mq_close(mqd);

  }



  wait(NULL);
  #if DEBUG
    printf("DEBUG in MAIN -> die my darling\n");
  #endif
  kill(0,SIGKILL);

  #if DEBUG
  	printf("DEBUG in MAIN-> Execution Finished, waiting for the exit\n");
  #endif

 	exit(1);
}

/*
  Function for the publisher
Get a message as an argument and then send it to the queue
*/
void
publisher (void * argument)
{
	int i, se;
	char ** msg = (char **)argument;
	#if DEBUG
		printf("DEBUG in publisher -> msg ready to be send\n");
	#endif
	for (i = 1; i < size; i++)
	{
		sleep(6);
 		se = mq_send(mqd,msg[i],MAX_MESSAGE_SIZE,0);
 		if(se == -1)
    {
      printf("XXXX  Oh dear, something went wrong with mq_send()! %d ... %s\n", errno, strerror(errno));
			exit(-1);
		}
    printf("Cliente: '%s'\n", msg[i]);
	}
	#if DEBUG
		printf("DEBUG in publisher -> out of the for loop\n");
	#endif
	se = mq_send(mqd,ending,MAX_MESSAGE_SIZE,0);
 	if(se == -1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_send()! %d ... %s\n", errno, strerror(errno));
		exit(-1);
	}
  #if DEBUG
  	printf("DEBUG in publisher -> Exiting function\n");
  #endif

}

/*
  Function for enabling notify

Using the queue descriptor enables the notifying
 */

static void
notifySetup(mqd_t *mqdp)
{
  #if DEBUG
    printf("        DEBUG in notifySetup -> BEGIN\n");
  #endif
  struct sigevent sev;

  sev.sigev_notify = SIGEV_THREAD;        /* Notify via thread */
  sev.sigev_notify_function = threadFunc;
  sev.sigev_notify_attributes = NULL;     /* Could be pointer to pthread_attr_t structure */
  sev.sigev_value.sival_ptr = mqdp;       /* Argumento to threadFunc()*/

  #if DEBUG
  	printf("        DEBUG in notifySetup -> sev struct initialized\n");
  #endif

  if (mq_notify(*mqdp,&sev) == -1)
  {
    printf("XXXX    ERROR in notifySetup -> mq_notify\n");
    exit(-1);
  }
  #if DEBUG
  	printf("        DEBUG in notifySetup -> EXIT\n");
  #endif
}

static void
threadFunc (union sigval sv)
{
  #if DEBUG
    printf("    DEBUG in threadFunc\n");
  #endif
  ssize_t numRead;
  mqd_t *mqdp;
  char *buffer;
  struct mq_attr attr;

  mqdp = sv.sival_ptr;

  if (mq_getattr(*mqdp,&attr) == -1)
  {
    printf("    ERROR in threadFunc -> mq_getattr\n");
  }

  buffer = (char *)malloc(attr.mq_msgsize);
  if (buffer == NULL)
  {
    printf("    ERROR in threadFunc -> malloc\n");
    exit(-1);
  }

  notifySetup (mqdp);

  while ((numRead = mq_receive(*mqdp, buffer, attr.mq_msgsize,NULL)) >=0)
  {
    #if DEBUG
    printf("    Read %ld bytes \n", (long) numRead);
    #endif
    printf("  	 ---Operator    '%s' recibida. Atendiendo\n",buffer);
    sleep(2);
    printf("  	 ---Operator    '%s' servida ***\n", buffer);
    if (strncmp(buffer,ending,MAX_MESSAGE_SIZE)==0)
    {
      #if DEBUG
        printf("    %s ending the receive\n",buffer);
      #endif
      break;
    }
  }

  if(numRead == (ssize_t)-1)
  {
    printf("XXXX  Oh dear, something went wrong with mq_receive()! %d ... %s\n", errno, strerror(errno));
    exit(-1);

  }
    printf("     ---Operator    CLOSING FOR TODAY\n");
  pthread_exit(NULL);
}
