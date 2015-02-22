#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>


static int glob = 0;// globally shared resource
int read_count = 0;// Keeps track of how threads are currently reading the object
static sem_t rw_mutex;// common to both reader and writer threads
static sem_t mutex;//Initial vals of 1
struct timespec tim, tim2;//Sleep variables

void static writer(void *args)
{
	int loc;

	sem_wait(&rw_mutex);
	glob+=10;
	sem_post(&rw_mutex);
	pthread_exit(NULL);
}

void static reader(void *args)
{
	int tries = *((int *) args);
	int j = 0;
	do
	{
		if(sem_wait(&mutex) == -1)
		{
			exit(2);
		}
		read_count++;
		if (read_count == 1)
		{
			if(sem_wait(&rw_mutex) == -1)
			{
					exit(2);
			}
		}
		if(sem_post(&mutex) == -1)
		{
			exit(2);
		}
		printf("\nValue of glob %i",glob);
		if(sem_wait(&mutex) == -1)
		{
			exit(2);
		}
		read_count --;
		if (read_count == 0)
		{
			sem_wait(&rw_mutex);
		}
		sem_post(&mutex);
		j++;
	}while(j < tries);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t t1, t2;
  int s;/*indicates if successful thread creation or not*/
  int loops;/* amount of loops for a thread*/
  int tries = 10; /*amount of tries to do*/

  if (sem_init(&rw_mutex, 1, 1) == -1) {
    printf("Error, init semaphore\n");
    exit(1);
  }

  if(sem_init(&mutex,1,1) == -1){
	    printf("Error, init semaphore\n");
	    exit(1);
	  }

  for(loops = 0; loops < 10;loops++)
  {
	  s = pthread_create(&t1, NULL, writer,&tries);
	  if (s != 0) {
		printf("Error, creating threads\n");
		exit(1);
	  }
  }
  for(loops = 0; loops < 100;loops++)
  {
	  s = pthread_create(&t2, NULL,reader,&tries);
	  if (s != 0) {
		printf("Error, creating threads\n");
		exit(1);
	  }
  }
  s = pthread_join(t1, NULL);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_join(t2, NULL);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  exit(0);
}

