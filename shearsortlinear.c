#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


struct Params
{
    int *start;
    size_t len;
    int depth;
};

// only used for synchronizing stdout from overlap.
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static const unsigned int n = 4;
int a[4][4];

void rowsort(int i){
    int j, k;
    for(j=0;j<n-1;j++){
        for(k=0;k<n-j-1;k++){
            if(a[i][k]>a[i][k+1]){
                int temp=a[i][k];
                a[i][k]=a[i][k+1];
                a[i][k+1]=temp;
            }
        }
    }
}

void rowrevsort(int i){
    int j, k;
    for(j=0;j<n-1;j++){
        for(k=0;k<n-j-1;k++){
            if(a[i][k]<a[i][k+1]){
                int temp=a[i][k];
                a[i][k]=a[i][k+1];
                a[i][k+1]=temp;
            }
        }
    }
}

void colsort(int i){
    int j, k;
    for(j=0;j<n-1;j++){
        for(k=0;k<n-j-1;k++){
            if(a[k][i]>a[k+1][i]){
                int temp=a[k][i];
                a[k][i]=a[k+1][i];
                a[k+1][i]=temp;
            }
        }
    }
}

// public-facing api
void shear_sort(){
    int i, j;
    
    for (i=0;i<1;i++){
        rowsort(0);
        rowrevsort(1);
        rowsort(2);
        rowrevsort(3);

        for (j=0;j<4;j++) colsort(j);

        rowsort(0);
        rowrevsort(1);
        rowsort(2);
        rowrevsort(3);

        for (j=0;j<4;j++) colsort(j);
    }
    rowsort(0);
    rowrevsort(1);
    rowsort(2);
    rowrevsort(3);  
}

int main(){
    unsigned int i, j = 0;
    //static const unsigned int N = 16;

    FILE *myFile;
    myFile = fopen("input.txt", "r");
 
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            fscanf(myFile, "%d", &a[i][j]);
            printf("%4d ", a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    shear_sort();
    
    for(i=0;i<n;i++){
        for(j=0;j<n;j++) printf("%4d ", a[i][j]);
        printf("\n");
    }
     
    return 0;
}