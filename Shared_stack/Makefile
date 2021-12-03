all: build run

build:
	gcc -Wall -Werror -c -g main.c Shared_stack.c 
	gcc main.o Shared_stack.o -o main
	gcc -Wall -Werror -g -c Test.c Shared_stack.c
	gcc Test.o Shared_stack.o -o Test
	gcc -Wall -Werror -g -c Test_1.c Shared_stack.c
	gcc Test_1.o Shared_stack.o -o Test_1
run: build
	./main