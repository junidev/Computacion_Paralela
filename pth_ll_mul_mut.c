#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

const int MAX_KEY = 100000000;

struct list_node_s {
   int    data;
   struct list_node_s* next;
   pthread_mutex_t mutex;
};

void Usage(char* prog_name);
void Get_input(int * values_cant);

int  Insert(int value);
void Print();
int  Member(int value);
int  Delete(int value);
void Free_list(void);
int  Is_empty(void);
int Advance_ptrs(struct list_node_s** curr_pp,
        struct list_node_s** pred_pp);

void* Thread_work(void *rank);

struct list_node_s* head_p = NULL;  /* start with empty list */
int thread_count;
pthread_mutex_t mutex;
pthread_mutex_t count_mutex;
int member_total=0,insert_total=0,delete_total=0;

int total_ops;
double insert_percent;
double search_percent;
double delete_percent;


int main(int argc,char *argv[]) {
    int values_cant;
    long i;
    int value,attempts;
    int verbose;
    unsigned seed = 1;        
    pthread_t* thread_handles;
    double start, finish;

    if(argc!=2) Usage(argv[0]);
    thread_count = strtol(argv[1],NULL,10);

    Get_input(&values_cant);

    i = attempts = 0;
    pthread_mutex_init(&mutex,NULL);
    while (i < values_cant && attempts < 2*values_cant){
        value = my_rand(&seed) % MAX_KEY;
        verbose = Insert(value);
        attempts++;
        if(verbose) i++;
    }
    printf("%ld valores insertados en la lista\n",i);
# ifdef OUTPUT
    printf("lista value = \n");
    Print();
    printf("\n");
# endif
    thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
    pthread_mutex_init(&count_mutex,NULL);

    GET_TIME(start);
    for(i = 0; i < thread_count; i++)
        pthread_create(&thread_handles[i],NULL,Thread_work, (void*) i);

    for(i = 0; i < thread_count; i++)
        pthread_join(thread_handles[i],NULL);
    GET_TIME(finish);

    printf("Elapsed time = %f seconds\n",finish-start);

    Free_list();
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&count_mutex);
    free(thread_handles);

   return 0;
}  
  

void* Thread_work(void* rank){
    long my_rank = (long) rank;
    int i,val;
    double which_op;
    unsigned seed = my_rank + 1; 
    int my_member=0,my_insert=0,my_delete=0;
    int ops_per_thread = total_ops/thread_count;
    for(i = 0; i < ops_per_thread; i++){
        which_op = my_drand(&seed);
        val = my_rand(&seed) % MAX_KEY;
        
        if (which_op < search_percent ){
            Member(val);
            my_member++;
        }
        else if (which_op < search_percent + insert_percent){ 
            Insert(val);
            my_insert++;
        }
        else{
            Delete(val);
            my_delete++;
        }
    }
    pthread_mutex_lock(&count_mutex);
    member_total += my_member;
    insert_total += my_insert;
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
    printf("porcentaje de operaciones que deberian ser inserciones? (entre 0 y 1\n)");
    scanf("%lf",&insert_percent);
    delete_percent = 1.0 -(search_percent + insert_percent);
}

