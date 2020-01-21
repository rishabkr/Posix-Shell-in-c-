#include "myheader.h"
int changedirectory(char** command_tokens,char* cwd,char* base_directory)
{
	 if(strcmp(command_tokens[1], "~/\0") == 0||strcmp(command_tokens[1], "~\0") == 0) {
                chdir(base_directory);
                strcpy(cwd, base_directory);
                modifycwd(cwd);
                root_flag=0;
        }
        else if(chdir(command_tokens[1]) == 0) {
                getcwd(cwd, MLEN);
                modifycwd(cwd);
                if(command_tokens[1]=='/')
                root_flag=1;
                return 0;
        }
        else {
                printf("Error in executing cd ! \n");
        }
}
int procinfo(char** command_tokens)
{
	int c,length=0;
	pid_t proc_pid;
	if(command_tokens[1]==NULL)
	{
		proc_pid=getpid();
	}
	else
	{
		proc_pid=atoi(command_tokens[1]);
	}
	char path[MLEN];
	char buffer[MLEN];
	printf("Process Id --> %d \n",proc_pid);
	sprintf(path,"/proc/%d/stat",proc_pid);
	FILE* fp=fopen(path,"r");
	if(!fp)
	{
		perror("Error while getting process status ");
	}
	else
	{
		char state;
		int ppid;
		char comm[1000];
		int unused;
		fscanf(fp,"%d %s %c %d",&unused,comm,&state,&ppid);
		fgets(path,256,fp);
		printf("process name is -> %s \n",comm);
		printf("process state is -> %c \n",state);
		printf("process parent id is -> %d \n",ppid);
		fclose(fp);
	}
}
int open_inputfile()
{
	int fd=open(inputfile,O_RDONLY,S_IRUSR|S_IWUSR|S_IXUSR);
	close(0);
	dup(fd);
	close(fd);
	return fd;
}
int open_output_file()
{
	int fd;
	if(!append_flag)
	{
		fd=open(outputfile,O_CREAT | O_WRONLY | O_TRUNC,S_IRUSR|S_IWUSR|S_IXUSR);
	}
	else if(append_flag)
	{
		fd=open(outputfile,O_CREAT | O_WRONLY | O_APPEND,S_IRUSR|S_IWUSR|S_IXUSR);
	}
	close(1);
	dup(fd);
	close(fd);
	return fd;
}

void add_process(int pid,char* name)
{
	proc_list[numofjobs].pid=pid;
	proc_list[numofjobs].pname=strdup(name);
	proc_list[numofjobs].active=1;
	numofjobs++;
}
void remove_process(int pid)
{
	int i;
	for(i=0;i<numofjobs;i++)
	{
		if(proc_list[i].pid==pid)
		{
			proc_list[i].active=0;
			break;
		}
	}
}
void echo(char** command_tokens, int tokens, char* cmd) {
        int num_of_tokens=tokens;
        if(num_of_tokens > 1 || command_tokens[1][0] == '-') {
                exec_cmd(command_tokens);
                return;
        }
}
void signaldefaults()
{
signal(SIGINT,SIG_DFL);
signal (SIGTTIN,SIG_DFL);
signal (SIGTTOU,SIG_DFL);
signal (SIGTSTP,SIG_DFL);

signal (SIGQUIT,SIG_DFL);
signal (SIGCHLD,SIG_DFL);
}
void procs()
{
	int i;
	int count=0;
	if(numofjobs==0)
	{
		printf("There are no current background jobs \n");
	}else
	{
	for(i=0;i<numofjobs;i++)
	{	
		if(proc_list[i].active==1)
		{	count++;
			printf("[%d]\t%s\t[%d]\n",i,proc_list[i].pname,proc_list[i].pid);
		}
	}
	if(count==0)
	{
	printf("There are no current background jobs \n");
	}
}
}
volatile sig_atomic_t x=1;//controls the termination of main loop
void custom_alarm_handler(int sign)
{
int x=0;
signal(sign,custom_alarm_handler);
//printf("Alert!   Alarm!!  Alarm!! Alarm!! \n");
}
/*void myalarm(int tokens,char** command_tokens)
{
	if(tokens!=2)
	{
		printf("Please enter the time \n");
		return;
	}
	int al_pid=fork();
		if(al_pid==0)
		{
			setpgid(al_pid,al_pid); 
			add_process(al_pid,alarm_list);
			//is_background = 1;
             //exec_cmd(command_tokens); 
int set_time=atoi(command_tokens[1]);
alarm(set_time);
while(x);
//sleep(set_time);
printf("Alert!   Alarm!!  Alarm!! Alarm!! \n");
alarm_flag=set_time;
}
}*/
void fg(int tokens,char** command_tokens)
{
	if(tokens!=2)
	{
		fprintf(stderr,"Invalid arguments for fg !! \n");
		return;
	}
	int jobnum=atoi(command_tokens[1]),status;
	if(jobnum>numofjobs)
	{
		printf("job doesnt exist \n");
		return;
	}
	if(proc_list[jobnum].active==0)
	{
		printf("Job doesnt exist !! \n");
		return ;
	}
	if(proc_list[jobnum].active==1)
	{
		int pid=proc_list[jobnum].pid,pgid;
		pgid=getpgid(pid);//get fg process group id
		tcsetpgrp(shell,pgid); //put the job in the foreground..assign shell to the process
		fgpid=pgid; //
		if(killpg(pgid,SIGCONT)<0) //send continue signal to the process if the process was stopped  and resume execution when brought in fg
		{
			perror("Error bringing to foreground ! \n");
		}
		waitpid(pid,&status,WUNTRACED);//wait for the child until the status has changed state or stopped without trace (wuntraced)
				if(!WIFSTOPPED(status))//if stopped by a signal returns true 
 		{
			proc_list[jobnum].active=0;
			fgpid=0;
		}
		tcsetpgrp(shell,my_pid);//shell is back in the foreground
	}
}
