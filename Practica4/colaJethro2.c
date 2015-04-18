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
#define NUMERO_HEBRAS 2

mqd_t mqd;
char * name ="/Cola";
void * h_productora(void * palabras);
void * h_consumidora ();
int size = 0;

int main(int argc, char *argv[])
{
	int flags;
	//mode_t perms; tercer argumento mq_open
	struct mq_attr attr;
	pthread_t h[NUMERO_HEBRAS];
	int h1, ret, i;
	printf("PALABRAS = { ");
	for (i = 1; i < argc-1; i++)
	{
	printf("%s, ", argv[i]);
	}
	printf("%s }\n", argv[argc-1]);
	size = argc;

	attr.mq_maxmsg = MAX_QUEUE_SIZE;
	attr.mq_msgsize = MAX_MESSAGE_SIZE;
	flags = O_RDWR | O_CREAT;

	mqd = mq_open(name, flags, 0777, &attr);

	if ((h1 = pthread_create(&(h[0]), NULL, h_productora, argv)))
	{
		errno = h1;
		perror("pthread_create");
		exit(-1);
	}
	if ((h1 = pthread_create(&(h[1]), NULL, h_consumidora, NULL)))
	{
		errno = h1;
		perror("pthread_create");
		exit(-1);
	}
	for (i = 0; i < NUMERO_HEBRAS; i++)
	{
		ret = pthread_join(h[i], NULL);
		if (ret >0)
			{
				errno = ret;
				perror("pthread_join");
				exit(-1);
			}
	}
	mq_unlink(name);
	mq_close(mqd);

	return 0;
}

void * h_productora (void * palabras)
{
 printf("Productora en curso\n");
 char ** arguments = (char **) palabras;
 int i;
 for (i = 1; i < size; i++)
 {
 	mq_send(mqd, arguments[i], MAX_MESSAGE_SIZE, 0);
 	printf("Productor: %s\n", arguments[i]);  sleep(1);
 }
 mq_close(mqd);
 exit(1);
}
void * h_consumidora ()
{
	char * buffer = (char *)malloc(MAX_MESSAGE_SIZE);
	int bytes_received = 1;
	while (bytes_received > 0)
	{  //hijo, consumidor
		bytes_received = mq_receive(mqd, buffer, MAX_MESSAGE_SIZE, 0);
		printf("—Consumidor: %s\n", buffer);
		sleep(1);
		memset(buffer, 0, sizeof(buffer));
	}
	mq_close(mqd);
	exit(1);
}
