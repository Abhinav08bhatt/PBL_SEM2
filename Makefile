CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

app: main.c
	$(CC) $(CFLAGS) main.c -o app

run: app
	./app

clean:
	rm -f app
