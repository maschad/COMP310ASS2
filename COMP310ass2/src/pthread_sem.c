#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>


static int glob = 0;
static sem_t sem,r,w;//Initial vals of 1
struct timespec tim, tim2;//Sleep variables

static void *readFunc(void *arg)
{
	int loc, j,z;
	tim.tv_sec = 1;
	tim.tv_nsec = 500;

	for (j = 0; j < 100; j++)
	{

		if (sem_wait(&sem) == -1)
			exit(2);
		glob = loc;
		z = nanosleep(&tim,&tim2);
		printf("\n Value at shared %i",glob);
		if (sem_post(&sem) == -1)
			exit(2);
	}
	  return NULL;
}

static void *writerFunc(void *arg)
{
	  int loc, j;

	  for (j = 0; j < 10; j++)
	  {
	     if (sem_wait(&sem) == -1)
	       exit(2);

	    loc = glob;
	    loc+=10;
	    glob = loc;
		if (sem_post(&sem) == -1)
			exit(2);
	  }
	  return NULL;
}

int main(int argc, char *argv[]) {

  pthread_t t1, t2;
  int s;/*indicates if successful thread creation or not*/


  if (sem_init(&sem, 0, 1) == -1) {
    printf("Error, init semaphore\n");
    exit(1);
  }

  s = pthread_create(&t1, NULL, writerFunc,0);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_create(&t2, NULL, readFunc,0);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
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

