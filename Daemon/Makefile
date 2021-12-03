all: build run
build:
	gcc -Wall -c -g -O0 Daemon.c
	gcc Daemon.o -o Daemon
	gcc -Wall -c -g -O0 interface.c
	gcc interface.o -o interface
run: build
	./Daemon /home/lev/Third_sem_cs_mipt/Daemon/source /home/lev/Third_sem_cs_mipt/Daemon/Destination