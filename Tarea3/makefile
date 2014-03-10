all: pthread clone

pthread: pthread.c
	gcc -o thread pthread.c -pthread
	gcc -o clone clone.c


clean:
	rm thread

run:
	sudo ./thread
