/*
Command interpreter.
Each shell has ambient variables. A special variable 
"Path" is used to run programs not available in the
current directory.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define COMMANDSIZE	256
#define DEBUG		1
#define CMDEXIT		"exit"
#define CMDSHUTDOWN	"shutdown"
#define TYPEEXIT	1
#define TYPESHUTDOWN	2
#define TYPEGENERIC	3

typedef struct
{
	char **argv;
	int args;
	int type;
} COMMAND;

COMMAND *parseInput(char *input);
void executeCommand(COMMAND *cmd);
COMMAND *createCommand(int args, char **argv, int type);
void execExit();
void execShutdown();
void execGeneric(COMMAND *cmd);
void destroyCommand(COMMAND *cmd);

int initID;

int main(int argc, char *argv[])
{

	if(argc < 2)
		return 1;
	
	initID = atoi(argv[1]);
	
	char input[COMMANDSIZE];
	COMMAND *command = NULL;
	int args = 0;
	while(1)
	{
		printf("sh> ");
		gets(input);
		
		#if DEBUG
		printf("!!!COMMAND: %s\n", input);
		#endif
		
		command = parseInput(input);
		executeCommand(command);
		destroyCommand(command);
	}
	return 0;
}

COMMAND *parseInput(char *input)
{
	
	#if DEBUG
	printf("!!!PARSING: %s\n", input);
	#endif
	
	if(input == NULL)
	{
		#if DEBUG
		printf("!!!CANNOT PARSE, NULL INPUT\n");
		#endif
		return NULL;
	}
	
	char *token = NULL;
	char **argv = (char **) calloc(15, sizeof(char*));
	token = strtok(input, " ");
	int args = 0;

	while(token != NULL)
	{
		#if DEBUG
		printf("!!!Token = %s", token);
		#endif
		argv[args] = token;
		args++;
		token = strtok(NULL, " ");
	}
		
	COMMAND *cmd = NULL;
	//check for custom commands(exit and shutdown)
	if(strcmp(argv[0], CMDEXIT) == 0)
	{
		#if DEBUG
		printf("!!!PARSED EXIT COMMAND\n");
		#endif
		
		cmd = createCommand(args, argv, TYPEEXIT);
	}
	else if(strcmp(argv[0], CMDSHUTDOWN) == 0)
	{
		#if DEBUG
		printf("!!!PARSED SHUTDOWN COMMAND\n");
		#endif
		
		cmd = createCommand(args, argv, TYPESHUTDOWN);
	}
	else
	{
		#if DEBUG
		printf("!!!PARSED GENERIC SHELL COMMAND\n");
		#endif
		
		cmd = createCommand(args, argv, TYPEGENERIC);
	}
	return cmd;
}

COMMAND *createCommand(int args, char **argv, int type)
{
	#if DEBUG
	printf("!!!CREATING COMMAND\n");
	#endif
	
	if(argv == NULL || argv[0] == NULL)
	{
		#if DEBUG
		printf("!!!CANNOT CREATE COMMAND, NULL NAME\n");
		#endif
		return NULL;
	}
	
	COMMAND *cmd;

	cmd = (COMMAND *) malloc(sizeof(COMMAND));
	cmd->args = args;
	cmd->argv = argv;
	cmd->type = type;
	
	#if DEBUG
	printf("!!!COMMAND CREATED WITH NAME: %s AND %i ARGUMENTS.\n",cmd->argv[0], cmd->args);
	#endif
	
	return cmd;
}

void executeCommand(COMMAND *cmd)
{
	#if DEBUG
	printf("!!!ATTEMPTING TO EXECUTE COMMAND\n");
	#endif
	
	if(cmd == NULL)
	{
		#if DEBUG
		printf("!!CANNOT EXECUTE COMMAND, NULL COMMAND\n");
		#endif
		return;
	}
	
	int type = cmd->type;
	
	switch(type)
	{
		case TYPEEXIT:
			#if DEBUG
			printf("!!!EXECUTING EXIT COMMAND\n");
			#endif
			execExit();
			break;

		case TYPESHUTDOWN:
			#if DEBUG
			printf("!!!EXECUTING SHUTDOWN COMMAND\n");
			#endif
			execShutdown();
			break;

		case TYPEGENERIC:
			#if DEBUG
			printf("!!!EXECUTING GENERIC COMMAND WITH NAME: %s AND %i ARGUMENTS.\n", cmd->argv[0], cmd->args);
			#endif
			execGeneric(cmd);
			break;

		default:
			#if DEBUG
			printf("!!!COULDNT EXECUTE COMMAND. INVALID TYPE\n");
			#endif
			break;
	}
}

void execExit()
{
	exit(0);
}

void execShutdown()
{
	kill(initID, 9);
}

void execGeneric(COMMAND *cmd)
{
	int pid;
	int status;
	pid = fork();
	if(pid == 0)
	{
		execvp(cmd->argv[0],cmd->argv);
		printf("ERROR EXECUTING PROGRAM\n");
	}
	wait(&status);
}

void destroyCommand(COMMAND *cmd)
{
	//free(cmd->name);
	free(cmd->argv);
	free(cmd);
}
