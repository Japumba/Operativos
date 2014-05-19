#include "vdisk.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
extern int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
void printSECBOOT(struct SECBOOT *superbloque);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: vdformat <diskNumber>\n");
		return 1;
	}
	
	int disknum = atoi(argv[1]);
	
	if(disknum < 0 || disknum > 3)
	{
		printf("Invalid disk number. Please try a disk between 0 and 3\n");
		return 1;
	}
	
	printf("Formatting disk%i...\n", disknum);
	

	printf("  Creating SECBOOT structure...\n");
	
	struct SECBOOT superbloque;
	
	char nombredisco[8] = "disco";
	strcat(nombredisco, argv[1]);
	bzero(superbloque.jump, 4);
	printf("    jump = %i%i%i%i\n", superbloque.jump[0], superbloque.jump[1], superbloque.jump[2], superbloque.jump[3]);
	bzero(superbloque.nombre_disco, 8);
	strcpy(superbloque.nombre_disco, nombredisco);
	printf("    nombre_disco = %s\n", superbloque.nombre_disco);
	superbloque.sec_res = 1;
	printf("    sec_res = %u\n", superbloque.sec_res);
	superbloque.sec_mapa_bits_nodos_i = 0;
	printf("    sec_mapa_bits_nodos_i = %u\n", superbloque.sec_mapa_bits_nodos_i);
	superbloque.sec_mapa_bits_bloques = 1;
	printf("    sec_mapa_bits_bloques = %u\n", superbloque.sec_mapa_bits_bloques);
	superbloque.sec_tabla_nodos_i = 5;
	printf("    sec_tabla_nodos_i = %u\n", superbloque.sec_tabla_nodos_i);	
	superbloque.sec_log_unidd = 54400;
	printf("    sec_log_unidd = %u\n", superbloque.sec_log_unidd);
	superbloque.sec_x_bloque = 4;
	printf("    sec_x_bloque = %u\n", superbloque.sec_x_bloque);
	superbloque.heads = 20;
	printf("    heads = %u\n", superbloque.heads);
	superbloque.cyls = 160;
	printf("    cyls = %u\n", superbloque.cyls);
	superbloque.secfis = 17;
	printf("    secfis = %u\n", superbloque.secfis);
	bzero(superbloque.restante, 487);
	printf("  SECBOOT structure created\n");
		
	printf("  Writing SECBOOT...\n");
	
	char buffer[512]= "";
	char namefile[11];
	strcpy(namefile, superbloque.nombre_disco);
	strcat(namefile, ".vd");
	printf("Opening %s\n", namefile);
	int fd = open(namefile, O_WRONLY);
	memcpy(buffer, &superbloque, sizeof(struct SECBOOT));
	
	if(vdwritesector(0,0,0,1,1,buffer) > 0)
		printf("  SECBOOT wrote succesfully\n");
	else
	{
		printf("  Could not write SECBOOT. Stop\n");
		return -1;
	}
	printf("  Cleaning inode bitmap...\n");
	bzero(buffer, 512);
	vdwritesector(0,0,0,1 + (int)superbloque.sec_res + (int)superbloque.sec_mapa_bits_nodos_i,1,buffer);
	printf("  Inode bitmap clean\n");

	printf("  Cleaning block bitmap...\n");
	char blockbuffer[2048];
	bzero(blockbuffer, 2048);
	vdwritesector(0,0,0,1 + (int)superbloque.sec_res + (int)superbloque.sec_mapa_bits_bloques,4,blockbuffer);
	printf("  Block bitmap clean\n");

	printf("  Cleaning inode table...\n");
	vdwritesector(0,0,0,1 + (int)superbloque.sec_res + (int)superbloque.sec_tabla_nodos_i,4,blockbuffer);
	printf("  Inode table clean\n");
	printf("Format Finished\n");
	
	return 0;
}

void printSECBOOT(struct SECBOOT *superbloque)
{
	printf("---------SECBOOT----------\n");
	printf("jump = %i%i%i%i\n", superbloque->jump[0], superbloque->jump[1], superbloque->jump[2], superbloque->jump[3]);
	printf("nombre_disco = %s\n", superbloque->nombre_disco);
	printf("sec_res = %u\n", superbloque->sec_res);
	printf("sec_mapa_bits_nodos_i = %u\n", superbloque->sec_mapa_bits_nodos_i);
	printf("sec_mapa_bits_bloques = %u\n", superbloque->sec_mapa_bits_bloques);
	printf("sec_tabla_nodos_i = %u\n", superbloque->sec_tabla_nodos_i);	
	printf("sec_log_unidd = %u\n", superbloque->sec_log_unidd);
	printf("sec_x_bloque = %u\n", superbloque->sec_x_bloque);
	printf("heads = %u\n", superbloque->heads);
	printf("cyls = %u\n", superbloque->cyls);
	printf("secfis = %u\n", superbloque->secfis);
	printf("--------------------------\n");
}
