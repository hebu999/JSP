/*
* Heiner Büscher, Steffen Tietzel
* Programm zur parallelisierung des Job-Shop Problems
*
*
*
*/
#define _CRT_SECURE_NO_DEPRECATE
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void calcDimensions(FILE* fp, int& row, int& maxcolumns, int& maxDigits) {
	int ch = 0;
	int digits = 0;
	int columns = 1;

	row++;
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
		{
			row++;
			if (columns > maxcolumns) maxcolumns = columns;
			columns = 1;
			if (digits > maxDigits) maxDigits = digits;
			digits = 0;
		}
		else if (ch == ' ')
		{
			columns++;
			if (digits > maxDigits) maxDigits = digits;
			digits = 0;
		}
		else
		{
			digits++;
		}
	}
}

/*int calcDimensions(FILE* fp, int* size) {
	int ch = 0;
	int i = 0;

	int m = 0;
	int n = 0;

	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
		{

		}
		else if (ch == ' ')
		{


		}
		else
		{
			num[i] = ch;
			i++;
		}
	}
}
*/

int main(int argc, char** argv) {

	int facilities_count=-1;
	int jobs_count=-1;

	const int facilities[] = {1};


	MPI_Init(&argc, &argv);

	/*
	int MPI_Send(void *buf, int count,
		MPI_Datatype datatype, int dest, int
		tag, MPI_Comm comm);

	int MPI_Recv(void *buf, int count,
		MPI_Datatype datatype, int source, int
		tag, MPI_Comm comm, MPI_Status *status);
	*/

	//hier kommt noch Code hin, der bestimmt ganz Toll sein wird



	FILE *fp;
	fp = fopen("jobs.txt", "r");

	const int jobs[] = { 1 };
	if (fp == NULL) {
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {
		printf("Datei ist lesbar\n");
		int jobRows = 0;
		int jobColums = 0;
		int jobMaxDigits = 0;
		calcDimensions(fp, jobRows, jobColums, jobMaxDigits);
		printf("%i, %i, %i", jobRows, jobColums, jobMaxDigits);
		
		fclose(fp);
	}


	MPI_Finalize();

	getchar();
	return 0;
}