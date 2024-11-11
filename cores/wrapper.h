//
//  wrapper.h
//  AAOS
//
//  Created by huyi on 18/6/13.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __wrapper_h
#define __wrapper_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <time.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <netdb.h>
#include <pthread.h>
#include <pwd.h>
#include <regex.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>

#ifdef MACOSX
#include <sys/event.h>
#include <uuid/uuid.h>
#endif

#ifdef LINUX
#include <mntent.h>
#include <sys/epoll.h>
#endif

typedef struct sockaddr SA;

#define LOG_INGORE -1
#define LOG_PRINT   0
#define LOG_INFILE  1

#ifndef TIMER_ABSTIME
#define TIMER_ABSTIME 1
#endif

#ifdef LINUX
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <netinet/tcp.h>
struct sf_hdtr {
    struct iovec *headers;
    int hdr_cnt;
    struct iovec *trailers;
    int trl_cnt;
};
#endif

void err_open(const char *, const char *, int);
void err_close(void);

int Cfsetispeed(struct termios *, speed_t);
int Cfsetospeed(struct termios *, speed_t);
void *Malloc(size_t);
void *Realloc(void *, size_t);
int Tcflush(int, int);
int Tcgetattr(int, struct termios *);
int Tcsetattr(int, int, const struct termios *);

int Accept(int, SA *, socklen_t *);
int Access(const char *, int);
int Chdir(const char *);
int Clock_gettime(clockid_t, struct timespec *tp);
int Close(int);
int Dup(int);
int Fcntl(int, int, ...);
pid_t Fork(void);
int Ftruncate(int, off_t);
int Getrlimit(int, struct rlimit *);
int Kill(pid_t, int);
int Mount(const char *, const char *, const char *, unsigned long, void *);
void *Mmap(void *, size_t, int, int, int, off_t);
int Munmap(void *, size_t);
int Open(const char *, int, ...);
DIR *Opendir(const char *);
ssize_t Read(int, void *, size_t);
struct dirent *Readdir(DIR *);
int Select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
int Setsockopt(int, int, int, const void *, socklen_t);
int Stat(const char *, struct stat *);
int Unlink(const char *);
ssize_t Write(int, const void *, size_t);
ssize_t Writev(int, struct iovec *, int);

int Pthread_cancel(pthread_t);
int Pthread_cond_destroy(pthread_cond_t *);
int Pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int Pthread_cond_broadcast(pthread_cond_t *);
int Pthread_cond_signal(pthread_cond_t *);
int Pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
int Pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int Pthread_create(pthread_t *, const pthread_attr_t *, void*(*start_routine)(void *), void *);
int Pthread_create2(pthread_t *, const pthread_attr_t *, void*(*start_routine)(void *), ...);
int Pthread_detach(pthread_t);
int Pthread_join(pthread_t, void **);
int Pthread_mutex_lock(pthread_mutex_t *);
int Pthread_mutex_trylock(pthread_mutex_t *);
#ifdef LINUX
int Pthread_mutex_timedlock(pthread_mutex_t *, const struct timespec *);
#endif
int Pthread_mutex_destroy(pthread_mutex_t *);
int Pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
int Pthread_mutex_unlock(pthread_mutex_t *);
int Pthread_once(pthread_once_t *, void (*)(void));
int Pthread_setcancelstate(int state, int *oldstate);
int Pthread_sigmask(int, const sigset_t *, sigset_t *);
int Pthread_rwlock_destroy(pthread_rwlock_t *);
int Pthread_rwlock_init(pthread_rwlock_t *, const pthread_rwlockattr_t *);
int Pthread_rwlock_rdlock(pthread_rwlock_t *);
int Pthread_rwlock_wrlock(pthread_rwlock_t *);


int Getidbyname(const char *, uid_t *, gid_t *);
void mkdirp(const char *);
int Nanosleep(double);
ssize_t Writen(int, const void *, size_t);
ssize_t Readn(int, void *, size_t);
ssize_t Writen2(int, const void *, size_t);
ssize_t Readn2(int, void *, size_t);
ssize_t Sendfile(int, int, off_t, off_t *, struct sf_hdtr *, int);
int Tcp_connect(const char *, const char *, SA *, socklen_t *);
int Tcp_connect_nb(const char *, const char *, SA *, socklen_t *, double);
int Tcp_listen(const char *, const char *, SA *, socklen_t *);

#endif /* wrapper_h */
