#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void * fctThread(int * param); 
void * fctThreadB(int * param); 
void * fctThreadC(int * param); 
void * fctThreadD(int * param); 
pthread_t threadHandle;  
pthread_t threadHandleb; 
pthread_t threadHandlec; 
pthread_t threadHandled; 

int main() 
{ 
    int retp [4];
    int i=0, ret;
    int * retThread; 
    int paramEff = 5; 
    ret = pthread_create(&threadHandle, NULL, (void *(*) (void *))fctThread, &paramEff);
    ret = pthread_create(&threadHandleb, NULL, (void *(*) (void *))fctThreadB, &paramEff);
    ret = pthread_create(&threadHandlec, NULL, (void *(*) (void *))fctThreadC, &paramEff);
    ret = pthread_create(&threadHandled, NULL, (void *(*) (void *))fctThreadD, &paramEff);
        
    ret = pthread_join(threadHandle, (void **)&retThread);  
    retp[0] = *retThread;
    ret = pthread_join(threadHandleb, (void **)&retThread);  
    retp[1] = *retThread;
    ret = pthread_join(threadHandlec, (void **)&retThread);  
    retp[2] = *retThread;
    ret = pthread_join(threadHandled, (void **)&retThread);  
    retp[3] = *retThread;
    for (i=0;i<4;i++)
    {
        printf("\nLe Thread %d vaut %d",i, retp[i]);
    }

}


void * fctThread (int * param) 
{ 

 int * avr = (int *)malloc(sizeof(int)); 
    char buffer[7];
    float print;
    float c;
    float i = 0;
    int fp;
    fp= open("output.txt", O_RDONLY);
            printf("*\n");

    if ( fp == -1 )
    {
     printf("Echec Ouverture\n") ;
     exit(0) ;
    }
    c = lseek(fp, 0,SEEK_END);
     close(fp);
    for (i=0; i< (c); i++)
    {
        fp= open("output.txt", O_RDONLY);
        printf("*\n");

        lseek(fp, i,SEEK_SET);
        read(fp,buffer, 6);
        if (strcmp(buffer, "printf") == 0)
        {
            print ++;
        }
        
        close(fp);
    }
    *avr = print;
    pthread_exit(avr); 
    return 0; 
}

void * fctThreadB (int * param) 
{ 

 int * avr = (int *)malloc(sizeof(int)); 
    char buffer[3];
    float print;
    float c;
    float i = 0;
    int fp;
    fp= open("output.txt", O_RDONLY);
            printf("    *B\n");

    if ( fp == -1 )
    {
     printf("Echec Ouverture\n") ;
     exit(0) ;
    }
    c = lseek(fp, 0,SEEK_END);
     close(fp);
    for (i=0; i< (c); i++)
    {
        fp= open("output.txt", O_RDONLY);
                printf("    *B\n");

        lseek(fp, i,SEEK_SET);
        read(fp,buffer, 2);
        if (strcmp(buffer, "10") == 0)
        {
            print ++;
        }
        
        close(fp);
    }
    *avr = print;
    pthread_exit(avr); 
    return 0; 
}

void * fctThreadC (int * param) 
{ 

 int * avr = (int *)malloc(sizeof(int)); 
    char buffer[6];
    float print;
    float c;
    float i = 0;
    int fp;
    fp= open("output.txt", O_RDONLY);
            printf("        *C\n");

    if ( fp == -1 )
    {
     printf("Echec Ouverture\n") ;
     exit(0) ;
    }
    c = lseek(fp, 0,SEEK_END);
     close(fp);
    for (i=0; i< (c); i++)
    {
        fp= open("output.txt", O_RDONLY);
                printf("        *C\n");

        lseek(fp, i,SEEK_SET);
        read(fp,buffer, 5);
        if (strcmp(buffer, "Ligne") == 0)
        {
            print ++;
        }
        
        close(fp);
    }
    *avr = print;
    pthread_exit(avr); 
    return 0; 
}

void * fctThreadD (int * param) 
{ 

 int * avr = (int *)malloc(sizeof(int)); 
    char buffer[7];
    float print;
    float c;
    float i = 0;
    int fp;
    fp= open("output.txt", O_RDONLY);
    printf("            *D\n");

    if ( fp == -1 )
    {
     printf("Echec Ouverture\n") ;
     exit(0) ;
    }
    c = lseek(fp, 0,SEEK_END);
     close(fp);
    for (i=0; i< (c); i++)
    {
        fp= open("output.txt", O_RDONLY);
          printf("            *D\n");
        lseek(fp, i,SEEK_SET);
        read(fp,buffer, 6);
        if (strcmp(buffer, "printg") == 0)
        {
            print ++;
        }
        
        close(fp);
    }
    *avr = print;
    pthread_exit(avr); 
    return 0; 
}