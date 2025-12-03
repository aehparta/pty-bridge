/*
 * Pseudoterminal bridge
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pty.h>
#include "../common/opt.h"
#include "../common/uart.h"


#define MAX_PTY_COUNT 32


const char *baud_rates[] = {
    "1200",
    "2400",
    "9600",
    "19200",
    "38400",
    "57600",
    "115200",
    NULL
};

struct opt_option opt_all[] = {
    { 'h', "help", no_argument, 0, NULL, NULL, "display this help and exit", { 0 } },
    { 'v', "ptys", required_argument, 0, "2", NULL, "count of ptys to create", { OPT_FILTER_INT, 1, MAX_PTY_COUNT, NULL } },
    { 'd', "device", required_argument, 0, NULL, NULL, "serial port device", { 0 } },
    { 'b', "baudrate", required_argument, 0, "9600", NULL, "serial port baudrate, accepts: 1200, 2400, 9600, 19200, 38400, 57600, 115200", { OPT_FILTER_STR, 0, 0, baud_rates } },
    { 0 }
};

const char *help_prepend =
    "Pseudoterminal bridge\n"
    "Bridges data between pseudoterminals (ptys) and a physical serial port.";

const char *help_append =
    "Create 3 ptys bridged with /dev/ttyUSB0:\n"
    "  bridge -v 3 -d /dev/ttyUSB0 -b 9600\n"
    "\n"
    "Create 4 bridged ptys without real serial port:\n"
    "  bridge -v 4\n"
    "\n"
    "Note: created ptys are symlinked to /dev/ttyPTYBR* if the program is run as root.\n";


int main(int argc, char *argv[])
{
    int fds[MAX_PTY_COUNT + 1] = { 0 };
    int ptyc = 0;

    /* parse command line arguments */
    if (opt_init(opt_all, NULL, help_prepend, help_append) || opt_parse(argc, argv)) {
        return 1;
    }

    /* get virtual pty count */
    ptyc = opt_get_int('v');

    /* open serial port if defined*/
    if (opt_used('d')) {
        fds[ptyc] = uart_open(opt_get('d'), opt_get_int('b'));
        if (fds[ptyc] < 0) {
            fprintf(stderr, "failed to open serial port, reason(%d): %s\n", errno, strerror(errno));
            return 1;
        }
        printf("serial port %s at %d baud\n", opt_get('d'), opt_get_int('b'));
    }

    /* open ptys */
    int slave_fd;
    for (int i = 0; i < ptyc; i++) {
        char pty_path[256];

        /* open pty */
        int err = openpty(&fds[i], &slave_fd, pty_path, NULL, NULL);
        if (err < 0) {
            fprintf(stderr, "openpty() failed for pty #%d, reason (%d): %s\n", i, errno, strerror(errno));
            return 1;
        }

        /* disable echo */
        struct termios tmios;
        tcgetattr(fds[i], &tmios);
        tmios.c_lflag &= ~ECHO;
        tcsetattr(fds[i], TCSANOW, &tmios);

        /* create /dev/PTYBR# if root */
        if (getuid() == 0) {
            char pty_link[256];
            chmod(pty_path, 0666);
            sprintf(pty_link, "/dev/ttyPTYBR%d", i);
            unlink(pty_link);
            symlink(pty_path, pty_link);
            printf("pty: %s -> %s\n", pty_link, pty_path);
        } else {
            printf("pty: %s\n", pty_path);
        }
    }

    /* main loop */
    while (1) {
        fd_set read_fds;
        int max_fd;

        FD_ZERO(&read_fds);
        for (int i = 0; fds[i]; i++) {
            FD_SET(fds[i], &read_fds);
            if (fds[i] > max_fd) {
                max_fd = fds[i];
            }
        }

        int err = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (err < 0) {
            fprintf(stderr, "select() failed, reason (%d): %s\n", errno, strerror(errno));
            break;
        } else if (err == 0) {
            /* timeout, loop to start */
            continue;
        } else {
            for (int i = 0; fds[i]; i++) {
                if (FD_ISSET(fds[i], &read_fds)) {
                    uint8_t buf[1024];
                    int n = read(fds[i], buf, sizeof(buf) - 1);
                    if (n > 0) {
                        for (int j = 0; fds[j]; j++) {
                            if (j != i) {
                                write(fds[j], buf, n);
                            }
                        }
                    } else if (n < 0) {
                        fprintf(stderr, "error when from #%d (%d): %s\n", i, errno, strerror(errno));
                    }
                }
            }
        }
    }

    return 0;
}
