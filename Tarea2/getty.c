#include <stdio.h>

int main(void)
{
	char nombre[30], pass[30];
	//leer el archivo shadow
	FILE *fp = fopen("shadow.txt","r");
	if (fp == NULL)
	{
		printf("Archivo de usuarios no encontrado\n");
	}	
	else
	{
		printf("Ingrese su usuario: ");
		gets(nombre);
		printf("Usted ingreso: %s\n", nombre);
		printf("Ingrese su contraseña: ");
		gets(pass);
		printf("Usted ingreso: %s\n", pass);
	}
return 0;
}
