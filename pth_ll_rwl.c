
/* 
 * 
 * Compile:  gcc -g -Wall -o ll_rwl ll_rwl.c 
 *              my_rand.c -lpthread
 *           needs timer.h and my_rand.h
 * Usage:    ./ll_rwl <thread_count>
 * Input:    total number of keys inserted by main thread
 *           total number of ops of each type carried out by each
 *              thread.
 * Output:   Elapsed time to carry out the ops
 */

 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

const int MAX_KEY = 100000000;

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

void Usage(char* prog_name);
void Get_input(int * values_cant);

int  Insert(int value);
void Print(void);
int  Member(int value);
int  Delete(int value);
void Free_list(void);
int  Is_empty(void);

void* Thread_work(void *rank);

struct list_node_s* head_p = NULL;  /* start with empty list */
int thread_count;

int total_ops;
double insert_percent;
double search_percent;
double delete_percent;

pthread_rwlock_t rwlock;
pthread_mutex_t count_mutex;
int member_total=0, insert_total=0,delete_total=0;

int main(int argc,char *argv[]) {
    int values_cant;
    long i;
    int value, attempts;
    int verbose;
    unsigned seed = 1;        
    pthread_t* thread_handles;
    double start, finish;

    if(argc!=2) Usage(argv[0]);
    thread_count = strtol(argv[1],NULL,10);

    Get_input(&values_cant);

    i= attempts = 0;
    while (i < values_cant && attempts < 2*values_cant){
        value = my_rand(&seed) % MAX_KEY;
        verbose = Insert(value);
        attempts++;
        if(verbose) i++;
    }
    printf("%ld valores insertados en la lista\n",i);

    printf("lista value = \n");
    Print();
    printf("\n");
    thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
    pthread_mutex_init(&count_mutex,NULL);
    pthread_rwlock_init(&rwlock,NULL);

    GET_TIME(start);
    for(i = 0; i < thread_count; i++)
        pthread_create(&thread_handles[i],NULL,Thread_work, (void*) i);

    for(i = 0; i < thread_count; i++)
        pthread_join(thread_handles[i],NULL);
    GET_TIME(finish);

    printf("Elapsed time = %f seconds\n",finish-start);
    printf("Total ops = %d\n",total_ops);
    printf("member ops = %d\n",member_total);
    printf("insert_ops = %d\n",insert_total);
    printf("delete ops = %d\n",delete_total);


    Free_list();
    pthread_rwlock_destroy(&rwlock);
    pthread_mutex_destroy(&count_mutex);
    free(thread_handles);

   return 0;
}  
  

void* Thread_work(void* rank){
    long my_rank = (long) rank;
    int i,val;
    double which_op;
    unsigned seed = my_rank + 1;
    int ops_per_thread = total_ops/thread_count;
    int my_member=0,my_insert=0,my_delete=0;
    for(i = 0; i < ops_per_thread; i++){
        which_op = my_drand(&seed);
        val = my_rand(&seed) % MAX_KEY;
        
        if (which_op < search_percent ){
            pthread_rwlock_rdlock(&rwlock);
            Member(val);
            pthread_rwlock_unlock(&rwlock);
            my_member++;
        }
        else if (which_op < search_percent + insert_percent){ 
            pthread_rwlock_rdlock(&rwlock);
            Insert(val);
            pthread_rwlock_unlock(&rwlock);
            my_insert++;
        }
        else{
            pthread_rwlock_rdlock(&rwlock);
            Delete(val);
            pthread_rwlock_unlock(&rwlock);
            my_delete++;
        }
    }
    pthread_mutex_lock(&count_mutex);
    member_total += my_member;
    insert_total +=my_insert;
    delete_total += my_delete;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
} 

void Usage(char* prog_name) {
    fprintf(stderr,"Usage: %s <thread_count> \n", prog_name);
    exit(0);
}

void Get_input(int *values_cant){
    
    printf("Cantidad de valores que se insertaran en el hilo principal\n");
    scanf("%d",values_cant);
    printf("Cantidad de operaciones que va a ejecutar?\n");
    scanf("%d",&total_ops);
    printf("procentaje de operaciones que deberian ser buscados? (entre 0 y 1)\n");
    scanf("%lf",&search_percent);
    printf("porcentaje de operaciones que deberian ser inserciones? (entre 0 y 1)\n");
    scanf("%lf",&insert_percent);
    delete_percent = 1.0 -(search_percent + insert_percent);
}

/*-----------------------------------------------------------------*/
/* inserta un valor en la lista de manera ordenada
 * retorna 1 si el valor a sido insertado o 0 caso contrario
 */            
int Insert(int value) {
   struct list_node_s* curr_p = head_p;
   struct list_node_s* pred_p = NULL;
   struct list_node_s* temp_p;
   
   while (curr_p != NULL && curr_p->data < value) {
      pred_p = curr_p;
      curr_p = curr_p->next;
   }

   if (curr_p == NULL || curr_p->data > value) {
       temp_p = (struct list_node_s*)malloc(sizeof(struct list_node_s));
       temp_p->data = value;
       temp_p->next = curr_p;
       if (pred_p == NULL)
           head_p = temp_p;
       else
          pred_p->next = temp_p;
       return 1;
   } 
   else { /* value in list */
       return 0;
   }
}  

/* Imprime la lista
 */
void Print(void) {
   struct list_node_s* curr_p;

   printf("list = ");

   curr_p = head_p;
   while (curr_p != (struct list_node_s*) NULL) {
      printf("%d ", curr_p->data);
      curr_p = curr_p->next;
   }
   printf("\n");
}  /* Print */


/* Busca un valor en la lista  devuelve true(1) o false(0) */
int  Member(int value) {
   struct list_node_s* curr_p;

   curr_p = head_p;
   while (curr_p != NULL && curr_p->data < value)
      curr_p = curr_p->next;

   if (curr_p == NULL || curr_p->data > value) {
      return 0;
   } else {
      return 1;
   }
} 

/* Elimina un valor de la lista
 * retorna 1 si ha sido eliminado, caso contrario 0
 */
int Delete(int value) {
   struct list_node_s* curr_p = head_p;
   struct list_node_s* pred_p = NULL;

   while (curr_p != NULL && curr_p->data < value) {
      pred_p = curr_p;
      curr_p = curr_p->next;
   }
   
   if (curr_p != NULL && curr_p->data == value) {
      if (pred_p == NULL) { /* first element in list */
         head_p = curr_p->next;
         free(curr_p);
      } else { 
         pred_p->next = curr_p->next;
         free(curr_p);
      }
      return 1;
   } else {
      return 0;
   }
}

void Free_list() {
   struct list_node_s* curr_p;
   struct list_node_s* succ_p;

   if (Is_empty()) return;
   curr_p = head_p; 
   succ_p = curr_p->next;
   while (succ_p != NULL) {
      free(curr_p);
      curr_p = succ_p;
      succ_p = curr_p->next;
   }
   free(curr_p);
   head_p = NULL;
}  /* Free_list */

int  Is_empty(void) {
   if (head_p == NULL)
      return 1;
   else
      return 0;
}  /* Is_empty */
