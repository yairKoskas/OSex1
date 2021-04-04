//Yair Koskas 325699999
#define _POSIX_SOURCE
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define DELIMITER " "
#define APOSTROPHES '"' 
#define DONE "DONE"
#define RUNNING "RUNNING"
/**
 * Node Struct
 * @param name the name of the command
 * @param state the state of the command(DONE/RUNNING)
 * @param pid the pid of the process of the command
 */
typedef struct Node {
    char name[100];
    char state[100];
    pid_t pid;
} Node;

// global array of commands for the jobs and history commands
static Node commands[100];

/**
 * @name del
 * @brief deletes all occurences of a character in a string
 * @param str the string to be modified
 * @param ch the char to remove
 */
void del(char str[100], char ch) {
   int i, j = 0;
   int size;
   char ch1;
   char str1[100];
   strcpy(str1,str);
   for (i = 0; i < 100; i++) {
      if (str1[i] != ch) {
         ch1 = str1[i];
         str[j] = ch1;
         j++;
      }
   }
   str[j] = '\0';
}

/**
 * @name printCommand
 * @brief prints out a command and it's state
 */
void printCommand(Node n) {
    if(strchr(n.name,'&') != NULL) {
        n.name[strlen(n.name)-2] = '\0';
    }
    printf("%s %s\n",n.name,n.state);
}

/**
 * @name update
 * @brief updates the state of all child processes started by the program
 * @param index the index of the commands array
 */
void update(int index) {
    int status;
    int i;
    for (i=0;i<index;++i) {
        if(!strcmp(commands[i].state,RUNNING) && strcmp(commands[i].name,"history")) {
            int r = kill(commands[i].pid,0);
            if(r < 0 || errno == ESRCH) {
                strcpy(commands[i].state,DONE);
            } else {
                strcpy(commands[i].state,RUNNING);
            }
        }
    }
}

/**
 * @name history
 * @brief prints out the history of commands used by the user
 * @param index the index of the commands array
 */
void history(int index) {
    update(index);
    int i;
    for(i=0;i<index;++i) {
        printCommand(commands[i]);
    }
}

/**
 * @name jobs
 * @brief prints out all of the background processes
 * @param index the index of the commands array
 */
void jobs(int index) {
    update(index);
    int i;
    for(i=0;i<index;++i) {
        if(!strcmp(commands[i].state,RUNNING)) {
            if(strchr(commands[i].name,'&') != NULL) {
                commands[i].name[strlen(commands[i].name)-2] = '\0';
            }
            printf("%s\n",commands[i].name);
        }
    }
}

/**
 * @name removeLineDelimiter
 * @brief removing the '\n' character from the string
 * @param command the string to be modified
 */
void removeLineDelimiter(char command[100]) {
	int i;
    for(i=0;i<100;++i) {
        if(command[i] == '\n') {
            command[i] = '\0'; //string delimiter
            break;
        }
    }
}

/**
 * @name parse_command
 * @brief parsing a shell command for execvp use
 * @param command the command to parse
 * @param file a pointer to the string specifing the filename for the execvp function
 * @param arguments a pointer to the vector of arguments used by the execvp function
 * @param background an indicator variable indicating if the command should run in foreground or background
 * @return the number of arguments
 */
int parseCommand(char command[100],char **file, char* arguments[100],int *background) {
    char *token = strtok(command,DELIMITER);
    int count = 0;
    *file = token;
    *background = 0;
    int index = 0;
    do {
        if (!strcmp(token,"&")) {
            *background = 1;
            break;
        }
        arguments[index++] = token;
        token = strtok(NULL,DELIMITER);
        count++;
    } while (token != NULL);
    arguments[index] = NULL;
    return count;
}
/**
 * @name cd
 * @brief changing the working directory for the current process
 * @param arguments the path to go to from current directory
 */ 
void cd(char* arguments[100],int r) {
    static char oldpwd[100];
    char *home = getenv("HOME"); //guranteed to be a valid directory
	if(r > 2) {
    	printf("Too many arguments\n");
		return;
	} else if (r == 1) {
        getcwd(oldpwd,100);
        int r = chdir(home);
		return;	
	}
    //now change to path
    if (!strcmp(arguments[1],"-")) {
        char temp[100];
        getcwd(temp,100);
        int r = chdir(oldpwd);
        strcpy(oldpwd,temp);
        if (r == -1) {
            printf("chdir failed\n");
        }
    } else if (!strcmp(arguments[1],"~")) {
        getcwd(oldpwd,100);
        int r = chdir(home);
        if (r == -1) {
            printf("chdir failed\n");
        }
    } else {
        getcwd(oldpwd,100);
		//replace ~ with HOME
		if(strchr(arguments[1],'~') != NULL) {
			char path[100];
			strncpy(path,home,strlen(home));
			strcpy(path+strlen(home),arguments[1]+1);
			int r = chdir(path);
        	if (r == -1) {
            	printf("chdir failed\n");
        	}
		} else {
        	int r = chdir(arguments[1]);
        	if (r == -1) {
            	printf("chdir failed\n");
        	}
		}
    }
}

int main() {
    int index = 0;
    while(1) {
        char command[100];
        char *arguments[100];
        int status;
        int background=0;
        char *file;
        printf("$ ");
        fflush(stdout);
        fgets(command,100,stdin);
        if(!strcmp(command,"\n")) { // special case for empty line
            continue;
        }
        if(!strncmp(command,"echo",4)) { // special case for echo command
            del(command,APOSTROPHES);
        }
        removeLineDelimiter(command);
        strcpy(commands[index].name, command);
        int r = parseCommand(command,&file,arguments,&background);
        if(!strcmp(file,"cd")) {
			cd(arguments,r);
            strcpy(commands[index].state, DONE);
        } else if (!strcmp(file,"exit")) {
            exit(0);
        } else if (!strcmp(file,"history")) {
            strcpy(commands[index].state, RUNNING);
            history(index+1);
            strcpy(commands[index].state, DONE);
        } else if (!strcmp(file,"jobs")) {
            strcpy(commands[index].state, DONE);
            jobs(index+1);
        } else {
            pid_t pid = fork();
            if (pid == -1) {
                printf("fork failed\n");
            }
            if (pid == 0) { //we're the child process
                int success = execvp(file,arguments);
                fflush(stdout);
                if (success == -1) {
                    printf("exec failed\n");
                }
                exit(0);
            } else { //we're the parent process
                if(background) {
                    waitpid(pid,&status,WNOHANG);
                    strcpy(commands[index].state, RUNNING);
                    commands[index].pid = pid;
                } else {
                    wait(&status);
                    strcpy(commands[index].state, DONE);
                    commands[index].pid = pid;
                }
            }
        }
        index++;
    }    
}
