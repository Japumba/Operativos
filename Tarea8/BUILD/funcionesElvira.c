//inode
struct INODE {
	char name[20];
	unsigned short uid;
	unsigned short gid;
	unsigned short perms;
	unsigned int datatiemcreat;
	unsigned int datetimemodif;
	unsigned int size;
	unsigned short blocks[10];
	unsigned short indirect;
	unsigned short indirect2;
};

//Superbloque
struct SECBOOT {
	char jump[4];
	char nombre_disco[8];
	unsigned char sec_res;
	unsigned char sec_mapa_bits_nodos_i;
	unsigned char sec_mapa_bits_bloques;
	unsigned short sec_tabla_nodos_i;
	unsigned short sec_log_unidd;
	unsigned char sec_x_bloque;
	unsigned char heads;
	unsigned char cyls;
	unsigned char secfis;
	char restante[487];
}


struct OPENFILES
{
	int inuse;
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512];
};

//Para el mapa de bits del área de nodos i

int isnodefree(int inode)	
{
		int offset = inode/8;
		int shift=inode%8;
		int result;
		
		//checar si el sector del superbloque está en memoria
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		mapa_bits_nodos_i= secboot.sec_res; //Usamos la información del superbloque para
											//determinar en que sector inicia el 
											//mapa de bits de nodos 1
		//Ese mapa está en memoria
		if(!inodesmap_en_memoria)
		{
			//si no está en memoria hay que leerlo del disco
			result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
			inodesmap_en_memoria =1;
		}
		
		if (inodesmap[offset] & (1<<shift))
			return(0); //no está libre
		else
			return(1); //Si está libre
	
}

int assigninode (int inode)
{
		int offset = inode/8;
		int shift=inode%8;
		int result;
		
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		mapa_bits_nodos_i= secboot.sec_res;
		
		//Ese mapa está en memoria
		if(!inodesmap_en_memoria)
		{
			//si no está en memoria hay que leerlo del disco
			result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
			inodesmap_en_memoria =1;
		}
		
		inodesmap[offset]|=(1<<shift);
		vdwriteseclog(mapa_bits_nodos_i,inodesmap);
		return(1);
}

int unassigninode(int inode)
{
		int offset = inode/8;
		int shift=inode%8;
		int result;
		
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		mapa_bits_nodos_i= secboot.sec_res;
		
		//Ese mapa está en memoria
		if(!inodesmap_en_memoria)
		{
			//si no está en memoria hay que leerlo del disco
			result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
			inodesmap_en_memoria =1;
		}
		
		inodesmap[offset]&=(char) ~(1<<shift);
		vdwriteseclog(mapa_bits_nodos_i,inodesmap);
		return(1);
}


int isblockfree(int block)
{
		int offset=block/8;
		int shift=block%8;
		int result;
		int i;
		
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		
		mapa_bits_bloques = secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
		
		if(!blocksmap_en_memoria)
		{
			//Cargar todos los sectores que corresponden al mapa de bits
			//del bloque de datos
			for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
				result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
			blockmap_en_memoria = 1;
		}
		
		if(blocksmap[offset] & (1<<shift))
			return(0);
		else
			return(1);
		
}

int nextfreeblock()
{
		int i,j;
		int result;
		
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		
		mapa_bits_bloques = secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
		
		if(!blocksmap_en_memoria)
		{
			for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
				result=dreadseclog(mapa_bits_bloques+1,blocksmap+i*512)
				blockmap_en_memoria=1;
		}
		
		i=0;
		while (blockmap[i]=0xFF && i<secboot.sec_mapa_bits_bloques*512)
			i++;
		
		if(i<secboot.sec_mapa_bits_bloques*512)
		{
			j=0;
			while(blocksmap[i] & (1<<j) && j<8)
				j++;
			
			return(i*8+j);
		}
		else
			return(-1);
			
}

int assignblock(int block)
{
		int offset=block/8;
		int shift=block%8;
		int result;
		int i;
		int sector;
		
		
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		
		mapa_bits_bloques= secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
		
		if(!blocksmap_en_memoria)
		{
			for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
				result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
				blocksmap_en_memoria=1;
		
		}
		
		blocksmap[offset]|=(1<<shift);
		
		sector=(offset/512)*512;
		vdwriteseclog(mapa_bits_bloques+sector,bloquesmap+sector*512);
		//estos estaban comentados en la foto
		//for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
		//	vdwriteseclog(mapa_bits_bloques+1,blocksmap+1*512);
		return(1);
}


