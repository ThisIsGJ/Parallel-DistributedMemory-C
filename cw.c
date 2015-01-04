#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv){
	 
	int rc,myrank,nproc,dimension;
	dimension = 10;
	const int root = 0;
	srand(time(NULL));
	double* iArray = malloc(dimension*dimension*sizeof(double));

	rc = MPI_Init(&argc,&argv);
	if(rc != MPI_SUCCESS){
		printf("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD,rc);
	}

 	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  	double* subArray;
  	int line_per = (dimension-2)/nproc;
  	if(myrank == nproc-1){
  		subArray = malloc(dimension*((dimension-myrank*line_per))*sizeof(double));
  	}else{
  		subArray = malloc(dimension*(line_per+2)*sizeof(double));
  	}

	if(myrank == root){
		// printf("main reports %d procs\n", nproc);
		// iArray = {{0.0,0.0,0.0,0.0,0.0},{0.0,1.0,1.0,1.0,0.0,0.0},
	 //           {0.0,2.0,2.0,2.0,0.0},{0.0,3.0,3.0,3.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
		for (int i = 0; i < dimension*dimension; i++){	
				iArray[i] = rand()%50;
    	}	
    	
    	for (int i = 0; i < dimension*(line_per+2); i++){	
				subArray[i] = iArray[i];
    	}

    	for (int i = 1; i < nproc; ++i)
    	{	
    		if(i < (nproc-1)){
    			MPI_Send((iArray+(dimension*(i*line_per))),dimension*(line_per+2),MPI_DOUBLE,i,i,MPI_COMM_WORLD);
    		}else{
    		 	MPI_Send((iArray+(dimension*i*line_per)),(dimension-i*line_per)*dimension,MPI_DOUBLE,i,i,MPI_COMM_WORLD);	
    		 }
    	}
	}else{
		if(myrank != 0){
			MPI_Status stat;
			if(myrank != (nproc-1)){
    			MPI_Recv(subArray,dimension*(line_per+2),MPI_DOUBLE,root,myrank,MPI_COMM_WORLD,&stat);	
    		}else{
      			MPI_Recv(subArray,(dimension-myrank*line_per)*dimension,MPI_DOUBLE,root,myrank,MPI_COMM_WORLD,&stat);	   		
    		}
		}
		
	}

	if (myrank == 0){
		for (int i = 0; i < dimension; i++){
			for (int j = 0; j < dimension; j++) { 
				printf("%f   ",iArray[i*dimension+j]);
			}
			printf("\n");

		}
		printf("next1!!!!!!!!!\n");
	}


	if(myrank != nproc-1){
		printf("-------------\n");
		for (int i = 0; i < line_per+2; i++){
			for (int j = 0; j < dimension; j++) { 
				printf("%f   ",subArray[i*dimension+j]);
			}
		printf("\n");
		 }
	 }else{
	 	printf("-------------\n");
	 	for (int i = 0; i < (dimension-myrank*line_per); i++){
			for (int j = 0; j < dimension; j++) { 
				printf("%f   ",subArray[i*dimension+j]);
			}
		printf("\n");
		 }
	 }
	 
	
	MPI_Barrier(MPI_COMM_WORLD);

	

	// namelen = LEN;
	// MPI_Get_processor_name(name, &namelen);
	// printf("hello world %d", myrank);
	
	/* implicit barrier in Finalize */
	/*MPI_Barrier(MPI_COMM_WORLD);*/



	MPI_Finalize();
	return 0;
}





































