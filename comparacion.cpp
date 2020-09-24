#include <iostream>
using namespace std;
#include <chrono>
#include <cstdio>
#include <ctime>
#define MAX 1000

double A[1000][1000], x[1000], y[1000];

void primero(){
    for(int i = 0; i < MAX ; i++)
        for(int j = 0; j < MAX ; j++)
            y[i] += A[i][j] * x[j];
}

void segundo(){
    for(int j = 0; j < MAX ; j++)
        for(int i = 0; i < MAX ; i++)
            y[i] += A[i][j] * x[j];
}



int main(){
    clock_t start, start2;
    double duration , duration2;
    start = clock();

    primero();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout<<"Primer : "<< duration <<'\n';

    start2 = clock();
    segundo();
    duration2 = ( clock() - start2 ) / (double) CLOCKS_PER_SEC;
    cout<<"Segundo:  "<< duration2 <<'\n';
}