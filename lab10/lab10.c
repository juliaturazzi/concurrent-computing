#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h>

#define L 2
#define E 4

#define NTHREADS L+E

sem_t em_e, em_l, escr, leit; //semaforos
int e=0, l=0; //globais

//funcao executada pelos leitores
void *Leitor (void *arg) {
  int *id =  (int*) arg;
  while(1) {
     fprintf(stderr, "Leitor: %d quer ler!\n", *id);
     sem_wait(&leit); 
     sem_wait(&em_l); l++; 
     
     if(l == 1) sem_wait(&escr); 
     
     sem_post(&em_l);
     sem_post(&leit);

     fprintf(stderr, "Leitor: %d esta lendo!\n", *id);
     
     sleep(1);
     
     fprintf(stderr, "Leitor: %d terminou de ler!\n",  *id);

     //le...
     sem_wait(&em_l); l--;
     if(l==0) sem_post(&escr);

     sem_post(&em_l);

    //  sleep(1);
  }
  
  free(arg);
  pthread_exit(NULL);
}


//funcao executada pelos escritores
void *Escritor (void *arg) {
  int *id =  (int*) arg;
  while(1) {
     fprintf(stderr, "Escritor: %d quer escrever!\n", *id);
     sem_wait(&em_e); e++;

     if(e == 1) sem_wait(&leit);

     sem_post(&em_e);
     sem_wait(&escr);

     fprintf(stderr, "Escritor: %d esta escrevendo!\n", *id);
    
    //escreve...
     sem_post(&escr);
     
     fprintf(stderr, "Escritor: %d terminou de escrever!\n", *id);
     
     sem_wait(&em_e); e--;

     if(e == 0) sem_post(&leit);

     sem_post(&em_e);
     
     sleep(1);
  }

  free(arg);
  pthread_exit(NULL);
}


//funcao principal
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int t, id[NTHREADS];
  
  //inicia os semaforos
  sem_init(&leit, 0, 1);
  sem_init(&escr, 0, 1);
  sem_init(&em_l, 0, 1);
  sem_init(&em_e, 0, 1);

  //cria as threads
  for (t=0; t<E; t++) {
     id[t+L]=t+1;
     if (pthread_create(&tid[t+L], NULL, Escritor, (void *)&id[t+L])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  }
  for (t=0; t<L; t++) {
     id[t]=t+1;
     if (pthread_create(&tid[t], NULL, Leitor, (void *)&id[t])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  }  

  //--espera todas as threads terminarem
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }

  pthread_exit(NULL);
}