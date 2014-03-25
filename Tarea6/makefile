all: semaphores messages

semaphores: semaphores.c
	gcc -pthread -o sem semaphores.c

messages: messages.c
	gcc -pthread -o msj messages.c

clean:
	rm sem
	rm msj
