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
#include <vector>
#include <string.h>

//Funktion zum einlesen der Job Textdatei
void readjobs(FILE* fp, int** array,int linesToRead, int facilityCount) {
	int ch = 0;
	int rows = 0;
	while ((ch = fgetc(fp)) != '\n')
	{
		rows = ch - 48;
		//rows = rows * 10 + (ch - 48);
	}

	if (rows > linesToRead) rows = linesToRead;

	array = (int**)malloc(rows * sizeof(int*));

	for (int i = 0; i < rows; i++) {
		/* size_y is the height */
		array[i] = (int*)malloc(facilityCount * sizeof(int));
	}
	int i = 0, j = 0;
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
		{
			i++;
			printf("\n");
		}
		else if (ch == ' ')
		{
			j++;
			printf(" ");
		}
		else	//wen es ne nummer ist
		{
			array[i][j] = ch - 48;
			printf("%i", array[i][j]);
			//array[i][j] = array[i][j] * 10 + (ch - 48);
		}
	}
}

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


	int** jobs = NULL;
	FILE *fp;	//Zeiger für Datei
	fp = fopen("jobs.txt", "r");	//Dateizugriff, Datei als read 

	
	if (fp == NULL) {	//falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	//Datei konnte geoeffnet werden
		printf("Datei ist lesbar\n");
		readjobs(fp, jobs, 6, 6);
		printf("\n\n%i", jobs[5][5]);
		/*for (int i = 0; i < 6; i++) {
			printf("\n");
			for (int j = 0; j < 6; j++) {
				printf("%d ", jobs[i][j]);
			}
		}*/
		fclose(fp);	//Dateizugriff wieder freigeben
	}


	MPI_Finalize();

	getchar();
	return 0;
}