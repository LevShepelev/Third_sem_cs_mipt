all: build run


build:
	gcc -Wall -Werror -c -g -O0 Task_1.c 
	gcc -Wall -Werror Task_1.o -o Task_1

run: build
	./Task_1 file_in.txt file_out.txt