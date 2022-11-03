#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>

#ifndef LOCKMODE
#define LOCKMODE        (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#endif

static int
lockfile(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return (fcntl(fd, F_SETLK, &fl));
}

static int
already_running(const char *lockfilepath)
{
    int fd;

    fd = open(lockfilepath, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0) {
        fprintf(stderr, "Open lockfile \"%s\" failed.\n", lockfilepath);
        return EXIT_FAILURE;
    }

    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return EXIT_FAILURE;
        } else {
            fprintf(stderr, "Lock lockfile \"%s\" failed.\n", lockfilepath);
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int
main(int agrc, char *argv[])
{
    const char *lockfilepath = argv[1];
    int sockets[2], ret;
    pid_t pid;
    ssize_t n;

    socketpair(PF_LOCAL, SOCK_STREAM, 0, sockets);

    pid = fork();
    switch (pid) {
        case -1:
            fprintf(stderr, "fork error\n");
            exit(EXIT_FAILURE);
            break;
        case 0:
            close(STDOUT_FILENO);
            close(sockets[0]);
            if (already_running(lockfilepath)) {
                ret = 1;
                n = write(sockets[1], &ret, sizeof(int));
		if (n < 0) {
		    fprintf(stderr, "write error\n");
		}
                close(sockets[1]);
                exit(EXIT_FAILURE);
            }
            ret = 0;
            n = write(sockets[1], &ret, sizeof(int));
	    if (n < 0) {
		fprintf(stderr, "write error\n");
	    }
            close(sockets[1]);
            for (; ;) {
                sleep(31536000);
            }
            return 0;
            break;
        default:
            close(sockets[1]);
            n = read(sockets[0], &ret, sizeof(int));
	    if (n < 0) {
		fprintf(stderr, "read error\n");
		close(sockets[0]);
		exit(EXIT_FAILURE);
	    }
            printf("%d\n", (int) pid);
            close(sockets[0]);
            return ret;
            break;
    }
}
