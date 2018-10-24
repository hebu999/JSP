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
			(*array)[m][n] = ch;
		}
		fscanf(fp, "%c", &ch);
	}
}

//Funktion zum berechnen der Hamming-Distanz
int hammingDistance(int *str1, int *str2, int stringLength)
{
	int i = 0, count = 0;

	while (i < stringLength)
	{
		//printf("\n");
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
		//printf("if (%i-%i>%i)\n", stringlength,i, j);
		if (stringlength-i>j) {
			ret[i] = '0';
			//printf("make null\n");
		}
		else {
			ret[i] = hexadecimal[ (stringlength - 1) - i];
			//printf("swap: %c\n", hexadecimal[j - 1 + ((stringlength - 1) - i)]);
		}
	}
	
	return ret;
}

unsigned long long power(int base, unsigned int exp) {
	unsigned long long  result = 1;
	for (unsigned int i = 0; i < exp; i++)
		result *= base;
	return result;
}

//Funktion um korrekten String zu finden (Entwurf)
int *findClosestString( int ***strings, int stringcount, int stringLength)
{
	int* closestString=malloc(stringLength*sizeof(char));
	int* currentStringHex=malloc(stringLength * sizeof(char));
	int closestDistance=-1;
	int totalDistance = 0;

	for (unsigned long long currentStringDec = 0; currentStringDec <= power(16, stringLength) - 1; ++currentStringDec) {
		totalDistance = 0;
		//printf("color: %5i    \n", color);
		convertToHex(currentStringHex, currentStringDec, stringLength);
		//printf("%5i  ", color);
		for (int i = 0; i < stringcount; i++)
		{
			printf("stringcount: %i\n", stringcount);
			printf("stringlength: %i\n", stringLength);
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
		}
		printf("TotalDistance:%i\n", totalDistance);
		if (totalDistance < closestDistance||closestDistance==-1)
		{	
			// TO-DO - - - Kann man so einfach kopieren? oder mit schleife?? pointer und addressen etc.
			//closestString = currentStringHex;
			for (int i = 0; i < stringLength; i++) {
				closestString[i] = currentStringHex[i];
			}
			closestString = currentStringHex;
			closestDistance = totalDistance;
		}
		printf("\n\n");
	}
	printf("bestString: ");
	for (int j = 0; j < stringLength; j++) {
		printf("%c", closestString[j]);
	}
	printf(" with a Distance of %i\n", closestDistance);
	return closestString;
}

int main(int argc, char** argv) {
	
	// MPI wird initalisiert
	MPI_Init(&argc, &argv);

	int **strings=NULL;
	int stringcount;
	int stringlength;
	int linesToRead;
	int process_count, rank;
	int verbosity;

	int distance=384;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/*
	if (argc < 3) {
		printf("Nicht ausreichend Parameter bei Programmaufruf(Anzahl zu lesender Strings, Verbosity), das Programm terminiert sich jetzt selbst\n");
		system("pause");
		exit(1);
	}
	*/
	
	if (argv[1]) linesToRead = (int)*argv[1];
	
	if (argv[2]) verbosity = (int)*argv[2];

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

	linesToRead = 6;

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
			findClosestString(&strings, linesToRead, stringlength);
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
	MPI_Finalize();
	printf("\n\n");
	
	/*
	int length = 6; //testvariable um die hex schleife zu testen
	int *tmp = malloc(length * sizeof(char));
	for (unsigned long long color = 0; color <= power(16,length)-1; ++color) {
		//printf("color: %5i    \n", color);
		convertToHex(tmp, color, length);
		//printf("%5i  ", color);
		for (int i = 0; i < length; i++) {
			printf("%c", tmp[i]);
		}
		printf("\n");
	}
	printf("\n");*/
	system("pause");
	return 0;
}