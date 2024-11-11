build: ms_seq.c
	gcc ms_seq.c helpers.c -o ms_seq -lm -lpthread -Wall -Wextra
clean:
	rm -rf ms_seq