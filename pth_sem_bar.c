/* Usamos semaforo para sincronizar los threads
 *
 * Compile:
 *    gcc -g -Wall -o pth_sem_bar pth_sem_bar.c -lpthread
 *    timer.h needs to be available

 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "timer.h"

//dimension que tomará el semaforo
#define BARRIER_COUNT 20

int thread_count;

int counter;
//vector de semaforo
sem_t barrier_sems[BARRIER_COUNT];
sem_t count_sem;

void Usage(char* prog_name);
void *Thread_work(void* rank);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread, i;
   pthread_t* thread_handles; 
   double start, finish;

   if (argc != 2)
      Usage(argv[0]);
   //inicializamos el numero de hilos
   thread_count = strtol(argv[1], NULL, 10);
   //reservamos memoria
   thread_handles = malloc (thread_count*sizeof(pthread_t));
   for (i = 0; i < BARRIER_COUNT; i++)
      //inicializamos los semaforos
      sem_init(&barrier_sems[i], 0, 0);
   sem_init(&count_sem, 0, 1);

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      //creamos los hilos y pasamos el Thread_work
      pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL,
          Thread_work, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);
   //liberamos memoria
   sem_destroy(&count_sem);
   for (i = 0; i < BARRIER_COUNT; i++)
      sem_destroy(&barrier_sems[i]);
   free(thread_handles);
   return 0;
}  



void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}  /* Usage */

//entran varios hilos uno x uno 
void *Thread_work(void* rank) {
   //el hilo
   long my_rank = (long) rank;
   int i, j;

   for (i = 0; i < BARRIER_COUNT; i++) {
      //count_same => protege al contador
      sem_wait(&count_sem);
      if (counter == thread_count - 1) {
         //si es el ultimo hilo contador se reinicia y se desbloquea el counts_sems
         //se tiene un llamado a sem_post
         counter = 0;
         sem_post(&count_sem);
         for (j = 0; j < thread_count-1; j++)
            //barrier_sems => bloqueará los hilos que han entrado a la barrera
            sem_post(&barrier_sems[i]);
      } else {
         counter++;
         // cada llamada aumenta
         sem_post(&count_sem);
         //cada llamada disminute
         sem_wait(&barrier_sems[i]);
      }
      if (my_rank == 0) {
          printf("Todos los hilos han llegado a la barrera %d\n",i);
         fflush(stdout);
      }
   }

   return NULL;
}  /* Thread_work */
