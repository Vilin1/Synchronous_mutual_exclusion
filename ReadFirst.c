#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

sem_t readcount, wrt, mutex;

struct data{
	int tid;
	char rw;
	int sleepTime;
	int lastTime;
};

void *write(void* param) {
	struct data temp = *(struct data*)param;
	printf("Here comes a writer in Pthread %d.\n", temp.tid);
	sleep(temp.sleepTime);
	sem_wait(&wrt);
	printf("Pthread %d is writing.\n", temp.tid);
	sleep(temp.lastTime);
	printf("Pthread %d writes end.\n", temp.tid);
	sem_post(&wrt);
}

void *read(void* param) {
	struct data temp = *(struct data*)param;
	printf("Here comes a reader in Pthread %d.\n", temp.tid);
	sleep(temp.sleepTime);
	sem_wait(&mutex);
	//readcount++
	int tempreadcount;
	sem_getvalue(&readcount, &tempreadcount);
	tempreadcount++;
	sem_init(&readcount, 0, tempreadcount);
	sem_getvalue(&readcount, &tempreadcount);
	if(tempreadcount == 1) {
		sem_wait(&wrt);
	}
	sem_post(&mutex);
	printf("Pthread %d is reading\n", temp.tid);
	sleep(temp.lastTime);
	printf("Pthread %d reads end.\n", temp.tid);

	sem_wait(&mutex);
	//readcount--
	sem_getvalue(&readcount, &tempreadcount);
	tempreadcount--;
	sem_init(&readcount, 0, tempreadcount);
	sem_getvalue(&readcount, &tempreadcount);
	if(tempreadcount == 0) {
		sem_post(&wrt);
	}
	sem_post(&mutex);

}

int main() {
	int n;
	scanf("%d", &n);
	struct data data1[50];
	pthread_t tid[50];
	pthread_attr_t attr[50];
	//init
	sem_init(&mutex, 0, 1);
	sem_init(&readcount, 0, 0);
	sem_init(&wrt, 0, 1);

	for(int i = 0; i < n; i++) {
		pthread_attr_init(&attr[i]);
		scanf("%d", &(data1[i].tid));
		char c;
		scanf("%c", &c);
		scanf("%c", &(data1[i].rw));
		scanf("%d", &(data1[i].sleepTime));
		scanf("%d", &(data1[i].lastTime));
		if(data1[i].rw == 'R') {	
			pthread_create(&tid[i], &attr[i], read, &data1[i]);
		}
		if(data1[i].rw == 'W') {
			pthread_create(&tid[i], &attr[i], write, &data1[i]);
		}
	}
	for(int i = 0; i < n; i++) {
		pthread_join(tid[i], NULL);
	}
	sem_destroy(&mutex);
    sem_destroy(&readcount);
    sem_destroy(&wrt);
	return 0;
}