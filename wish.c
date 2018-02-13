#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define String = char*;

char error_message[30] = "An error has occurred\n";

void printError() {
	write(STDERR_FILENO, error_message, strlen(error_message));
}

int getArgs(char *args[], char *line) {
	//parse through line until & or \0
	//add each word to an array of strings


	}
}

int main(int argc, char *argv[]) {
	while(1) {
		if (argc != 1) {
		//INTERACTIVE MODE
			size_t n = 0;
			int numChars;
			char *line = NULL;
			char *command = NULL;
			char *arg = NULL;
			char *process = NULL;
			int numArgs = 0;

			//Parse each line
			numChars = getline(&line, &n, stdin);
			while (line != NULL) {
				//Get command
				command = strtok(line, '&');
				//First arg is the process to run
				process = strtok(command, ' ');
				//Now need to get args
				//Obtain number of args, malloc a *char[] for them
				for (int i = 0; i < strlen(command); i++)
					if (command[i] == ' ') numArgs++;
				char *argsArray[] = calloc(strlen(command), sizeof(char));





				// while (curr != NULL) {
				// 	//Built in commands
				// 	//Exit
				// 	if (*curr == "exit") exit(0);
				// 	//CD
				// 	if (*curr = "cd") {
				// 		curr = strtok(line);
				// 		next = strtok(line);
				// 		//Check
				// 		if (curr == NULL || next != "&" || next != NULL)
				// 				printError();
				//
				//
				// 	}
				// }
			}
		}
	}
}

