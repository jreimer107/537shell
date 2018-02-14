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
				//First arg is the process to run
				process = strtok(command, " ");
				//Now need to get args

				//Find number of args
				for (int i = 0; i < strlen(command); i++){
					if (command[i] == ' ') numArgs++;
				}

				//Create array to put args in and put them there
				char *argsArray[numArgs];
				for (int i = 0; i < numArgs; i++) {
					//argsArray[i] = malloc(strlen(command) * sizeof(char));
					argsArray[i] = strtok(command, " ");
				}

				//We now have our proc name, our number of args, and an array of them.
				//Can now execute process.

				//Going to code for built in commands first as those are probably
				//more likely to occur.
				//Exit
				char *string = "exit";
				printf("%p\n", string);
				//strcpy(string, "exit");
				if (!strcmp(process, string)) exit(0);
				else printf("%s\n", "nope");





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
