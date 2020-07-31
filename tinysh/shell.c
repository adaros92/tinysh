/***********************************************************************************************************
 * Filename: shell.c
 * Date: 7/18/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Contains implementations for the core shell operations (executing commands)
 **********************************************************************************************************/

#include "shell.h"
#include "util.h"
#include "cli.h"

#include <unistd.h>
#include <string.h>
#include <sys/wait.h> 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h> 
#include <signal.h>

/**
* Function: executeCommand
* -------------------------------------------
* Executes a given command by searching in current path for matching file/program
*
* inputArgs - an array of inputs given to the shell where the command is available at
*	inputArgs[0]
*/
void executeCommand(char **inputArgs) 
{
	execvp(inputArgs[0], inputArgs);
	perror(inputArgs[0]);
}

/**
* Function: executeChangeDirectory
* -------------------------------------------
* Defines logic for the 'cd' shell command, which changes the current directory
* to the one by the given path
*
* targetDirectory - the full path of the directory to change to
*/
void executeChangeDirectory(char *targetDirectory)
{
	// Get home directory
	char homeDirectory[PATH_MAXIMUM];
	assignHomeDirectory(homeDirectory);

	// If no target provided then travel to home as default option
	if (targetDirectory == NULL || targetDirectory == 0)
		travelToDirectory(homeDirectory);
	else
		travelToDirectory(targetDirectory);
}

/**
* Function: executeStatusCommand
* -------------------------------------------
* Defines logic for the 'status' shell command, which prints the exit status
* of the last child process that ran
*
* terminationStatus - the last known child termination status
* signalOrTerminated - 0 = normal exit, 1 = signal terminated
*/
void executeStatusCommand(int *terminationStatus, int *signalOrTerminated)
{
	// Process exited successfully
	if (*signalOrTerminated == 0)
		printf("exit value %d\n", *terminationStatus);
	// Process was killed by a signal
	else if (*signalOrTerminated == 1)
		printf("terminated by signal %d\n", *terminationStatus);
}

/**
* Function: processSingleThreadedCommand
* -------------------------------------------
* Executes commands that don't need to run as children of main process
*
* inputArgs - contains the arguments given to CLI component
* exitIndicator - has the user specified that they want to exit 0 = no 1 = yes
* terminationStatus - the last known child termination status
* signalOrTerminated - 0 = normal exit, 1 = signal terminated
*/
void processSingleThreadedCommand(char **inputArgs, int *exitIndicator, int *terminationStatus, int *signalOrTerminated)
{
	if (strncmp(inputArgs[0], "cd", 2) == 0)
		executeChangeDirectory(inputArgs[1]);

	else if (strncmp(inputArgs[0], "exit", 4) == 0)
		*exitIndicator = 1;

	else if (strncmp(inputArgs[0], "status", 5) == 0)
		executeStatusCommand(terminationStatus, signalOrTerminated);
}

/**
* Function: assignChildStatus
* -------------------------------------------
* Records a child process' exit or signal termination status
*
* childStatus - the child status recorded by parent process via waitpaid
* terminationStatus - a pointer to the last known termination status that will be reassigned
* signalOrTerminated - 0 = normal exit, 1 = signal terminated
*/
void assignChildStatus(int *childStatus, int *terminationStatus, int *signalOrTerminated)
{
	// Child terminated normally so record the exit status
	if(WIFEXITED(*childStatus))
	{
		*signalOrTerminated = 0;
		*terminationStatus = WEXITSTATUS(*childStatus);
	}
	// Child was terminated by a signal so record the terminating signal
    else if (WIFSIGNALED(*childStatus))
    {
    	*signalOrTerminated = 1;
    	*terminationStatus = WTERMSIG(*childStatus);
    }
}

