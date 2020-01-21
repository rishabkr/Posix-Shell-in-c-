#include "myheader.h"
//#include<curses.h>
char* readcmd_line()
{
	int length=0;
	int next_char;
	char* command=malloc(sizeof(char)*MLEN);//to store the input from the keyboard
	while(1)
	{
		next_char=getchar();
		if(next_char=='\n')
		{
			command[length++]='\0';
			break;
		}
		else
		{
			command[length]=next_char;
			length+=1;
		//printf("%c \n",command[length-1]);
		}
	}
		for(int i=0;command[i]!='\0';i++)
		{
			cmd[i]=command[i];
			//store the command in cmd array character by character
		}
		free(hist[curr]);//remove any garbage value
		hist[curr]=strdup(cmd);//keep the current command as a string
		curr=(curr+1)%HIST_SIZE;//increment current
		return command;//return the read command from terminal
}
int checkifpipe(char* cmd)
{
	int i;
	piping=0; //to indicate whether the command contains pipe or not
	input_redi=output_redi=0; //to indicate presence of input or output redirection > or <
	for(i=0;cmd[i]!='\0';i++)
	{
		if(cmd[i]=='|')
		{
			piping=1; //pipe is present
		}
		if(cmd[i]=='<')
		{
			input_redi=1; //input redurection is present,will be used for parsing later			
		}
		if(cmd[i]=='>')
		{
			output_redi=1;  //output redirection is present will be used for parsing later
		}		
	}
		if(piping>0)
		{
			return 1;
		}
		else
		{
			return -1; //piping was not present
		}
}

int getnumofcmds(char* commandline, char** cmds) {
        int numofcommands = 0;
        char* token = strtok(commandline, ";"); //used in place of return key..useful in multiple commands  in a single line..
        while(token!=NULL) {
                cmds[numofcommands++] = token;
                token = strtok(NULL, ";");
        }
        //printf("%s \n",*cmds);
        return numofcommands;
}
int parsecmd(char* cmd,char** cmd_tokens)
{
	int noftoken=0;
	char* token=strtok(cmd,CMD_DELIMS);
	while(token!=NULL)
	{
		cmd_tokens[noftoken++]=token;
		token=strtok(NULL,CMD_DELIMS);
	}
	return noftoken;
}
int parseforredirection(char* cmd,char** command_tokens)
{	int index_i,index_o;
	char* command_copy=strdup(cmd);
	index_i=index_o=0; //current input index and output index where  >  and < are encountered
	append_flag=0;//to differetiate between append or just output redirection >>  or >
	input_redi=output_redi=0; //resetting the value from previous function call
	inputfile=outputfile=NULL;
	int i,k=0;
	for(i=0;cmd[i];i++)
	{
		if(cmd[i]=='<')
		{
			input_redi=1;
			index_i=i; //index where < was first encountered
		}
		if(cmd[i]=='>')
		{
			output_redi=1;
			index_o=i; //index where > was first encountered
			
		}
		if(cmd[i]=='>' && cmd[i+1]=='>')
		{
			append_flag=1;	//if both are continuous > indicating append
		}
	}
	if(input_redi==1)
	{
		char* token;
		char* command_copy;
		strcpy(command_copy,cmd);
		char** inputredirectcmd=malloc((sizeof(char)*MLEN)*MLEN);
        token = strtok(command_copy, "<");
        while(token!=NULL)
        {
			inputredirectcmd[k++]=token;
			token=strtok(NULL,"<");
		}
		strcpy(command_copy,inputredirectcmd[k-1]);
		token=strtok(command_copy,"> |\t\n");
		strcpy(inputfile,token);
		k=0;
		token=strtok(inputredirectcmd[0],CMD_DELIMS);
		while(token!=NULL) {
          command_tokens[k++] = strdup(token);
          token = strtok(NULL, CMD_DELIMS);
          }
          free(command_tokens[k]);
          free(inputredirectcmd);
	}
	  if(output_redi==1)
	  {
		  char* command_copy;
		  strcpy(command_copy,cmd);		//eg :[cat p1.c < f1.txt]
		  char* token;
          char** outputredirectcmd = malloc((sizeof(char)*MLEN)*MLEN);
            if(!append_flag)
              {
				  token = strtok(command_copy, ">");	//[cat p1.c]
                }
                else if(append_flag)
                 {
		        token = strtok(command_copy, ">>");
                }
                while(token!=NULL) {
                        outputredirectcmd[k++] = token;
                        if(!append_flag)
                        { 
							token = strtok(NULL, ">"); //[cat p1.c | f1.txt]
					}
                        else if(append_flag){
							 token = strtok(NULL, ">>");
						 }
                }
                command_copy=strdup(outputredirectcmd[k-1]);//[cat p1.c]
                token=strtok(command_copy,"< |\t\n"); //[cat p1.c]
                outputfile=strdup(token);//[open f1.txt]
                k=0;
                token = strtok(outputredirectcmd[0], CMD_DELIMS);
                while(token!=NULL) {
                        command_tokens[k++] = token;  
                        token = strtok(NULL, CMD_DELIMS);
                }//[cat | p1.c ]
                free(outputredirectcmd);
			}
			char* cp;
			if(!input_redi&&!output_redi)
			{
			return parsecmd(strdup(cmd),command_tokens);//returns number of tokens in the commmand if there is no redirection
			}
			else
			{	
				return k;
			}
		}
