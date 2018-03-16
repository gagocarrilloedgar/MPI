#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void checkr(int r,char *txt) {
  if (r!=MPI_SUCCESS) {
    fprintf(stderr,"Error: %s\n",txt);
    exit(-1);
  }
}
 
int quisoc() {
  int a,b;
  a=MPI_Comm_rank(MPI_COMM_WORLD,&b);
  checkr(a,"quisoc");
  return(b);
}
 
int quants() {
  int a,b;
  a=MPI_Comm_size(MPI_COMM_WORLD,&b);
  checkr(a,"quants");
  return(b);
}
 

void worksplit(int start, int end,int P,int whoami,int *mystart, int *myend) {

	int N=0,x=0,r=0;

	*mystart=start;
	N=end-start;

	for (int i =0; i<=P-1; ++i)
	{
		x=N/P;
		r=N%P;

		if (i<r){
			x=x+1;
		}

		*myend=*mystart+x-1;

		if (i==whoami){
			break;
		}

		*mystart=*myend+1;	
	
	}
	return;
}
	


 

int main(int argc, char  **argv){
	int a=1 /*start*/ ,b=1005,/*end*/ x=0,v=0;
	int p; // será la variable que contenga la dirección del contador que utilizaremos para dividir;
	int mystart;
	int myend,g,whoami,P,start=0,end=4;



	g=MPI_Init(&argc,&argv);
	checkr(g,"init");
	

	worksplit(start,end,quants(),quisoc(),&mystart,&myend);

	printf("mystart=%d  myend=%d quisoc=%d \n",mystart,myend,quisoc());

	MPI_Finalize();
	exit (0);
}
