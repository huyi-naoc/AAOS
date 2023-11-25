#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <errno.h>
#include <getopt.h>
#include <unistd.h>

static struct option long_options[] = {
    {"--delay", required_argument, NULL, 'd'}, \
    {"--timeout", required_argument, NULL, 't'}, \
    {0, no_argument, NULL, 0}
};

static double delay, timeout=5.;
static bool oneshot;

#ifdef LINUX
#include <signal.h>

#include <sys/select.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0)
#ifndef HAS_PIDFD_FEATURE
#define HAS_PIDFD_FEATURE
#endif
#else
#ifdef HAS_PIDFD_FEATURE
#undef HAS_PIDFD_FEATURE
#endif
#endif

#ifdef HAS_PIDFD_FEATURE
#ifndef __NR_pidfd_open
#define __NR_pidfd_open 434
#endif

static int
pidfd_open(pid_t pid, unsigned int flags)
{
    return syscall(__NR_pidfd_open, pid, flags);
}
#endif

/*
 * return 0, timeout
 * return 1, OK
 * return -1, error happened
 */
static int
waitpid_(pid_t pid)
#ifdef HAS_PIDFD_FEATURE
{
    int pidfd, ret;
    fd_set readfds;
    struct timeval tv;

    tv.tv_sec = floor(timeout);
    tv.tv_usec = (timeout - tv.tv_sec) * 1000000;

    if ((pidfd = pidfd_open(pid, 0)) < 0) {
        if (errno == ESRCH) {
            fprintf(stderr, "Process %d does not exist.", (int) pid);
        } else {
            fprintf(stderr, "pidfd_open error.\n");
        }
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&readfds);
    FD_SET(pidfd, &readfds);
    if ((ret = select(pidfd + 1, &readfds, NULL, NULL, &tv)) < 0) {
        fprintf(stderr, "select error.\n");
        ret = -1;
        goto error;
    } else {
        if (ret == 0) {
            fprintf(stderr, "select timed out.\n");
            ret = 0;
            goto error;
        } else {
            fprintf(stdout, "Process %d has terminated.\n", (int) pid);
            ret = 1;
            goto error;
        }
    }
error:
    return ret;
}
#else
{
    int ret;
    if ((ret = kill(pid, 0)) < 0) {
        if (errno == ESRCH) {
            fprintf(stderr, "Process %d does not exist.", (int) pid);
        } else {
            fprintf(stderr, "kill error.\n");
        }
        exit(EXIT_FAILURE);
    }
    struct timespec tp;
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
    nanosleep(&tp, NULL);
    if ((ret = kill(pid, 0)) < 0) {
        if (errno == ESRCH) {
            return 1;
        } else {
            fprintf(stderr, "kill error.\n");
            exit(EXIT_FAILURE);
            return -1;
        }
    } else {
        return 0;
    }
}
#endif /* ifdef HAS_PIDFD_FEATURE */
#endif /* ifdef LINUX */

#ifdef MACOSX
#include <sys/event.h>
#include <sys/time.h>

static int
waitpid_(pid_t pid)
{
    int kq, ret;
    struct kevent kev, eventlist[10];
    
    if ((kq = kqueue()) < 0) {
        fprintf(stderr, "kqueue error.\n");
        exit(EXIT_FAILURE);
    }

    EV_SET(&kev, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, pid, NULL);
    struct timespec tp;
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
    if ((ret = kevent(kq, &kev, 1, eventlist, 10, &tp)) < 0) {
        fprintf(stderr, "kevent error.\n");
        return -1;
    } else if (ret == 0) {
        return 0;
    } else {
        if ((pid_t) eventlist[0].ident == pid) {
            return 1;
        } else {
            return 0;
        }
    }
}
#endif /* ifdef MACOSX */

int
main(int argc, char *argv[])
{
    int ch, ret;
    pid_t pid;
    struct timespec tp;
    while ((ch = getopt_long(argc, argv, "d:t:s", long_options, NULL)) != -1) {
        switch (ch) {
            case 'd':
                delay = atof(optarg);
                break;
            case 't':
                timeout = atof(optarg);
                break;
            default:
                break;
        }
    }
    argc -= optind;
    argv += optind;

    pid = (pid_t) atoi(argv[0]);
    if (oneshot) {
        waitpid_(pid);
    } else {
        tp.tv_sec = floor(delay);
        tp.tv_nsec = (delay - tp.tv_sec) * 1000000000;
        while ((ret = waitpid_(pid)) != 1) {
            nanosleep(&tp, NULL);
        }
    }
    return 0;
}

