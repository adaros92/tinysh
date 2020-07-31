/***********************************************************************************************************
 * Filename: smallsh.c
 * Date: 7/15/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Implements main terminal loop 
 **********************************************************************************************************/

#include "cli.h"
#include "shell.h"
#include "util.h"

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define STARTING_NUMBER_OF_BG_PIDS 4

int FOREGROUND_ONLY = 0;

/**
* Function: handle_SIGTSTP
* -------------------------------------------
* Custom implementation of SIGTSTP signal to enter or exit foreground process only mode
*
* signo - signal number
* foregroundOnly - pointer to binary indicator (0 = not in foreground only mode; 1 = in foreground only mode)
*/
void handle_SIGTSTP(int signo){
	if (FOREGROUND_ONLY == 0)
	{
		FOREGROUND_ONLY = 1;
		printf("Entering foreground-only mode (& is now ignored)\n");
	} else if (FOREGROUND_ONLY == 1)
	{
		FOREGROUND_ONLY = 0;
		printf("Exiting foreground-only mode\n");
	}
}

int main(int argc, char *argv[])
{
	int runInBackground = 0;
	int exit = 0;
	int terminationStatus = 0;
	int signalOrTerminated = 0;
	int numberOfPids = 0;
	int numberOfTerminatedPids = 0;
	int redirectInput = 0;
	int redirectOutput = 0;
	int maxNumberOfTerminatedPids = STARTING_NUMBER_OF_BG_PIDS;
	int maxNumberOfPids = STARTING_NUMBER_OF_BG_PIDS;

	char inputBuffer[MAX_INPUT_BUFFER];
	char *inputArgs[MAX_ARGS];
	char workingDirectory[PATH_MAXIMUM];
	char inputRedirect[MAX_REDIRECT_FILENAME_SIZE];
	char outputRedirect[MAX_REDIRECT_FILENAME_SIZE];
	pid_t *pidArray = NULL;
	pid_t *terminatedPids = NULL;

	// Custom signal handling
	struct sigaction ignore_action = {0}, SIGTSTP_action = {0};
	// Ignore sigint at the parent process level
	ignore_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &ignore_action, NULL);
	// Handle foreground only mode signals
	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	memset(inputArgs, 0, sizeof(inputArgs));

	// Initialize arrays to keep track of individual pids and their statuses
	pidArray = initializeDynamicPidArray(maxNumberOfPids);
	terminatedPids = initializeDynamicPidArray(maxNumberOfTerminatedPids);

	// Keep track of the working directory the shell was launched from
	assignCurrentWorkingDirectory(workingDirectory);

	// Keep accepting user commands unless told otherwise
	while (!exit)
	{
		// First display the prompt
		displayPrompt();
		// Third accept user input and parse the arguments given by user
		parseArgs(
			inputBuffer, inputArgs, &redirectInput, &redirectOutput, 
			inputRedirect, outputRedirect, &runInBackground);
		// Overwrite run in background indicator if currently in foreground only
		runInBackground = resolveBackgroundRun(FOREGROUND_ONLY, runInBackground);
		// Only if command was given attempt to process
		if (inputArgs[0] != 0)
		{
			// Fourth run the command from user
			pid_t tmpPid = processCommand(
				inputArgs, &exit, &terminationStatus, &signalOrTerminated, &runInBackground,
				redirectInput, redirectOutput, inputRedirect, outputRedirect);
			// Fifth if a command was run in background then keep track of it
			if (runInBackground == 1)
				appendPidToArray(pidArray, tmpPid, &numberOfPids, &maxNumberOfPids);
			// Sixth clear the collection of arguments parsed for next round and reset background indicator
			memset(inputArgs, 0, sizeof(inputArgs));
			runInBackground = 0;
			redirectInput = 0;
			redirectOutput = 0;
		}
		// Last monitor the ongoing background processes
		monitorBackgroundPids(
			pidArray, terminatedPids, &numberOfPids, &maxNumberOfPids,
			&numberOfTerminatedPids, &maxNumberOfTerminatedPids);
	}
	return 0;
}
