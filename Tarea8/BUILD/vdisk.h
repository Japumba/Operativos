#define HEADS 20
#define SECTORS 17
#define CYLINDERS 160

//Superbloque
struct SECBOOT {
	char jump[4];//0
	char nombre_disco[8];//disk0
	unsigned char sec_res;//1
	unsigned char sec_mapa_bits_nodos_i;//0
	unsigned char sec_mapa_bits_bloques;//1
	unsigned short sec_tabla_nodos_i;//5
	unsigned short sec_log_unidd;//54400
	unsigned char sec_x_bloque;//4
	unsigned char heads;//20
	unsigned char cyls;//160
	unsigned char secfis;//17
	char restante[487];//basura rebelde
};

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

struct OPENFILES
{
	int inuse;
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512];
};
