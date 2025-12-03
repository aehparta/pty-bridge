
BINARY = pty-bridge
SRCS = main.c opt.c uart.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lutil


all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $(BINARY)
clean:
	rm -f $(BINARY)

PHONY: all clean
