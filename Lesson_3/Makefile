all: build run
	
build:
	gcc -c Read_sev_fifo.c
	gcc -Wall Read_sev_fifo.o -o Read_sev_fifo

run: build
	 ./Read_sev_fifo ./file1 ./file2 ./file3 ./file4