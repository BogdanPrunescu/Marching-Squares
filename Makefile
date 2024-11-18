build: ms_seq.c ms_pthread.c
	gcc ms_seq.c helpers.c -o ms_seq -lm -Wall -Wextra
	gcc ms_pthread.c helpers.c -o ms_pthread -lm -lpthread -Wall -Wextra
clean:
	rm -rf ms_seq