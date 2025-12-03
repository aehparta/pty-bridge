/*
 * Simple UART interface implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "uart.h"


static int uart_baud_to_const(int baudrate)
{
    switch (baudrate) {
    case 1200:
        return B1200;
    case 2400:
        return B2400;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    }

    return -1;
}

int uart_open(const char *path, int baudrate)
{
    struct termios tty;
    int fd, baudrate_const;

    if (path == NULL) {
        fprintf(stderr, "serial port device must be given\n");
        return -1;
    }

    baudrate_const = uart_baud_to_const(baudrate);
    if (baudrate_const == -1) {
        fprintf(stderr, "unsupported baudrate: %d\n", baudrate);
        return -1;
    }

    fd = open(path, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "unable to open serial port, reason (%d): %s\n", errno, strerror(errno));
        return -1;
    }

    if (tcgetattr(fd, &tty) != 0) {
        fprintf(stderr, "tcgetattr() failed, reason (%d): %s\n", errno, strerror(errno));
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, baudrate_const);
    cfsetispeed(&tty, baudrate_const);
    tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                             // 8-bit characters
    tty.c_cflag &= ~PARENB;                         // no parity bit
    tty.c_cflag &= ~CSTOPB;                         // only need 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                        // no hardware flowcontrol
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // shut off xon/xoff ctrl
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
    tty.c_oflag &= ~OPOST;                          // raw output

    tty.c_cc[VMIN] = 0;  // read doesn't block
    tty.c_cc[VTIME] = 0; // 0 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "tcsetattr() failed, reason (%d): %s\n", errno, strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}
