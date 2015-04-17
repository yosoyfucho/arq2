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

int
main (int argc, char *argv[])
{
  mqd_t mqd;
  struct mq_attr attr;
  char * nombre = "/quejas";
  int i, bytes_rec, bytes_env, flags;
  pid_t pid;

  attr.mq_maxmsg = MAX_QUEUE_SIZE;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  //attr.mq_curmsgs = 0;
  //attr.mq_flags = O_NONBLOCK;
  flags = O_RDWR | O_CREAT;

  if (argc < 2 || strcmp(argv[1], "--help")==0){
      printf("MAIN -> Formato incorrecto, ./ejecutable quejas_separadas_por_espacios\n");
      exit(-1);
  }else{
    printf("Quejas = { ");
    for(i=1; i<argc-1; i++){
      printf("%s, ",argv[i]);
    }
    printf("%s }\n", argv[argc-1]);
  }

    mqd = mq_open(nombre,flags, 0777, &attr);

  if (mqd == -1){
      // Err en mq_open
      printf("MAIN-> Error al crear la cola \n");
      perror("mq_open");
      exit(-1);
  }
  
  pid = fork();

  if(pid == 0){

    //Consumidor
    #if DEBUG
      printf("MAIN -> Proceso hijo creado = Consumidor, pid: %d , ppid: %d\n", getpid(), getppid());
    #endif

    //consumidor();

  }else{

    //Productor
    #if DEBUG
      printf("MAIN -> Proceso padre = Productor, pid: %d \n", getpid());
    #endif

    for (i = 1; i < argc; i++){
      bytes_env = mq_send(mqd, argv[i], MAX_MESSAGE_SIZE, 0);
      printf("bytes_env = %d", bytes_env);
      printf("Cliente: %s\n", argv[i]);
      sleep(1);
    }
    mq_unlink(nombre);
    mq_close(mqd);

  }
 // notifySetup(&mqd);
 // pause(); /* Wait for notifications via thread function */
  return 0;
}
