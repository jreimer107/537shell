#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

char error_message[30] = "An error has occurred\n";

void printError() {
	write(STDERR_FILENO, error_message, strlen(error_message));
}

typedef struct {
	char **args;
	char *name;
	char *redirect;
	char *full;
	int numArgs;
	char builtin;
	char ok;
} Command;

//Edits the given string to not have excess white space.
//Returns number of words.
int destroyWhitespace(char* string) {
	if (strstr(string, ">") || string[0] == '\0') return 0; //Check for incorrect redirect chars.
	int numWords = 1;
	char *out = malloc(strlen(string) + 1);
	//first need to find first letter.
	int i = 0;
	int j = 0;
	while (isspace(string[i])) {
		i++;
		if (string[i] == '\0') {
			strcpy(string, "");
			return 0;
		}
	}
	//Have found first letter. Now need to parse through so that one space is left
	//between words.
	//Check for all white space
	while(i < strlen(string)) {
		if (!isspace(string[i])) out[j++] = string[i];
		else if (!isspace(string[i+1]) && string[i+1] != '\0') {
			out[j++] = ' ';
			numWords++;
		}
		i++;
	}
	out[j] = '\0';
	strcpy(string, out);
	free(out);
	return numWords;
}

int countTerms(char *string, char target) {
	int count = 1;
	while (*string) {
		if (*string == target) count++;
		string++;
	}
	return count;
}

/*Allocates space for an args array, places args, returns pointer to arg array
* For each arg:
* Malloc space for the argument.
* Get the argument from process into temp.
* Copy the arg into the malloced space.
*/
char** getArgs(int numArgs, char *process) {
	char** args;
	//Set temp to parse through given process.
	char *temp = strtok(process, " ");
	args = malloc(numArgs * sizeof(char*));
	args[0] = malloc(strlen(process) + 1);
	strcpy(args[0], temp);
	for (int i = 1; i < numArgs; i++) {
		temp = strtok(NULL, " ");
		args[i] = strdup(temp);
	}

	return args;
}

//Splits a string into a pre > and post >.
void getRedirect(char *redirect, char *process, char *string) {
	char *temp = strdup(string);
	char *tempPtr;
	tempPtr = strtok(temp, ">");
	strcpy(process, tempPtr);
	tempPtr = strtok(NULL, "\0");
	if (tempPtr) strcpy(redirect, tempPtr);
	free(temp);
}

/* Path
*User supplies paths to search for programs to use.
*This function replaces the the allocated paths to new ones supplied
*by user command. The zeroth element of path[] is "path", which is ignored
*when reading paths.
*/
int updatePaths(char **path, Command cmd, int numPaths){
	//Free old paths
	for (int i = 0; i < numPaths; i++) free(path[i]);
	free(path);

	//Allocate for and place new paths
	path = malloc((cmd.numArgs) * sizeof(char*));
	for (int i = 0; i < cmd.numArgs; i++){
		path[i] = malloc(strlen(cmd.args[i]) + 2);
		strcpy(path[i], cmd.args[i]);
		//Put a '/' at the end if there isn't one.
		if (path[i][strlen(path[i]) - 1] != '/')
		strcat(path[i], "/");
	}
	return cmd.numArgs;
}

