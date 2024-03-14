#include "./presentation/presentation.h"
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <string.h>
#include <time.h>
#include <sched.h>

void *fctThreadFenetreGraphique(void *);
void *fctThreadEvenements(void *);
void *fctThreadStanley(void *);
void *fctThreadEnnemis(void *);
void *fctThreadGuepe(void *);
void *fctThreadChenilleG(void *);
void *fctThreadChenilleD(void *);
void *fctThreadAraigneeG(void *);
void *fctThreadAraigneeD(void *);
void *fctThreadInsecticideG(void *);
void *fctThreadInsecticideD(void *);
void fctFinThread(void *);

void handlerSIGINT(int);
void handlerSIGALRM(int);
void handlerSIGUSR1(int);
void handlerSIGUSR2(int);
void handlerSIGQUIT(int); 

void destructeurVS(void *p);

pthread_cond_t condEvenement;
pthread_cond_t condEchec;

pthread_key_t keySpec;

pthread_mutex_t mutexEtatJeu;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexEchec;

pthread_t ThreadHandler;

struct sigaction sigAct;
typedef struct
{
    int presence;
    pthread_t tid;
} S_PRESENCE; 



typedef struct
{
    int etatStanley;
    int positionStanley;
    int actionStanley;
    int etatAmis[5];
    S_PRESENCE guepes[2];
    S_PRESENCE chenillesG[5];
    S_PRESENCE chenillesD[7];
    S_PRESENCE araigneesG[5];
    S_PRESENCE araigneesD[5];
    S_PRESENCE insecticidesG[4];
    S_PRESENCE insecticidesD[4];
    int score;
    int nbEchecs;
} S_ETAT_JEU;

S_ETAT_JEU etatJeu = 
     { BAS, 1, NORMAL,
       { NORMAL, NORMAL, NORMAL, NORMAL, NORMAL },
       { { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, 
         { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       0, 0 };

int evenement = AUCUN; 
int echec = AUCUN;

int main(int argc, char* argv[])
{
    int i;

    ouvrirFenetreGraphique();

    sigset_t mask;
    sigAct.sa_handler = handlerSIGINT;
    sigAct.sa_flags = 0;
    sigemptyset(&sigAct.sa_mask);
    sigaction(SIGINT, &sigAct, NULL); 
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    printf("Id du Thread main: %u\n",pthread_self());
    pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadFenetreGraphique, NULL);

    pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadEvenements, NULL);
   /* while(1)
    {
        evenement = lireEvenement();

        switch(evenement)
        {
            case SDL_QUIT:
                exit(0);

            case SDLK_UP:
                printf("KEY_UP\n");
                break;

            case SDLK_DOWN:
                printf("KEY_DOWN\n");
                break;

            case SDLK_LEFT:
                printf("KEY_LEFT\n");
                break;

            case SDLK_RIGHT:
                printf("KEY_RIGHT\n");
                break;

            case SDLK_SPACE:
                printf("SDLK_SPACE\n");
        }
    }*/
    pause();
}

void* fctThreadFenetreGraphique(void*)
{
    int i;
    pthread_cleanup_push(fctFinThread, NULL);
    printf("Id du Thread FenetreGraphique: %u\n",pthread_self());
    while(true)
    {
        restaurerImageInterne();
        pthread_mutex_lock(&mutexEtatJeu);// debut utilisation variable etat jeu
        afficherScore(etatJeu.score);
        afficherEchecs(etatJeu.nbEchecs);
        afficherStanley(etatJeu.etatStanley, etatJeu.positionStanley, etatJeu.actionStanley);

        for(i = 0; i < 5; i++)
         {
         afficherAmi(i, etatJeu.etatAmis[i]);
         }
        pthread_mutex_unlock(&mutexEtatJeu);//fin utilisation variable etat jeu
        actualiserFenetreGraphique();
    }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
void *fctThreadEvenements(void *)
{
    printf("Id du Thread Evenement: %u\n",pthread_self());
    pthread_cleanup_push(fctFinThread, NULL);
    while(1)
     {
       
        pthread_mutex_lock(&mutexEvenement);
         evenement = lireEvenement();
         switch(evenement)
        {
            case SDL_QUIT:
             exit(0);

             case SDLK_UP:
             printf("KEY_UP\n");
             break;

             case SDLK_DOWN:
             printf("KEY_DOWN\n");
             break;

             case SDLK_LEFT:
             printf("KEY_LEFT\n");
             break;

             case SDLK_RIGHT:
             printf("KEY_RIGHT\n");
             break;

             case SDLK_SPACE:
             printf("SDLK_SPACE\n");
        }
        pthread_mutex_unlock(&mutexEvenement);
    }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
void handlerSIGINT(int)
{
    printf("Id du Thread reçu: %u\n",pthread_self());
}

void fctFinThread(void *)
{
    printf("fctFinThread : Tid du thread Terminé: %ld\n", pthread_self());
}
