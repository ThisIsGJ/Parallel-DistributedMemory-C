#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv){
	 
	int rc,myrank,nproc,dimension;
	int* flag = 0;
	double precision = 1.0;
	dimension = 5;
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
  	double* copyArray;
  	double* midArray;
  	int line_per = (dimension-2)/nproc;
  	if(myrank == nproc-1){
  		subArray = malloc(dimension*(dimension-myrank*line_per)*sizeof(double));
  		copyArray = malloc(dimension*(dimension-myrank*line_per)*sizeof(double));
  		midArray = malloc((dimension-2)*(dimension-myrank*line_per-2)*sizeof(double));  		
  	}else{
  		subArray = malloc(dimension*(line_per+2)*sizeof(double));
  		copyArray = malloc(dimension*(line_per+2)*sizeof(double));
  		midArray = malloc((dimension-2)*line_per*sizeof(double));  		  	
  	}

  	if(myrank == root){
  		for (int i = 0; i < dimension*dimension; i++){	
				iArray[i] = rand()%50;
    	}	
	    	if (myrank == 0){
			printf("next1!!!!!!!!!\n");
			for (int i = 0; i < dimension; i++){
				for (int j = 0; j < dimension; j++) { 
					printf("%f   ",iArray[i*dimension+j]);
				}
				printf("\n");
			}
	
		}	
  	}

	if(myrank == root){
    	
		for (int i = 0; i < dimension*(line_per+2); i++){	
				subArray[i] = iArray[i];
    	}

    	for (int i = 1; i < nproc; ++i)
    	{	
    		if(i < (nproc-1)){
    			MPI_Send((iArray+(dimension*i*line_per)),dimension*(line_per+2),MPI_DOUBLE,i,i,MPI_COMM_WORLD);
    		}else{
    		 	MPI_Send((iArray+(dimension*i*line_per)),(dimension-i*line_per)*dimension,MPI_DOUBLE,i,i,MPI_COMM_WORLD);	
    		 }
    	}
	}else{

		MPI_Status stat;
		if(myrank != (nproc-1)){
			MPI_Recv(subArray,dimension*(line_per+2),MPI_DOUBLE,root,myrank,MPI_COMM_WORLD,&stat);	
		}else{
  			MPI_Recv(subArray,(dimension-myrank*line_per)*dimension,MPI_DOUBLE,root,myrank,MPI_COMM_WORLD,&stat);	   		
		}	
	}

	if(myrank != nproc-1){
		for (int i = 0; i < line_per+2; i++){
			for (int j = 0; j < dimension; j++) { 
				copyArray[i*dimension+j] = subArray[i*dimension+j];
			}
		 }
	 }else{
	 	for(int i = 0; i < (dimension-myrank*line_per); i++){
			for (int j = 0; j < dimension; j++) { 
				copyArray[i*dimension+j] = subArray[i*dimension+j];
			}
		 }
	 }

	 for (int i = 1; i < line_per+1; i++){
	 	for (int j = 1; j < dimension-1; j++) {		
				double newValue = (copyArray[(i+1)*dimension+j] + copyArray[(i-1)*dimension+j]
								+ copyArray[i*dimension+j+1] + copyArray[i*dimension+j-1])/4;
			*(midArray+(i-1)*dimension+j-1) = newValue;
			// if(flag == 0){
			// 	if((copyArray[i][j] - newValue) >= precision){
			// 		*flag = 1;
			// 		MPI_Bcast(flag,1,MPI_INT,myrank,MPI_COMM_WORLD);
			// 	}
			// }

		}
	}

	if(myrank != root){
		if(myrank != (nproc-1)){
			MPI_Send(midArray,(dimension-2)*line_per,MPI_DOUBLE,root,myrank,MPI_COMM_WORLD);	
		}else{
			MPI_Send(midArray,(dimension-myrank*line_per-2)*(dimension-2),MPI_DOUBLE,root,myrank,MPI_COMM_WORLD);	   		
		}
		
	}else{

		for (int i = 0; i < line_per; i++)
		{
			for(int j = 1; j < dimension-1; j++)
			{
				*(iArray+(i+1)*dimension+j) = midArray[i*dimension+j-1];
			}
		}
		
		MPI_Status stat;
		for (int i = 1; i < nproc; ++i){	
    		if(i < (nproc-1)){
    			int len = (dimension-2)*line_per;
    			double arr[len];
				MPI_Recv(arr,(dimension-2)*line_per,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&stat);
				
				for (int p = 0; p < line_per; p++)
				{
					for (int j = 1; j < dimension-1; j++)
					{
						*(iArray+(p+i*line_per+1)*dimension+j) = arr[p*dimension+j-1];
					}
				}
			}else{
				int len = (dimension-i*line_per-2)*(dimension-2);
				double arr[len];
		 		MPI_Recv(arr,(dimension-i*line_per-2)*(dimension-2),MPI_DOUBLE,i,i,MPI_COMM_WORLD,&stat);	
				
				for (int p = 0; p < (dimension-i*line_per-2); p++)
				{
					for (int j = 1; j < dimension-1; j++)
					{
						*(iArray+(p+i*line_per+1)*dimension+j) = arr[p*dimension+j-1];
					}

				}
			}
		}
	}
	


	MPI_Barrier(MPI_COMM_WORLD);

	if (myrank == root){
		printf("NEW!!!!!!!!!\n");
		for (int i = 0; i < dimension; i++){
			for (int j = 0; j < dimension; j++) { 
				printf("%f   ",iArray[i*dimension+j]);
			}
			printf("\n");
		}
	}




	//  if(myrank == nproc - 1){
	//  	new_len = (dimension-myrank*line_per-2)+1;
	// }else{
	// 	new_len = line_per + 1;
	// }


	// if (myrank == 0){
	// 	for (int i = 0; i < dimension; i++){
	// 		for (int j = 0; j < dimension; j++) { 
	// 			printf("%f   ",iArray[i*dimension+j]);
	// 		}
	// 		printf("\n");

	// 	}
	// 	printf("next1!!!!!!!!!\n");
	// }

	// if(myrank != nproc-1){
	// 	printf("-------------\n");
	// 	for (int i = 0; i < line_per+2; i++){
	// 		for (int j = 0; j < dimension; j++) { 
	// 			printf("%f   ",subArray[i*dimension+j]);
	// 		}
	// 	printf("\n");
	// 	 }
	//  }else{
	//  	printf("-------------\n");
	//  	for (int i = 0; i < (dimension-myrank*line_per); i++){
	// 		for (int j = 0; j < dimension; j++) { 
	// 			printf("%f   ",subArray[i*dimension+j]);
	// 		}
	// 	printf("\n");
	// 	 }
	//  }
	 

	// namelen = LEN;
	// MPI_Get_processor_name(name, &namelen);
	// printf("hello world %d", myrank);
	
	/* implicit barrier in Finalize */
	/*MPI_Barrier(MPI_COMM_WORLD);*/



	MPI_Finalize();
	return 0;
}





































