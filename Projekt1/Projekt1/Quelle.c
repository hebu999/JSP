/*
* Heiner Büscher, Steffen Tietzel
* Programm zur parallelisierung des Job-Shop Problems
* 
* 02.08.18
*
*/
#define _CRT_SECURE_NO_DEPRECATE

#define _GNU_SOURCE
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <Windows.h>

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
readjobs(FILE* fp, int ***array, int *jobcount,  int facilityCount, int linesToRead) {
	int ch = 0;
	fscanf(fp, "%i", jobcount);
	printf("read %i jobs\n", *jobcount);
	if (linesToRead < *jobcount) *jobcount = linesToRead;
	*array = createMatrix(*jobcount, facilityCount);
	printf("Reading Jobs\n");

	for (int m = 0; m < *jobcount; m++) {
		for (int n = 0; n < facilityCount; n++) {
			fscanf(fp, "%i", &ch);
			(*array)[m][n] = ch;
		}
	}
}
//Facilitys werden aus der Textdatei eingelesen
readfacilitys(FILE* fp, int **array, int *facilityCount) {
	int ch = 0;
	fscanf(fp, "%i", facilityCount);
	*array = malloc(*facilityCount * sizeof *array);
	if (!*array) { perror("Error: "); exit(EXIT_FAILURE); }
	printf("Reading Facilitys\n");

	for (int m = 0; m < *facilityCount; m++) {
		fscanf(fp, "%i", &ch);
		(*array)[m] = ch;
	}
}

//Funktion zum lösen des Problems
solveProblem(int * facilities, int **jobs) {

	int ***allTask;

	for (int i = 0; i < (*jobs)[i]; i++)
	{
		
		 



	}

}

int main(int argc, char** argv) {
	
	// MPI wird initalisiert
	MPI_Init(&argc, &argv);
	
	if (argc < 3) {
		printf("nicht ausreichend parameter bei Programmaufruf\nProgramm wird beendet\n");
		system("pause");
		exit(1);
	}
	int linesToRead;
	if (argv[1])  linesToRead = argv[1];
	int verbosity;
	if (argv[2]) verbosity = argv[2];
	/*
	int MPI_Send(void *buf, int count,
	MPI_Datatype datatype, int dest, int
	tag, MPI_Comm comm);

	int MPI_Recv(void *buf, int count,
	MPI_Datatype datatype, int source, int
	tag, MPI_Comm comm, MPI_Status *status);
	*/

	int *facilities;
	int **jobs;
	int facilitycount;
	int jobcount;

	//Zeiger für Datei
	FILE *fp;
	
	fp = fopen("facilities.txt", "r");	// Dateizugriff, Datei als read zugegriffen
	if (fp == NULL) {	// falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	// Datei konnte geoeffnet werden
		printf("facilities.txt ist lesbar\n");
		readfacilitys(fp, &facilities, &facilitycount);
		if (facilities == NULL) printf("facilities - nullpointer");
		else {
			for (int i = 0; i < facilitycount ; i++) {
				printf("%i \n", facilities[i]);
			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}

	fp = fopen("jobs.txt", "r");	//Dateizugriff, Datei als read 
	if (fp == NULL) {	//falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	//Datei konnte geoeffnet werden
		printf("jobs.txt ist lesbar\n");
		readjobs(fp,&jobs,&jobcount, facilitycount, linesToRead);
		if (jobs == NULL) printf("jobs - nullpointer");
		else {
			for (int i = 0; i < jobcount; i++) {
				for (int j = 0; j < facilitycount; j++) {
					printf("%i ", jobs[i][j]);
				}
				printf("\n");
			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}
	// funktionsblock um die Jobs darzustellen
	if (verbosity == 1 || verbosity == 3) {
		
	}

	// funktionsblock um die Zeit darzustellen
	if (verbosity == 2 || verbosity == 3) {
		
	}
	
	MPI_Finalize();
	
	system("pause");
	return 0;
}