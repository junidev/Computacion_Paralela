#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
#include <time.h>
using namespace std;
#define MAX 800

void matrix_multiplication(){
    unsigned start,end;
    int M[MAX][MAX];
    int X[MAX][MAX];
    int Y[MAX][MAX];

    

    for (int h = 0; h < MAX; h++){
          for (int w = 0; w < MAX; w++){
                M[h][w] = 0;
                X[h][w] = 1+rand()%(100-1);
                Y[h][w] = 1+rand()%(100-1);
                //cout << h <<" - "<Y[h][w]<< endl; 
          }
    }



    //Compruebo loos arrays
    /* for(int x=0;x<MAX;x++) {
        for(int y=0;y<MAX;y++){
            cout<<X[x][y] <<" - ";  
        }
         cout<<endl;  
    }

    
    int count; 
    for(int x=0;x<MAX;x++) {
        for(int y=0;y<MAX;y++){
            cout<< x << " , "<<y<< " : "<<Y[x][y] <<"   ***";   
            count++;
        }
         cout<<endl;  
    }

    cout <<"cantidad: " <<count; */


    start = clock();
    for(int i = 0; i < MAX; ++i){
        for(int j = 0; j < MAX; ++j){
            for(int k = 0; k < MAX; ++k)
            {
                M[i][j] += X[i][k] * Y[k][j];
            }
        }
    }
    end = clock();
    cout << "Tiempo: "<< (double(end-start)/CLOCKS_PER_SEC)<<endl;
}

int main(){
    matrix_multiplication();

}