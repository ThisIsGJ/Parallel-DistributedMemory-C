#include <stdio.h>
#include <mpi.h>
#define LEN (128)

int main(int argc, char **argv,int nu)
{
	 
  printf("%d\n", nu); 
	int rc,myrank,nproc,namelen;
	char name[LEN];

	rc = MPI_Init(&argc,&argv);
	if(rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD,rc);
	}

 	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);


  if (myrank == 0) 
  {
   	// printf("main reports %d procs\n", nproc);
    int iArray[4][4] = {{0,0,0,0},{0,1,1,0},
              {0,2,2,0},{0,0,0,0}};

      //  for (int i = 0; i < 4; i++)
      // {
      // for (int j = 0; j < 4; j++) 
      // { 
      //   printf("%d",iArray[i][j]);
      // }
      //   printf("\n");
      // }

              
	}
	
	namelen = LEN;
	MPI_Get_processor_name(name, &namelen);
	printf("hello world %d from '%s'\n", myrank , name);





	/* implicit barrier in Finalize */
	/*MPI_Barrier(MPI_COMM_WORLD);*/





	MPI_Finalize();
	return 0;
}





































