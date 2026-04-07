build: tema1

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

main.o: main.c utils.h
	gcc -c main.c -o main.o

tema1: main.o utils.o
	gcc main.o utils.o -o tema1

clean:
	rm main.o utils.o tema1