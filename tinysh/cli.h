/***********************************************************************************************************
 * Filename: cli.h
 * Date: 7/15/2020
 * Author: Adams Rosales (rosalead@)
 *
 * Description: Contains declarations for CLI parsing and prompt display as the main interface with user
 **********************************************************************************************************/

#ifndef CLI_H_
#define CLI_H_

#define PATH_MAXIMUM 4096
#define MAX_INPUT_BUFFER 2048
#define MAX_ARGS 512
#define MAX_REDIRECT_FILENAME_SIZE 128

/*
 * Function:  parseArgs
 * --------------------------------
 * Utility function to accept and parse input from user
 *
 *	inputBuffer: holds each input line given in terminal
 *  inputArgs: array holding each individual argument/command when not a redirection or background process req
 *  redirectInput: binary flag denoting whether input needs redirection or not
 *  redirectOutput: binary glag denoting whether output needs redirection or not
 *	inputRedirect: holds the name of the file for redirecting input
 *  outputRedirect: holds the name of the file for redirecting output
 *	backgroundProcess: binary indicator to denote whether command should be executed in background or foreground
 */
void parseArgs(
	char *inputBuffer, char **inputArgs, int *redirectInput, int *redirectOutput,
	char *inputRedirect, char *outputRedirect, int *backgroundProcess);
/*
 * Function:  displayPrompt
 * --------------------------------
 * Utility function to display user prompt in terminal
 *
 */
void displayPrompt();


#endif /* CLI_H_ */
