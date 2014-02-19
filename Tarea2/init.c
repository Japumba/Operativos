//Creates 6 processes that will be transformed into getty processes via Xterm
//If a process dies, init creates another one.
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
int main()
	{
	int pid; 
	int status;
	int procesos = 6; 
	int i; 
	for(i=0;i<procesos; i++) 
	{ 
		pid = fork(); 
		if(pid ==0) 
		{ 
		execlp("xterm","xterm","-e","./getty",0); 
		} 
	}
	while(1) 
	{ 
	waitpid(-1,&status,0); 
	pid=fork();
	if(pid==0) 
	{ execlp("xterm","xterm","-e","./getty",0); 
}
}
}
