/***********************************************************************************************************
 * Filename: shell.h
 * Date: 7/18/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Contains declarations for the core shell operations (executing commands)
 **********************************************************************************************************/

#ifndef SHELL_H_
#define SHELL_H_

#include <sys/types.h>

int checkBackgroundPidStatus(pid_t pid);
/**
* Function: monitorBackgroundPids
* -------------------------------------------
* Track and print the status of processes running in the background; remove completed background tasks from
* collection of background processes
*
* pidArray - an array of pid ids running in the background
* terminatedPids - an array of background pids that have completed
* numberOfPids - the length of the pid array
* maxNumberOfPids - the current size allocated to the pid array
* numberOfTerminatedPids - the length of the terminatedPids array
* maxNumberOfTerminatedPids - the current size allocated to the terminatedPids array
*/
void monitorBackgroundPids(
	pid_t *pidArray, pid_t *terminatedPids, 
	int *numberOfPids, int *maxNumberOfPids,
	int *numberOfTerminatedPids, int *maxNumberOfTerminatedPids);
/**
* Function: processCommand
* -------------------------------------------
* Executes a command from an array of arguments given to CLI
*
* inputArgs - contains the arguments given to CLI component
* exitIndicator - used to indicate whether an exit command has been found and the shell should exit
* terminationStatus - the last known child termination status
* signalOrTerminated - 0 = normal exit, 1 = signal terminated
* runInBackground - 0 = run in foreground, 1 = run in background
* redirectInput - 0 = don't redirect input, 1 = redirect input
* redirectOutput - 0 = don't redirect output, 1 = redirect output
* inputRedirect - name of file to redirect input via stdin
* outputRedirect - name of file to redirect output via stdout
*/
pid_t processCommand(
	char **inputArgs, int *exitIndicator, int *terminationStatus, int *signalOrTerminated, int *runInBackground,
	int redirectInput, int redirectOutput, char *inputRedirect, char *outputRedirect
	);

#endif /* SHELL_H_ */
