#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>


static int glob = 0;
static sem_t sem,r,w;//inital vals of 1


static void *threadFunc(void *arg) {

  int loops = *((int *) arg);
  int loc, j;

  for (j = 0; j < loops; j++) {

     if (sem_wait(&sem) == -1)
       exit(2);

    loc = glob;
    loc++;
    glob = loc;

        if (sem_post(&sem) == -1)
         exit(2);
  }
  return NULL;
}



int main(int argc, char *argv[]) {

  pthread_t t1, t2, t3, t4;
  int s;
  int loops = 100000;

  if (sem_init(&sem, 0, 1) == -1) {
    printf("Error, init semaphore\n");
    exit(1);
  }

  s = pthread_create(&t1, NULL, threadFunc, &loops);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_create(&t2, NULL, threadFunc, &loops);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_create(&t3, NULL, threadFunc, &loops);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_create(&t4, NULL, threadFunc, &loops);
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

  s = pthread_join(t3, NULL);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  s = pthread_join(t4, NULL);
  if (s != 0) {
    printf("Error, creating threads\n");
    exit(1);
  }

  printf("glob value %d \n", glob);
  exit(0);
}


void reader()/*Reader*/
{
	wait(r);
	wait(sem);
	glob++;
	if (glob == 1)
	  wait(w);
	signal(sem);
	signal(r);

        // reading is performed

	wait(sem);
	glob--;
	if (glob == 0)
		signal(w);
	signal(sem);
}


void writer()//WRITER
{

    wait(r);
    wait(w);

     // writing is performed

    signal(w);
    signal(r);
}
