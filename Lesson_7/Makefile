all: build run
	
build:
	gcc -c -g -O0 Task_1.c 
	gcc -Wall Task_1.o -o Task_1
	gcc -c -g -O0 Task_1_getter.c 
	gcc -Wall Task_1_getter.o -o Task_1_getter

run: build
	 ./Task_1 input.txt
	 ./Task_1_getter output.txt