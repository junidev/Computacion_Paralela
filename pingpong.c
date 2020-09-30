
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  //inicializamos
  MPI_Init(NULL, NULL);
  // ontenemos el rank y zize
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  //iteaciones
  const int iteraraciones = 20;

  //asumimos que son dos procesos
  if (size != 2) {
    fprintf(stderr, "World size must be two for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int contador = 0;

  char *mensaje = "-Mensaje- ";
  int mensajeint =999;
  int partner_rank = (rank + 1) % 2;
  while (contador < iteraraciones) {
    if (rank == contador % 2) {
        contador++;
        MPI_Send(&contador, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
        printf("Proceso %d envio .  e incremento contador . %d to %d\n", rank, contador, partner_rank);
    } else {
        MPI_Recv(&contador, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso %d recibio .  contador %d from %d\n",  rank,  contador, partner_rank);
    }

  }
  MPI_Finalize();
}