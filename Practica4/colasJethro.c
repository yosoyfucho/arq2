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

void main(int argc, char *argv[]) 
{
  int pid_a;
  int flags, opt;
  //mode_t perms; tercer argumento mq_open
  mqd_t mqd;
  struct mq_attr attr;
  char * name = "/cola12";

  attr.mq_maxmsg = MAX_QUEUE_SIZE;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  flags = O_RDWR | O_CREAT;

  mqd = mq_open(name, flags, 0777, &attr);

  int i;
  printf("PALABRAS = { ");
  for (i = 1; i < argc-1; i++)
  {
  	printf("%s, ", argv[i]);
  }
  printf("%s }\n", argv[argc-1]);
  pid_a = fork();
  
  if (pid_a == 0)
  {
  	char * buffer = (char *)malloc(MAX_MESSAGE_SIZE);  
  	int bytes_received = 1;     
    while (bytes_received > 0)
    {
    	//hijo, consumidor
    	bytes_received = mq_receive(mqd, buffer, MAX_MESSAGE_SIZE, 0);
    	printf("---Consumidor: %s\n", buffer);
    	sleep(1);
    	memset(buffer, 0, sizeof(buffer));
    }
    mq_close(mqd);
  }else
  {
  	
	//padre, productor
	for (i = 1; i < argc; i++)
	{
		mq_send(mqd, argv[i], MAX_MESSAGE_SIZE, 0);
		printf("Productor: %s\n", argv[i]);
		sleep(1);
	}
	mq_unlink(name);
	mq_close(mqd);

  }
} 