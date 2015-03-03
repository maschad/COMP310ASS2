#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>


static int glob = 0;// globally shared resource
int read_count = 0;// Keeps track of how threads are currently reading the object
static sem_t rw_mutex;// common to both reader and writer threads
static sem_t mutex;//Initial vals of 1
static useconds_t usec; /* amount of time to wait at minimum, altered by random*/
struct timespec *waitTime;/*structure for measuring wait times of threads*/

void static writer(void *args)
{
	int tries = *((int *) args);
	int j;

	srand(time(NULL));/*time editing alterations for random sleep*/
	usec = rand()%100;/*used for random # generation*/
	usec = usec *1000;
	for(j = 0; j < tries; j++)
	{
		srand(time(NULL));/*time editing alterations for random sleep*/
		usec = rand()%100;
		usec = usec *10000;

		sem_trywait(&rw_mutex);/* instead of blocking if thread cannot gain access then returns error to EAGAIN*/
		glob+=10;
		sem_post(&rw_mutex);
		usleep(usec);
		pthread_exit((void*)waitTime);/*return the waiting times*/
	}
}

void static reader(void *args)
{
	int tries = *((int *) args);
	int j = 0;
	srand(time(NULL));/*time editing alterations for random sleep*/
	usec = rand()%100;
	usec = usec *10000;

	do
	{
		if(sem_trywait(&mutex) == -1)
		{
			exit(2);
		}
		read_count++;
		if (read_count == 1)
		{
			if(sem_trywait(&rw_mutex) == -1)
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
		usleep(usec);
		j++;
	}while(j < tries);
	pthread_exit((void*)waitTime);/*return the waiting times*/
}

int main(int argc, char *argv[]) {
  pthread_t t1, t2;
  int s;/*indicates if successful thread creation or not*/
  int loops;/* amount of loops for a thread*/
  int tries = 10; /*amount of tries to do*/
  long maxR = 0 , minR = 0;/*values to store the max, min and average wait times of readers*/
  long maxW = 0, minW = 0; /*values to stores the max, min and average wait times of writers*/

  if (sem_init(&rw_mutex, 0, 1) == -1) { /*initialize to a value of 1, with 0 indicated to shared between threads of processes */
    printf("Error, init semaphore\n");
    exit(1);
  }

  if(sem_init(&mutex,0,1) == -1){
	    printf("Error, init semaphore\n");
	    exit(1);
	  }

  for(loops = 0; loops < 10;loops++)
  {
	  clock_gettime(CLOCK_MONOTONIC,waitTime);/*function to get intial time at start of loop*/
	  s = pthread_create(&t1, NULL, writer,&tries);
	  waitTime->tv_nsec+= waitTime->tv_nsec;/*incrementing wait times*/
	  if (s != 0) {
		printf("Error, creating threads\n");
		exit(1);
	  }
  }
  for(loops = 0; loops < 100;loops++)
  {
	  clock_gettime(CLOCK_MONOTONIC,waitTime);/*function to get intial time at start of loop*/
	  s = pthread_create(&t2, NULL,reader,&tries);
	  waitTime->tv_nsec+= waitTime->tv_nsec;/*incrementing wait times*/
	  if (s != 0) {
		printf("Error, creating threads\n");
		exit(1);
	  }
  }
  s = pthread_join(t1,waitTime);
  if(waitTime->tv_nsec > maxW)/*checking conditions for max & min*/
	  maxW = waitTime->tv_nsec;
  if(waitTime->tv_nsec < minW)
	  minW = waitTime->tv_nsec;
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_join(t2,waitTime);
  if(waitTime->tv_nsec > maxR)/*checking conditions for max & min*/
  	  maxR = waitTime->tv_nsec;
    if(waitTime->tv_nsec < minR)
  	  minR = waitTime->tv_nsec;
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }
  printf("\nMax reader time %lu",maxR);
  printf("\nMax writer time %lu",maxW);
  printf("\nMin reader time %lu",minR);
  printf("\nMin writer time %lu",minW);
  printf("\nAverage reader time %d",(maxR+minR/2));
  printf("\nAverage writer time %d",(maxW+minW/2));
  exit(0);
}

