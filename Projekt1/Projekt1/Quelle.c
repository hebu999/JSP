/*
* Heiner Büscher, Steffen Tietzel
* Programm zur parallelisierung des Closest-String Problems
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

//Funktion zum erstellen eines 2D-Matrix-Arrays
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
readJobs(FILE* fp, int ***array, int *jobcount,  int facilityCount, int linesToRead) {
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
readStrings(FILE* fp, int **array, int *facilityCount) {
	int ch = 0;
	fscanf(fp, "%i", facilityCount);
	*array = malloc(*facilityCount * sizeof *array);
	if (!*array) { perror("Error: "); exit(EXIT_FAILURE); }
	printf("Reading Strings\n");

	for (int m = 0; m < *facilityCount; m++) {
		fscanf(fp, "%i", &ch);
		(*array)[m] = ch;
	}
}

//Funktion zum berechnen der Hamming-Distanz
int hammingDistance(char *str1, char *str2) 
{
	int i = 0, count = 0;

	while (str1[i] != "")
	{
		if (str1[i] != str2[i])
			count++;
		i++;
	}
	return count;

}

//Funktion um korrekten String zu finden (Entwurf)
char findClosestString(int linesToRead, int **strings, int stringLength)
{
	char closestString = 0;

	for (int i = 0; i < linesToRead; i++)
	{
		for (int j = 0; j < stringLength; j++)
		{
			if (hammingDistance(strings[i][j], strings[i][j - 1] > 0))

			{
				closestString += 1;

			
			}

		}
	}
	return closestString;
}


int main(int argc, char** argv) {
	
	// MPI wird initalisiert
	MPI_Init(&argc, &argv);

	int *strings;
	int stringcount;
	int jobcount;
	int linesToRead;
	int process_count, rank;
	int verbosity;

	int distance;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/*
	if (argc < 3) {
		printf("Nicht ausreichend Parameter bei Programmaufruf(Anzahl zu lesender Strings, Verbosity), das Programm terminiert sich jetzt selbst\n");
		system("pause");
		exit(1);
	}
	*/
	
	if (argv[1])  linesToRead = argv[1];
	
	if (argv[2]) verbosity = argv[2];

	// funktionsblock um die Strings darzustellen
	if (verbosity == 1 || verbosity == 3) {

	}

	// funktionsblock um die Zeit darzustellen
	if (verbosity == 2 || verbosity == 3) {

	}
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
	
	fp = fopen("strings.txt", "r");	// Dateizugriff, Datei als read zugegriffen
	if (fp == NULL) {	// falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	// Datei konnte geoeffnet werden
		printf("strings.txt ist lesbar\n");
		readfacilitys(fp, &stringcount);
		if (&strings == NULL) printf("Keine Strings vorhanden!");
		else {
			for (int i = 0; i < stringcount ; i++) {
				printf("%i \n", strings[i]);
			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}

	linesToRead = 7;
	fp = fopen("strings.txt", "r"); //Dateizugriff, Datei als read 
	if (fp == NULL) {	//falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	//Datei konnte geoeffnet werden
		printf("jobs.txt ist lesbar\n");
		readStrings(fp,&jobs,&jobcount, stringcount, linesToRead);
		if (jobs == NULL) printf("Keine Jobs vorhanden!");
		else {
			for (int i = 0; i < jobcount; i++) {
				for (int j = 0; j < stringcount; j++) {
					printf("%i ", jobs[i][j]); //TODO an Strings anpassen
				}
				printf("\n");
			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}

	MPI_Finalize();
	
	system("pause");
	return 0;
}