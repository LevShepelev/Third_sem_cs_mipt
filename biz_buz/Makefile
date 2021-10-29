all: build run

build:
	gcc -c -g -O0 biz_buz_try_2.c -Wall -Werror
	gcc biz_buz_try_2.o -o biz_buz_try_2

run: build
	 ./biz_buz_try_2 ./file_source.txt ./file_destination.txt
