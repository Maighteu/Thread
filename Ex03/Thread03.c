#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <string.h>
#include <time.h>
#include <sched.h>
#define ThreadMax 5

typedef struct 
{
	char nom[30];
	int nbSecondes;
}DONNEE;

DONNEE data[]= 
{
	"MATAGNE",15,
	"WILVERS",10,
	"WAGNER",17,
	"QUETTIER",8,
	"",0
};
DONNEE Param;

void handlerSignal (int sig); 
void *FctThread(DONNEE* Param);
void fctFinThread(void *);

pthread_t threadHandle[ThreadMax];
struct sigaction sigAct;
pthread_mutex_t M;


int main ()
{
	sigset_t mask;
	//int *retThreads[ThreadMax];
	sigAct.sa_handler = handlerSignal; 
	sigAct.sa_flags = 0;
	sigemptyset(&sigAct.sa_mask);
	sigaction(SIGINT, &sigAct, NULL); 
	sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    for (int i = 0; i < ThreadMax; i++) 
	{
		pthread_mutex_lock(&M);
		memcpy(&Param,&data[i],sizeof(DONNEE)) ;
        pthread_create(&threadHandle[i], NULL, (void*(*)(void*)) FctThread, &Param);// ;

    }
 
    for (int i = 0; i < ThreadMax; i++) 
	{
    	pthread_join(threadHandle[i], (NULL));
    }
    //pause();

}
void *FctThread(DONNEE* Param)
{
	pthread_cleanup_push(fctFinThread, NULL);
	struct timespec temps; 
	temps.tv_sec = Param->nbSecondes;
	printf("Thread de: %s avec une attente de %d secondes\n", Param->nom, Param->nbSecondes);
	printf("TId du Thread: %u\nPId du Thread:%d \n\n",pthread_self(), getpid());
	pthread_mutex_unlock(&M);
	nanosleep(&temps, 0);

	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}
void handlerSignal (int sig)
{
	printf("Id du Thread reçu: %u\n",pthread_self());
}

void fctFinThread(void *)
{
    printf("fctFinThread : Tid du thread reçu: %ld\n", pthread_self());
}