#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
////////////////////////
#include <unistd.h>
////////////////////////
#include <signal.h>
////////////////////////
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#define NAME_QUEUE "/prodcons"
#define MAX_MESSAGE_SIZE 25

mqd_t my_queue;

void consumidor()
{
	char buffer[25];
	int nBytes;

	while(1)
	{
		nBytes = mq_receive(my_queue, buffer, sizeof(buffer), 0);

		if (nBytes == -1)
		{
			
			fprintf (stderr, "ERROR: mq_send(): %s\n", strerror(errno));

			if (mq_close(my_queue) == -1)
			{
			  	fprintf (stderr, "ERROR: mq_close(): %s\n", strerror(errno));
			  	exit(EXIT_FAILURE);
			}      

			fprintf (stdout, "mq_close(): Queue [%s] closed\n", NAME_QUEUE);

		}

		fprintf (stdout, "mq_receive(): Message '%s' received\n", buffer);

		sleep(1);
	}

}

void productor(char * palabras[], int length)
{
	int i;

	for (i=0; i<length; i++)
   	{
   		if (mq_send(my_queue, palabras[i], strlen(palabras[i])+1, 0) == -1)
   		{

    		fprintf (stderr, "ERROR: mq_send(): %s\n", strerror(errno));
    	
    		if (mq_close(my_queue) == -1)
			{
			  	fprintf (stderr, "ERROR: mq_close(): %s\n", strerror(errno));
			  	exit(EXIT_FAILURE);
			}      
		
			fprintf (stdout, "mq_close(): Queue [%s] closed\n", NAME_QUEUE);
   		
   		}	

   		fprintf (stdout, "mq_send(): Message '%s' sent\n", palabras[i]);

		sleep(1);
   	}


}

int main (int argc, char * argv[])
{
	struct mq_attr atributos;
	atributos.mq_flags = 0;
	atributos.mq_maxmsg = 5;
	atributos.mq_msgsize = MAX_MESSAGE_SIZE;
	atributos.mq_curmsgs = 0;

	my_queue = mq_open(NAME_QUEUE, O_RDWR | O_CREAT, 0600, &atributos);
	if (my_queue == -1)
	{
	  fprintf (stderr, "ERROR: mq_open(): %s\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}
   	fprintf (stdout, "mq_open(): Queue [%s : %d] opened\n", NAME_QUEUE, my_queue);

   	int i;
   	char * palabras[argc-1];
   	for (i=0; i<argc-1; i++)
   	{
   		palabras[i] = argv[i+1];
   		fprintf (stdout, "%s\n", palabras[i]);
   	}

	pid_t pid = fork();
	if (pid == -1)
	{
		fprintf (stderr, "ERROR: fork(): %s\n", strerror(errno));
	}
	else if (pid == 0) //Hijo
	{
		sleep(1);
		consumidor();
	}
	else //Padre
	{
		productor(palabras, argc-1);
	}

	return 1;
	
}