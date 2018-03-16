#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main()
{
	double R=5.0;
	double r=0.0;
	double in=0.0;
	int i;
	double pos[2];
	double pi;
	int info=1; // para cambiar el resultado que sale del programa 
	for(i =1; i<=10000; i++){
		pos[0]=drand48() * (R-(-R)) - R;
		pos[1]=drand48() * (R-(-R)) - R;

		r = (pos[0]*pos[0] + pos[1]*pos[1]);
		r =sqrt(r);

		if(r<=R) in++; 

		pi = 4*(in/i);
		if (info==1) printf("%e %e\n",pos[0],pos[1]);
		if (info>1 && i%1000==0)
			printf("i=%d pi=%f \n",i,pi );

	}
	if (info==0) 
			printf("i=%d pi=%f \n",i,pi );

	return 0;
}
