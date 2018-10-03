/*
* Heiner Büscher, Steffen Tietzel
* Programm zur parallelisierung des Clostest String Problems
* 
* 30.09.18
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

//Funktion zum einlesen der String Textdatei
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
//Strings werden aus der Textdatei eingelesen
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
solveProblem(int * facilities, int **jobs, int facilitycount, int *jobcount) {

}

int main(int argc, char** argv) {
	
	// MPI wird initalisiert
	MPI_Init(&argc, &argv);

	int *facilities;
	int **jobs;
	int facilitycount;
	int jobcount;
	int linesToRead;
	int process_count, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/*
	if (argc < 3) {
		printf("Nicht ausreichend Parameter bei Programmaufruf(Anzahl zu lesender Jobs, Verbosity), das Programm terminiert sich jetzt selbst\n");
		system("pause");
		exit(1);
	}
	*/
	
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

	//Zeiger für Datei
	FILE *fp;
	
	fp = fopen("facilities.txt", "r");	// Dateizugriff, Datei als read zugegriffen
	if (fp == NULL) {	// falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	// Datei konnte geoeffnet werden
		printf("facilities.txt ist lesbar\n");
		readfacilitys(fp, &facilities, &facilitycount);
		if (facilities == NULL) printf("Keine Facilities vorhanden!");
		else {
			for (int i = 0; i < facilitycount ; i++) {
				printf("%i \n", facilities[i]);
			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}

	linesToRead = 7;
	fp = fopen("jobs.txt", "r"); //Dateizugriff, Datei als read 
	if (fp == NULL) {	//falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	//Datei konnte geoeffnet werden
		printf("jobs.txt ist lesbar\n");
		readjobs(fp,&jobs,&jobcount, facilitycount, linesToRead);
		if (jobs == NULL) printf("Keine Jobs vorhanden!");
		else {
			solveProblem(&facilities, &jobs, facilitycount, jobcount);
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