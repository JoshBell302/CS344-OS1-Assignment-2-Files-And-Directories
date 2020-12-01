/*
Assignment 2: Files & Directories
Creator: Josh Bell
Date of Creation: October 16th 2020
Date of Completion: October --- 2020
File: main.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define PREFIX "movies_"
#define DONOTTOUCHFILE  "movies_by_year"
// ====================================================================================================

/*
Struct: movie
title		Title of the Movie			- pointer char
year		Year of release				- pointer int
language	Languages available			- pointer char 2D array
rating		Rating Value of the Movie	- pointer float
next									- pointer to the next movie
*/
struct movie {
	char* title;
	int* year;
	char* language[5][20];
	float* rating;
	struct movie* next;
};

// ====================================================================================================

/*
int userInputBegin
Requests user input [1 or 2] and checks for input errors and returns a valid value
Parameters: userValue
Returns:	int
*/
int userInputBegin(int * userValue) {
	// Ask for the type of data the user wants to see, or to exit the program
	printf("Enter a choice either 1 or 2: ");
	scanf("%d", &userValue);

	// Checks to see if input is valid, if not report the value and prompt again
	if (userValue == 1 || userValue == 2)
		return userValue;

	printf("That was not a valid choice, please try again!\n\n");

	return 0;
}

// ====================================================================================================

/*
int userInputData
Requests user input [1 to 3] and checks for input errors and returns a valid value
Parameters: userValue
Returns:	int
*/
int userInputData(int * userValue) {

	// Ask for the type of data the user wants to see, or to exit the program
	printf("Enter a choice from 1 to 3: ");
	scanf("%d", &userValue);

	// Checks to see if input is valid, if not report the value and prompt again
	if (userValue >= 1 && userValue <= 3)
		return userValue;

	printf("That was not a valid choice, please try again!\n\n");

	return 0;
}

// ====================================================================================================

/*
struct movie * createMovie
Takes the current line and takes the values and puts them into a movie node to form a linked list
Parameters: currLine
Returns:	movie *
*/
struct movie* createMovie(char* currLine) {
	// Initialize Values
	struct movie* currMovie = malloc(sizeof(struct movie));
	char* saveptr = NULL;
	char* languageStr = NULL;
	char* ratingstr = NULL;

	// The first token is the Movie's Title
	char* token = strtok_r(currLine, ",", &saveptr);
	currMovie->title = calloc(strlen(token) + 2, sizeof(char));
	strcpy(currMovie->title, token);


	// The next token is the Movie's Realese Date
	token = strtok_r(NULL, ",", &saveptr);
	currMovie->year = atoi(token);


	// The next token is the Movie's Languages
	token = strtok_r(NULL, "]", &saveptr);

	// Gets all Languages in a string then removes the first char '[' from the string
	languageStr = calloc(strlen(token) + 1, sizeof(char));
	strcpy(languageStr, token);
	memmove(&languageStr[0], &languageStr[1], strlen(languageStr));

	// Splits the Language string and inputs each language in the movie's language array
	int count = 0;
	token = strtok(languageStr, ";");
	while (token != NULL) {
		currMovie->language[count][0] = token;
		char* movieLan = currMovie->language[count][0];
		token = strtok(NULL, ";");
		count++;
	}


	// The last token is the Movie's Rating
	token = strtok_r(NULL, "\n", &saveptr);
	ratingstr = calloc(strlen(token) + 1, sizeof(char));
	strcpy(ratingstr, token);

	// Removes the ',' from the begining of the string then converts to float
	memmove(&ratingstr[0], &ratingstr[1], strlen(ratingstr));
	currMovie->rating = ratingstr;
	*(currMovie->rating) = atof(currMovie->rating);


	// Set the next node to NULL in the newly created student entry
	currMovie->next = NULL;