void runCmds(Command *cmd, char** path, int numCmds, int numPaths) {
	int children = 0;
	char *correctPath[numCmds];
	for (int i = 0; i < numCmds; i++) {

		correctPath[i] = calloc(100,1);
		if (cmd[i].builtin == 0 && cmd[i].ok == 1) {
			char *currPath = calloc(100,1);
			int j = 0;
			//Find correct path
			while(j < numPaths - 1 && correctPath[i][0] == '\0') {
				strcpy(currPath, path[j + 1]);
				strcat(currPath, cmd[i].name);
				if (!access(currPath, X_OK)) {
					strcpy(correctPath[i], currPath);
				}
				j++;
			}
			free(currPath);
		}
	}

	//Now execute each nonbuiltin cmd
	for (int i = 0; i < numCmds; i++) {
		if (correctPath[i][0] != '\0' && cmd[i].ok == 1) {
			children++;
			if (fork() == 0) {
				//Redirect output
				if(cmd[i].redirect[0] != '\0') {
					int out = open(cmd[i].redirect, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					dup2(out, 1);
					close(out);
				}
				cmd[i].args[cmd[i].numArgs] = NULL;
				execv(correctPath[i], cmd[i].args);\
			}
		}
		else if (correctPath[i][0] == '\0' && cmd[i].builtin == 0 && cmd[i].ok == 1) {
			printError();
		}
	}

	//Wait for children and clean up
	for (int i = 0; i < children; i++) {
		wait(NULL);
		free(correctPath[i]);
	}
}

Command* buildCmds(char *line, int numCmds, int numChars) {
	char *linePtr = strtok_r(line, "&\n", &line);
	Command *cmd = malloc(numCmds * sizeof(Command));

	//For each parallel command
	for (int i = 0; i < numCmds; i++) {
		if (linePtr) {
			cmd[i].builtin = 0;
			cmd[i].ok = 1;
			//Set redirection
			//Get args, set name and args
			cmd[i].full = malloc(numChars);
			strcpy(cmd[i].full, linePtr);
			cmd[i].redirect = calloc(1, numChars);
			char *process = malloc(numChars);
			if (strstr(cmd[i].full, ">")) {
				getRedirect(cmd[i].redirect, process, cmd[i].full);
				cmd[i].ok = destroyWhitespace(cmd[i].redirect);
				if (cmd[i].redirect[0] == '\0' || cmd[i].ok != 1) {
					cmd[i].ok = 0;
					printError();
				}
			}
			else strcpy(process, cmd[i].full);

			//Smash excess white space
			cmd[i].numArgs = destroyWhitespace(process);
			if (process[0] == '\0' || cmd[i].numArgs == 0) {
				cmd[i].ok = 0;
			//	return NULL;
			}

			//Organize into arguments
			if (cmd[i].ok == 1) {
				cmd[i].args = getArgs(cmd[i].numArgs, process);
				cmd[i].name = malloc(strlen(cmd[i].args[0]) + 1);
				strcpy(cmd[i].name, cmd[i].args[0]);
			}
		}
		else cmd[i].ok = 0;
		linePtr = strtok_r(NULL, "&\n", &line);
	}
	return cmd;
}

int main(int argc, char *argv[]) {
	size_t n = 0;
	char *line = NULL;
	char **path;
	int numPaths = 2;
	int mode = 0;
	int numChars = 0;
	path = malloc(2 * sizeof(char*));
	path[0] = strdup("path");
	path[1] = strdup("/bin/");

	//Batch vs interactive mode
	if (argc == 2) {
		int in = open(argv[1], O_RDONLY);
		if (in == -1) {
			printError();
			exit(1);
		}
		dup2(in, 0);
		close(in);
		mode = 1;
	}
	else if (argc >= 2) {
		printError();
		exit(1);
	}

	//Parse each line
	if(!mode) printf("%s", "wish> ");
	numChars = getline(&line, &n, stdin);
	while (1) {
		if (numChars == -1) exit(0);
		Command *cmd = NULL;
		int numCmds = countTerms(line, '&');
		if (numCmds) {
			cmd = buildCmds(line, numCmds, numChars);
			if (cmd == NULL) numCmds = 0;
		}

		//Command Array built. Now run built-ins.

		//Builtins
		for (int i = 0; i < numCmds; i++) {
			if (cmd[i].ok == 1) {
				//exit
				if (!strcmp(cmd[i].name, "exit")) {
					cmd[i].builtin = 1;
					if (cmd[i].numArgs != 1) printError();
					else exit(0);
				}

				//cd
				else if (!strcmp(cmd[i].name, "cd")) {
					cmd[i].builtin = 1;
					if (cmd[i].numArgs != 2)printError();
					else if(chdir(cmd[i].args[1]) == -1) printError();
				}

				//path
				else if (!strcmp(cmd[i].name, "path")) {
					cmd[i].builtin = 1;
					numPaths = updatePaths(path, cmd[i], numPaths);
				}
			}
		}

		if (numPaths) runCmds(cmd, path, numCmds, numPaths);
		else printError();

		//Free up everything
		for (int i = 0; i < numCmds; i++) {
			if (cmd[i].ok) {
				for (int j = 0; j < cmd[i].numArgs; j++) free(cmd[i].args[j]);
				free(cmd[i].args);
				free(cmd[i].name);
				free(cmd[i].redirect);
				free(cmd[i].full);
			}
		}
		if (cmd) free(cmd);
		cmd = NULL;
		if (!mode) {
			printf("%s", "wish> ");
			fflush(stdout);
		}
		numChars = getline(&line, &n, stdin);
	}
	exit(0);
}
