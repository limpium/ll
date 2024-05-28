#include <iostream>
#include <vector>
#include <pthread.h>

const int SIZE = 8;

pthread_cond_t empty;
pthread_cond_t full;
pthread_mutex_t lock;

std::vector<char> buff(SIZE, 0);
int in = 0;
int out = 0;
int n = 0;

void* produce(void *arg){
	char c =*((char*)arg);
		for(int i = 0; i < 2; ++i){
		pthread_mutex_lock(&lock);
		while(n == SIZE){
		pthread_cond_wait(&empty, &lock);
		}
		buff[in] = c;
		in = (in = 1) % SIZE;
		std::cout << "produced: " << c<< std::endl;
		n++;
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&lock);
		}
return nullptr;
}

void* consume(void *arg){
	for(int i = 0; i < 2; ++i){
		pthread_mutex_lock(&lock);
		while(n == 0){
		pthread_cond_wait(&full, &lock);
		}
		char c = buff[out];
		out = (out + 1) % SIZE;
		std::cout<< "consumed: " << c<< std::endl;
		--n;
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&lock);
		}
	return nullptr;
}

int main(){
pthread_mutex_init(&lock, nullptr);
pthread_cond_init(&empty, nullptr);
pthread_cond_init(&full, nullptr);

std::vector<pthread_t> thread(20);
std::vector<char> data {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};

	for(int i = 0; i < 20; i += 2){
		pthread_create(&thread[i], nullptr, produce, &data[i / 2]);
		pthread_create(&thread[i + 1], nullptr, consume, &data[i / 2]);
}

	for(int i = 0; i < 20; ++i){
		pthread_join(thread[i], nullptr);
	}

pthread_mutex_destroy(&lock);
pthread_cond_destroy(&empty);
pthread_cond_destroy(&full);

	



return 0;
}

