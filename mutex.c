#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

//with max length 10
#define BUFFERSIZE 10
int buffer[BUFFERSIZE+1];
int front = 0, rear = 0;
sem_t mutex, empty, full;

struct data{
	int pid;
	char cp;
	int sleepTime;
	int lastTime;
	int produceOrRemoveItemNum;
};
//insert an item
int insertItem(int itemNum) {
	if((rear+1)%(BUFFERSIZE+1) == front) {
		return -1;
	}
	buffer[rear] = itemNum;
	rear = (rear+1)%(BUFFERSIZE+1);
	return 0;
}

//remove an item
int removeItem(int* itemNum) {
	if(front == rear) {
		return -1;
	}
	*itemNum = buffer[front];
	front = (front+1)%(BUFFERSIZE+1);
	return 0;
}

//produce an item
void *producer(void* param) {
	struct data temp = *(struct data*)param;
	printf("%d producer comes.\n", temp.pid);
	sleep(temp.sleepTime);
	sem_wait(&empty);
	sem_wait(&mutex);
	if(insertItem(temp.produceOrRemoveItemNum) != 0) {
		printf("Insert error");
	} else {
		printf("producer uses pthread %d to produce test %d\n", temp.pid, temp.produceOrRemoveItemNum);
	}
	sleep(temp.lastTime);
	printf("%d producer finish.\n", temp.pid);
	sem_post(&mutex);
	sem_post(&full);
}

//consumer an item
void *consumer(void* param) {
	struct data temp = *(struct data*)param;
	printf("%d consumer comes.\n", temp.pid);
	sleep(temp.sleepTime);
	sem_wait(&full);
	sem_wait(&mutex);
	if(removeItem(&temp.produceOrRemoveItemNum) != 0) {
		printf("remove error");
	} else {
		printf("consumer uses pthread %d to consume test %d\n", temp.pid,temp.produceOrRemoveItemNum);
	}
	sleep(temp.lastTime);
	printf("%d consumer finish.\n", temp.pid);
	sem_post(&mutex);
	sem_post(&empty);
}

int main() {
	int n;
	scanf("%d", &n);
	//printf("%d\n", n);
	struct data data1[10];
	pthread_t tid[10];
	pthread_attr_t attr[10];

	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, BUFFERSIZE);
	sem_init(&full, 0, 0);

	for(int i = 0; i < n; i++) {
		pthread_attr_init(&attr[i]);
		scanf("%d", &(data1[i].pid));
		//printf("%d\n", data1[i].pid);
		//to get a char before a true char 
		char c;
		scanf("%c", &c);
		scanf("%c", &(data1[i].cp));
		if(data1[i].cp == 'P') {
			scanf("%d", &(data1[i].sleepTime));
			scanf("%d", &(data1[i].lastTime));
			scanf("%d", &(data1[i].produceOrRemoveItemNum));
			pthread_create(&tid[i], &attr[i], producer, &data1[i]);
		}
		if(data1[i].cp == 'C'){
			scanf("%d", &(data1[i].sleepTime));
			//printf("%d", (data1[i].sleepTime));
			scanf("%d", &(data1[i].lastTime));
			//printf("%d", (data1[i].lastTime));
			pthread_create(&tid[i], &attr[i], consumer, &data1[i]);
		}
	}
	//wiit all pthread to finish
	for(int i = 0; i < n; i++) {
		pthread_join(tid[i], NULL);
	}
	//printf("jjjj\n");

	sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
	return 0;
}