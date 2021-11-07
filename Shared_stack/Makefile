all: build run

build:
	gcc -c -g main.c Shared_stack.c 
	gcc main.o Shared_stack.o -o main
	gcc -g -c Test.c Shared_stack.c
	gcc Test.o Shared_stack.o -o Test
run: build
	./main