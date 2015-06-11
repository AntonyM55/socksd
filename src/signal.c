#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include <logger.h>


#define LF  (uint8_t) 10
#define CR  (uint8_t) 13


static ssize_t
read_file(int fd, uint8_t *buf, size_t size, off_t offset) {
    ssize_t n = pread(fd, buf, size, offset);
    if (n == -1) {
        return -1;
    }
    return n;
}

static int32_t
get_pid(const char *pidfile) {
    u_char    buf[10];
    ssize_t   n;

    int fd = open(pidfile, O_RDONLY);

    if (fd == -1) {
        return -1;
    }

    n = read_file(fd, buf, 10, 0);

    close(fd);

    if (n == -1) {
        return -1;
    }

    while (n-- && (buf[n] == CR || buf[n] == LF)) { /* void */ }

    return atoi((const char*)buf);
}

int
signal_process(char *signal, const char *pidfile) {
    int32_t  pid;

    pid = get_pid(pidfile);
    if (pid == -1) {
		logger_log(LOG_ERR, "open \"%s\" failed (%d: %s)", pidfile, errno, strerror(errno));
        return 1;
    }

    if (strcmp(signal, "stop") == 0) {
        if (kill(pid, SIGTERM) != -1) {
            return 0;
        } else {
            logger_log(LOG_ERR, "stop socksd failed (%d: %s)", pid, strerror(errno));
        }
    }
    if (strcmp(signal, "quit") == 0) {
        if (kill(pid, SIGQUIT) != -1) {
            return 0;
        } else {
            logger_log(LOG_ERR, "quit socksd failed (%d: %s)", errno, strerror(errno));
        }
    }

    return 1;
}
