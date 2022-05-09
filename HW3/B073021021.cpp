#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int counter = 0;

class Monitor {
public:
    Monitor() {
		pthread_mutex_init(&mutex, NULL);
		for (int i=0;i<N;i++) {
			pthread_cond_init(&s[i], NULL);
			state[i] = THINKING;
		}
    }

    void take_forks(int i) {
		down(&mutex);
		state[i] = HUNGRY;
		test(i);
        if (state[i] != EATING)
		    pthread_cond_wait(&s[i], &mutex);
		up(&mutex);
	}

	void put_forks(int i) {
		down(&mutex);
		state[i] = THINKING;
		test(LEFT);
		test(RIGHT);
		up(&mutex);
	}

	void test(int i) {
		if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
			state[i] = EATING;
			pthread_cond_signal(&s[i]);
		}
	}
	void think() {
		sleep(1);
	}
	void eat() {
		sleep(1);
	}
	void show() {
		down(&mutex);
		printf("\n---------- \x1b[;33;1;1m%d\x1b[0m ----------\n", counter);
		for(int i=0;i<N;i++) {
			if(state[i] == THINKING)
				printf("Philosopher %d: THINKING\n", i+1);
			else if(state[i] == HUNGRY)
				printf("Philosopher %d: HUNGRY\n", i+1);
			else
				printf("Philosopher %d: EATING (Yum-Yum)\n", i+1);
		}
		up(&mutex);
	}

private:
    int state[N];
	pthread_mutex_t mutex;
    pthread_cond_t s[N];
	void down(pthread_mutex_t *mutex) {
		pthread_mutex_lock(mutex);
	}

	void up(pthread_mutex_t *mutex) {
		pthread_mutex_unlock(mutex);
	}
} dp;

void* philosopher(void* i)  {
	int num = *( (int*)i );
	while (true) {
		dp.think();        
		dp.take_forks(num);
		dp.eat();           
		dp.put_forks(num);  
	}
	return NULL;
}


int main() {
	pthread_t p[N];
	int id[N];
	for (int i=0;i<N;i++) {
		id[i] = i;
		pthread_create(&p[i], NULL, &philosopher, &id[i]);
	}
	
	while (true) {
		dp.show();
		sleep(1);
		counter++;
	}

	for (int i=0;i<N;i++) {
		pthread_join(p[i], NULL);
	}

    return 0;
}