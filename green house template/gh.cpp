#include "./presentation/presentation.h"
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <string.h>
#include <time.h>
#include <sched.h>
#include<math.h>



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
pthread_key_t keyGuepe;


pthread_mutex_t mutexEtatJeu;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexEchec;

pthread_t ThreadHandler;


struct timespec AttenteEnnemi;

typedef struct
{
    int presence;
    pthread_t tid;
} S_PRESENCE; 
struct timespec temps; 


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
    pthread_mutex_init (&mutexEvenement,NULL);
    pthread_mutex_init (&mutexEtatJeu,NULL);
    pthread_mutex_init (&mutexEchec,NULL);

    pthread_cond_init(&condEvenement,NULL);
    pthread_cond_init (&condEchec,NULL);

    pthread_key_create(&keySpec, destructeurVS);

    ouvrirFenetreGraphique();
   
    struct sigaction sigAct;
    sigAct.sa_flags = 0;
    sigemptyset(&sigAct.sa_mask);

    sigAct.sa_handler = handlerSIGALRM;
    sigaction(SIGALRM, &sigAct, NULL); 

    sigAct.sa_handler = handlerSIGINT;
    sigaction(SIGINT, &sigAct, NULL); 

    sigAct.sa_handler = handlerSIGUSR1;
    sigaction(SIGUSR1, &sigAct, NULL); 

    sigAct.sa_handler = handlerSIGUSR2;
    sigaction(SIGUSR2, &sigAct, NULL); 

    sigAct.sa_handler = handlerSIGQUIT ;
    sigaction(SIGQUIT, &sigAct, NULL); 

    sigaddset(&sigAct.sa_mask,SIGALRM);
    sigaddset(&sigAct.sa_mask,SIGINT);
    sigaddset(&sigAct.sa_mask,SIGUSR1);
    sigaddset(&sigAct.sa_mask,SIGUSR2);
    sigaddset(&sigAct.sa_mask,SIGQUIT);

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask,NULL);

    printf("Id du Thread main: %u\n",pthread_self());
    pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadFenetreGraphique, NULL);

    pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadEvenements, NULL);

    pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadStanley, NULL);
    pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadEnnemis, NULL);
             while (etatJeu.nbEchecs<3)
        {
             pthread_cond_wait(&condEchec,&mutexEchec);
             //etatJeu.nbEchecs ++;
             pthread_mutex_unlock(&mutexEchec);
        }


    pthread_join(ThreadHandler, NULL);

}

void* fctThreadFenetreGraphique(void*)
{
        sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask,NULL);
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

         for(i = 0; i < 2; i++)
         {
            if ( etatJeu.guepes[i].presence == NORMAL) afficherGuepe(i);
         }

         for(i = 0; i < 6; i++)
         {
            if ( etatJeu.araigneesG[i].presence == NORMAL) afficherGuepe(i);
         }

         for(i = 0; i < 6; i++)
         {
            if ( etatJeu.araigneesD[i].presence == NORMAL) afficherGuepe(i);
         }

         for(i = 0; i < 6; i++)
         {
            if ( etatJeu.chenillesG[i].presence == NORMAL) afficherGuepe(i);
         }

         for(i = 0; i < 8; i++)
         {
            if ( etatJeu.chenillesD[i].presence == NORMAL) afficherGuepe(i);
         }
        
         pthread_mutex_unlock(&mutexEtatJeu);//fin utilisation variable etat jeu

  

        
        actualiserFenetreGraphique();
    }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
void *fctThreadEvenements(void *)
{
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask,NULL);
    printf("Id du Thread Evenement: %u\n",pthread_self());
    pthread_cleanup_push(fctFinThread, NULL);
    while(1)
     {
      

        pthread_mutex_lock(&mutexEvenement);
        evenement = lireEvenement(); 
        if (evenement == SDL_QUIT) exit(0);
        temps.tv_sec =0;
        temps.tv_nsec = 200000000;
        pthread_mutex_unlock(&mutexEvenement);
        pthread_cond_signal(&condEvenement);
        nanosleep(&temps, NULL);

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.actionStanley = NORMAL;
        pthread_mutex_unlock(&mutexEtatJeu);



    }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}

