#include <stdio.h>
#include <mpi.h>

void Get_input(int my_rank, int comm_sz, double* a_p, double* b_p,int* n_p);
/* Calculamos la integral  */
double Trap(double left_endpt, double right_endpt, int trap_count,  double base_len);    
/* funcion para derivar */
double f(double x); 


void Get_input(int my_rank, int comm_sz, double* a_p, double* b_p, int* n_p) {
   if (my_rank == 0) {
      printf("Ingrese a, b, and n\n");
      scanf("%lf %lf %d", a_p, b_p, n_p);
   } 
   MPI_Bcast(a_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(b_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(n_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
}  

double Trap(
      double left_endpt  , 
      double right_endpt , 
      int    trap_count  , 
      double base_len    ) {
   double estimate, x; 
   int i;

   estimate = (f(left_endpt) + f(right_endpt))/2.0;
   for (i = 1; i <= trap_count-1; i++) {
      x = left_endpt + i*base_len;
      estimate += f(x);
   }
   estimate = estimate*base_len;

   return estimate;
} 


double f(double x) {
   return x*x;
} 


int main(void) {
   int my_rank, comm_sz, n, local_n;   
   double a, b, h, local_a, local_b;
   double local_int, total_int;

   MPI_Init(NULL, NULL);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   Get_input(my_rank, comm_sz, &a, &b, &n);

   // Para todos los procesos
   h = (b-a)/n;          
   local_n = n/comm_sz; //numero de trapezoides

   /* Duración de intervalo en cada procesos */
   local_a = a + my_rank*local_n*h;
   local_b = local_a + local_n*h;
   local_int = Trap(local_a, local_b, local_n, h);

   /* SUma de integrales para cada proceso */
   MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0,
         MPI_COMM_WORLD);

   if (my_rank == 0) {
      printf("Con n = %d trapezoides, un estimado\n", n);
      printf("del integral  %f a %f = %.15e\n",
          a, b, total_int);
   }

   MPI_Finalize();

   return 0;
} 

