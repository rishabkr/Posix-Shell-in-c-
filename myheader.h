#include<stdio.h>
#include<stdlib.h>
#include<time.h>
//#include<curses.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<fcntl.h>
#include<termios.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#define HIST_SIZE 1024
#define MAX_COMMNAND_SIZE 256
#define MLEN 2048
#define CMD_DELIMS " \t\n"
#define FNAME_LEN 45
void add_process(int pid,char *pname); //to add process to process list
void remove_process(int pid);// to remove process from process list
void procs(); //to list all the currently running background jobs
void echo(char** cmd_tokens,int tokens,char *cmd); //echoing text on screen,with and without quotes
void pwd(char** cmd_tokens); // get the present working directory
void redirectandpipe(char* cmd); //if command argument contains both redirection and pipes
void init(); //initializing the shell and fetching details 
void handle_signal(int signal_num); //signal handler required for catching signo 
void fg(int tokens,char** cmd_tokens); //bring as foreground process
void normalcmd(int tokens,char** cmd_tokens,char* copy);//match and call required functions for normal commands
void modifycwd(char* cwd); //if directory is changed then modify the cwd
char *readcmd_line(); // to read command lines
int getnumofcmds(char* cmd,char** cmds); //number of commands in the argument
int parsecmd(char* cmd,char** cmdtokens); //parse each command and put the tokens
int exec_cmd(char** tokens); //execute the commands
int changedirectory(char** cmd_tokens,char* cwd,char* base_dir); //to change the directory
int procinfo(char** cmd_tokens); //get process details
int open_inputfile(); // open ip file
int open_output_file(); // open op file
int checkifpipe(char* cmd); //check if pipe exists in cmd or not and return the number of pipes
int parseforredirection(char* cmd, char** command_tokens); //as the name suggests
void custom_alarm_handler(int signal);
struct proc_fields
{
	int pid,pgid;
	char* pname;
	int active;
};
struct alarm_collection
{
	int alarm_no;
	int set_time;
	char *reminder;
};

typedef struct alarm_collection alarm_collection;
alarm_collection alarm_list[10];
typedef struct proc_fields proc_fields;
int numofjobs;
proc_fields proc_list[MLEN];
pid_t my_pid,my_pgid,fgpid;
char username[MLEN];
char hostname [MLEN];
int root_flag;
int alarm_flag;
char base_directory[MLEN];
char* pipe_cmds[MLEN];
char cwd[MLEN];
//char** set_variables=malloc((sizeof(char)*MLEN)*MLEN);
int append_flag;
int is_background;
char cmd[MAX_COMMNAND_SIZE];
char* inputfile;
char *hist[HIST_SIZE];
int curr;
char* outputfile;
int shell,shell_pgid;
int history(char* hist[]);
int num_pipe;
int piping;
int input_redi,output_redi;
char** inputredirectcmd;
char** outputredirectcmd;

