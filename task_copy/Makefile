
all: build run
	
build:
	gcc -c task_cp.c
	gcc -Wall task_cp.o -o task_cp

run: build
	 ./task_cp ./file_source.txt ./file_destination.txt