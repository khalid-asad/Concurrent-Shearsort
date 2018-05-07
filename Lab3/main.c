#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>


struct Params{
    int tid;
    int answer;
};

// only used for synchronizing stdout from overlap.
pthread_mutex_t mtx0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx3 = PTHREAD_MUTEX_INITIALIZER;
static const unsigned int n = 4;
int a[4][4];
sem_t mut0, mut1, mut2, mut3;


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
void shear_sort_mt(){
    int i, j;
    
    for (i=0;i<1;i++){
        /*rowsort(0);
        rowrevsort(1);
        rowsort(2);
        rowrevsort(3);*/
        
        for (j=0;j<n;j++){
            if (j%2) rowrevsort(j);
            else rowsort(j);
        }
        
        for (j=0;j<n;j++) colsort(j);

        /*rowsort(0);
        rowrevsort(1);
        rowsort(2);
        rowrevsort(3);*/
        
        for (j=0;j<n;j++){
            if (j%2) rowrevsort(j);
            else rowsort(j);
        }
        
        for (j=0;j<n;j++) colsort(j);
    }
    
    /*rowsort(0);
    rowrevsort(1);
    rowsort(2);
    rowrevsort(3);*/
    
    for (j=0;j<4;j++){
        if (j%2) rowrevsort(j);
        else rowsort(j);
    }
}

void lock_unlock_mutex(int i, int lock){
    switch(i){
        case (0):
            if(lock){
                //pthread_mutex_lock(&mtx0);
                sem_wait(&mut1);
                sem_wait(&mut2);
                sem_wait(&mut3);
            }else{
                //pthread_mutex_unlock(&mtx0);
                sem_post(&mut0);
            }
            break;
        case (1):
            if(lock){
                //pthread_mutex_lock(&mtx1);
                sem_wait(&mut0);
                sem_wait(&mut2);
                sem_wait(&mut3);
            }else{
                //pthread_mutex_unlock(&mtx1);
                sem_post(&mut1);
            }
            break;
        case (2):
            if(lock){
                //pthread_mutex_lock(&mtx2);
                sem_wait(&mut0);
                sem_wait(&mut1);
                sem_wait(&mut3);
            }else{
                //pthread_mutex_unlock(&mtx2);
                sem_post(&mut2);
            }
            break;
        case (3):
            if(lock){
                //pthread_mutex_lock(&mtx3);
                sem_wait(&mut0);
                sem_wait(&mut1);
                sem_wait(&mut2);
            }else{
                //pthread_mutex_unlock(&mtx3);
                sem_post(&mut3);
            }
            break;
    }
}

void shear_sort_thread(void *arg){
    struct Params *arg_struct = (struct Params*) arg;

    printf("Thread #%d\n", arg_struct->tid);
    
    //pthread_mutex_lock(&mtx1);
    lock_unlock_mutex(arg_struct->tid, 1);
    /*sem_wait(&mut1);
    sem_wait(&mut2);
    sem_wait(&mut3);*/
    if (arg_struct->tid %2) rowrevsort(arg_struct->tid); // odd
    else rowsort(arg_struct->tid); // even
    /*
    if(arg_struct->tid == 0){
        sem_wait(&mut0);
    }else if(arg_struct->tid == 1){
        
    }else if(arg_struct->tid == 2){
        
    }else if(arg_struct->tid == 3){
        
    }
    sem_post(&mut1);
    sem_post(&mut2);
    sem_post(&mut3);*/
    lock_unlock_mutex(arg_struct->tid, 0);
    //pthread_mutex_unlock(&mtx1);
    
    //pthread_mutex_lock(&mtx1);
    lock_unlock_mutex(arg_struct->tid, 1);
    colsort(arg_struct->tid);
    lock_unlock_mutex(arg_struct->tid, 0);
    //pthread_mutex_unlock(&mtx1);
    
    //pthread_mutex_lock(&mtx1);
    lock_unlock_mutex(arg_struct->tid, 1);
    if (arg_struct->tid %2) rowrevsort(arg_struct->tid); // odd
    else rowsort(arg_struct->tid); // even
    lock_unlock_mutex(arg_struct->tid, 0);
    //pthread_mutex_unlock(&mtx1);
    
    //pthread_mutex_lock(&mtx1);
    lock_unlock_mutex(arg_struct->tid, 1);
    colsort(arg_struct->tid);
    lock_unlock_mutex(arg_struct->tid, 0);
    //pthread_mutex_unlock(&mtx1);
    
    //pthread_mutex_lock(&mtx1);
    lock_unlock_mutex(arg_struct->tid, 1);
    if (arg_struct->tid %2) rowrevsort(arg_struct->tid); // odd
    else rowsort(arg_struct->tid); // even
    lock_unlock_mutex(arg_struct->tid, 0);
    //pthread_mutex_unlock(&mtx1);
    
    pthread_exit(0);
}

// multi-threaded shear sort
void shear_sort(){
    //int limit = 4;
    int i = 0;
   
    //pthread_mutex_lock(&mtx);
    printf("Starting subthread...\n");
    //pthread_mutex_unlock(&mtx);
    
    sem_init(&mut0, 0, 1);
    sem_init(&mut1, 0, 0);
    sem_init(&mut2, 0, 0);
    sem_init(&mut3, 0, 0);
    
    struct Params args[4];
    pthread_t thrd[4];
    
    for (i=0;i<n;i++){
        args[i].tid = i;
        // create our thread
        pthread_create(&thrd[i], NULL, shear_sort_thread, &args[i]);
    }
    
    for (i=0;i<n;i++){
        // join on the launched thread
        pthread_join(thrd[i], NULL);
    }
    
    //pthread_mutex_lock(&mtx);
    printf("Finished subthread.\n");
    //pthread_mutex_unlock(&mtx);
}



int main(){
    unsigned int i, j = 0;
    //static const unsigned int N = 16;
    
    //int limit = 4;
    
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
    
    //shear_sort();
    shear_sort_mt();
    
    for(i=0;i<n;i++){
        for(j=0;j<n;j++) printf("%4d ", a[i][j]);
        printf("\n");
    }
    
    return 0;
}