#include "myheader.h"
//#include<curses.h>
int exec_cmd(char** command_tokens)
{
	pid_t pid;
	pid=fork(); //create child process
	if(pid<0) //if error in creating child process
	{
		perror("Error in Child Process Creation \n");
		return -1;
	}
	else if(pid==0) //if child process
	{						//got idea from gnu.org
		int fdin,fdout;
		setpgid(pid,pid); //set process group id same as the process id
		if(input_redi)
		{
			fdin=open_inputfile(); //if the commmand is input redirected then open the input file
		}
		if(output_redi)
		{
			fdout=open_output_file(); //if the command is ouput redirected then open the output file
		}
		if(is_background==0)
		{
			tcsetpgrp(shell,getpid()); //assign the terminal to the process if it is not a background process
		}
			signaldefaults();
		   //put the default signals in the process                
	int execute_status;
	execute_status=execvp(command_tokens[0],command_tokens);
	if(execute_status<0)
	{
		printf("Error in execution of the command \n");
		_exit(-1); //exit with error
	}
	_exit(0); //exit child after execution
}
if(is_background==0)
{
	tcsetpgrp(shell,pid); //so that parent doesnt interfere while child is using the terminal/shell
	add_process(pid,command_tokens[0]); //add the created process in the proc_list
	int status;
	fgpid=pid;
	waitpid(pid,&status,WUNTRACED); //wait till the child has  finished
	if(!WIFSTOPPED(status)) //if child was stopped by a signal then remove it from the processlist
	{
		remove_process(pid);
	}
	else
	{
		printf("\n %s with Process Id %d has stopped ! \n",command_tokens[0],pid);
	}
		tcsetpgrp(shell,my_pgid);//the parent process is again in control of the terminal
}
else
{	
	printf(" [%d]-------------------%d \n",numofjobs,pid); //if the process is executed as background then display its job number
	add_process(pid,command_tokens[0]);				//and process id
	return 0;
}
}
void normalcmd(int tokens,char** command_tokens,char* command_copy)
{
	if(tokens>0)
	{			char* ent=command_tokens[0];
				if(strcmp(ent, "fg\0") == 0 )
				{
					 fg(tokens, command_tokens); //to bring the background process to foreground
				 }
                else if(strcmp(ent, "killall\0") == 0) // to kill all the processes
                {
					int i;
	for(i=0;i<numofjobs;i++)
	{
		if(proc_list[i].active==1)
		{
			if(kill(proc_list[i].pid,SIGKILL)<0)
			{
				perror("error in killing all processes ! \n");
			}
		}
	}
				 }
				 else if(strcmp(ent,"alarm\0")==0)
				 {
				command_tokens[0]="sleep";
				//	is_background=1;
				exec_cmd(command_tokens);
				printf("alert!! alarm  alarm  alarm !! \n");
				//	myalarm(tokens,command_tokens);
				 }
                else if(strcmp(ent, "jobs\0") == 0)
                {
					 procs(); //list all the currently active jobs
				 }
                else if(strcmp(command_tokens[tokens-1], "&\0") == 0) 
                {
					    command_tokens[tokens - 1] = NULL;//to make sure & is not passed as an argument in the exec command
                        is_background = 1;
                        exec_cmd(command_tokens);        // for running background process
                }
                else if(strcmp(ent, "cd\0") == 0)
                {
					 changedirectory(command_tokens, cwd, base_directory);
                }
                else if(strcmp(ent, "pwd\0") == 0)
                {
					 char present_dir[MLEN];
					getcwd(present_dir,MLEN-1);
					if(command_tokens[1]==NULL)
					{
					printf("%s\n",present_dir);
					}
					else
					{
					exec_cmd(command_tokens);
					}
				 }
                else if(strcmp(ent, "exit\0") == 0)
                 {
                        _exit(0);
                }
                else if(strcmp(ent, "echo\0") == 0)
                {
					 echo(command_tokens, tokens, command_copy);
                }
                else if(strcmp(ent, "pinfo\0") == 0)
                {
					 procinfo(command_tokens);
				 }
				 else if(strcmp(ent, "history\0")==0)
				 {
					history(hist);
				 }
                else if(isalpha(command_tokens[0][0]))
                {
                        exec_cmd(command_tokens);
				}
			}
        free(command_tokens);
}
void redirectandpipe(char* cmd)
{
/*count_pipes(cmd);
int status;
int i=0;
pid_t pid;
int pipe_fd[2];
while(cmd!=NULL)
{
	pipe(pipe_fd);
	
for(i=0;i<2*num_pipe-1;i=i++)
{
if(pipe(pipe_fd+i)<0)
{
perror("Bad Pipe !! Pipe not opened");
return;
}
}
int j=0;
for(j=0;j<num_pipe;j++){
char** command_tokens=malloc((sizeof(char)*MLEN)*MLEN);
int tokens=parseforredirection(strdup(pipe_cmds[i]),command_tokens);
*/
  char* command_copy = strdup(cmd);
  char* t;
  int p = 0;
  t = strtok(command_copy, "|");
  while(t!= NULL) {
  pipe_cmds[p++] = t;
  t = strtok(NULL, "|");
  }
num_pipe=p;
int status;
int i=0;
pid_t pid;
int pipe_fd[2*num_pipe-1];
for(i=0;i<2*num_pipe-1;i=i++)
{
if(pipe(pipe_fd+i)<0)
{
perror("Bad Pipe !! Pipe not opened");
return;
}
}
int j=0;
for(j=0;j<num_pipe;j++){
char** command_tokens=malloc((sizeof(char)*MLEN)*MLEN);
int tokens=parseforredirection(strdup(pipe_cmds[i]),command_tokens);
	pid=fork();
	if(pid==0)
	{
		
		if(cmd+1)
		{
			if(dup2(pipe_fd[j+1],1)<0)
			{
				perror("dup2");
			}
			exit(EXIT_FAILURE);
		}
		if(j!=0)
		{
			if(dup2(pipe_fd[j-2],0)<0)
			{
				perror("dup2 \n");
				exit(EXIT_FAILURE);
			}
		}
		for(i=0;i<2*num_pipe;i++)
		{
			close(pipe_fd[i]);
	}
	int st=execvp(command_tokens[0],command_tokens);
	if(st<0)
	{
		perror("error in exec \n");
		exit(EXIT_FAILURE);
	}
}
else if(pid<0)
{
	perror("error creating a child \n");
	exit(EXIT_FAILURE);
}
cmd+=1;
j+=2;
}
for(i=0;i<2*num_pipe;i++)
{
	close(pipe_fd[i]);
}
for(i=0;i<num_pipe+1;i++)
{
	wait(&status);
}
tcsetpgrp(shell,my_pgid);
}
