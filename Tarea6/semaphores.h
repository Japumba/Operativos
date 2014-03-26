#include <sys/sem.h>
#include <sys/icp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>



int createsem()
{
	int semid;
	key_t key = 1;
	return semid = semget(key, 1, 0666 | IPC_CREAT);
}

void seminit(int idsem, int val)
{
	sembuff 
}

void semwait(int idsem)
{
	
}

void semsignal(int idsem)
{
	
}
