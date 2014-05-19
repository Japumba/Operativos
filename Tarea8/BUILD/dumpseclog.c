#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"

#define LINESIZE 16
#define SECSIZE 512
#define SEC_X_TRACK 17
#define SUPERFICIES 20

int main(int argc,char *argv[])
{
	int drive;
	int seclog;
	int ncyl,nhead,nsec;
	int fd;
	unsigned char buffer[SECSIZE];
	int offset;
	int i,j,r;
	unsigned char c;
 
	if(argc==2)
	{	
		drive=0;
		seclog =atoi(argv[1]);
		
		nsec = ((seclog % SEC_X_TRACK) + 1);
		nhead = ((seclog/SEC_X_TRACK) % SUPERFICIES);
		ncyl = (seclog/(SEC_X_TRACK * SUPERFICIES));
		
		printf("Desplegando de disco%d.vd Cil=%d, Sup=%d, Sec=%d\n",drive,ncyl,nhead,nsec);
		
	}
	else	
	{
		fprintf(stderr,"Error en los argumentos\n");
		exit(1);
	}

	if(vdreadsector(drive,nhead,ncyl,nsec,1,buffer)==-1)
	{
		fprintf(stderr,"Error al abrir disco virtual\n");
		exit(1);
	}

	for(i=0;i<SECSIZE/LINESIZE;i++)
	{
		printf("\n %3X -->",i*LINESIZE);
		for(j=0;j<LINESIZE;j++)
		{
			c=buffer[i*LINESIZE+j];
			printf("%2X ",c);
		}
		printf("  |  ");
		for(j=0;j<LINESIZE;j++)
		{
			c=buffer[i*LINESIZE+j]%256;
			if(c>0x1F && c<127)
				printf("%c",c);
			else
				printf(".");
		}
	}
	printf("\n");
}