int unassignblock(int block)
{
	int offset=block/8;
	int shift=block%8;
	int result;
	char mask;
	int sector;
	int i;
	
		if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
	
	mapa_bits_bloques= secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
	
	if(!blocksmap_en_memoria)
	{
		for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
			result=bdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;	

	}
	
	blocksmap[offset] &= (char) ~(1<<shift);
	
	sector=(offset/512);
	vdwriteseclog(mapa_bits_bloques+sector,blocksmap+sector*512);
	//en comentarios
	//for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}



//segundo set de fotos



//8
int writeblock(int block, char *buffer)
{
	int result;
	int i;
	if(!secboot_en_memoria)
		{
			//si no está en memoria, cárgalo
			result = vdreadsector(0,0,0,1,1,(char *) &secboot);
			secboot_en_memoria=1;
		}
		
		inicio_area_datos=secboot.sec_res + secboot.sec_mapa_bits_nodos_i + secboot.sec_mapa_bits_bloques + secbot.sec_tabla_nodos_i;
		
		for(i=0;i<secboot.sec_x_bloque;i++)
			vdwriteseclog(inicio_area_datos+(block-1)*secbppt.sec_x_bloque+1,buffer+512*i);
		return(1);
}

//7

unsigned int datetoint(struct DATE date)
{
	unsigned int val = 0;
	
	val = date.year-1970;
	val<<=4;
	val|=date.month;
	val<<=5;
	val|=date.day;
	val<<=5;
	val|=date.hour;
	val<<=6;
	val|=date.min;
	val<<=6;
	val|=date.sec;

	return(val);
}


int inttudate(struct DATE *date, unsigned int val)
{
	date->sec=val&0x3F;
	val>>=6;
	date->min=val&0x3F;
	val>>=6;
	date->hour=val&0x1F;
	val>>=5;
	date->day=val&0x1F;
	val>>=5;
	date->month=val&0x0F;
	val>>=4;
	date->year=(val&0x3F) + 1970;
	
	return(1);
}

//Funciones auxiliares en el manejo de archivos
unsigned short *postoptr(int fd,int pos)
{
	int currinode;
	unsigned short *currptr;
	unsigned short indirect1;
	
	currinode = openfiles[fd].inode;
	
	//Está en los primeros 10 K
	if((pos/2048)<10)
		//Está entre los 10 apuntadores directos
		currptr = &inode[currinode].blocks[pos/2048];
	else if((pos/2048)<1034)
	{
		// Si el indirecto está vacío, asígnale un bloque
		if(inode[currinode].indirect == 0)
		{
			//El primer bloque disponible
			indirect1=nextfreeblock();
			assignblock(indirect1); //Asígnalo
			inode[currinode].indirect = indirect1;
		}
		currptr = &openfiles[fd].buffindirect[pos/2048-10];
	}
	else
		return (NULL);
	
	return(currptr);
´}

int vdwrite(int fd, char*buffer,int bytes)
{
	int currblock;
	int currinode;
	int cont =0;
	int sector;
	int i;
	int result;
	unsigned short *currptr;
	
	//si no esta abierto, regresa error
	if(openfles[fd].inuse==0)
		return (-1);
	
	//obtener el número de nodo-i actial de la tabla de archivos
	//abiertos
	currinode = openfiles[fd].inode;
	
	while(cont<bytes)
	{
		//Obtener la direccion de donde esta el bloque que
		//corresponde 
		// a la posicion actal
		currptr = currpostoptr(fd);
		if(currptr==NULL)
			return(-1);
			
		//El número de bloque actual en el que estoy escribiendo
		currblock =*currptr;
		
		//Si el bloque está en blanco, dale uno
		if(currblock ==0)
		{
			//Buscar un nuevo bloque
			currblock=nextfreeblock();
			
			//El bloque encontrado ponerlo en donde
			//apunta el apuntador al bloque actual
			*currptr = currblock;
			// Ponerlo como ocupado en el mapa de bits
			assignblock(currblock);
			//Escribir en el sector de la tabla de nodos i
			// En el disco
			sector =(currinode/8)*8;
			
			result = vdwriteseclog(inicio_nodos_i+sector,&inode[sector*8]);
		}
		
		//Si el bloque de la posición actual no está en memoria
		//Lee el bloque al buffer de archivo
		if(openfiles[fd+.currbloqueenmemoria!=currblock)
		{
			//Leer el bloque actual hacia el buffer que
			//Está en la tabla de archivos abiertos
			readblock(currblock,openfiles[fd].buffer);
			openfiles[fd].currbloqueenmemoria=currblock;
		}
		
		//Copia el caracter al buffer
		openfiles[fd].buffer[openfiles[fd].currpos%2048]=buffer`cont];
		
		//Incremente posición
		openfiles[fd].currpos++;
		
		//Si la posición es maor que el tamaño, sodifica el tamaño
		if(openfiles[fd].currpos>inode[currinode].size)
			inode[openfiles[fd].inode].size=openfiles[fd].currpos;
			
		//incremente el contador
		cont ++
		
		//Si se llena el buffer, escribleo
		if(openfiles[fd].currpos%2048==0)
			writeblock(currblock,openfiles[fd].buffer);
			
	}
	return (cont);
}

int vdseek(int fd, int offset,int whence)
{
	unsigned short oldblock,newblock;
	
	//Si no está abierto regresa error
	if(openfiles[fd].inuse==0)
		return (-1);
		
	//Obtener el número de bloque que corresponde a la posición
	//actual del archivo
	oldblock =*currpostoptr(fd);
	
	if(whence ==0) // A partir del inicio
	{
		if(offset<0 ||openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size)
			return (-1);
		
		openfiles[fd].currpos = offset;
	}else if (whence ==1) //A partir de la posición actual
	{
		if(openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size ||
			openfiles[fd].currpos+offset<0)
			return (-1);
		
		openfiles[fd].currpos+=offset;
		
	}else if (whence == 2) // A partir del final
	{
		if(offset>inode[openfiles[fd].inode].size ||
			openfiles[fd].currpos-offset<0)
			return(-1);
		
		openfiles[fd].currpos = inode[opnefiles[fd]-inode].size = offset;
	} else
		return (-1);
	
	newblock =*currpostoptr(fd);
	
	//Si después de mover el puntero, ahora la posición actual
	//corresponde a un bloque diferente
	if(newblock!=oldblock)
	{
		//Escribir en disco el bloque que corresponde a la
		//posición anterior al movimiento
		writeblock(oldblock,openfiles[fd].buffer);
		
		//Leer el bloque que corresponde a la posición actual
		//(después del movimiento)
		readblock(newblock,openfiles`fd].buffer);
		
		//Actualizar en la tabla de archivos abiertos el bloque
		openfiles[fd].currbloqueenmemoria = newblock;
	}
	
	return (openfiles[fd].currpos);
}

