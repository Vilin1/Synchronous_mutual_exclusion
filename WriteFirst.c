#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

int read_count = 0;
int write_count = 0;
static pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
sem_t mutex,entermutex,quemutex;

struct data{
	int tid;
	char rw;
	int sleepTime;
	int lastTime;
};

void *read(void* param) {
	struct data temp = *(struct data*)param;
	sleep(temp.sleepTime);
	printf("Here comes a reader %d.\n", temp.tid);
	sem_wait(&entermutex);//many reader wait before here
	sem_wait(&quemutex);//only one can in
	pthread_mutex_lock(&mutex2);
	if(read_count == 0) {
		sem_wait(&mutex);
	}
	read_count++;
	pthread_mutex_unlock(&mutex2);
	sem_post(&quemutex);
	sem_post(&entermutex);

	printf("Pthread %d is reading.\n", temp.tid);
	sleep(temp.lastTime);
	printf("Pthread %d reads end.\n", temp.tid);

	pthread_mutex_lock(&mutex2);
	read_count--;
	if(read_count == 0) {
		sem_post(&mutex);
	}
	pthread_mutex_unlock(&mutex2);
}

void *write(void* param) {
	struct data temp = *(struct data*)param;
	sleep(temp.sleepTime);
	printf("Here comes a writer %d.\n", temp.tid);
	pthread_mutex_lock(&mutex3);
	if(write_count == 0) {
		sem_wait(&quemutex);
	}
	write_count++;
	pthread_mutex_unlock(&mutex3);

	sem_wait(&mutex);
	printf("Pthread %d is writing.\n", temp.tid);
	sleep(temp.lastTime);
	printf("Pthread %d writes end.\n", temp.tid);
	sem_post(&mutex);

	pthread_mutex_lock(&mutex3);
	write_count--;
	if(write_count == 0) {
		sem_post(&quemutex);
	}
	pthread_mutex_unlock(&mutex3);
}

int main() {
	int n;
	scanf("%d", &n);
	struct data data1[50];
	pthread_t tid[50];
	pthread_attr_t attr[50];
	//init
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_init(&mutex3, NULL);
	sem_init(&mutex,0,1);
	sem_init(&entermutex,0,1);
	sem_init(&quemutex,0,1);
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
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
	sem_destroy(&mutex);
	sem_destroy(&entermutex);
	sem_destroy(&quemutex);
	return 0;
}