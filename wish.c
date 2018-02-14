#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char error_message[30] = "An error has occurred\n";

void printError() {
	write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char *argv[]) {
	size_t n = 0;
	char *line = NULL;
	char *command = NULL;
	char *process = NULL;
	int numArgs = 0;
	char *strChk = malloc(4);

	if (argc == 1) {
		//while(1) {
		//INTERACTIVE MODE
			size_t n = 0;
			line = NULL;
			command = NULL;
			process = NULL;
			numArgs = 0;

			//Parse each line
			printf("%s", "wish> ");
			getline(&line, &n, stdin);
			while (line != NULL) {
				//Get command
				command = strtok(line, "&");
				//Remove newline from command if there is one
				command = strtok(command, "\n");
				//First arg is the process to run
				process = strtok(NULL, " ");

				//Now need to get args

				//Find number of args
				printf("Command: %s\n", command);
				for (int i = 0; i < strlen(command); i++){
					if (command[i] == ' ' || command[i] == EOF) numArgs++;
				}

				//Create array to put args in and put them there
				char *argsArray[numArgs];
				for (int i = 0; i < numArgs; i++) {
					//argsArray[i] = malloc(strlen(command) * sizeof(char));
					argsArray[i] = strtok(NULL, " ");
				}  

				//We now have our proc name, our number of args, and an array of them.
				//Can now execute process.

				//Going to code for built in commands first as those are probably
				//more likely to occur.
				//Exit
				strcpy(strChk, "exit");
				//printf("%p\n", string);
				//printf("%s%s.", process, strChk);
				if (!strcmp(process, strChk)) exit(0);

				//cd
				strcpy(strChk, "cd");
				if (!strcmp(process, strChk)) {
					if (numArgs != 1) { 
						printError();
						printf("%s\n", "Bad Arg");
						printf("numArgs: %d\n", numArgs);
					}
					else {
						if(chdir(argsArray[0]) == -1) printError();
					}	
				}





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
				printf("%s", "wish> ");
				getline(&line, &n, stdin);
			}
		//}
	}
	exit(0);
}
