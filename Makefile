CC = gcc
CFLAGS = -Wall -Wextra -pedantic
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = prog

all: $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o