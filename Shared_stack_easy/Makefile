all: build run

build: Shared_stack.h
	gcc -Wall -Werror -c -g -O0 main.c Shared_stack.c 
	gcc main.o Shared_stack.o -o main
run: build
	./main