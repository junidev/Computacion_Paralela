#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // trabajqremos con semaforos

const int MAX_THREADS = 1024;
const int MSG_MAX = 100;
//variables globales que son accedidad con todos los hilos
int thread_count;
char** messages;
sem_t* semaphores; //contenedor de los semaforows

//recibe el nombre del programa e imprime el numero de hilos
void Usage(char* prog_name);
void *Send_msg(void* rank);

int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles; 

   //nombre programa- num hilo : solo acepta dos argumentos
   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);

   // en esta parte se reserva la memoria 
   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));
   messages = (char**)malloc(thread_count*sizeof(char*));
   semaphores = (sem_t*)malloc(thread_count*sizeof(sem_t));
   

   for (thread = 0; thread < thread_count; thread++){
      //cada indice se incializa con null
      messages[thread] = NULL;
      //inicia todos los semaforos
      sem_init(&semaphores[thread],0,0);
   }


   // Creacion de threads
   for (thread = 0; thread < thread_count; thread++)
      //pasamos contenedor , atributo y funcion
      pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL,
          Send_msg, (void*) thread);

   // Ejecucion de threads : Join: Para unir los hilos
   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }

   //limpiando memoria
   for (thread = 0; thread < thread_count; thread++){
      free(messages[thread]);
      sem_destroy(&semaphores[thread]);
   }
   free(messages);
   free(semaphores);
   free(thread_handles);
   return 0;
}  

void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}

//recibe de parametro el hilo
void *Send_msg(void* rank) {
   //casteamos
   long my_rank = (long) rank;
   //hacia donde vaa.. para hallar el destino hacemos esto
   long dest = (my_rank + 1) % thread_count; //Hilo 1: (1 + 1) % 5 =>  el residuo es 2 = destino

  //asignamos espacio de memoria.....
   char* my_msg = (char*) malloc(MSG_MAX*sizeof(char));
   //guardamos el hola deol destino haciua el hilo
   sprintf(my_msg, "Este es un mensaje para %ld de %ld", dest, my_rank);
   messages[dest] = my_msg;
   //sirve para desbloquear el semaforo del destino
   sem_post(&semaphores[dest]);
   //sirve para que esperen por todos los semaforos que  van hacer desbloqueados
   sem_wait(&semaphores[my_rank]);
   //imprimimos el hilo actual y el mensaje que va a continuar
   printf("Thread %ld > %s\n", my_rank, messages[my_rank]);
   return NULL;
}  