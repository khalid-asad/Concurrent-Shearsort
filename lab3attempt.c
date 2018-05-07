/*****************************************************************************
* FILE: dotprod_mutex.c
* DESCRIPTION:
*   This example program illustrates the use of mutex variables 
*   in a threads program. This version was obtained by modifying the
*   serial version of the program (dotprod_serial.c) which performs a 
*   dot product. The main data is made available to all threads through 
*   a globally accessible  structure. Each thread works on a different 
*   part of the data. The main thread waits for all the threads to complete 
*   their computations, and then it prints the resulting sum.
* SOURCE: Vijay Sonnad, IBM
* LAST REVISED: 01/29/09 Blaise Barney
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*   
The following structure contains the necessary information  
to allow the function "dotprod" to access its input data and 
place its output into the structure.  This structure is 
unchanged from the sequential version.
*/

typedef struct 
 {
   double      *a;
   double      *b;
   double     sum; 
   int     veclen; 
 } DOTDATA;

/* Define globally accessible variables and a mutex */

#define NUMTHRDS 4
#define VECLEN 100000
   DOTDATA dotstr; 
   pthread_t callThd[NUMTHRDS];
   pthread_mutex_t mutexsum;


int a[4][4];
static const unsigned int n = 4;

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


void *shear_sort_mt(void *arg)
{
    long offset;
    offset = (long)arg;
   
    if (offset %2) rowrevsort(offset); // odd
    else rowsort(offset); // even

    colsort(offset);
    
    if (offset %2) rowrevsort(offset); // odd
    else rowsort(offset); // even
    
    colsort(offset);
    
    if (offset %2) rowrevsort(offset); // odd
    else rowsort(offset); // even
    
    pthread_exit((void*) 0);
}
   
   
   
   
/*
The function dotprod is activated when the thread is created.
As before, all input to this routine is obtained from a structure 
of type DOTDATA and all output from this function is written into
this structure. The benefit of this approach is apparent for the 
multi-threaded program: when a thread is created we pass a single
argument to the activated function - typically this argument
is a thread number. All  the other information required by the 
function is accessed from the globally accessible structure. 
*/

void *shear_sort(void *arg)
{
    long offset;
    offset = (long)arg;
   
    pthread_mutex_lock (&mutexsum);
    if (offset %2) rowrevsort(offset); // odd
    else rowsort(offset); // even
    pthread_mutex_unlock (&mutexsum);

    pthread_mutex_lock (&mutexsum);
    colsort(offset);
    pthread_mutex_unlock (&mutexsum);
    
    pthread_mutex_lock (&mutexsum);
    if (offset %2) rowrevsort(offset); // odd
    else rowsort(offset); // even
    pthread_mutex_unlock (&mutexsum);
    
    pthread_mutex_lock (&mutexsum);
    colsort(offset);
    pthread_mutex_unlock (&mutexsum);
    
    pthread_mutex_lock (&mutexsum);
    if (offset %2) rowrevsort(offset); // odd
    else rowsort(offset); // even
    pthread_mutex_unlock (&mutexsum);
    
    pthread_exit((void*) 0);
}

/* 
The main program creates threads which do all the work and then 
print out result upon completion. Before creating the threads,
The input data is created. Since all threads update a shared structure, we
need a mutex for mutual exclusion. The main thread needs to wait for
all threads to complete, it waits for each one of the threads. We specify
a thread attribute value that allow the main thread to join with the
threads it creates. Note also that we free up handles  when they are
no longer needed.
*/

int main (int argc, char *argv[])
{
    long p;
    //double *a, *b;
    void *status;
    pthread_attr_t attr;

    /* Assign storage and initialize values */

    /*a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
    b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));

    for (i=0; i<VECLEN*NUMTHRDS; i++) {
    a[i]=1;
    b[i]=a[i];
    }

    dotstr.veclen = VECLEN; 
    dotstr.a = a; 
    dotstr.b = b; 
    dotstr.sum=0;*/
    
    
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

    pthread_mutex_init(&mutexsum, NULL);

    /* Create threads to perform the dotproduct  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(p=0;p<NUMTHRDS;p++)
    {
        /* Each thread works on a different set of data.
         * The offset is specified by 'i'. The size of
         * the data for each thread is indicated by VECLEN.
         */
         pthread_create(&callThd[p], &attr, shear_sort_mt, (void *)p); 
     }

    pthread_attr_destroy(&attr);
    /* Wait on the other threads */

    for(p=0;p<NUMTHRDS;p++) {
        pthread_join(callThd[p], &status);
    }
    /* After joining, print out the results and cleanup */

    for(i=0;i<n;i++){
        for(j=0;j<n;j++) printf("%4d ", a[i][j]);
        printf("\n");
    }
    //free (a);
    //free (b);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}   
