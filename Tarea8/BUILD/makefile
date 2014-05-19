all: vdisk createvd vdformat dumpsec dumpseclog

vdisk: vdisk.c vdisk.h
	gcc -c vdisk.c

createvd: createvd.c vdisk.h vdisk.o
	gcc -o createvd createvd.c vdisk.o

vdformat: vdformat.c vdisk.h vdisk.o
	gcc -o vdformat vdformat.c vdisk.o

dumpsec: dumpsec.c vdisk.h vdisk.o
	gcc -o dumpsec dumpsec.c vdisk.o

dumpseclog: dumpseclog.c vdisk.h vdisk.o
	gcc -o dumpseclog dumpseclog.c vdisk.o
