/***********************************************************************************************************
 * Filename: util.c
 * Date: 7/18/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Contains implementations for utility functions used by other components of the program
 **********************************************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include<fcntl.h> 
#include <signal.h>

/**
* Function: redirectStdout
* -------------------------------------------
* Redirects standard out to a given file and returns file descriptor to close later
*
* redirectOutput - 0 = no, don't redirect stdout; 1 = yes, redirect away
* outputRedirect - the name of the file to redirect output to
*/
int redirectStdout(int redirectOutput, char *outputRedirect)
{
	if (redirectOutput)
	{
		int fileDesc = open(outputRedirect, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		dup2(fileDesc, 1);

		return fileDesc;
	}

	return 0;
}
/**
* Function: redirectStdin
* -------------------------------------------
* Redirects standard in from a given file and returns file descriptor to close later
*
* redirectInput - 0 = no, don't redirect stdin; 1 = yes, redirect away
* outputRedirect - the name of the file to redirect output to
*/
int redirectStdin(int redirectInput, char *inputRedirect)
{
	if (redirectInput)
	{
		int fileDesc = open(inputRedirect, O_RDONLY);
		dup2(fileDesc, 0);

		return fileDesc;
	}

	return 0;
}

/**
* Function: closeFile
* -------------------------------------------
* Closes a file by the given fileDescriptor
*
* redirection - denotes whether a file has been opened
* fileDescriptor - the file descriptor to close
*/
void closeFile(int isFileOpen, int fileDescriptor)
{
	if (isFileOpen)
		close(fileDescriptor);
}

/**
* Function: assignCurrentWorkingDirectory
* -------------------------------------------
* Retrieves the current working directory and assigns to given pointer to char
*
* workingDirectory - stores the full path of the current working directory
*/
void assignCurrentWorkingDirectory(char *workingDirectory)
{
   getcwd(workingDirectory, sizeof(workingDirectory));
}

/**
* Function: handle_SIGINT
* -------------------------------------------
* Custom implementation of SIGINT signals to simply exit process with status code 2
*
* signo - signal number
*/
void handle_SIGINT(int signo){
	exit(2);
}

/**
* Function: assignHomeDirectory
* -------------------------------------------
* Retrieves the user's home directory and assigns to given pointer to char
*
* homeDirectory - stores the full path of the user's home directory
*/
void assignHomeDirectory(char *homeDirectory)
{
	const char *tmp;
	// Check HOME environment variable first
	if ((tmp = getenv("HOME")) == NULL)
		// If HOME env variable empty then check the password entry of current user
    	tmp = getpwuid(getuid())->pw_dir;
    strcpy(homeDirectory, tmp);
}

/**
* Function: directoryExists
* -------------------------------------------
* Returns 1 if a given directory path exists; 0 otherwise
*
* destinationDirectory - stores the full path of the directory to check
*/
int directoryExists(char *destinationDirectory)
{
	DIR* dir = opendir(destinationDirectory);
	if (dir) 
	{
	    closedir(dir);
	    return 1;
	} else
	    return 0;
}

/**
* Function: travelToDirectory
* -------------------------------------------
* Changes the current directory to the directory by the given name
*
* destinationDirectory - stores the full path of the directory to change to
*/
void travelToDirectory(char *destinationDirectory)
{
	// Change directory as long as it exists and inform user if it doesn't
	if (directoryExists(destinationDirectory)) {
		chdir(destinationDirectory);
	} else
	    printf("The %s directory doesnt exist.\n",destinationDirectory);
}

/**
* Function: initializeDynamicPidArray
* -------------------------------------------
* Dynamically allocated space for an array of pid pointers
*
* initialSize - the beginning length to make the array
*/
pid_t * initializeDynamicPidArray(int initialSize)
{
	return malloc(sizeof(pid_t) * initialSize);
}

/**
* Function: assignPidToArray
* -------------------------------------------
* Assigns the value at the index of arrayFrom to arrayTo
*
* arrayTo - an array of pid ids to assign value to
* arrayFrom - an array of pid ids to assign value from
* index - the index of the value that will be assigned
*/
void assignPidToArray(pid_t *arrayTo, pid_t *arrayFrom, int index)
{
	arrayTo[index] = arrayFrom[index];
}

/**
* Function: freeDynamicArray
* -------------------------------------------
* Deallocated dynamic memory allocated to an array
*
* array - a dynamically allocated array of any type
*/
void freeDynamicArray(void *array)
{
	free(array);
}

/**
* Function: resizeDynamicArrayOfPids
* -------------------------------------------
* Makes room for additional items in an existing array by creating new array
* and copying the items over
*
* array - an array of pid ids
* currentSize - the current size of the pid array
* maxSize - the maximum size the array is allowed to get
*/
void resizeDynamicArrayOfPids(pid_t *array, int currentSize, int *maxSize) {
	// Only resize if the current size of array is at maximum
	if (currentSize == *maxSize)
	{
		pid_t *tmpArray = NULL;

		// Dynamically allocate a new array of pointers
		int newMaxSize = *maxSize * 2;
		tmpArray = realloc(array, newMaxSize);

		// Point the new array to existing pointers
		int i;
		for (i = 0; i < currentSize; i++)
		{
			assignPidToArray(tmpArray, array, i);
		}

		// Free up old array, assign new max size, and repoint old array to new resized one
		*maxSize = newMaxSize;
		//freeDynamicArray(array);
		array = tmpArray;
	}
}

/**
* Function: appendPidToArray
* -------------------------------------------
* Appends a pid id to the end of an existing array
*
* array - an array of pid ids
* newPid - a new pid id to append
* currentSize - a pointer to the current size of the pid array
* maxNumber - a pointer to the max size the array is allocated for
*/
void appendPidToArray(pid_t *array, pid_t newPid, int *currentSize, int *maxNumber)
{
	// Resize the array if necessary to make room for one more pid
	resizeDynamicArrayOfPids(array, *currentSize, maxNumber);
	// Append the pid to the end of the array
	array[*currentSize] = newPid;
	*currentSize += 1;
}

/**
* Function: removePidFromArray
* -------------------------------------------
* Removes a pid id from a given array of pid ids
*
* array - an array of pid ids
* pidToRemove - a pid id to find and delete
* currentSize - a pointer to the current size of the pid array
*/
void removePidFromArray(pid_t *array, pid_t pidToRemove, int *currentSize)
{
	int i;
	// Iterate over each item in array to find and remove a given pid
	for (i = 0; i < *currentSize; i++)
	{
		// If it's last item in array just set to 0 and decrement size by 1
		if ((i == *currentSize - 1) && (array[i] == pidToRemove))
		{
			array[i] = 0;
			*currentSize=*currentSize - 1;
		}
		// If it's not the last item then we need to shift all subsequent items back by one
		else if (array[i] == pidToRemove)
		{
			// Iterate over all indeces after the current one for the pid to remove
			int j = i;
			for (j = i; j < *currentSize; j++)
			{
				// Assign value at current index to value at next index as long as next is not last
				if (j+1 < *currentSize)
					array[j] = array[j + 1];
			}
			// Assign 0 to last index and decrement size of array by 1
			array[j + 1] = 0;
			*currentSize=*currentSize - 1;
		}
	}
}

/**
* Function: resolveBackgroundRun
* -------------------------------------------
* Given a foreground only mode indicator, this returns whether the process is allowed to run in background 
* or not
*
* foregrounfOnly - indicator for current mode (1 = in foreground only; 0 = not in foreground only)
* runInBackground - indicator parsed from input for background run or not
*/
int resolveBackgroundRun(int foregroundOnly, int runInBackground)
{
	if (foregroundOnly == 1)
		return 0;
	else
		return runInBackground;
}