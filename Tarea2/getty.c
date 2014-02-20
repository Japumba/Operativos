#include <stdio.h>
#include <string.h>


int main(void)
{
char nombre[15], pass[15];
char linea[35], *tmp;
char nombreT[15],passT[15];
int validUser = 0;
//leer el archivo shadow
FILE *fp = fopen("shadow.txt","r");
	//validación	
	if (fp == NULL)
	{
	printf("Archivo de usuarios no encontrado\n");
	}	
	else	
	{
		//input del usuario
		printf("Ingrese su usuario: ");
		gets(nombre);
		printf("Ingrese su contraseña: ");
		gets(pass);
	//recorre el archivo linea por linea
	    while(fgets(linea,35,fp)!=NULL){
 	//Recorre la linea hasta un : y lo copia en un temporal
            tmp=strtok(linea,":");
            strcpy(nombreT,tmp);
		//Compara el nombre con el temporal
		if(strcmp(nombre,nombreT)==0)
			{
			tmp=strtok(NULL,"\r\n");
            		strcpy(passT,tmp);
			//valida si el password es correcto				
			if(strcmp(pass,passT)==0)
				{
				printf("Contraseña correcta\n");
				validUser = 1;
				}
			else
				printf("Contraseña incorrecta\n");
			
			break;
			}
		//termina la linea y si no es el final del archivo pasa a la siguiente
            tmp=strtok(NULL,"\r\n");
        }
        fclose(fp); 
		

	}
if (validUser == 1)
	printf("usuario valido\n");
else
	printf("usuario incorrecto\n");
sleep(5);
return 0;
}
