all: build run
	
build:
	gcc -c -g -O0 Sender.c -Wall 
	gcc -Wall Sender.o -o Sender
	gcc -c -g -O0 Receiver.c -Wall
	gcc -Wall Receiver.o -o Receiver

run: build
	 ./Sender