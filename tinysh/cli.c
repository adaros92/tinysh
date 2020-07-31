/***********************************************************************************************************
 * Filename: cli.c
 * Date: 7/15/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Implements CLI parsing and prompt display as the main interface with user
 **********************************************************************************************************/

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Function:  parseArgs
 * --------------------------------
 * Utility function to accept and parse input from user
 *
 *	inputBuffer: holds each input line given in terminal
 *  inputArgs: array holding each individual argument/command when not a redirection or background process req
 *	inputRedirect: holds the name of the file for redirecting input
 *  outputRedirect: holds the name of the file for redirecting output
 *	backgroundProcess: binary indicator to denote whether command should be executed in background or foreground
 */
void parseArgs(
    char *inputBuffer, char **inputArgs, int *redirectInput, int *redirectOutput,
    char *inputRedirect, char *outputRedirect, int *backgroundProcess)
{
	char *token;
	char *separator = " ";
	int nextIsInputRedirect = 0;
	int alreadyRedirectedInput = 0;
	int nextIsOutputRedirect = 0;
	int alreadyRedirectedOutput = 0;
	int argsCount = 0;
    fgets(inputBuffer, MAX_INPUT_BUFFER, stdin);

    // If not command was given then nothing to do
    if (inputBuffer[0] != '\n')
    {

        // Tokenize the input given by user and iterate over each token where any two tokens are separated by a space
        token = strtok(inputBuffer, separator);
        while (token != NULL)
        {
        	// First remove trailing newline from fgets
        	token[strcspn(token, "\n")] = 0;
        	// An input redirection was provided with "<" so record input file name
        	if (nextIsInputRedirect) 
        	{
        		strcpy(inputRedirect, token); 
        		nextIsInputRedirect = 0;
        		alreadyRedirectedInput = 1;
        	} 
        	// An output redirection was provided with ">" so record output file name
        	else if (nextIsOutputRedirect)
        	{
        		strcpy(outputRedirect, token); 
        		nextIsOutputRedirect = 0;
        		alreadyRedirectedOutput = 1;
        	}
        	// A request to run command in background was given so keep track in binary indicator
        	else if (strncmp(token, "&", 1) == 0)
        		*backgroundProcess = 1;
        	// A request to redirect input was given so keep note that next argument is input filename
        	else if (strncmp(token, "<", 1) == 0 && !alreadyRedirectedInput)
            {
        		nextIsInputRedirect = 1;
                *redirectInput = 1;
            }
        	// A request to redirect output was given so keep note that next argument is output filename
        	else if (strncmp(token, ">", 1) == 0 && !alreadyRedirectedOutput)
            {
        		nextIsOutputRedirect = 1;
                *redirectOutput = 1;
            }
        	// Anything else refers to a command itself or other argument not handled above
        	else
        	{
        		inputArgs[argsCount] = token;
        		argsCount++;
        	}

        	// Get next token
        	token = strtok(NULL, separator);
        }
        inputArgs[argsCount] = NULL;
    }
}

/*
 * Function:  displayPrompt
 * --------------------------------
 * Utility function to display user prompt in terminal
 */
void displayPrompt() 
{
	fprintf(stdout, ":");
}
