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
#include <time.h>

//Funktion zum erstellen eines 2D-Matrix-Arrays
int ** createMatrix(int rows, int columns) {
	printf("Create Matrix with %d rows and %d columns\n", rows, columns);
	int **ret;
	ret = malloc(rows * sizeof *ret);
	if (!ret) { perror("Error: "); exit(EXIT_FAILURE); }
	for (int i = 0; i<rows; i++) {
		ret[i] = malloc(columns * sizeof *ret[0]);
		if (!ret[i]) { perror("Error: "); exit(EXIT_FAILURE); }
	}
	return ret;
}

unsigned long long power(int base, unsigned int exp) {
	unsigned long long  result = 1;
	for (unsigned int i = 0; i < exp; i++)
		result *= base;
	return result;
}

//Strings werden aus der Textdatei eingelesen
readStrings(FILE* fp, int ***array, int *stringcount, int *stringlength, int linesToRead) {
	char ch = 0;
	fscanf(fp, "%i", stringcount);
	printf("stringcount: %i\n", *stringcount);
	fscanf(fp, "%i", stringlength);
	printf("stringlength: %i\n", *stringlength);
	fscanf(fp, "%c", &ch);
	*array = createMatrix(*stringcount, *stringlength);

	if (!*array) { perror("Error: "); exit(EXIT_FAILURE); }
	printf("Reading Strings\n");

	for (int m = 0; m < *stringcount && m < linesToRead; m++) {
		for (int n = 0; n < *stringlength; n++) {
			fscanf(fp, "%c", &ch);
			//printf("%c ", ch);
			(*array)[m][n] = ch;
		}
		fscanf(fp, "%c", &ch);
		//printf("\n");
	}
}

//Funktion zum berechnen der Hamming-Distanz
int hammingDistance(int *str1, int *str2, int stringLength)
{
	int i = 0, count = 0;

	while (i < stringLength)
	{
		//printf("%c != %c\n", str1[i], str2[i]);
		if (str1[i] != str2[i])
			count++;
		i++;
	}
	return count;
}

int * convertToHex(int* ret, unsigned long long decimal, int stringlength)
{
	int remainder;
	unsigned long long quotient;
	int j = 0;
	char hexadecimal[16];
	quotient = decimal;
	while (quotient != 0){
		remainder = quotient % 16;
		if (remainder < 10)	{ 
			hexadecimal[j++] = 48 + remainder;
		}
		else{
			hexadecimal[j++] = 55 + remainder;
		}
		quotient = quotient / 16;
	}
	
	for (int i = 0; i<stringlength;i++) {
		
		if (stringlength-i>j) {
			ret[i] = '0';
		}
		else {
			ret[i] = hexadecimal[ (stringlength - 1) - i];
		}
	}
	
	return ret;
}

