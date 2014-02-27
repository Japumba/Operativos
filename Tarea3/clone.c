#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <linux/sched.h>
#include <sys/types.h>

#define STACK_SIZE 5000000
#define NTHREADS	8
#define DIF 16

// NOMBRE DEL ARCHIVO A PROCESAR
char filename[]="imagen.bmp";
int *threadID;

#pragma pack(2) // Empaquetado de 2 bytes
typedef struct {
	unsigned char magic1; // 'B'
	unsigned char magic2; // 'M'
	unsigned int size; // Tamaño
	unsigned short int reserved1, reserved2;
	unsigned int pixelOffset; // offset a la imagen
} HEADER;

#pragma pack() // Empaquetamiento por default
typedef struct {
	unsigned int size; // Tamaño de este encabezado INFOHEADER
	int cols, rows; // Renglones y columnas de la imagen
	unsigned short int planes;
	unsigned short int bitsPerPixel; // Bits por pixel
	unsigned int compression;
	unsigned int cmpSize;
	int xScale, yScale;
	unsigned int numColors;
	unsigned int importantColors;
} INFOHEADER;

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} PIXEL;


typedef struct {
	HEADER header;
	INFOHEADER infoheader;
	PIXEL *pixel;
} IMAGE;

IMAGE imagenfte,imagendst;

int loadBMP(char *filename, IMAGE *image)
{

	FILE *fin;

	int i=0;
	int totpixs=0;

	fin = fopen(filename, "rb+");

	// Si el archivo no existe
	if (fin == NULL)
		return(-1);

	// Leer encabezado
	fread(&image->header, sizeof(HEADER), 1, fin);

	// Probar si es un archivo BMP
	if (!((image->header.magic1 == 'B') && (image->header.magic2 	== 'M')))
	return(-1);
	
	fread(&image->infoheader, sizeof(INFOHEADER), 1, fin);

	// Probar si es un archivo BMP 24 bits no compactado
	if (!((image->infoheader.bitsPerPixel == 24) && !image->infoheader.compression))
		return(-1);

	image->pixel=(PIXEL *)malloc(sizeof(PIXEL)*image->infoheader.cols*image->infoheader.rows);

	totpixs=image->infoheader.rows*image->infoheader.cols;

	while(i<totpixs)
	{
		fread(image->pixel+i, sizeof(PIXEL),512, fin);
		i+=512;
	}
	fclose(fin);
}

int saveBMP(char *filename, IMAGE *image)
{
	FILE *fout;
	int i,totpixs;
	printf("Saving file: %s\n", filename);
	fout=fopen(filename,"wb");
	if (fout == NULL)
		return(-1); // Error

	// Escribe encabezado
	fwrite(&image->header, sizeof(HEADER), 1, fout);

	// Escribe información del encabezado
	fwrite(&image->infoheader, sizeof(INFOHEADER), 1, fout);

	i=0;
	totpixs=image->infoheader.rows*image->infoheader.cols;
	while(i<totpixs)
	{
		fwrite(image->pixel+i,sizeof(PIXEL),512,fout);
		i+=512;
	}

	fclose(fout);
}

unsigned char blackandwhite(PIXEL p)
{
	return((unsigned char) (0.3*((float)p.red)+
	0.59*((float)p.green)+0.11*((float)p.blue)));
}

void *processBMP(void *arg)
{
	//printf("Comenzando el thread %i\n", *(int *)arg);
	int i,j;
	int count=0;
	PIXEL *pfte,*pdst;
	PIXEL *v0,*v1,*v2,*v3,*v4,*v5,*v6,*v7;
	int imageRows,imageCols;
	int numThread = *(int *)arg;
	
	imageRows = (&imagenfte)->infoheader.rows;
	imageCols = (&imagenfte)->infoheader.cols;

	
	for(i=1+numThread;i<imageRows-1;i+=NTHREADS)
		for(j=1;j<imageCols-1;j++)
		{
		//printf("Thread %i trabajando\n",numThread);
		pfte=(&imagenfte)->pixel+imageCols*i+j;
		//printf("Punto fuente asignado\n");
		v0=pfte-imageCols-1;
		v1=pfte-imageCols;
		v2=pfte-imageCols+1;
		v3=pfte-1;
		v4=pfte+1;
		v5=pfte+imageCols-1;
		v6=pfte+imageCols;
		v7=pfte+imageCols+1;
		//printf("Puntos asignados\n");
	
		pdst=(&imagendst)->pixel+imageCols*i+j;
		//printf("Punto destino asignado\n");
		if(abs(blackandwhite(*pfte)-blackandwhite(*v0))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v1))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v2))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v3))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v4))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v5))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v6))>DIF ||
			abs(blackandwhite(*pfte)-blackandwhite(*v7))>DIF)
		{
		pdst->red=0;
		pdst->green=0;
		pdst->blue=0;
		}
		else
		{
		pdst->red=255;
		pdst->green=255;
		pdst->blue=255;
		}
	}
}

int main()
{
	int res, i, j;
	clock_t t_inicial,t_final;
	char namedest[80];

	t_inicial=clock();

	strcpy(namedest,strtok(filename,"."));
	
	strcat(filename,".bmp");
	strcat(namedest,"_P.bmp");
	printf("Archivo fuente %s\n",filename);
	printf("Archivo destino %s\n",namedest);

	res=loadBMP(filename,&imagenfte);

	if(res==-1)
	{
		fprintf(stderr,"Error al abrir imagen\n");
		exit(1);
	}
	
	printf("Procesando imagen de: Renglones = %d, Columnas = %d\n",imagenfte.infoheader.rows,imagenfte.infoheader.cols);
	
	//start paralelizar
	printf("Paralelizando usando %i threads\n", NTHREADS);
	threadID = (int *) malloc(sizeof(int)*NTHREADS);
	
	memcpy(&imagendst, &imagenfte, sizeof(IMAGE)-sizeof(PIXEL *));
	int imageRows = (&imagenfte)->infoheader.rows;
	int imageCols = (&imagenfte)->infoheader.cols;
	int status=0;
	(&imagendst)->pixel=(PIXEL *)malloc(sizeof(PIXEL)*imageRows*imageCols);
	
	int pids[NTHREADS];
	char *stack;
	stack = malloc(STACK_SIZE*NTHREADS);
	
	for(i=1;i<NTHREADS+1;i++)
	{
		//printf("Creando thread %i\n", i-1);
		threadID[i-1] = i-1;
		//printf("Thread %i creado\n", threadID[i-1]);
		pids[i-1] = clone(processBMP,stack + STACK_SIZE*i,CLONE_FS|CLONE_FILES|CLONE_VM,(void*)&threadID[i-1]);
	}

	for(j=0;j<NTHREADS;j++)
	{
		waitpid(pids[j], &status, __WALL);
	}
	
	//end paralelizar
	
	res=saveBMP(namedest,&imagendst);
	if(res==-1)
	{
		fprintf(stderr,"Error al escribir imagen\n");
		exit(1);
	}
	t_final=clock();
	
	printf("Tiempo %3.6f segundos\n",((float) t_final- (float)t_inicial)/CLOCKS_PER_SEC);
}



