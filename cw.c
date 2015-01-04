#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>
#define LEN (128)

int main(int argc, char **argv){
	 
	int rc,myrank,nproc,namelen,dimension;
	dimension = 5;
  	double iArray[dimension*dimension];
	char name[LEN];
	const int root = 0;

	rc = MPI_Init(&argc,&argv);
	if(rc != MPI_SUCCESS){
		printf("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD,rc);
	}

 	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  	//initial a 2D array
  	srand(time(NULL));
  	// iArray = malloc((dimension * dimension) * sizeof(double));

	if(myrank == root){
		// printf("main reports %d procs\n", nproc);
		// iArray = {{0.0,0.0,0.0,0.0,0.0},{0.0,1.0,1.0,1.0,0.0,0.0},
	 //           {0.0,2.0,2.0,2.0,0.0},{0.0,3.0,3.0,3.0,0.0},{0.0,0.0,0.0,0.0,0.0}};

		for (int i = 0; i < dimension*dimension; i++){	
				iArray[i] = rand()%50;
    	}	                
	}

	MPI_Bcast(iArray,dimension*dimension,MPI_DOUBLE,root,MPI_COMM_WORLD); 
	MPI_Barrier(MPI_COMM_WORLD);



	for (int i = 0; i < dimension; i++){
			for (int j = 0; j < dimension; j++) { 
				printf("%f   ",iArray[i+j]);
			}
			printf("\n");
	}
	printf("\n");
	namelen = LEN;

	// MPI_Get_processor_name(name, &namelen);
	// printf("hello world %d", myrank);
	/* implicit barrier in Finalize */
	/*MPI_Barrier(MPI_COMM_WORLD);*/



	MPI_Finalize();
	return 0;
}





































