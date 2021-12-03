all: build run
	
build:
	gcc -c -g -O0 Task_1.c 
	gcc -Wall Task_1.o -o Task_1 -lm -pthread

run: build
	 ./Task_1 100 10000