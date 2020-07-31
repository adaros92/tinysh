/***********************************************************************************************************
 * Filename: util.c
 * Date: 7/18/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Contains declarations for utility functions used by other components of the program
 **********************************************************************************************************/

#ifndef UTIL_H_
#define UTIL_H_

#include <sys/types.h>

/**
* Function: redirectStdout
* -------------------------------------------
* Redirects standard out to a given file and returns file descriptor to close later
*
* redirectOutput - 0 = no, don't redirect stdout; 1 = yes, redirect away
* outputRedirect - the name of the file to redirect output to
*/
int redirectStdout(int redirectOutput, char *outputRedirect);
/**
* Function: redirectStdin
* -------------------------------------------
* Redirects standard in from a given file and returns file descriptor to close later
*
* redirectInput - 0 = no, don't redirect stdin; 1 = yes, redirect away
* outputRedirect - the name of the file to redirect output to
*/
int redirectStdin(int redirectInput, char *inputRedirect);
/**
* Function: closeFile
* -------------------------------------------
* Closes a file by the given fileDescriptor
*
* redirection - denotes whether a file has been opened
* fileDescriptor - the file descriptor to close
*/
void closeFile(int isFileOpen, int fileDescriptor);
/**
* Function: assignCurrentWorkingDirectory
* -------------------------------------------
* Retrieves the current working directory and assigns to given pointer to char
*
* workingDirectory - stores the full path of the current working directory
*/
void assignCurrentWorkingDirectory(char *workingDirectory);
/**
* Function: handle_SIGINT
* -------------------------------------------
* Custom implementation of SIGINT signals to simply exit process with status code 2
*
* signo - signal number
*/
void handle_SIGINT(int signo);
/**
* Function: assignHomeDirectory
* -------------------------------------------
* Retrieves the user's home directory and assigns to given pointer to char
*
* homeDirectory - stores the full path of the user's home directory
*/
void assignHomeDirectory(char *homeDirectory);
/**
* Function: travelToDirectory
* -------------------------------------------
* Changes the current directory to the directory by the given name
*
* destinationDirectory - stores the full path of the directory to change to
*/
void travelToDirectory(char *destinationDirectory);
/**
* Function: initializeDynamicPidArray
* -------------------------------------------
* Dynamically allocated space for an array of pid status pointers
*
* arrayIn - the array to initialize
* initialSize - the beginning length to make the array
*/
pid_t * initializeDynamicPidArray(int initialSize);
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
void appendPidToArray(pid_t *array, pid_t newPid, int *currentSize, int *maxNumber);
/**
* Function: removePidFromArray
* -------------------------------------------
* Removes a pid id from a given array of pid ids
*
* array - an array of pid ids
* pidToRemove - a pid id to find and delete
* currentSize - a pointer to the current size of the pid array
*/
void removePidFromArray(pid_t *array, pid_t pidToRemove, int *currentSize);
/**
* Function: resolveBackgroundRun
* -------------------------------------------
* Given a foreground only mode indicator, this returns whether the process is allowed to run in background 
* or not
*
* foregrounfOnly - indicator for current mode (1 = in foreground only; 0 = not in foreground only)
* runInBackground - indicator parsed from input for background run or not
*/
int resolveBackgroundRun(int foregroundOnly, int runInBackground);

#endif /* UTIL_H_ */
