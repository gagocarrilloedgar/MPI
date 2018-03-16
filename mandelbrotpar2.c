#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
 

void linspace(double a, double b, int n, double p[]);
void matrix(int n, double p[],double g[]);
int iteracions (double cx, double cy,int max_it);
void checkr(int r,char *txt);
int quants();
int whoami();
int worksplit(int start, int end,int P,int whoami,int *mystart, int *myend);

 
int main(int argc , char **argv){
 
int n=1000, x0=-2, x1=1,start=0,end=1000, mystart,myend;
double cx = 0.0,cy = 0.0,*x,*y,zx=0.0,zy=0.0,y0=-1.5,y1=1.5;
int thisk,info=1,max_it=100,width,height;
const int root =0;
int *pos,*gvalue,*k;
MPI_Status st; /* allows to check the tag (if needed) */
// *(p+(i-1)+(j-1)*4); --> matriz

checkr(MPI_Init(&argc,&argv),"init");

double time=MPI_Wtime();

 x=(double *)malloc(sizeof(double)*n);
 y=(double *)malloc(sizeof(double)*n);

    if (x==NULL || y==NULL) 
    {
        printf("Out of memory\n");
        exit(1);
    }
    
// generamos el vector y el espacio de forma uniforme
linspace(x0,x1,n,x); 
linspace(y0,y1,n,y);


worksplit(start,end,quants(),whoami(),&mystart,&myend);
printf("quisoc=%d quants=%d mystart=%d myend=%d \n",whoami(),quants(),mystart,myend);

	width=n;
	height=myend-mystart+1;

	k=(int *)malloc(sizeof(int)*width*height); // initial matrix 

    if (k==NULL ) 
    {
        printf("k Out of memory\n");
        exit(1);
    }

	// para cada procesador calculamos el y guardamos en k solo las filas necesarias 

	for (int px = 0; px <height; ++px)
	{
		for (int py=0; py<width; ++py)
			{
				thisk=iteracions(x[px],y[py+mystart],max_it);
				*(k+(py)+(px)*width)=thisk;
				/*printf("k=%d \n",*(k+(py)+(px)*width));*/
			}	
	}
	
	
	// enviamos la inforamación al procesador 0;
	if (whoami()==root)
	{
		pos=(int *)malloc(sizeof(int)*n*n);
		for (int px = 0; px <height; ++px)
		{
			for (int py=0; py<width; ++py)
				{
					*(pos+(py)+(px)*width)=*(k+(py)+(px)*width);
				}	
		}
		
		for (int i = 1; i <quants(); ++i)
		{
			worksplit(start,end,quants(),i,&mystart,&myend);
			height=myend-mystart+1;

			MPI_Status st;
			checkr(MPI_Recv(k,width*height,MPI_INT,i,0,MPI_COMM_WORLD,&st),"receiving");

			for (int px = 0; px <height; ++px)
			{
				for (int py=0; py<width; ++py)
				{
					*(pos+(py)+(px+mystart)*width)=*(k+(py)+(px)*width);
				}	
			}
		}
	}

	else
	{
		checkr(MPI_Ssend(k,width*height,MPI_INT,0,0,MPI_COMM_WORLD),"sending" );
		
	}

	if(whoami()==root){
		printf("\n time=%f \n", MPI_Wtime()-time );

		for (int px = 0; px <n; ++px)
	{
		for (int py=0; py<n; ++py)
			{
			
				printf("x=%d y=%d pos(i,j)=%d \n",px,py,*(pos+py+px*width));
			}	
	}
		free (pos);
	}

	

free (x); free (y); free (k);
MPI_Finalize();

exit(0);
}
 
int iteracions (double cx, double cy,int max_it){
 
double zx=0.0, zy=0.0,zx2=0.0,zy2=0.0;
 int k=0;

	for (int i=1; i<=max_it; i++){
    	zx2 = cx + zx*zx-zy*zy;
    	zy2 = cy + zx*zy*2;
    	zx=zx2;
    	zy=zy2;

    	if (zx*zx+zy*zy > 4)
    	{
        	k= max_it-i;
        	break;
    	}
	}
	return k;
} 

void linspace(double a, double b, int n, double p[])
{
	double c;
	c=(b - a)/((double)n-1);

	for (int i = 0; i <=n-1; ++i)
	{
		*(p+i)= a+(double)i*c;
	}

	return;
}

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

int worksplit(int start, int end,int P,int whoami,int *mystart, int *myend){

	int N=0,x=0,r=0;

	*mystart=start;
	N=end-start;

	if(N<P) return(0); // not ok 
	if(end<start) return(0); // not ok

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
	return (1);
}
	
