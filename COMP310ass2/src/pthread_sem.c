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

void static writer(pthread_t thread)
{
	int loc;

	wait(rw_mutex);
	loc = glob;
	loc+= 10;
	glob = loc;
	signal(rw_mutex);
}

void static read(pthread_t thread)
{
	wait(mutex);
	read_count++;
	if (read_count == 1)
	wait(rw_mutex);
	signal(mutex);
	printf("\nValue of glob %i",glob);
	wait(mutex);
	read_count --;
	if (read_count == 0)
		signal(rw_mutex);
	signal(mutex);
}

int main(int argc, char *argv[]) {

  pthread_t t1, t2;
  int s;/*indicates if successful thread creation or not*/
  int loops;


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
	  s = pthread_create(&t1, NULL, writer,0);
	  if (s != 0) {
		printf("Error, creating threads\n");
		exit(1);
	  }
  }
  for(loops = 0; loops < 100;loops++)
  {
	  s = pthread_create(&t2, NULL,read,0);
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

