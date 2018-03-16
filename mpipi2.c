#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"
#include <time.h>
#include <sys/time.h>
 
// we use this function to check the return value of every MPI call
void checkr(int r,char *txt) {
  if (r!=MPI_SUCCESS) {
    fprintf(stderr,"Error: %s\n",txt);
    exit(-1);
  }
}

int quants() {
  int a,b;
  a=MPI_Comm_size(MPI_COMM_WORLD,&b);
  checkr(a,"quants");
  return(b);
}

int whoami() {
  int a,b;
  a=MPI_Comm_rank(MPI_COMM_WORLD,&b);
  checkr(a,"whoami");
  return(b);
}

void worksplit(int start, int end,int *pos1, int *pos2) {

  int i=0,N=0,x=0,p=0;

  N=end-start;
  p=N%quants();

  if (p==0){
    *pos1=i;
    *pos2=N/quants();
  }

  else{
    while (x==0){
      if(p==i){
        *pos1=i; // guardamos la posición de i
        *pos2 =((N-i)/quants()); // quardamos la posición de M --> nomber of tasks of each processor
        x=1;
      }

      else {
        i++;
      }
    }
  }
  return;
}
 

int tiran(int n){ // tira i dardos i compta el número de que hi ha dintre

  int in=0; 
  double pos[2],r=0.0, R=5.0;
  
    for(int i=1; i<=n; i++){
      pos[0]=drand48()*(R+R)-R; // dran48 --> random decimal numbers. 
      pos[1]=drand48()*(R+R)-R;

      r=(pos[0]*pos[0] + pos[1]*pos[1]);
      r=sqrt(r);

      if(r<=R) in++;
    }
      return in ; 
}

// devuleve "a-b" en segundos

double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
 
int main(int argc, char **argv){

  char name[MPI_MAX_PROCESSOR_NAME+1]; /* processor name */
  double pi=0.0;
  int N0=3/* start */,globalval=0,N1=789300 /*end */,/*i */extra,M,r,x,l,myrank; /* from 0 to P-1 */
  //x is the return of the function tiran( number of point inside the cercle)

  struct timeval t_ini, t_fin; // we will mesure the time we spend doing the tasks in order to improve it.
  double secs;

  r=MPI_Init(&argc,&argv); checkr(r,"init"); // allways at the begining

    gettimeofday(&t_ini,NULL);

      r=MPI_Comm_rank(MPI_COMM_WORLD,&myrank); checkr(r,"rank");

      /* if (argc!=2) { printf("montepi N\n"); MPI_Finalize(); exit(-1);}
      //  printf("argv[1]=%s \n",argv[1]); ara es un string
      sscanf(argv[1],"%d",&N0); // leemos el valor que va despues a.out para poder introducir el valor N1 
      sscanf(argv[2],"%d",&N1) */
  
      worksplit(N0,N1,&extra,&M); // we use pointer in order to make possible the output of more than one variable throw a function

      srand48(myrank); // we use this to get a random order of the proces execution

      if(myrank==(quants()-1)){
        x=tiran(M+extra);
        printf("whoami=%d dintre=%d M=%d extra=%d \n ",myrank,x,M+extra, extra);
      }
  
      else {
        x=tiran(M);
        printf("whoami=%d dintre=%d M=%d \n",myrank,x,M);
      }

      r=MPI_Reduce(&x, &globalval, 1, MPI_INT, MPI_SUM,1/*destination*/,MPI_COMM_WORLD);
      pi=4.*(globalval / (1.*(N1-N0)));
 
      if(myrank==1)
        printf("REDUCE SUM:whoami=%d globalval=%d pi=%f \n" ,myrank, globalval, pi); 

    gettimeofday(&t_fin, NULL);

    secs = timeval_diff(&t_fin, &t_ini);
    printf("%.16g milliseconds\n",secs*1000.0 );

  MPI_Finalize();
 
  exit(0);
}
