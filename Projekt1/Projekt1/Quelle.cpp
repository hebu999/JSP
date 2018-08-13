/*
* Heiner Büscher, Steffen Tietzel
* Programm zur parallelisierung des Job-Shop Problems
*
*
*
*/

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {

	int facilities_count;
	int jobs_count;

	const int facilities[] = {1};

	const int jobs[] = { 1 };

	MPI_Init(&argc, &argv);

	int MPI_Send(void *buf, int count,
		MPI_Datatype datatype, int dest, int
		tag, MPI_Comm comm);

	int MPI_Recv(void *buf, int count,
		MPI_Datatype datatype, int source, int
		tag, MPI_Comm comm, MPI_Status *status);

	//hier kommt noch Code hin, der bestimmt ganz Toll sein wird

	MPI_Finalize();

	FILE *fp;

	fp = fopen("jobs.txt", "r");

	if (fp == NULL) {

		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {

		//TODO hier kommt auch noch was hin
		

		fclose(fp);
	}

	return 0;

}