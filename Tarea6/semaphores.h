#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

union semnum
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int createsem(key_t key)
{
	int semid;
	return semid = semget(key, 1, 0666 | IPC_CREAT);
}

void seminit(int semid, int val)
{
	union semnum arg;
	arg.val = val;
	semctl(semid, 0, SETVAL, arg);
}

void semwait(int semid)
{
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;

	semop(semid, &buf, 1);
}

void semsignal(int semid)
{
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO | IPC_NOWAIT;

	semop(semid, &buf, 1);
}
