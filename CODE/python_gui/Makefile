CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

backend_app: backend_api.c ../c_core/student.c ../c_core/attendance.c ../c_core/student.h ../c_core/attendance.h
	$(CC) $(CFLAGS) backend_api.c ../c_core/student.c ../c_core/attendance.c -o backend_app

clean:
	rm -f backend_app