/**
* Function: checkBackgroundPidStatus
* -------------------------------------------
* Checks the status of a process currently running in the background and returns if
* the process completed or not
*
* pid - the id of the background process
*/
int checkBackgroundPidStatus(pid_t pid)
{
	int childStatus;
	int terminationStatus;
	int signalOrTerminated;
	pid_t returnStatus;

	// Check the status of the ongoing background process
	returnStatus = waitpid(pid, &childStatus, WNOHANG);
	// Print the status if the background process completed and return 1 to denote completion
	if (returnStatus > 0)
	{
		printf("background pid %d is done: ", pid);
		// What was the exit or signal code
		assignChildStatus(&childStatus, &terminationStatus, &signalOrTerminated);
		// Print status to stdout
		executeStatusCommand(&terminationStatus, &signalOrTerminated);
		return 1;
	}
	return 0;
}

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
	int *numberOfTerminatedPids, int *maxNumberOfTerminatedPids)
{
	int i;
	int bgPidStatus;
	// Iterate over existing background pids and print their statuses
	for (i = 0; i < *numberOfPids; i++)
	{
		// Print statuses of background pid and get back whether it finished or not
		bgPidStatus = checkBackgroundPidStatus(pidArray[i]);
		// Keep track of pid in an array of pids to delete if the corresponding process terminated
		if (bgPidStatus)
			appendPidToArray(terminatedPids, pidArray[i], numberOfTerminatedPids, maxNumberOfTerminatedPids);
	}
	// Iterate over pids of processes that ended
	for (i = 0; i < *numberOfTerminatedPids; i ++)
	{
		// Remove the pid from the array of ongoing background pids
		removePidFromArray(pidArray, terminatedPids[i], numberOfPids);
		terminatedPids[i] = 0;
	}
	*numberOfTerminatedPids = 0;
}

/**
* Function: processMultiThreadedCommand
* -------------------------------------------
* Executes a command as a child process and returns the pid created
*
* inputArgs - contains the arguments given to CLI component
* terminationStatus - the last known child termination status
* signalOrTerminated - 0 = normal exit, 1 = signal terminated
* runInBackground - 0 = run in foreground, 1 = run in background
*/
pid_t processMultiThreadedCommand(
	char **inputArgs, int *terminationStatus, int *signalOrTerminated, int *runInBackground,
	int redirectInput, int redirectOutput, char *inputRedirect, char *outputRedirect)
{
	int childStatus;
	int backgroundChildStatus;
	pid_t spawnPid = fork();
	int outputFileDescriptor;
	int inputFileDescriptor;
	struct sigaction SIGINT_action = {0};
	SIGINT_action.sa_handler = handle_SIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;

	switch(spawnPid)
	{
	    case -1:
	    	// Error creating child process
	    	perror("fork()\n");
	    	break;
	    case 0:
	    	if (*runInBackground != 1)
	    		sigaction(SIGINT, &SIGINT_action, NULL);
	    	// Try to redirect output first
	    	outputFileDescriptor = redirectStdout(redirectOutput, outputRedirect);
	    	if (outputFileDescriptor < 0)
	    	{
	    		printf("cannot open %s for output\n", outputRedirect);
	    		exit(1);
	    	}
	    	closeFile(redirectOutput, outputFileDescriptor);
	    	// then input
	    	inputFileDescriptor = redirectStdin(redirectInput, inputRedirect);
	    	if (inputFileDescriptor < 0)
	    	{
	    		printf("cannot open %s for input\n", inputRedirect);
	    		exit(1);
	    	}
	    	closeFile(redirectInput, inputFileDescriptor);
	    	// Child process
	    	executeCommand(inputArgs);
	    	exit(1);
	    	break;
	    default:
	   		if (*runInBackground == 1)
	   		{
	   			// Parent process launched child in background
	   			waitpid(spawnPid, &backgroundChildStatus, WNOHANG);
	   			printf("background pid is %d\n", spawnPid);
	   		}
	   		else
	   		{
	   			// Parent process waits for child to complete
	   			spawnPid = waitpid(spawnPid, &childStatus, 0);
	   			// Keep track of the child's exit or signal termination status
	    		assignChildStatus(&childStatus, terminationStatus, signalOrTerminated);
	    		if (*signalOrTerminated == 1)
	    			executeStatusCommand(terminationStatus, signalOrTerminated);
	    	}
	    	break;
	} 
	return spawnPid;
}

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
	int redirectInput, int redirectOutput, char *inputRedirect, char *outputRedirect)
{
	// Process build-in commands that should not be run in child process
	if 
	(
		strncmp(inputArgs[0], "cd", 2) == 0 || 
		strncmp(inputArgs[0], "exit", 4) == 0 ||
		strncmp(inputArgs[0], "status", 5) == 0 ||
		strncmp(inputArgs[0], "#", 1) == 0
	) {
		processSingleThreadedCommand(inputArgs, exitIndicator, terminationStatus, signalOrTerminated);
		*runInBackground = 0;
		return getpid();
	}
	// Process any other command in child process
	else
		return processMultiThreadedCommand(
			inputArgs, terminationStatus, signalOrTerminated, runInBackground,
			redirectInput, redirectOutput, inputRedirect, outputRedirect);
}
