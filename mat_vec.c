 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Read_matrix(float A[], int m, int n);
void Read_vector(float v[], int n);
void Serial_matrix_vector_prod(float A[], int m, int n,
          float x[], float y[]);
    
int main(void) {
    float*    A; 
    float*    x;
    float*    y;
    int       m, n;

    printf("Ingrese el tamaño\n");
    scanf("%d %d", &m, &n);

    A = malloc(m*n*sizeof(float));
    Read_matrix(A, m, n);
    x = malloc(n*sizeof(float));
    Read_vector(x, n);

    y = malloc(m*sizeof(float));

    clock_t t; 
    t = clock(); 
    Serial_matrix_vector_prod(A, m, n, x, y);
    t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Tiempo %f segundos\n",time_taken);    

    free(A);
    free(x);
    free(y);
    return 0;
}  /* main */
   
void Read_matrix(
         float     A[]     /* out */,
         int       m       /* in  */,
         int       n       /* in  */) {
    int i, j;

    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            A[i*n+j] = j-i;
}  /* Read_matrix */

void Read_vector(
         float  v[]     /* out */,
         int    n       /* in  */) {
    int i;

    for (i = 0; i < n; i++)
        v[i] = 5;
}  /* Read_vector */

void Serial_matrix_vector_prod(
        float     A[]  /* in  */,
        int       m    /* in  */,
        int       n    /* in  */,
        float     x[]  /* in  */,
        float     y[]  /* out */) {

    int i, j;

    for (i = 0; i < m; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++)
            y[i] += A[i*n + j]*x[j];
    }
}  /* Serial_matrix_vector_prod */