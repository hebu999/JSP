/*
* Heiner Büscher, Steffen Tietzel
* Programm zur parallelisierung des Job-Shop Problems
*
*
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>


int main(int argc, char **argv) {

	int facilities;
	int operations;

	int MPI_Init(int *argc, char ***argv);

	//hier kommt noch Code hin, der bestimmt ganz Toll sein wird

	int MPI_Finalize(void);

	FILE *fp;

	fp = fopen("jobs.txt", "r");

	if (fp == NULL) {

		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {

		//TODO
		

		fclose(fp);
	}



}