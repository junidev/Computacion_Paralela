#include <iostream>
using namespace std;
#include <chrono>
#include <cstdio>
#include <ctime>
#define MAX 1000

double A[1000][1000], x[1000], y[1000];

void firstLoop(){
    for(int i = 0; i < MAX ; i++)
        for(int j = 0; j < MAX ; j++)
            y[i] += A[i][j] * x[j];
}

void secondLoop(){
    for(int j = 0; j < MAX ; j++)
        for(int i = 0; i < MAX ; i++)
            y[i] += A[i][j] * x[j];
}



int main(){
    clock_t start, start2;
    double duration , duration2;
    start = clock();

    firstLoop();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout<<"printf: "<< duration <<'\n';

    start2 = clock();
    secondLoop();
    duration2 = ( clock() - start2 ) / (double) CLOCKS_PER_SEC;
    cout<<"printf: "<< duration2 <<'\n';
}