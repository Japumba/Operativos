all: vdisk vdformat

vdisk: vdisk.c vdisk.h
	gcc -c vdisk.c

vdformat: vdformat.c vdisk.o
	gcc -o vdformat vdformat.c vdisk.o
