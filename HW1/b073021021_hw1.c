#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		//getcwd, chdir, pid_t, fork, pipe
#include <sys/wait.h>		//waitpid
#include <sys/stat.h>  		//RDONLY...
#include <ctype.h>		// isspace()
#include <fcntl.h>	  	//RDONLY...

#define False 0
#define True 1
#define STD_INPUT 0
#define STD_OUTPUT 1
#define MAX_SIZE 1000

void reset();
void print_status();
void trim(char* cmd);
void get_command();
void exe_command();
void create_pipe();
void l_pipe(char** process1, char** process2);

char cmd[MAX_SIZE];
char *process1[MAX_SIZE], *process2[MAX_SIZE];
char *proc1_path[2] = {'\0', '\0'}, *proc2_path[2] = {'\0', '\0'};
char status[MAX_SIZE];
int exec_background = False;
int cmd_empty = False;
int need_pipe = False;
int proc1_redirect[2] = {False, False}, proc2_redirect[2] = {False, False};

int main() {
	while (True) {
		reset();
		print_status();
		get_command();
		if (!strcmp(cmd, "exit")) break;
		if (!cmd_empty) exe_command();
	}
	return 0;
}

void reset() {
	memset(cmd, '\0', sizeof(cmd));
	memset(process1, '\0', sizeof(process1));
	memset(process2, '\0', sizeof(process2));
	memset(proc1_path, '\0', sizeof(proc1_path));
	memset(proc2_path, '\0', sizeof(proc2_path));
	exec_background = False;
	cmd_empty = False;
	need_pipe = False;
	proc1_redirect[0] = False, proc1_redirect[1] = False;
	proc2_redirect[0] = False, proc2_redirect[1] = False;
}

void print_status() {
	memset(status, '\0', sizeof(status));
	getcwd(status, MAX_SIZE);
	printf("\033[0;32;34mB073021021_Shell:\033[36m~%s\033[0m$ ", status);
}

void trim(char* cmd) {
	while(isspace(*cmd))
		++cmd;

	char* tail = cmd;
	tail += strlen(cmd)-1;
	
	while(isspace(*tail)) {
		*tail = '\0';
		--tail;
	}

	cmd = tail;
}

void get_command() {
	fgets(cmd, sizeof(cmd), stdin);
	trim(cmd);

	if (cmd[strlen(cmd)-1] == '&') {
		exec_background = True;
		cmd[strlen(cmd)-1] = '\0';
	}
	
	char* pr1;
	char* pr2;
	pr1 = strtok(cmd, "|");
	pr2 = strtok(NULL, "|");

	if (pr1) {
		int index = 0;

		char *proc1[MAX_SIZE];
		memset(proc1, '\0', sizeof(proc1));
		proc1[index] = strtok(pr1, " ");

		while (proc1[index]) {
			index++;
			proc1[index] = strtok(NULL," ");
		}

		for (int i = 0, j = 0; proc1[i]; ++i) {
			if (!strcmp(proc1[i], "<")) {
				proc1_redirect[0] = True;
				proc1_path[0] = proc1[i+1];
				++i;
			}
			else if (!strcmp(proc1[i], ">")) {
				proc1_redirect[1] = True;
				proc1_path[1] = proc1[i+1];
				++i;
			}
			else {
				process1[j++] = proc1[i];
			}
		}
	}else 
		cmd_empty = True;
	
	if (pr2) {
		int index = 0;
		char *proc2[MAX_SIZE];
		need_pipe = True;
		proc2[index] = strtok(pr2, " ");

		while(proc2[index]) {
			index++;
			proc2[index] = strtok(NULL, " ");
		}

		for (int i = 0, j = 0; proc2[i]; ++i) {
			if (!strcmp(proc2[i], "<")) {
				proc2_redirect[0] = True;
				proc2_path[0] = proc2[i+1];
				++i;
			}
			else if (!strcmp(proc2[i], ">")) {
				proc2_redirect[1] = True;
				proc2_path[1] = proc2[i+1];
				++i;
			}
			else {
				process2[j++] = proc2[i];
			}
		}
	}
}

void exe_command() {
	if (!strcmp(cmd, "cd")) {	
		if (chdir(process1[1])) 
			printf("%s: %s: No such file or directory\n", process1[0], process1[1]);
	}

	pid_t pid = fork();
	int status;

	if (pid == -1) {
		puts("fork() error");
		return;
	}else if (pid) {	
		if (!exec_background) waitpid(pid, &status, 0);
	}else {
		if (need_pipe)
			create_pipe();
		else if (!proc1_redirect[0] && !proc1_redirect[1]) {
			if (execvp(process1[0],process1) < 0) {				
				if(strcmp(process1[0], "cd"))
 					printf("Can not execute !");
			}
		}else if (proc1_redirect[0] && !proc1_redirect[1]) {
			int fd_in = open(proc1_path[0], O_RDONLY);
			dup2(fd_in, STD_INPUT);
			if (execvp(process1[0], process1) < 0)
				puts("Can not execute !");
			close(fd_in);
		}else if (!proc1_redirect[0] && proc1_redirect[1]) {
			int fd_out = open(proc1_path[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup2(fd_out, STD_OUTPUT);
			if (execvp(process1[0], process1) < 0)
				puts("Can not execute !");
			close(fd_out);
		}else {
			int fd_in = open(proc1_path[0], O_RDONLY);
			int fd_out = open(proc1_path[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup2(fd_in, STD_INPUT);
			dup2(fd_out, STD_OUTPUT);
			if (execvp(process1[0], process1) < 0)
				puts("Can not execute !");
			close(fd_in);
			close(fd_out);
		}
		exit(0);
	}
}

void create_pipe() {
	int fd[2];

	if (pipe(&fd[0]) < 0) {
		puts("pipe() error");
		return;
	}

	pid_t pid = fork();
	int status;

	if (pid == -1) {
		puts("fork() error");
		return;
	}
	
	if (!pid) {
		close(fd[0]);
		dup2(fd[1], STD_OUTPUT);
		close(fd[1]);
		if (execvp(process1[0], process1) < 0) {
			puts("Can not execute !");
		}
		exit(0);
	}else {
		close(fd[1]);
		dup2(fd[0], STD_INPUT);
		close(fd[0]);

		pid_t ppid = fork();
		int sstatus;
		if (ppid == -1) {
			puts("fork() error");
		}else if (ppid) {
			waitpid(ppid, &sstatus, 0);
		}else {
			if (execvp(process2[0], process2) < 0)
				puts("Can not execute !");
			exit(0);
		}

		waitpid(pid, &status, 0);
	}
}