	// To show all data processed, not needed to show for assignment but nice to look at
	//char * movieTitle = currMovie->title;
	//char * movieLan1 = currMovie->language[0][0];
	//char * movieLan2 = currMovie->language[1][0];
	//char * movieLan3 = currMovie->language[2][0];
	//char * movieLan4 = currMovie->language[3][0];
	//char * movieLan5 = currMovie->language[4][0];
	//printf("currMovie ---\nTitle: %s\nYear: %d\nLanguage 1: %s\nLanguage 2: %s\nLanguage 3: %s\nLanguage 4: %s\nLanguage 5: %s\nRating: %.1f\n\n", movieTitle, currMovie->year, movieLan1, movieLan2, movieLan3, movieLan4, movieLan5, *(currMovie->rating));

	return currMovie;
}

// ====================================================================================================

/*
struct movie * processFile
Return a list of students by parsing data from each line of the specified file.
Parameters: filePath
Returns:	movie *
*/
struct movie* processFile(char* filePath)
{
	// Open the specified file for reading only
	FILE* movieFile = fopen(filePath, "r");

	// Initialize Values
	char* currLine = NULL;
	size_t len = 0;
	ssize_t nread;
	int count = 0;

	// The head and tail of the linked list
	struct movie* head = NULL;
	struct movie* tail = NULL;

	// Skips the first line of the csv file
	getline(&currLine, &len, movieFile);

	// Read the file line by line
	while ((nread = getline(&currLine, &len, movieFile)) != -1)
	{
		// To count number of movies parsed 
		count++;

		// Get a new movie node corresponding to the current line
		struct movie* newNode = createMovie(currLine);

		// Is this the first node in the linked list? If so then set the 
		if (head == NULL)
		{
			// This is the first node in the linked link
			// Set the head and the tail to this node
			head = newNode;
			tail = newNode;
		}
		else
		{
			// This is not the first node.
			// Add this node to the list and advance the tail
			tail->next = newNode;
			tail = newNode;
		}
	}
	free(currLine);
	fclose(movieFile);
	return head;
}

// ====================================================================================================

/*
void fillDirectory
Opens the given directory and fills in the directory with .txt files corresponding to each year found in the given linked list of movies,
then fills the .txt files with the titles of the movies corresponding to the year of the movies realese 
Parameters: dirName, movieList
Returns:	---
*/
void fillDirectory(char dirName[], struct movie * movieList) {
	// Initialize Values
	struct movie* temp;
	int arraySize = 30;
	int yearArray[arraySize];
	int arrayCount = 0;
	int movieCount = 0;

	char arrayYearFile[10];
	memset(arrayYearFile, '\0', sizeof(arrayYearFile)); // Set to NULL
	char directory[50] = "./"; // 50 chosen because a directory shouldnt be bigger than 50
	strcat(directory, dirName);
	strcat(directory, "/");
	char fullDirectory[100];
	memset(fullDirectory, '\0', sizeof(fullDirectory)); // Set to NULL

	for (int i = 0; i < arraySize; i++)
		yearArray[i] = 0;

	// Go through linked list and pull out all years in the array
	temp = movieList;
	while (temp != NULL) {
		yearArray[arrayCount] = temp->year;
		temp = temp->next;
		arrayCount++;
		movieCount++;
	}

	// Delete duplicate years in yearArray
	for (int i = 0; i < arrayCount; i++) {
		for (int j = i + 1; j < arrayCount; j++) {
			if (yearArray[i] == yearArray[j]) {
				for (int k = j; k < arrayCount; k++)
					yearArray[k] = yearArray[k + 1];
				arrayCount--;
				j--;
			}
		}
	}

	// Create a loop to create a .txt file for every year in the yearArray
	for (int i = 0; i < arrayCount; i++) {
		memset(fullDirectory, '\0', sizeof(fullDirectory));
		strcat(fullDirectory, directory);
		
		sprintf(arrayYearFile, "%d", yearArray[i]);
		
		strcat(arrayYearFile, ".txt");
		strcat(fullDirectory, arrayYearFile);

		temp = movieList;

		// Traverses through the linked list to see which movies have what year, if a movie has the same year as the one currently being looked at in the yearArray then
		// add the title to the .txt file and a new line char, then continue through the rest of the movies
		FILE* yearFile = fopen(fullDirectory, "w+");
		for (int j = 0; j < movieCount; j++) {
			if (yearArray[i] == temp->year) {
				fprintf(yearFile, "%s\n", temp->title);
			}
			temp = temp->next;
		}
		fclose(yearFile);
	}
}