int vdcreat(char *filename,unsigned short perms)
{
	int numinode;
	int i;
	
	//Ver si ya existe el archivo
	numinode=searchinode(filename);
	if(numinode=1)
	{
		//Buscar un inodo en blanco en el mapa de bits (nodos i)
		numinode = nextfreeinode();
		if(numinode == -1)
		{
			return (-1); //No hay espacio para más archivos
		}
	}else	//Elimina el inodo
		removeinode(numinode);
	
	//Escribir el archivo en el inodo encontrado
	
	setinode(numinode,filename,perms,getuid(),getgid());
	assigninode(numinode);
	
	if(!openfiles_inicializada)
	{
		for(i=3;i<16;i++)
		{
			openfiles[i].inuse=0;
			openfiles[i].currbloqueenmemoria=-1;
		}
		openfiles_inicializada =1;
	}
	
	//Buscar si hay lugar en la abla de archivos abiertos
	//Si no hay lugar, regresa -1
	i=3;
	while(openfiles[i].inuse && i<16)
		i++
	
	if(i>=16)
		return (-1);
	
	openfiles[i].inuse=1;
	openfiles[i].inode = numinode;
	openfiles[i].curpos =0;
	return(i);
}

unsigned int currdatetimetoint()
{
	struct tm*tm_ptr;
	time_t the_time;
	
	struct Date now;
	
	(void) time(&the_time);
	tm_ptr = gmtime(&the_time);
	
	now.year = tm_ptr->tm_year-70;
	now.month = tm_ptr->tm_mon+1;
	now.day=tm_ptr->tm_mday;;
	now.hour = tm_ptr->tm_hour;
	now.min=tm_ptr->tm_min;
	now.sec=tem_ptr->tm_sec;
	return(datoint(now));
}


//Funcionde de elvira hechas por charly

void vdunlink(char*filename)
{
	//buscar el nodo del archivo
	int numinode = searchinode(filename);
	//remover el nodo
	removeinode(numinode);
}

void removeinode(int numinode)
{
	//eliminar un inodo, libera el bloque
}

int searchnode(char *filename)
{
	int i =0;
	for(i=0;i<sizeof(tablainodo);i++)
	{
		bool equal = (std::strcmp(tablainodo[i].name, filename) == 0);
		if(equal)
			return i
	}
	return (-1);
	//Busca en la tabla de nodos i el archivo
	//filename, si no lo encuentra regresa -1
	
	//Si lo encuentra, regresa el numero de inodo que le
	//corresponde al archivo
}



int vdreadseclog(int seclog, char *buffer)
{

	drive=0;
	int nsec = ((seclog % SEC_X_TRACK) + 1);
	int nhead = ((seclog/SEC_X_TRACK) % SUPERFICIES);
	int ncyl = (seclog/(SEC_X_TRACK * SUPERFICIES));

	printf("Desplegando de disco%d.vd Cil=%d, Sup=%d, Sec=%d\n",drive,ncyl,nhead,nsec);
	
	if(vdreadsector(drive, nhead, ncyl, nsec, 1, buffer))
		return (1);
	
	return(-1);
	
}

int vdwriteseclog(int seclog, char *buffer)
{

	drive=0;
	int nsec = ((seclog % SEC_X_TRACK) + 1);
	int nhead = ((seclog/SEC_X_TRACK) % SUPERFICIES);
	int ncyl = (seclog/(SEC_X_TRACK * SUPERFICIES));

	printf("Desplegando de disco%d.vd Cil=%d, Sup=%d, Sec=%d\n",drive,ncyl,nhead,nsec);
	
	if(vdwritesector(drive, nhead, ncyl, nsec, 1, buffer))
		return (1);
	
	return(-1);
	
}


