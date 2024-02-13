#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#define ThreadMax 4

void handlerSignal (int sig); 
void handlerSIGUSR1 (int sig);
void *FctThread(void *);
void *FctThreadMaster(void *);
void fctFinThread(void *);

pthread_t threadHandle[ThreadMax], threadMasterHandle;
struct sigaction sigAct, sigusr;

int main ()
{
	sigset_t mask;
	//int *retThreads[ThreadMax];
	sigAct.sa_handler = handlerSignal; 
	sigAct.sa_flags = 0;
	sigemptyset(&sigAct.sa_mask);
	sigaction(SIGINT, &sigAct, NULL); 

	sigusr.sa_handler = handlerSIGUSR1;
	sigusr.sa_flags = 0;
	sigemptyset(&sigusr.sa_mask);
	sigaction(SIGUSR1, &sigusr, NULL);
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);

	 for (int i = 0; i < ThreadMax; i++) 
	{
		//création threads Slave
        pthread_create(&threadHandle[i], NULL, (void*(*)(void*)) FctThread, NULL);// ajout;
    }
    pthread_create(&threadMasterHandle, NULL, (void*(*)(void*)) FctThreadMaster, NULL);
     /*for (int i = 0; i < ThreadMax; i++) 
    {
    	//retour threads Slave
    	pthread_join(threadHandle[i], (void **)&retThreads[i]);
    }*/
    printf("Id du Thread principal: %u\n",pthread_self());
    printf("Attente d'un signal\n \n");

 	for (int i = 0; i < ThreadMax; i++) 
    {
    	//retour threads Slave
    	pthread_join(threadHandle[i], NULL);
    }
    pthread_cancel(threadMasterHandle);
	pthread_exit(NULL);
}
void *FctThread(void *)
{
	sigset_t mask;
	sigemptyset(&mask); 
 	sigaddset(&mask, SIGUSR1);
 	sigprocmask(SIG_UNBLOCK, &mask, NULL); 

	printf("Id du Thread Créé: %u \n",pthread_self());
	printf("Attente d'un signal\n");
	printf("masquage du signal\n \n");
	pause();
	pthread_exit(NULL);

}
void *FctThreadMaster(void *)
{
	pthread_cleanup_push(fctFinThread, NULL);
    int ancEtat;
	sigset_t mask;
	
	sigemptyset(&mask); 
 	sigaddset(&mask, SIGINT);
 	sigprocmask(SIG_UNBLOCK, &mask, NULL); 
 	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &ancEtat);
    printf("ID thread master: %ld \n", pthread_self());
     while(1)
    {
        pause();
    }
    pthread_cleanup_pop(1);
    pthread_exit(0);
}
void handlerSignal (int sig)
{
	printf("Id du Thread reçu: %u\n",pthread_self());
	 kill(getpid(), SIGUSR1);
}
void handlerSIGUSR1 (int sig)
{
	printf("Id du Thread reçu: %u\n",pthread_self());
}
void fctFinThread(void *)
{
    printf("fctFinThread : id du thread %ld\n", pthread_self());
}