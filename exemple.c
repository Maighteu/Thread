#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#define ThreadMax 4

pthread_cond_t C;
int cpt;
pthread_mutex_t M;

int main ()
{
	cpt = 5;
	pthread_mutex_lock(&M);
	while (cpt!=0);
	pthread_cond_wait(&C,&M);
	pthread_mutex_unlock(&M);
}

void* Fonction(&M)(void* p)
{
	pthread_mutex_lock(&M);
	cpt--;
	pthread_mutex_unlock(&M);
	pthread_cond_signal(&C);
}