// ====================================================================================================

/*
void processBigFile
Filters through the current files directory to find the file that starts with PREFIX and ends with csv. Also makes sure not to interact
with the movies_by_year file, it causes a big mess! When it finds the biggest file under the correct conditions then calls the fillDirectory
function to fill out the created directory, with the found file.
Parameters: dirName
Returns:	---
*/
void processBigFile(char dirName[]) {
	// Code modified from "https://canvas.oregonstate.edu/courses/1818570/pages/exploration-directories?module_item_id=20000705" under Getting File and Directory Meta-Data
	// Open the current directory
	DIR* currDir = opendir(".");

	// Initialize Values
	struct dirent* aDir;
	int currentSize = 0;
	int biggestSize = 0;
	int i = 0;
	struct stat dirStat;

	char* saveptr = NULL;
	char csv[3] = "csv";
	char entryStartName[256];
	char trash[256];
	char entryName[256];
	char fileExtension[5];

	// Go through all the entries
	while ((aDir = readdir(currDir)) != NULL) {

		if (strncmp(DONOTTOUCHFILE, aDir->d_name, strlen(DONOTTOUCHFILE)) == 0) { 
			// Do nothing skip to the next file
		}
		else {
			// Ensure that we are looking for files with the prefix "movies_"
			if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {
				memset(entryStartName, '\0', sizeof(entryStartName));
				strcpy(entryStartName, aDir->d_name);

				// Set fileExtension to NULL
				memset(fileExtension, '\0', sizeof(fileExtension));

				// Checks the files extension for "csv"
				char* token = strtok(entryStartName, ".");
				memset(trash, '\0', sizeof(trash));
				strcpy(trash, token);
				token = strtok(NULL, "\0");
				strcpy(fileExtension, token);

				// If the files extension is csv than continue to check for the largest file 
				if (fileExtension[0] == csv[0] && fileExtension[1] == csv[1] && fileExtension[2] == csv[2]) {

					// Get meta-data for the current entry
					stat(aDir->d_name, &dirStat);

					currentSize = dirStat.st_size;

					// Check to see if the file is the biggest file so far, if so have the biggest files size be inputed into biggestSize and name be saved into entryName
					if (i == 0 || currentSize >= biggestSize) {
						biggestSize = currentSize;
						memset(entryName, '\0', sizeof(entryName));
						strcpy(entryName, aDir->d_name);
					}
					i++;
				}
			}
		}
	}

	// If entryName is still NULL that means no file was found
	if (entryName == NULL) {
		printf("No file found, please try again\n");
		fileProcess();
	}
	else {
	// Close the directory
	closedir(currDir);

	// Process the found file
	printf("Now processing the chosen file: %s\n", entryName);
	struct movie* list = processFile(entryName);

	// Fill in the directory
	fillDirectory(dirName, list);
	}
}

// ====================================================================================================

