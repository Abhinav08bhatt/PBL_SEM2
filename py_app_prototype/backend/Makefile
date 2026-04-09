CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
TARGET=backend_app
SRC=backend_api.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