void *fctThreadStanley(void*)
{
    struct timespec attentemort;
    attentemort.tv_nsec= 200000000;
    int i;
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask,NULL);
    printf("Id du Thread etatStanley: %u\n",pthread_self());
    pthread_cleanup_push(fctFinThread, NULL);
    while(true)
    {
        pthread_mutex_lock(&mutexEvenement);
        pthread_cond_wait(&condEvenement,&mutexEvenement);
        pthread_mutex_lock(&mutexEtatJeu);
        switch(etatJeu.etatStanley)
        {
            
            case BAS:
                switch(evenement)
                {
                    case SDLK_SPACE:
                    if ((etatJeu.positionStanley == 0) || (etatJeu.positionStanley >1))
                    {
                    etatJeu.actionStanley = SPRAY;

                    if (etatJeu.positionStanley == 2)
                    {

                        for (i= 0;i<2;i++)
                        {

                            if (etatJeu.guepes[i].presence == NORMAL) 
                            {
                                printf("\n%d\n",i);
                                printf("\n id du thread kill %d",etatJeu.guepes[i].tid);
                                pthread_kill(etatJeu.guepes[i].tid, SIGINT);
                                etatJeu.score ++;
                            }
                        }
                    }

                    pthread_mutex_unlock(&mutexEtatJeu);
                    
                    
                    }
                    break;
                    
                    case SDLK_LEFT:
                    if ((etatJeu.positionStanley >0) && (etatJeu.positionStanley <=3))
                    {
                        etatJeu.positionStanley --; 
                        pthread_mutex_unlock(&mutexEtatJeu);
                    }
                    break;

                    case SDLK_RIGHT:
                    if ((etatJeu.positionStanley >=0) && (etatJeu.positionStanley <3))
                    {
                        etatJeu.positionStanley ++; 
                        pthread_mutex_unlock(&mutexEtatJeu);
                    }
                    break;
                    case SDLK_UP:
                    if (etatJeu.positionStanley ==1)
                    {
                        etatJeu.etatStanley = ECHELLE;
                        pthread_mutex_unlock(&mutexEtatJeu);
                    }
                    break;
                } 
            break;
            case ECHELLE:
                switch(evenement)
                {
                   
                    case SDLK_DOWN:
                        if (etatJeu.positionStanley ==1)
                        {
                            etatJeu.etatStanley = BAS;
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                        if (etatJeu.positionStanley== 0)
                        {
                            etatJeu.positionStanley ++; 
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                    break;
                    case SDLK_UP:
                        if (etatJeu.positionStanley == 0)
                        {
                            etatJeu.positionStanley ++; 
                            etatJeu.positionStanley ++; 
                            etatJeu.etatStanley = HAUT;
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                        if (etatJeu.positionStanley == 1)
                        {
                            etatJeu.positionStanley --; 
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                    break;
                }
            break;
            case HAUT:
                switch(evenement)
                {
                    case SDLK_SPACE:
                        if ((etatJeu.positionStanley <= 1) || ((etatJeu.positionStanley <=5)&& (etatJeu.positionStanley >2)))
                        {
                        etatJeu.actionStanley = SPRAY;
                        pthread_mutex_unlock(&mutexEtatJeu);
                        
                       
                        }
                    break;
                    
                    case SDLK_LEFT:
                        if ((etatJeu.positionStanley >0) && (etatJeu.positionStanley <=5))
                        {
                            etatJeu.positionStanley --; 
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                    break;

                    case SDLK_RIGHT:
                        if ((etatJeu.positionStanley >=0) && (etatJeu.positionStanley <5))
                        {
                            etatJeu.positionStanley ++; 
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                    break;
                    case SDLK_DOWN:
                        if (etatJeu.positionStanley ==2)
                        {
                            etatJeu.positionStanley--;
                            etatJeu.positionStanley--;
                            etatJeu.etatStanley = ECHELLE;
                            pthread_mutex_unlock(&mutexEtatJeu);
                        }
                    break;
                } 
            break;
        }
    pthread_mutex_unlock(&mutexEtatJeu);
    evenement=AUCUN;
    pthread_mutex_unlock(&mutexEvenement);
    }


    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}

void *fctThreadEnnemis(void *)
{

    sigset_t mask;
    sigfillset (&mask);
    sigdelset(&mask, SIGALRM);
     sigprocmask(SIG_SETMASK, &mask,NULL);
    int spawn ;
    int *tempsennemi = (int*) malloc (sizeof (int));
    *tempsennemi=16;

    pthread_setspecific(keySpec,tempsennemi);
    sigdelset(&mask, SIGALRM);

    
    alarm(10);

    while(1)
    {
        AttenteEnnemi.tv_sec = (*tempsennemi/10);
        AttenteEnnemi.tv_nsec = ( 100000000*(*tempsennemi%10));

        nanosleep(&AttenteEnnemi, NULL);//doit avoir un pthread specific
        spawn = rand() %5;
        switch(spawn)
        {
            case 0:
            pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadGuepe, NULL);
            printf("\ncreation thread guepe");
            break;

            case 1:
           // pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadChenilleG, NULL);
            break;

            case 2:
            //pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadChenilleD, NULL);
            break;

            case 3:
            //pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadAraigneeG, NULL);
            break;

            case 4:
           // pthread_create(&ThreadHandler, NULL, (void*(*)(void*)) fctThreadAraigneeD, NULL);
            break;
        }
    

    }


}

void *fctThreadGuepe(void *)
{
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nId du Thread guepe: %u\n",pthread_self());


     int* position = (int*) malloc (sizeof (int));
     *position = 0;
    pthread_mutex_lock (&mutexEtatJeu);
    etatJeu.guepes[*position].presence = NORMAL;
    etatJeu.guepes[*position].tid = pthread_self();
    pthread_mutex_unlock(&mutexEtatJeu);
    
    struct timespec AttenteGuepe;
    AttenteGuepe.tv_sec = 1 ;
    AttenteGuepe.tv_nsec = 0;
   pthread_setspecific(keySpec, position);
    
    pthread_mutex_lock(&mutexEtatJeu);
    if (etatJeu.etatStanley==BAS&&etatJeu.positionStanley==2 && etatJeu.actionStanley== SPRAY)
    {
        etatJeu.guepes[*position].presence= AUCUN;
         etatJeu.guepes[*position].tid= 0;
        etatJeu.score++;
        pthread_mutex_unlock(&mutexEtatJeu);
        pthread_exit(0);
    }
    pthread_mutex_unlock(&mutexEtatJeu);
    //event deplacement
        nanosleep(&AttenteGuepe, NULL);
        (*position)++;

        pthread_mutex_lock (&mutexEtatJeu);
        etatJeu.guepes[*position].presence = NORMAL;
        etatJeu.guepes[*position].tid =pthread_self();
        etatJeu.guepes[*position-1].presence = AUCUN;
        etatJeu.guepes[*position-1].tid = 0;
        pthread_mutex_unlock(&mutexEtatJeu);

    //event dmg
        nanosleep(&AttenteGuepe, NULL);


        pthread_mutex_lock (&mutexEchec);
        echec = CHAT;
        pthread_mutex_unlock(&mutexEchec);
        pthread_cond_signal(&condEchec);

        AttenteGuepe.tv_nsec = 500000000;

        nanosleep(&AttenteGuepe, NULL);
        pthread_mutex_lock (&mutexEtatJeu);
        etatJeu.guepes[*position].presence = AUCUN;
        etatJeu.guepes[*position].tid = 0;
        pthread_mutex_unlock(&mutexEtatJeu);
        pthread_exit(0);

        
}

void *fctThreadChenilleG(void *)
{

}

void *fctThreadChenilleD(void *)
{

}

void *fctThreadAraigneeG(void *)
{

}

void *fctThreadAraigneeD(void *)
{

}

void handlerSIGINT(int)
{

    int *positionMort = (int *) pthread_getspecific(keySpec);
    pthread_mutex_lock(&mutexEtatJeu);
    etatJeu.guepes[*positionMort].presence = AUCUN;
    etatJeu.guepes[*positionMort].tid = 0;
    pthread_mutex_unlock(&mutexEtatJeu);
    pthread_exit(0);
}

void handlerSIGALRM(int )
{
    int *attente = (int *) pthread_getspecific(keySpec);
    int value = (10 + (rand() %5)+ 1);
    *attente = value;
    alarm(10);

}
void handlerSIGUSR1(int)
{
    printf("Id du Thread reçu: %u\n",pthread_self());
}
void handlerSIGUSR2(int)
{
    printf("Id du Thread reçu: %u\n",pthread_self());
}
void handlerSIGQUIT(int)
{
    printf("Id du Thread reçu: %u\n",pthread_self());
}
void fctFinThread(void *)
{
    printf("fctFinThread : Tid du thread Terminé: %ld\n", pthread_self());
}

  
  void destructeurVS(void *p)
{

    free(p);
}
