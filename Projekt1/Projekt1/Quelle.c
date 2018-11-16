/*a
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
char ** createMatrix(int rows, int columns) {
	char **ret;
	ret = malloc(rows * sizeof *ret);
	if (!ret) { perror("Error: "); exit(EXIT_FAILURE); }
	for (int i = 0; i<rows; i++) {
		ret[i] = malloc(columns * sizeof *ret[0]);
		if (!ret[i]) { perror("Error: "); exit(EXIT_FAILURE); }
	}
	return ret;
}

//Berechne exponentiell zu einer gewünschten Basis
long long power(int base, int exp) {
	long long  result = 1;
	for (int i = 0; i < exp; i++)
		result *= base;
	return result;
}

//Ersetze Kleinbuchstaben durch Großbuchstaben
stringToUpper(char *string) {
	int i;
	size_t len = strlen(string);
	for (i = 0; i<len; i++) {
		if (string[i] >= 'a' && string[i] <= 'z') {
			string[i] -= 32;
		}
	}
}
//Strings werden aus der Textdatei eingelesen
readStrings(FILE* fp, char ***array, int *stringcount, int *stringlength, int linesToRead, int strLenInput) {
	fscanf(fp, "%i", stringcount);
	fscanf(fp, "%i", stringlength);
	if (strLenInput) *stringlength = strLenInput;
	if (linesToRead < *stringcount) *stringcount = linesToRead;
	*array = createMatrix(*stringcount, *stringlength + 1);
	if (!*array) { perror("Error: "); exit(EXIT_FAILURE); }
	for (int m = 0; m < *stringcount; m++) {
		fscanf(fp, "%s", (*array)[m]);
		stringToUpper((*array)[m]);
	}
}

//Funktion zum berechnen der Hamming-Distanz
char hammingDistance(char *str1, char *str2, int stringLength)
{
	int i = 0, count = 0;
	while (i < stringLength)
	{
		if (str1[i] != str2[i])
			count++;
		i++;
	}
	return count;
}

//Konvertiere eine Dezimalzahl in das Hexadezimalsystem
char * convertToHex(char* ret, long long decimal, int stringlength)
{
	int remainder;
	long long quotient;
	int j = 0;
	char hexadecimal[16];
	quotient = decimal;
	while (quotient != 0) {
		remainder = quotient % 16;
		if (remainder < 10) {
			hexadecimal[j++] = 48 + remainder;
		}
		else {
			hexadecimal[j++] = 55 + remainder;
		}
		quotient = quotient / 16;
	}
	for (int i = 0; i<stringlength; i++) {
		if (stringlength - i>j) {
			ret[i] = '0';
		}
		else {
			ret[i] = hexadecimal[(stringlength - 1) - i];
		}

	}
	hexadecimal[stringlength] = 0;

	return ret;
}

//Funktion um korrekten String zu finden (Entwurf)
long long findClosestString(char ***strings, int stringcount, int stringLength)
{
	char* currentStringHex = malloc(stringLength + 1 * sizeof(currentStringHex));
	int process_count, rank, root_process;
	int flag = 0;
	int zero = 0;
	int one = 1;
	int taskRange = 1;
	long long taskCounter = 0;
	long long abortVar = -1;
	long long totalListSize = power(16, stringLength);
	long long closestStringDec = -1;
	long long closestDistance = -1;
	long long totalDistance = 0;
	MPI_Request request;
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	root_process = 0;

	if (process_count > 1) taskRange = 500 / stringLength*process_count;

	if (rank == root_process)
	{
		if (process_count > 1) {
			for (int i = 1; i < process_count; i++) //verteilt allen Unterprozessen erstmalig eine Aufgabe
			{
				MPI_Send(&taskCounter, 1,
					MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
				MPI_Send(&taskRange, 1,
					MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&closestDistance, 1,
					MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
				taskCounter++;
			}

			int ret = 0;
			long long closestStringDecTMP = 0;
			long long closestDistanceTMP = 0;
			if (taskCounter + taskRange + 1 > totalListSize / process_count) {
				taskRange = ((totalListSize - taskCounter) / process_count) / 100;
				if (taskRange == 0) taskRange = 1;
			}

			//Durchlaufe Liste der möglichen Zeichenkombinationen bis zum Listenende
			while (taskCounter + (taskRange - 1) < totalListSize) {
				flag = 0;
				MPI_Irecv(&ret, 1,
					MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, &request);
				MPI_Test(&request, &flag, &status);
				while (flag == 0) {
					totalDistance = 0;
					if (taskCounter < totalListSize) {
						convertToHex(currentStringHex, taskCounter, stringLength);
						for (int i = 0; i < stringcount; i++) {
							totalDistance += hammingDistance((*strings)[i], currentStringHex, stringLength);
							if (totalDistance >= closestDistance && closestDistance != -1) break;
						}
						if (totalDistance < closestDistance || closestDistance == -1) {
							closestDistance = totalDistance;
							closestStringDec = taskCounter;
						}
						taskCounter++;
						if (taskCounter + process_count * 5 * taskRange > totalListSize && taskRange > 1) {
							taskRange = taskRange / (process_count);

							if (taskRange == 0) taskRange = 1;
						}
						MPI_Test(&request, &flag, &status);
					}
					else {
						MPI_Wait(&request, &status);
						flag = 1;
						break;
					}
				}

				if (taskCounter + (taskRange - 1) < totalListSize) {
					if (ret) { //Falls ret 1 ist, ist ein besserer String gefunden worden

						MPI_Recv(&closestDistanceTMP, 1,
							MPI_LONG_LONG, status.MPI_SOURCE, MPI_ANY_TAG,
							MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

						MPI_Recv(&closestStringDecTMP, 1,
							MPI_LONG_LONG, status.MPI_SOURCE, MPI_ANY_TAG,
							MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
						if (closestDistanceTMP < closestDistance || closestDistance == -1) {
							closestDistance = closestDistanceTMP;
							closestStringDec = closestStringDecTMP;
						}
					}

					flag = 0;
					MPI_Send(&taskCounter, 1,
						MPI_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
					MPI_Send(&taskRange, 1,
						MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
					MPI_Send(&closestDistance, 1,
						MPI_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

					taskCounter += taskRange;

					if (taskCounter + process_count * 5 * taskRange > totalListSize && taskRange > 1) {
						taskRange = taskRange / (process_count);
						if (taskRange == 0) taskRange = 1;
					}
				}
			}




			for (int i = 1; i < process_count; i++) {
				if (flag) {
					flag = 0;
				}
				else
				{
					MPI_Recv(&ret, 1,
						MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
						MPI_COMM_WORLD, &status);
				}
				if (ret) { //Falls ret == 1 ist, ist ein besserer String gefunden worden

					MPI_Recv(&closestDistanceTMP, 1,
						MPI_LONG_LONG, status.MPI_SOURCE, MPI_ANY_TAG,
						MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

					MPI_Recv(&closestStringDecTMP, 1,
						MPI_LONG_LONG, status.MPI_SOURCE, MPI_ANY_TAG,
						MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
					if (closestDistanceTMP < closestDistance || closestDistance == -1) {
						closestDistance = closestDistanceTMP;
						closestStringDec = closestStringDecTMP;
					}
				}
				//schicke Abbruchbedingung an die Unterthreads
				MPI_Send(&abortVar, 1,
					MPI_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

			}
		}
		else 
		{
			while (taskCounter < totalListSize) {
				totalDistance = 0;
				convertToHex(currentStringHex, taskCounter, stringLength);
				for (int i = 0; i < stringcount; i++) {
					totalDistance += hammingDistance((*strings)[i], currentStringHex, stringLength);
					if (totalDistance >= closestDistance && closestDistance != -1) break;
				}
				if (totalDistance < closestDistance || closestDistance == -1) {
					closestDistance = totalDistance;
					closestStringDec = taskCounter;
				}
				taskCounter++;
			}
		}
	}
	else
	{
		long long betterStringDec = -1;
		long long currentStringDec = -1;
		int submitNewDistance = 0;

		//Durchlaufe Schleife bis abbruch
		while (TRUE) {
			MPI_Recv(&currentStringDec, 1,
				MPI_LONG_LONG, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			if (currentStringDec == -1) break;

			MPI_Recv(&taskRange, 1,
				MPI_INT, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			MPI_Recv(&closestDistance, 1,
				MPI_LONG_LONG, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			for (int taskNumber = 0; taskNumber < taskRange; taskNumber++) {
				totalDistance = 0;
				convertToHex(currentStringHex, currentStringDec + taskNumber, stringLength);
				for (int i = 0; i < stringcount; i++) {
					totalDistance += hammingDistance((*strings)[i], currentStringHex, stringLength);
					if (totalDistance >= closestDistance && closestDistance != -1) break;
				}
				if (totalDistance < closestDistance || closestDistance == -1) {
					closestDistance = totalDistance;
					betterStringDec = currentStringDec + taskNumber;
					submitNewDistance = 1;

				}
			}
			if (submitNewDistance)
			{

				//schick eine 1 zum Hauptprozess um zu signalisieren dass ein String mit einer geringeren Distanz gefunden wurde
				MPI_Send(&one, 1,
					MPI_INT, 0, 1, MPI_COMM_WORLD);
				//send closestDistance
				MPI_Send(&closestDistance, 1,
					MPI_LONG_LONG, 0, 2, MPI_COMM_WORLD);
				//send closestStringInDec
				MPI_Send(&betterStringDec, 1,
					MPI_LONG_LONG, 0, 3, MPI_COMM_WORLD);
			}
			else
			{
				//schicke eine Null wenn kein String mit geringerer Distanz gefunden werden konnte
				MPI_Send(&zero, 1,
					MPI_LONG, 0, 0, MPI_COMM_WORLD);
			}
		}
	}

	MPI_Finalize();
	if (rank) exit(0); 	//terminiere alle Unterprozesse

	return closestStringDec;
}


int main(int argc, char** argv) {

	char **strings = NULL;
	char* resultHex;
	int stringcount;
	int stringLength;
	int linesToRead;
	int pauseVar = 0;
	int strLenInput = 0;
	int process_count;
	int rank;
	int verbosity;
	long long result = -1;

	// time measurement variables
	double tstart, tend; 
	double time = 0;


	if (argc < 3) {
		printf("Nicht ausreichend Parameter bei Programmaufruf(Anzahl zu lesender Strings, Verbosity), das Programm terminiert sich jetzt selbst.\n");
		system("pause");
		exit(1);
	}

	//Liest die Parameter des Programmaufrufs ein
	linesToRead = atoi(argv[1]);	
	verbosity = atoi(argv[2]);

	//Dritter Parameter falls die Stringlänge manuell gesetzt werden soll
	if (argc > 3)  strLenInput = atoi(argv[3]);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	FILE *fp;//Zeiger für Datei der Strings
	fp = fopen("strings.txt", "r");	// Dateizugriff, Datei als read zugegriffen
	if (fp == NULL) {	// falls die Datei nicht geoeffnet werden kann
		printf("Datei konnte nicht geoeffnet werden!!\n");
		MPI_Finalize();
		exit(1);
	}
	else {	// Datei konnte geoeffnet werden
		//Strings werden eingelesen
		readStrings(fp, &strings, &stringcount, &stringLength, linesToRead, strLenInput);
		fclose(fp);	//Dateizugriff wieder freigeben
		if (&strings == NULL) printf("Keine Strings vorhanden!");
		else {
			//Synchronisiere alle Prozesse
			MPI_Barrier(MPI_COMM_WORLD);
			tstart = clock();
			result = findClosestString(&strings, stringcount, stringLength);
			tend = clock();
			time = (tend - tstart) / CLOCKS_PER_SEC;
		}
	}

	// funktionsblock um die Strings darzustellen
	if (verbosity == 1 || verbosity == 3 || verbosity == 4) {
		printf("==================================================\n");
		printf("Stringcount: %i \n", stringcount);
		printf("Stringlength: %i \n", stringLength);
		printf("New String in Decimal is %i\n", result);
		resultHex = malloc(stringLength + 1 * sizeof(char));
		convertToHex(resultHex, result, stringLength);
		printf("New String in Hexadecimal is %s\n", resultHex);
		printf("==================================================\n");
	}

	// Funktionsblock um die Zeit darzustellen
	if (verbosity == 2 || verbosity == 3 || verbosity == 4) {
		printf("Time: %f \n", time);
	}

	//Funktionsblock um testVariablen darzustellen verbosity<=>4
	if (verbosity == 4) {
		printf("process_count: %i\n", process_count);
	}

	return 0;
}