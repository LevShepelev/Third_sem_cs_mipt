all: build run
	
build:
	gcc -Wall -Werror -c -g -O0 Sender.c -Wall 
	gcc -Wall -Werror -Wall Sender.o -o Sender
	gcc -Wall -Werror -c -g -O0 Receiver.c -Wall
	gcc -Wall -Werror -Wall Receiver.o -o Receiver

run: build
	 ./Receiver file_out.txt