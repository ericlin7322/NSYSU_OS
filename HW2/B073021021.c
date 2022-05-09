#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>  
#define TRUE 1
#define FALSE 0
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2
typedef pthread_mutex_t semaphore;
int state[N];
semaphore mutex;
semaphore s[N];

void think() {
	sleep(1);
}

void eat() {
	sleep(1);
}

void down(semaphore *mutex) {
	pthread_mutex_lock(mutex);
}

void up(semaphore *mutex) {
	pthread_mutex_unlock(mutex);
}

void test(int i) {
	if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
		state[i] = EATING;
		up(&s[i]);
	}
}

void take_forks(int i) {
	down(&mutex);
	state[i] = HUNGRY;
	test(i);
	up(&mutex);
	down(&s[i]);
}

void put_forks(int i) {
	down(&mutex);
	state[i] = THINKING;
	test(LEFT);
	test(RIGHT);
	up(&mutex);
}

void *philosopher(void *num) {
	int i = *( (int*)num);
	while (TRUE) {
		think();
		take_forks(i);
		eat();
		put_forks(i);
	}
	printf("\nphilosopher %d STOP!",i);
}

int main() {
	int *phi_num[N], i, counter=1;
	pthread_t phi[N] = {0,1,2,3,4};
	
	for(i=0;i<N;i++) {
		phi_num[i] = malloc(sizeof(int));
		*phi_num[i] = i;
		down(&s[i]);
		pthread_create(&phi[i], NULL, philosopher, (void*)phi_num[i]);
	}
	
	while(TRUE) {
		down(&mutex);
		printf("\n---------- \x1b[;33;1;1m%d\x1b[0m ----------\n", counter);
		for(i=0;i<N;i++) {
			if(state[i] == THINKING)
				printf("Philosopher %d: THINKING\n", i+1);
			else if(state[i] == HUNGRY)
				printf("Philosopher %d: HUNGRY\n", i+1);
			else
				printf("Philosopher %d: EATING (Yum-Yum)\n", i+1);
		}
		up(&mutex);
		
		sleep(1);
		counter++;
}

	return 0;
}
