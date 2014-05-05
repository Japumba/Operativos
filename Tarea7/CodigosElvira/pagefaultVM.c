#include <stdio.h>
#include "mmu.h"

#define RESIDENTSETSIZE 24

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int processpagetablesize;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE processpagetable[];


// Rutina de fallos de página

int pagefault(char *vaddress)
{
    int i;
    int frame;
    int pag_a_expulsar;
    int pag_del_proceso;

    // Calcula la página del proceso
    pag_del_proceso=(int) vaddress>>12;
    // Cuenta los marcos asignados al proceso
    i=countframesassigned();
  
    // Busca un marco libre en el sistema
    frame=getfreeframe();

    if(frame==-1)
    {
        return(-1); // Regresar indicando error de memoria insuficiente
    }


    processpagetable[pag_del_proceso].presente=1;
    processpagetable[pag_del_proceso].framenumber=frame;


    return(1); // Regresar todo bien
}


int getfreeframe()
{
    int i;
    // Busca un marco libre en el sistema
    for(i=framesbegin;i<systemframetablesize+framesbegin;i++)
        if(!systemframetable[i].assigned)
        {
            systemframetable[i].assigned=1;
            break;
        }
    if(i<systemframetablesize+framesbegin)
        systemframetable[i].assigned=1;
    else
	
        i=-1;
    return(i);
}
int fifo()
{
	int i=0;
	int page;
	unsigned long oldest = processpagetable[i].tarrived;
	for(i=1;i<processpagetablesize;i++)
	{
		if(processpagetable[i].presente)
			if(processpagetable[i].tarrived < oldest)
			{
				page = i;
				oldest = processpagetable[i].tarrived;
			}
	}
	return page;		
}
void copyFromMemoryToFile(int page, int fd)
{
	
	int  fd = open("swap", O_WRONLY |  O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP));
	char * ptr = systemframetable[processpagetable[page].framenumber].frameaddr;
	lseek(fd, PAGESIZE * processpagetable[page].framenumber, SEEK_SET);
	write(fd,ptr,PAGESIZE);
}

void copyFromFileToMemory()
{
	
}


void swapInMemory(int fd, int origen, int destino)
{
	char oTemp [PAGESIZE];
	char dTemp [PAGESIZE];

	origen = processpagetable[origen].framenumber*PAGESIZE;
	destino = processpagetable[destino].virtualframenumber*PAGESIZE;

	lseek(fd, origen, SEEK_SET);
	memcpy(oTemp,origen,PAGESIZE);
	lseek(fd,destino, SEEK_SET);
	memcpy(dTemp, destino,PAGESIZE);
	
	lseek(fd, origen, SEEK_SET);
	memcpy(origen,dTemp, PAGESIZE);
	lseek(fd,destino, SEEK_SET);
	memcpy(destino, oTemp, PAGESIZE);

}
