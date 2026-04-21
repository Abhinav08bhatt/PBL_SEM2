CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

menu_app: menu.c student.c attendance.c student.h attendance.h
	$(CC) $(CFLAGS) menu.c student.c attendance.c -o menu_app

clean:
	rm -f menu_app
