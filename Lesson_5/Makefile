all: build run


build:
	gcc -Wall -Werror -c test.c
	gcc -Wall -Werror test.o -o test
	gcc -Wall -Werror -c Test_2.c
	gcc -Wall -Werror -Wall Test_2.o -o Test_2
	gcc -Wall -Werror -c Test_3.c
	gcc -Wall -Werror -Wall Test_3.o -o Test_3
	gcc -Wall -Werror -c -g -O0 pipe_easy_2.c 
	gcc -Wall -Werror pipe_easy_2.o -o pipe_easy_2

run: build
	./pipe_easy_2 input_2.txt