/*
void processSmallFile
Filters through the current files directory to find the file that starts with PREFIX and ends with csv. Also makes sure not to interact
with the movies_by_year file, it causes a big mess! When it finds the smallest file under the correct conditions then calls the fillDirectory
function to fill out the created directory, with the found file.
Parameters: dirName
Returns:	---
*/
void processSmallFile(char dirName[]) {
	// Code modified from "https://canvas.oregonstate.edu/courses/1818570/pages/exploration-directories?module_item_id=20000705" under Getting File and Directory Meta-Data
	// Open the current directory
	DIR* currDir = opendir(".");

	// Initialize Values
	struct dirent* aDir;
	int currentSize = 0;
	int smallestSize = 0;
	int i = 0;
	struct stat dirStat;

	char* saveptr = NULL;
	char csv[3] = "csv";
	char entryStartName[256];
	char trash[256];
	char entryName[256];
	char fileExtension[5];

	// Go through all the entries
	while ((aDir = readdir(currDir)) != NULL) {

		if (strncmp(DONOTTOUCHFILE, aDir->d_name, strlen(DONOTTOUCHFILE)) == 0) {
			// Do nothing skip to the next file
		}
		else {
			// Ensure that we are looking for files with the prefix "movies_"
			if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {
				memset(entryStartName, '\0', sizeof(entryStartName));
				strcpy(entryStartName, aDir->d_name);

				// Set fileExtension to NULL
				memset(fileExtension, '\0', sizeof(fileExtension));

				// Checks the files extension for "csv"
				char* token = strtok(entryStartName, ".");
				memset(trash, '\0', sizeof(trash));
				strcpy(trash, token);
				token = strtok(NULL, "\0");
				strcpy(fileExtension, token);

				// If the files extension is csv than continue to check for the largest file 
				if (fileExtension[0] == csv[0] && fileExtension[1] == csv[1] && fileExtension[2] == csv[2]) {

					// Get meta-data for the current entry
					stat(aDir->d_name, &dirStat);

					currentSize = dirStat.st_size;

					// Check to see if the file is the biggest file so far, if so have the biggest files size be inputed into biggestSize and name be saved into entryName
					if (i == 0 || currentSize <= smallestSize) {
						smallestSize = currentSize;
						memset(entryName, '\0', sizeof(entryName));
						strcpy(entryName, aDir->d_name);
					}
					i++;
				}
			}
		}
	}

	// If entryName is still NULL that means no file was found
	if (entryName == NULL) {
		printf("No file found, please try again\n");
		fileProcess();
	}
	else {
		// Close the directory
		closedir(currDir);

		// Process the found file
		printf("Now processing the chosen file: %s\n", entryName);
		struct movie* list = processFile(entryName);

		// Fill in the directory
		fillDirectory(dirName, list);
	}
}

// ====================================================================================================

/*
void processNameFile
Filters through the current files directory to find the file that the user supplies. Also makes sure not to interact
with the movies_by_year file, it causes a big mess! When it finds the file it then calls the fillDirectory function to 
fill out the created directory, with the found file. If it doesnt it shows an error message to the user and askes them 
to try and choose to process a file again.
Parameters: dirName
Returns:	---
*/
void processNameFile(char dirName[]) {
	// Code modified from "https://canvas.oregonstate.edu/courses/1818570/pages/exploration-directories?module_item_id=20000705" under Getting File and Directory Meta-Data
	// Open the current directory
	DIR* currDir = opendir(".");

	// Initialize Values
	struct dirent* aDir;
	struct stat dirStat;
	int i = 0;
	int count = 0;
	int first = 0;

	char entryName[256];
	memset(entryName, '\0', sizeof(entryName));
	char userFile[256];
	memset(userFile, '\0', sizeof(userFile));
	char csv[3] = "csv";
	char* saveptr = NULL;
	char entryStartName[256];
	char trash[256];
	char fileExtension[5];


	// Get File name from user
	printf("Enter the complete file name: ");
	scanf("%s", userFile);
	printf("\n");

	// Go through all the entries
	while ((aDir = readdir(currDir)) != NULL) {

		if (strncmp(DONOTTOUCHFILE, aDir->d_name, strlen(DONOTTOUCHFILE)) == 0) {
			// Do nothing skip to the next file
		}
		else {
			// Ensure that we are looking for files with the prefix "movies_"
			if (strncmp(PREFIX, userFile, strlen(PREFIX)) == 0) {
				memset(entryStartName, '\0', sizeof(entryStartName));
				strcpy(entryStartName, userFile);

				// Set fileExtension to NULL
				memset(fileExtension, '\0', sizeof(fileExtension));

				// Checks the files extension for ".csv"
				if (strchr(userFile, '.')) {
					char* token = strtok(entryStartName, ".");
					memset(trash, '\0', sizeof(trash));
					strcpy(trash, token);
					token = strtok(NULL, "\0");
					strcpy(fileExtension, token);


					// If the files extension is csv than compare the users inputed string and the current file
					if (fileExtension[0] == csv[0] && fileExtension[1] == csv[1] && fileExtension[2] == csv[2]) {
						if (strncmp(userFile, aDir->d_name, strlen(aDir->d_name)) == 0) {
							memset(entryName, '\0', sizeof(entryName));
							strcpy(entryName, aDir->d_name);
							count++;
							break;
						}
					}
				}
			}
		}
	}

	// If count is still 0 that means no file was found
	if (count == 0) {
		printf("The file %s was not found, please try again\n\n", userFile);
		fileProcess();
	}
	else {
		// Close the directory
		closedir(currDir);

		// Process the found file
		printf("Now processing the chosen file: %s\n", entryName);
		struct movie* list = processFile(entryName);

		// Fill in the directory
		fillDirectory(dirName, list);
	}
}

