#include "myheader.h"
//#include<curses.h>
void modifycwd(char* cwd)
{	
	int i,j;
	for(i=0;cwd[i]==base_directory[i]&&cwd[i]&&base_directory[i];i++);
	if(base_directory[i]=='\0')
	{
		cwd[0]='~';
		for(j=1;cwd[i]!='\0';j++)
		{
			cwd[j]=cwd[i];
			i+=1;
		}
	cwd[j]='\0';
}
else if(base_directory[i]=='/\0')
	{
		cwd[0]='#';
		for(j=1;cwd[i]!='\0';j++)
		{
			cwd[j]=cwd[i];
			i+=1;
		}
	cwd[j]='\0';
}
}
void handle_signal(int signal_num)
{
	if(signal_num==SIGINT)
	{
		signal(SIGINT,SIG_IGN);
		signal(SIGINT,handle_signal);//restore
	}
	else if(signal_num==SIGCHLD)
	{
		int i,status,child_die_pid;
		while((child_die_pid=waitpid(-1,&status,WNOHANG))>0)//pid -1 means status is requested for the any child process WNOHANG doesnt suspend execution of the calling process													
		{
			for(i=0;i<numofjobs;i++)
			{
				if(proc_list[i].active==0)//if process is not active continue
				{
					continue;
				}
				else if(proc_list[i].pid==child_die_pid)//when the terminated child pid is found in the active processlist
				{
					break;
				}
			}
			if(i<numofjobs)//to ensure that we didnt reach the end of job list and the child was found before the end of the list
			{
				if(WIFEXITED(status))//this is done to ensure that the child process exited normally
				{
				sprintf(stderr,"\nprocess %s with pid %d has exited normally \n", proc_list[i].pname, proc_list[i].pid);
				}
				else if(WIFSIGNALED(status))
                {	sprintf(stderr,"the child %s was terminated due to a signal \n",proc_list[i].pname);
					proc_list[i].active = 0;//change the status of the terminated process
                    } 
                       }
                }
        }
}
int history(char* hist[])
{
	int i=curr;
	int hist_index=1;
	do{
		if(hist[i])
		{
			printf("%5d		%s\n",hist_index,hist[i]); //print the value of the ith command executed from top
			hist_index++; //increment index
		}
		i=(i+1)%HIST_SIZE;
	}while(i!=curr);
	return 0;
}

void init()
{	//initscr();
	//raw()
	//keypad(stdscr,TRUE);
	//noecho();
	shell=STDOUT_FILENO; //assign shell the descriptor from  0,1,2{./any}
	numofjobs=0;//intialize number of jobs to 0
	curr=0;//no commands yet during initializing
	if(isatty(shell))//is a part of the shell proces i.e it is a file descriptor which is associated with the shell
	{ 					//returns 1 if it is associated with an fd of open terminal
		while(tcgetpgrp(shell)!=(shell_pgid=getpgrp())) //if associated then make sure it is placed in foreground by the parent
		kill(shell_pgid,SIGTTIN);		//compare it with the current process associated with the terminal,keep checking until it is in fg
	}	signal(SIGCHLD,SIG_IGN);
    	signal (SIGTTIN, SIG_IGN); 
		signal(SIGTTOU,SIG_IGN);
		signal(SIGALRM,custom_alarm_handler);
my_pid=my_pgid=getpid();
setpgid(my_pid,my_pgid);
if(tcsetpgrp(shell,my_pgid)<0)
{
	perror("error setting the terminal \n");
} //set terminal foreground process group
for(int i=0;i<HIST_SIZE;i++)
{
	hist[i]=NULL; //remove any garbage value 
}
//set_variables=malloc((sizeof(char)*MLEN)*MLEN);
int bfd=open("rishabrc",O_RDONLY);
char bf[1024];
read(bfd,bf,1024);
//set_variables[i]=strtok(bf,"\n");
getlogin_r(username,1023);//get user login id
gethostname(hostname,1023);//get hostname
getcwd(base_directory,1023);//get current working directory
strcpy(cwd,base_directory);	
modifycwd(cwd);//always called when current directory is changed
}
int main()
{		int k1,k2; //keyboard flags
		int ch; //to get next character
        init();
printf("************************************************************************************************************************\n");
		while(1) {
                signal(SIGCHLD,handle_signal);//wehen child process is terminated,it sends signal SIGCHLD to the parent process
                signal(SIGINT,handle_signal);//when interrupt is incurred
                if(getuid()==0)
                {
					root_flag=1;
				}       
				if(root_flag==0)
						{
                printf("%s@%s---%s$ ",username,hostname,cwd);
                   }
                   else if(root_flag==1)
                   {
				   printf("%s@%s---%s# ",username,hostname,cwd);
				   }
                   inputfile = outputfile = NULL;
                        is_background = 0,  num_pipe= 0;
                int i,j;
                char** cmds = malloc((sizeof(char)*MLEN)*MLEN); //initialize commands 2-d array
                for(j = 0; j < MLEN; j++) 
                {
					cmds[j] = '\0';
				}
                char* commandline = readcmd_line(); // read command line
                int numofcommands = getnumofcmds(commandline, cmds);
		//		ch=getch();
			//	switch(ch)
				//{
			//	case KEY_UP :printw("\n up arrow ");
				//break;
			//	}
				for(i = 0; i < numofcommands; i++) {
                        char* command_copy = strdup(cmds[i]);
	char** command_tokens = malloc((sizeof(char)*MLEN)*MLEN);
                        for(j = 0; j < MLEN; j++)
                        {
							 command_tokens[j] = '\0';
						}
                if(checkifpipe(strdup(cmds[i])) == -1) {
                                if(input_redi == 1 )
				{
				 normalcmd(parseforredirection(strdup(command_copy),command_tokens), command_tokens, command_copy);
				}
				else if(output_redi == 1)
				{
				 normalcmd(parseforredirection(strdup(command_copy),command_tokens), command_tokens, command_copy);
				}
                                else {
                                        int tokens = parsecmd(strdup(cmds[i]),command_tokens);
                                        normalcmd(tokens, command_tokens, command_copy);
                                }
                        }
                        else if(checkifpipe(cmds[i])!=-1)
                        { 
							redirectandpipe(cmds[i]);
						}
				}
                if(cmds||commandline)
                { 
					free(cmds);
					free(commandline);
				}
        
	}
        return 0;
}
