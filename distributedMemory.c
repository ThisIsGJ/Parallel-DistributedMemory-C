#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

int printArray(double* array, int dimension){
	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++) { 
			printf("%f\t",array[i*dimension+j]);
		}
		printf("\n");
	}
	return 0;
}

int main(int argc, char **argv){

	int rc,myrank,nproc,dimension,new_len;
	// use flag to test if the precision have achieved
	// flag = 1 mean there have some value do not achieve the precision 
	// flag = 0 mean all the value have achieved the precision and work have done
	int flag = 1;
	//initialized the precision
	double precision = 1;
	//initialized the dimension
	dimension = 5;
	const int root = 0;
	
	rc = MPI_Init(&argc,&argv);
	if(rc != MPI_SUCCESS){
		printf("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD,rc);
	}
 	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  	//malloc the space to array
  	double* iArray = malloc(dimension*dimension*sizeof(double));
	double* subArray;
  	double* copyArray;
  	double* midArray; 

  	int line_per = (dimension-2)/nproc;
  	if(myrank != nproc-1){
		subArray = malloc(dimension*(line_per+2)*sizeof(double));
		copyArray = malloc(dimension*(line_per+2)*sizeof(double));
		midArray = malloc((dimension-2)*line_per*sizeof(double)); 
  	}else{
		subArray = malloc(dimension*(dimension-myrank*line_per)*sizeof(double));
		copyArray = malloc(dimension*(dimension-myrank*line_per)*sizeof(double));
		midArray = malloc((dimension-2)*(dimension-myrank*line_per-2)*sizeof(double)); 			 		  	
  	}

  	srand(time(NULL));
  	//initialized the start array
  	if(myrank == root){
  		for (int i = 0; i < dimension*dimension; i++){	
				iArray[i] = rand()%100;
    	}	
	    	if (myrank == 0){
	    		printf("Initial Array:\n");
	    		printArray(iArray,dimension);
	    	}	
  	}

  	// when the flag = 0, main stop and work done.
  	while(flag == 1){
		if(myrank == root){
			for (int i = 0; i < dimension*(line_per+2); i++){subArray[i] = iArray[i];}

			//send the date to other processor	
	    	for (int i = 1; i < nproc; ++i){	
	    		if(i < (nproc-1)){
	    			MPI_Send((iArray+(dimension*i*line_per)),dimension*(line_per+2),
	    						MPI_DOUBLE,i,i,MPI_COMM_WORLD);
	    		}else{
	    		 	MPI_Send((iArray+(dimension*i*line_per)),(dimension-i*line_per)*dimension,
	    		 			MPI_DOUBLE,i,i,MPI_COMM_WORLD);	
	    		 }
	    	}

		}else{
			//receive data from root
			MPI_Status stat;
			if(myrank != (nproc-1)){
				MPI_Recv(subArray,dimension*(line_per+2),
						MPI_DOUBLE,root,myrank,MPI_COMM_WORLD,&stat);	
			}else{
	  			MPI_Recv(subArray,(dimension-myrank*line_per)*dimension,
	  					MPI_DOUBLE,root,myrank,MPI_COMM_WORLD,&stat);	   		
			}	
		}

		//creat a copyArray to copy array from subArray 
		//in order to calculate the average number
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
		if(myrank == nproc - 1){
		 	new_len = (dimension-myrank*line_per-2)+1;
		}else{
			new_len = line_per + 1;
		}
		//calculate the average
		 for (int i = 1; i < new_len; i++){
		 	for (int j = 1; j < dimension-1; j++) {		
					double newValue = (copyArray[(i+1)*dimension+j] + copyArray[(i-1)*dimension+j]
									+ copyArray[i*dimension+j+1] + copyArray[i*dimension+j-1])/4;
				*(midArray+(i-1)*(dimension-2)+j-1) = newValue;
			}
		}

		//processor send data back to root
		if(myrank != root){
			if(myrank != (nproc-1)){
				MPI_Send(midArray,(dimension-2)*line_per,MPI_DOUBLE,root,myrank,MPI_COMM_WORLD);	
			}else{	
				MPI_Send(midArray,(dimension-myrank*line_per-2)*(dimension-2),MPI_DOUBLE,root,myrank,MPI_COMM_WORLD);	   		
			}	
		}else{
			flag = 0;
			for (int i = 0; i < line_per; i++){
				for(int j = 1; j < dimension-1; j++){
					if(fabs(iArray[(i+1)*dimension+j] - midArray[i*(dimension-2)+j-1]) >= precision){
								flag = 1;
							}
					*(iArray+(i+1)*dimension+j) = midArray[i*(dimension-2)+j-1];
				}
			}

			//root receive updated data from other processor 
			//calculate the precision 
			MPI_Status stat;
			for (int i = 1; i < nproc; ++i){	
	    		if(i < (nproc-1)){
	    			int len = (dimension-2)*line_per;
	    			double arr[len];
					MPI_Recv(arr,len,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&stat);
					
					for (int p = 0; p < line_per; p++){
						for (int j = 1; j < dimension-1; j++){
							if(fabs(iArray[(p+i*line_per+1)*dimension+j] - arr[p*(dimension-2)+j-1]) >= precision){
								flag = 1;
							}
							*(iArray+(p+i*line_per+1)*dimension+j) = arr[p*(dimension-2)+j-1];
						}
					}
				}else{
					int len = (dimension-i*line_per-2)*(dimension-2);
					double arr[len];
			 		MPI_Recv(arr,len,MPI_DOUBLE,i,i,MPI_COMM_WORLD,&stat);	
					
					for (int p = 0; p < (dimension-i*line_per-2); p++){
						for (int j = 1; j < dimension-1; j++){
							if(fabs(iArray[(p+i*line_per+1)*dimension+j] - arr[p*(dimension-2)+j-1]) >= precision){
								flag = 1;
							}
							*(iArray+(p+i*line_per+1)*dimension+j) = arr[p*(dimension-2)+j-1];
						}

					}
				}
			}
		}

		if(myrank == 0){
			printf("The new Array:\n");
			printArray(iArray,dimension);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&flag,1,MPI_INT,root,MPI_COMM_WORLD);
	}

	free(subArray);
  	free(copyArray);
  	free(midArray);
  	free(iArray);

	MPI_Finalize();
	return 0;
}





































