
SRCS = main.c opt.c uart.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lutil


all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o bridge
clean:
	rm -f bridge

PHONY: all clean
