// CMPSC474/CMPEN441
//
// File Name: proj14.c
//
// Assisted by and Assisted line numbers:
//
// Your Name: Christian Dunn
//

//
// CMPSC474 Spring 2017
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define LINE_SIZE 1024
#define ARG_SIZE  512
#define READ  0
#define WRITE 1

char *getCMD();
char **getARGS(char *cmd_line);
int  redir_location(char **cmd_args);

// Function : main()
// Purpose  : Generate parent and child process to create shell interpreter
int main(int argc, char *argv[]) {
	// Define char pointers to hold user input and arguments
	char *cmd_line, **cmd_args;
	
	printf("Shell Interpreter [To exit use 'exit' command]\n\n");

	// While loop to perform until user elects to exit shell program
	while(1) {
		printf("cjd5551:<myshell> $ ");
		cmd_line = getCMD();
		if(strcmp(cmd_line, "exit\n") == 0)
			exit(0);
		cmd_args = getARGS(cmd_line);
		pid_t child_pid, parent_pid;
		int   parent_status, redir_loc;
		child_pid = fork();
		redir_loc = redir_location(cmd_args);
		if(child_pid == 0) {
			// Check if redirection character was found in arguments of user input
			// If redirection was found use open, dup2, close method to redirect i/o
			if(!(redir_loc <= 0)) {
                        	if(strcmp(cmd_args[redir_loc], "<") == 0) {
                                	int fd0 = open(cmd_args[redir_loc+1], O_RDONLY, 0);
                                	dup2(fd0, READ);
                                	close(fd0);
					cmd_args[redir_loc] = '\0';
                        	} else if(strcmp(cmd_args[redir_loc], ">") == 0) {
                                	int fd1 = creat(cmd_args[redir_loc+1], 0644);
                                	dup2(fd1, WRITE);
                                	close(fd1);
					cmd_args[redir_loc] = '\0';
                        	} // end if- else
                	} // end if
			execvp(cmd_args[0], cmd_args);
			exit(EXIT_FAILURE);
		} else {
			while(1) {
				parent_pid = waitpid(child_pid, &parent_status, WUNTRACED);
				if(WIFEXITED(parent_status) || WIFSIGNALED(parent_status))
					break;
			} // end while
		} // end if-else
	} // end while
	return 0;
}// end main()

// Function : redir_location()
// Purpose  : Check tokenized arguments for redirection character and return the element location
//            if redirection character exists within arguments.
int redir_location(char **cmd_args) {
	int i;
	for(i = 0; i < ARG_SIZE; i++) {
		if(cmd_args[i] == '\0') {
			return -1;
		} else if(strcmp(cmd_args[i], "<") == 0) {
			return i;		
		} else if (strcmp(cmd_args[i], ">") == 0) {
			return i;
		} // end if-else
	} // end for
	return -1;
} // end redir_location()
 
// Function : getCMD()
// Purpose  : Using getline function get user input from shell interface.
char *getCMD() {
	char*   command_line = NULL;
	ssize_t buffer_size  = 0;
	getline(&command_line, &buffer_size, stdin);
	return command_line;
} // end getCMD()

// Function : getARGS()
// Purpose  : Using tokens split the user input into a series of strings to store in string 
//            array of the individual argument parts.
char **getARGS(char *cmd_line) {
	char *current_token;
	char **all_tokens = malloc(ARG_SIZE*sizeof(char*));
	current_token = strtok(cmd_line, " \n");
	int  count = 0;
	while(current_token != NULL) {
		all_tokens[count] = current_token;
		current_token = strtok(NULL, " \n");
		count++;
	} // end while
	all_tokens[count] = NULL;
	return all_tokens;
} // end getARGS()

/**********SAMPLE OUTPUT**********
*
* cjd5551:~/labfiles/lab14/part2 $ ./proj14
* Shell Interpreter [To exit use 'exit' command]
*
* cjd5551:<myshell> $ ls
* org_shell.c  proj14  proj14.c  test.txt  test2.txt
* cjd5551:<myshell> $ echo hello > test.txt
* cjd5551:<myshell> $ man ls > test2.txt
* cjd5551:<myshell> $ exit
* cjd5551:~/labfiles/lab14/part2 $ 
*
*/




