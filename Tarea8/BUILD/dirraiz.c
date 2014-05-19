//manejo de directorio

typedef int VDDIR,

struct vddirent
{
	char *d_name;
};

VDDIR dire[2]={-1,-1};
struct vddirent current;

VDDIR *vdopendir(char* path)
{
	int i=0;
	int result;
	
	if(!secboot_en_memoria)
	{
		result = vdreadsector(0,0,0,1,1,(char*)secboot);
		secboot_en_memoria =1;
	}
	inicio_nodos_i =secboot.sec_res+secboot.sec_mapa_bits_nodos_1 +secboot.sec_mapa_bits_bloques;
	
	if(!nodos_i_en_memoria)
	{
		for(i=0;secboot.sec_tabla_nodos_i;i++)
			result = vdreadseclog(inicio_nodos_i+i,&inode[i*8]);
			
			nodos_i_en_memoria=1;
	}
	
	if(strcmp(path,".")!-0)
		return(NULL);
		
	i=0;
	while(dire[i]!==1 &&i<2)
		i++;
		
	if(i==2)
		return(NULL);
	
	dire[i] =0;
	
	return (&dire[i]);	
}

struct vddirent *vdreaddir(VDDIR *dirdaac)
{
	int i;
	
	int result;
	
	if(!nodos_i_en_memoria)
	{
		for(i=0;1<secboot.sec_tabla_nodos_i,i++)
			result= vdreadseclog(inicio_nodos_i+i,&inode[i*8]);
		
		nodos_i_en_memoria =1;
	}
	
	while(isinodefree(*dirdaac) && *dirdaac<4056)
		(*dirdaac)++;
		
	current.d_name = inode [*dirdaac].name;
	(*dirdaac)++;
	
	if(*dirdaac >= 4056)
		return (NULL);
	return(&current);
	
}
int vdcloseaadir(VDDIR *dirdaac)
{
	(*dirdaac)=-1;
}