// ====================================================================================================

/*
void fileProcess
Asks user to process a certain kind of file, then a directory is made and searches for a .csv file and processes it.
The directory is called 'bellj3.movies.[random digit here]', then calls the specifc file process function.
Parameters: ---
Returns:	---
*/
void fileProcess() {
	// Initialize Values
	int dirCheck = 0;
	int choiceData = 0;
	int randomValueI = 0;
	char randomValueC[6]; // 6 chosen because 100000 is 6 chars long
	memset(randomValueC, '\0', 6);
	char directoryName[21]; // 21 chosen beacuse with my directory name the longest string can be 20 chars long
	memset(directoryName, '\0', 21);
	strcpy(directoryName, "bellj3.movies.");

	// Prompt user to select type of file to process
	printf("\n-------------------File Process-------------------\n");
	printf("1. Pick the largest file\n");
	printf("2. Pick the smallest file\n");
	printf("3. Specify the name of a file\n");

	// Requests user input
	while (choiceData == 0) {
		choiceData = userInputData(choiceData);
	}

	// Creates a random int value thats a value from 0 to 100,000
	srand(time(0));
	randomValueI = rand() % 100000;
	sprintf(randomValueC, "%d", randomValueI);
	
	// Makes the full directory name
	strcat(directoryName, randomValueC);

	// Creates a directory named 'bellj3.movies.[random digit here]'
	dirCheck = mkdir(directoryName,0750);

	// Checks to see if directory has been made, if so alert user
	if (!dirCheck)
		printf("\nDirectory created with name %s\n", directoryName);
	else
		printf("Unable to create directory\n");

	// Determines type of file to process depending on value returned by userInputData
	if (choiceData == 1) 
		processBigFile(directoryName);

	if (choiceData == 2) 
		processSmallFile(directoryName);

	if (choiceData == 3) 
		processNameFile(directoryName);
}

// ====================================================================================================

/*
void output
Asks the user if they want to use this program or not, if so then call fileProcess() to see what file to process
Parameters: ---
Returns:	---
*/
void output() {
	// Initialize Values
	int choiceBegin = 0;

	// Gives a better outline 
	printf("==================================================\n\n");
	printf("-------------------Program Usage------------------\n");

	// Prompt user to either use the program or exit it
	printf("1. Select file to process\n");
	printf("2. Exit the program\n");

	// Requests user input
	while (choiceBegin == 0) {
		choiceBegin = userInputBegin(choiceBegin);
	}

	// Determines data to show depending on value returned by userInputBegin
	if (choiceBegin == 1) {
		fileProcess();
		output();
	}

	// If choiceBegin is 2 then we exit the program
	return;
}

// ====================================================================================================

int main() {
	output();
	printf("\n==================================================\n");
	return EXIT_SUCCESS;
}