int Advance_ptrs(struct list_node_s** curr_pp,struct list_node_s**pred_pp) {
    struct list_node_s* curr_p = *curr_pp;
    struct list_node_s* pred_p = *pred_pp;  
    int rv = 1;

    if (curr_p == NULL) {
        if (pred_p == NULL){
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        else {
            return 0;
        }
    }
    else{
        if (curr_p->next != NULL)
            pthread_mutex_lock(&(curr_p->next->mutex));
        else
            rv = 0;
        if (pred_p!= NULL)
            pthread_mutex_unlock(&(pred_p->mutex));
        else
            pthread_mutex_unlock(&mutex);
        *pred_pp=curr_p;
        *curr_pp=curr_p->next;
        return rv;
    }
}
/*-----------------------------------------------------------------*/
/* inserta un valor en la lista de manera ordenada
 * retorna 1 si el valor a sido insertado o 0 caso contrario
 */              
int Insert(int value) {
   struct list_node_s* curr_p;
   struct list_node_s* pred_p = NULL;
   struct list_node_s* temp_p;
   pthread_mutex_lock(&mutex);
   curr_p = head_p;
   if (curr_p != NULL)
       pthread_mutex_lock(&(curr_p->mutex));

   while (curr_p != NULL && curr_p->data < value) {
       //pred_p = curr_p;
       //curr_p = curr_p->next;
       Advance_ptrs(&curr_p,&pred_p);
   }

   if (curr_p == NULL || curr_p->data > value) {
       temp_p = (struct list_node_s*)malloc(sizeof(struct list_node_s));
       pthread_mutex_init(&(temp_p->mutex),NULL);
       temp_p->data = value;
       temp_p->next = curr_p;
       if (curr_p != NULL)
           pthread_mutex_unlock(&(curr_p->mutex));

       if (pred_p == NULL){
           head_p = temp_p;
           pthread_mutex_unlock(&mutex);
       }
       else{
           pred_p->next = temp_p;
           pthread_mutex_unlock(&(pred_p->mutex));
       }
       return 1;
   } 
   else { /* value in list */
       if (curr_p !=NULL)
           pthread_mutex_unlock(&(curr_p->mutex));
       if (pred_p !=NULL)  
           pthread_mutex_unlock(&(pred_p->mutex));
       else    
           pthread_mutex_unlock(&mutex);

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


/* Busca un valor en la lista 
 * devuelve true(1) o false(0)
 */
int  Member(int value) {
   struct list_node_s* curr_p, *old_curr;

   pthread_mutex_lock(&mutex);
   curr_p = head_p;
   if (curr_p != NULL) pthread_mutex_lock(&(curr_p->mutex));
   pthread_mutex_unlock(&mutex);

   while (curr_p != NULL && curr_p->data < value){ 
       if(curr_p->next!=NULL) 
           pthread_mutex_lock(&(curr_p->next->mutex));
       old_curr = curr_p;
       curr_p = curr_p->next;
       pthread_mutex_unlock(&(old_curr->mutex));
   }

   if (curr_p == NULL || curr_p->data > value) {
       if (curr_p!=NULL)
           pthread_mutex_unlock(&(curr_p->mutex));
      return 0;
   } else {
       pthread_mutex_unlock(&(curr_p->mutex));
      return 1;
   }
} 

/* Elimina un valor de la lista
 * retorna 1 si ha sido eliminado, caso contrario 0
 */
int Delete(int value) {
   struct list_node_s* curr_p;
   struct list_node_s* pred_p = NULL;

   pthread_mutex_lock(&mutex);
   curr_p = head_p;
   if (curr_p != NULL)
       pthread_mutex_lock(&(curr_p->mutex));

   while (curr_p != NULL && curr_p->data < value) {
      //pred_p = curr_p;
      //curr_p = curr_p->next;
      Advance_ptrs(&curr_p,&pred_p);
   }
   
   if (curr_p != NULL && curr_p->data == value) {
      if (pred_p == NULL) { /* first element in list */
         head_p = curr_p->next;
         pthread_mutex_unlock(&mutex);
         pthread_mutex_unlock(&(curr_p->mutex));
         pthread_mutex_destroy(&(pred_p->mutex));
         free(curr_p);
      } else { 
         pred_p->next = curr_p->next;
         pthread_mutex_unlock(&(pred_p->mutex));
         pthread_mutex_unlock(&(curr_p->mutex));
         pthread_mutex_destroy(&(curr_p->mutex));
         free(curr_p);
      }
      return 1;
   } else {
       if (pred_p != NULL)
           pthread_mutex_unlock(&(pred_p->mutex));
       if (curr_p != NULL)
           pthread_mutex_unlock(&(curr_p->mutex));
       if (curr_p == head_p)
           pthread_mutex_unlock(&mutex);
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