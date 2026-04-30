CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

menu_app: menu.c student.c attendance.c grades.c student.h attendance.h grades.h
	$(CC) $(CFLAGS) menu.c student.c attendance.c grades.c -o menu_app

clean:
	rm -f menu_app
