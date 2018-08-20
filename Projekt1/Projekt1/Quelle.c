/*
* Heiner B�scher, Steffen Tietzel
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
#include <string.h>

//Funktion zum erstellen einer 2D-Matrix
int ** createMatrix(int rows, int columns) {
	printf("Create Matrix with %d rows and %d columns\n", rows, columns);
	int **ret;
	ret = malloc(rows * sizeof *ret);
	if (!ret) { perror("Error: "); exit(EXIT_FAILURE); }
	for (int i = 0; i<columns; i++) {
		ret[i] = malloc(columns * sizeof *ret[0]);
		if (!ret[i]) { perror("Error: "); exit(EXIT_FAILURE); }
	}
	return ret;
}

//Funktion zum einlesen der Job Textdatei
int ** readjobs(FILE* fp, int linesToRead, int facilityCount) {
	int ch = 0;
	int rows = 0;
	while ((ch = fgetc(fp)) != '\n')
	{
		rows = ch - 48;
		//rows = rows * 10 + (ch - 48);
	}

	int ** array = createMatrix(rows, facilityCount);
	int i = 0, j = 0;
	printf("\nReading file\n");
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
		{
			i++;
			j = 0;
		}
		else if (ch == ' ')
		{
			j++;
		}
		else
		{
			//array[i][j] = (array[i][j] * 10) + (ch - 48);
			array[i][j] = ch - 48;
		}
	}
	return array;
}

int main(int argc, char** argv) {

	int facilities_count=-1;
	int jobs_count=-1;
	MPI_Init(&argc, &argv);

	/*
	int MPI_Send(void *buf, int count,
		MPI_Datatype datatype, int dest, int
		tag, MPI_Comm comm);

	int MPI_Recv(void *buf, int count,
		MPI_Datatype datatype, int source, int
		tag, MPI_Comm comm, MPI_Status *status);
	*/
	

	
	FILE *fp;	//Zeiger f�r Datei
	fp = fopen("jobs.txt", "r");	//Dateizugriff, Datei als read 
	
	if (fp == NULL) {	//falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	//Datei konnte geoeffnet werden
		printf("Datei ist lesbar\n");

		int ** jobs = readjobs(fp, 6, 6);

		if (jobs == NULL)printf("nullpionter");
		else {
			for (int i = 0; i < 6; i++) {
				printf("\n");
				for (int j = 0; j < 6; j++) {
					printf("%i ", jobs[i][j]);
				}
			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}

	MPI_Finalize();
	getchar();
	return 0;
}