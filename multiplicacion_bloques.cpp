#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
#include <time.h>
using namespace std;
#define MAX 50


void multiplicacion_matriz_bloques(){
    unsigned start,end;
    int M[MAX][MAX];
    int X[MAX][MAX];
    int Y[MAX][MAX];

    for (int i = 0; i < MAX; i++){
          for (int j = 0; j < MAX; j++){
                X[i][j] = 1+rand()%(100-1);
                Y[i][j] = 1+rand()%(100-1);
          }
    }

    //Compruebo loos arrays
    /* for(int x=0;x<MAX;x++) {
        for(int y=0;y<MAX;y++){
            cout<<X[x][y] <<" - ";  
        }
         cout<<endl;  
    }*/
    
    //bloques 
    int B=MAX/5;
    start = clock();
    for (int x = 0; x < MAX; x+=B) {
      for (int y = 0; y < MAX; y+=B) {
        for (int z = 0; z < MAX; z+=B) {
          for (int i = x; i < x+B; i++) {
            for (int j = y; j < y+B; j++) {
              for (int k = z; k < z+B; k++) {
                M[i][j] += X[i][k]*Y[k][j];
              }
            }
          }
        }
      }
    }
    end = clock();
    cout << "Tiempo : "<< (double(end-start)/CLOCKS_PER_SEC)<<endl;
}

int main(){
    multiplicacion_matriz_bloques();
}