//Funktion um korrekten String zu finden (Entwurf)
int *findClosestString(int ***strings, int stringcount, int stringLength)
{
	int closestStringDec;
	int* currentStringHex = malloc(stringLength * sizeof(char));
	int closestDistance = -1;
	int totalDistance = 0;
	int process_count, rank, root_process;
	long long int taskCounter=0;
	MPI_Status status;
	MPI_File logfile;
	long long int totalListSize = power(16, stringLength) - 1;
	int recProcess = 0;
	root_process = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_File_open(MPI_COMM_WORLD, "log.txt", MPI_MODE_WRONLY,
		MPI_INFO_NULL, &logfile);

	char mylogbuffer[1024];
	char line[100];

	if (rank == root_process)
	{
		sprintf(mylogbuffer, "Hier ist Prozess %i \n", rank);
		MPI_File_write_shared(logfile, mylogbuffer, strlen(mylogbuffer), MPI_CHAR, MPI_STATUS_IGNORE);
		
		for (int i = 1; i < process_count; i++) //läuft für alle unterprozesse durch 1-procCount
		 {
			MPI_Send(&taskCounter, 1,
				MPI_LONG, i, 0, MPI_COMM_WORLD);
			MPI_Send(&closestDistance, 1,
				MPI_LONG, i, 0, MPI_COMM_WORLD);
			taskCounter++;
		}
		int ret = 0;
		int closestStringDecTMP = 0;
		int closestDistanceTMP = 0;
		for (taskCounter; taskCounter < totalListSize-5; taskCounter++) {
			MPI_Recv(&ret, 1,
				MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG,
				MPI_COMM_WORLD, &status);

			if (ret) { //Falls ret 1 ist, ist ein besserer String gefunden worden

				MPI_Recv(&closestDistanceTMP, 1,
					MPI_LONG, status.MPI_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

				MPI_Recv(&closestStringDecTMP, 1,
					MPI_LONG, status.MPI_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
				if (closestDistanceTMP < closestDistance) {
					closestDistance = closestDistanceTMP;
					closestStringDec = closestStringDecTMP;
				}
			}
			MPI_Send(&taskCounter, 1,
				MPI_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			MPI_Send(&closestDistance, 1,
				MPI_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

		}

		for (int i = 1; i < process_count; i++) {

			sprintf(line, "#0 bekommt letzten ergebnisse\n");
			MPI_File_write_shared(logfile, line, strlen(line), MPI_CHAR, MPI_STATUS_IGNORE);
			MPI_Recv(&ret, 1,
				MPI_LONG, i, MPI_ANY_TAG,
				MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			sprintf(line, "#0 letztes ergebniss von %i ~~~ \n", status.MPI_SOURCE);
			MPI_File_write_shared(logfile, line, strlen(line), MPI_CHAR, MPI_STATUS_IGNORE);
			if (ret) { //Falls ret 1 ist, ist ein besserer String gefunden worden

				MPI_Recv(&closestDistanceTMP, 1,
					MPI_LONG, i, MPI_ANY_TAG,
					MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

				MPI_Recv(&closestStringDecTMP, 1,
					MPI_LONG, i, MPI_ANY_TAG,
					MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
				if (closestDistanceTMP < closestDistance) {
					closestDistance = closestDistanceTMP;
					closestStringDec = closestStringDecTMP;
				}
			}
			//schicke abbruchbedingung an die unterthreads
			int abortVar = -1;
			MPI_Send(&abortVar, 1,
				MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);

			sprintf(line, "#Prozess 0 sendet terminierung zu %i ~~~ \n", i);
			MPI_File_write_shared(logfile, line, strlen(line), MPI_CHAR, MPI_STATUS_IGNORE);

		}
		
		sprintf(mylogbuffer, "Prozess %i endet \n", rank);
		MPI_File_write_shared(logfile, mylogbuffer, strlen(mylogbuffer), MPI_CHAR, MPI_STATUS_IGNORE);
		MPI_File_close(&logfile);

		
	}

	else

	{
		sprintf(mylogbuffer, "Hier ist Prozess %i \n", rank);
		MPI_File_write_shared(logfile, mylogbuffer, strlen(mylogbuffer), MPI_CHAR, MPI_STATUS_IGNORE);

		long currentStringDec = 0;
		while (TRUE) {//Durchlaufen bis abbruch
			MPI_Recv(&currentStringDec, 1,
				MPI_LONG_LONG_INT, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			sprintf(mylogbuffer, "Prozess %i bearbeitet nun %i aus\n", rank,currentStringDec);
			MPI_File_write_shared(logfile, mylogbuffer, strlen(mylogbuffer), MPI_CHAR, MPI_STATUS_IGNORE);

			if (currentStringDec == -1) 
			{ 
				sprintf(mylogbuffer, "#Prozess %i bekommt terminierung ~~~ \n", rank);
				MPI_File_write_shared(logfile, mylogbuffer, strlen(mylogbuffer), MPI_CHAR, MPI_STATUS_IGNORE);
				break;
			}
			
			MPI_Recv(&closestDistance, 1,
				MPI_LONG, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			totalDistance = 0;
			convertToHex(currentStringHex, currentStringDec, stringLength);

			for (int i = 0; i < stringcount; i++)
			{
				totalDistance += hammingDistance((*strings)[i], currentStringHex, stringLength);
				if (totalDistance >= closestDistance && closestDistance != -1) break;
			}

			if (totalDistance < closestDistance || closestDistance == -1)
			{

				sprintf(line, "#%i sendet sendet besseren score \n", rank);
				MPI_File_write_shared(logfile, line, strlen(line), MPI_CHAR, MPI_STATUS_IGNORE);
				//schick ne 1 damit main thread bescheid weiß
				MPI_Send("1", 1,
					MPI_LONG, 0, 0, MPI_COMM_WORLD);				
				//send closestDistance
				MPI_Send(&totalDistance, 1,
					MPI_LONG, 0, 0, MPI_COMM_WORLD);
				//send closestStringInDec
				MPI_Send(&currentStringDec, 1,
					MPI_LONG, 0, 0, MPI_COMM_WORLD);
			}
			else {

				sprintf(line, "#%i sendet kein Ergebniss also eine null\n", rank);
				MPI_File_write_shared(logfile, line, strlen(line), MPI_CHAR, MPI_STATUS_IGNORE);
				//schick ne null
				MPI_Send("0", 1,
					MPI_LONG, 0, 0, MPI_COMM_WORLD);
			}
		}
		sprintf(mylogbuffer, "Prozess %i endet \n", rank);
		MPI_File_write_shared(logfile, mylogbuffer, strlen(mylogbuffer), MPI_CHAR, MPI_STATUS_IGNORE);
		MPI_File_close(&logfile);
	}

	MPI_Finalize();

	if (rank)
	{
		exit(0);
	}

	return closestStringDec;
		/*
		//bekomme neue aufgabe
		//bekomme neue closest distance
		totalDistance = 0;
		convertToHex(currentStringHex, currentStringDec, stringLength);

		for (int i = 0; i < stringcount; i++)
		{

			//printf("stringcount: %i\n", stringcount);
			
			//printf("stringlength: %i\n", stringLength);
			printf("checking String: ");
			for (int j = 0; j < stringLength; j++) {
				printf("%c", (*strings)[i][j]);
			}
			printf(" and ");
			for (int j = 0; j < stringLength; j++) {
				printf("%c", currentStringHex[j]);
			}
			printf("\n");
			printf("Distance: %i\n", hammingDistance((*strings)[i], currentStringHex, stringLength));
			totalDistance += hammingDistance((*strings)[i], currentStringHex, stringLength);
			if (totalDistance >= closestDistance&&closestDistance != -1) break;
		}

		
		//printf("TotalDistance:%i\n", totalDistance);
		if (totalDistance < closestDistance || closestDistance == -1)
		{
			//schick ne 1 damit main thread bescheid weiß
			//send closestDistance
			//send closestStringInDec
		}
		else {
			//schick ne null 
		}
		//bekommen neue aufgabe
		*/	
}


int main(int argc, char** argv) {
	
	int **strings = NULL;
	int stringcount;
	int stringlength;
	int linesToRead;
	
	int verbosity;
	double tstart,tend; // time measurement variables
	double time;
	
	
	if (argc < 3) {
		printf("Nicht ausreichend Parameter bei Programmaufruf(Anzahl zu lesender Strings, Verbosity), das Programm terminiert sich jetzt selbst.\n");
		system("pause");
		exit(1);
	}
	

	if (argv[1]) linesToRead = (int)*argv[1];
	
	if (argv[2]) verbosity = (int)*argv[2];

	
	MPI_Init(&argc, &argv);
	linesToRead = 8;

	//Zeiger für Datei
	FILE *fp;	
	fp = fopen("strings.txt", "r");	// Dateizugriff, Datei als read zugegriffen
	if (fp == NULL) {	// falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
	}
	else {	// Datei konnte geoeffnet werden
		printf("strings.txt ist lesbar\n");
		readStrings(fp, &strings, &stringcount, &stringlength, linesToRead);
		if (&strings == NULL) printf("Keine Strings vorhanden!");
		else {
			printf("\nStart find closest string\n");
			printf("\n");
			tstart = clock();
			findClosestString(&strings, stringcount, stringlength);
			tend = clock();
			//printf("tstart:%f\n", tstart);
			//printf("tend:%f\n", tend);
			time = (tend- tstart) / CLOCKS_PER_SEC;
			printf("Time: %f \n", time);
			printf("\nEnd find closest string\n");
			printf("%i \n", stringcount);
			printf("%i \n", stringlength);
			for (int i = 0; i < stringcount; i++) {
				for (int j = 0; j < stringlength; j++) {
					printf("%c", strings[i][j]);
				}
				printf("\n");

			}
		}
		fclose(fp);	//Dateizugriff wieder freigeben
	}
	
	printf("\n\n");
	// funktionsblock um die Strings darzustellen
	if (verbosity == 1 || verbosity == 3) {

		printf("Erfolgreiches Programm (^_^)");

		/*
		printf("The best String is: ");
		for (int j = 0; j < stringLength; j++) {
			printf("%c", closestStringDec[j]);
		}
		printf("\n");
		printf("With a Distance of %i\n", closestDistance);
		*/

	}

	// funktionsblock um die Zeit darzustellen
	if (verbosity == 2 || verbosity == 3) {

	}
	return 0;
}