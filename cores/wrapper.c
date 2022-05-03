//
//  wrapper.c
//  AAOS
//
//  Created by huyi on 18/6/13.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "def.h"
#include "wrapper.h"

#define MAX_RECONNECT_TIMES 3
#define SLEEP_TIME 60
#define BACKLOG 10

static int errfd = -1;
static char err_name[33];
static int err_mode = -1;

/*
 * Error tracing log.
 */

void
err_open(const char *path, const char *name, int mode)
{
    err_mode = mode;
    
    if (err_mode != -1) {
        errfd = open(path, O_CREAT | O_WRONLY | O_APPEND | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        snprintf(err_name, 33, "%s", name);
    }
}

void
err_close(void)
{
    if (errfd != -1) {
        close(errfd);
    }
}

static void
err_fatal(const char *func_name, int errnum)
{
    if (err_mode == 0) {
        fprintf(stderr, "%s -- %s (Fatal) -- %s\n", err_name, func_name, strerror(errnum));
    } else if (err_mode > 0) {
        if (errfd != -1 ) {
            char buf[1024], *idx = buf;
            struct tm result;
            time_t clock;
            ssize_t n, left;
            
            clock = time(&clock);
            gmtime_r(&clock, &result);
            n = strftime(idx, 1024, "%Y-%m-%d %H:%M:%S ", &result);
            idx += n;
            left = 1024 - n;
            snprintf(idx, left, "%s -- %s (Fatal) -- ", err_name, func_name);
            left -= strlen(idx);
            idx += strlen(idx);
            strerror_r(errnum, idx, left - 2);
            idx[strlen(idx)] = '\n';
            idx[strlen(idx) + 1] = '\0';
            
            n = write(errfd, buf, strlen(buf));
        }
    } else {
        return;
    }
    if (errfd != -1) {
        fsync(errfd);
    }
    abort();
}

static void
err_warn(const char *func_name, int errnum)
{
    if (err_mode == 0) {
        fprintf(stderr, "%s -- %s (Warn) -- %s\n", err_name, func_name, strerror(errnum));
    } else if (err_mode > 0) {
        if (errfd != -1 ) {
            char buf[1024], *idx = buf;
            struct tm result;
            time_t clock;
            ssize_t n, left;
            
            clock = time(&clock);
            gmtime_r(&clock, &result);
            n = strftime(idx, 1024, "%Y-%m-%d %H:%M:%S ", &result);
            idx += n;
            left = 1024 - n;
            snprintf(idx, left, "%s -- %s (Warn) -- ", err_name, func_name);
            left -= strlen(idx);
            idx += strlen(idx);
            strerror_r(errnum, idx, left - 2);
            idx[strlen(idx)] = '\n';
            idx[strlen(idx) + 1] = '\0';
            
            n = write(errfd, buf, strlen(buf));
        }
    }
    errno = errnum;
}

/*
 * Standard C library function wrappers.
 */


int
Cfsetispeed(struct termios *termptr, speed_t speed)
{
    int s;
    s = cfsetispeed(termptr, speed);
    
    if (s != 0) {
        err_warn("cfsetispeed", errno);
    }
    return s;
}

int
Cfsetospeed(struct termios *termptr, speed_t speed)
{
    int s;
    s = cfsetospeed(termptr, speed);
    
    if (s != 0) {
        err_warn("cfsetospeed", errno);
    }
    return s;
}


void *
Malloc(size_t size)
{
    void *s;
    s = malloc(size);
    if (s == NULL) {
        err_fatal("malloc", errno);
    }
    return s;
}

void *
Realloc(void *ptr, size_t size)
{
    void *s;
    s = realloc(ptr, size);
    if (s == NULL) {
        err_fatal("realloc", errno);
    }
    return s;
}

int
Tcflush(int fd, int queue_selector)
{
    int s;
    s = tcflush(fd, queue_selector);
    if (s < 0) {
        err_warn("tcflush", errno);
    }
    
    return s;
}

int
Tcgetattr(int fd, struct termios *termptr)
{
    int s;
    
    s = tcgetattr(fd, termptr);
    if (s < 0) {
        err_warn("tcgetattr", errno);
    }
    
    return s;
}

int
Tcsetattr(int fd, int opt, const struct termios *termptr)
{
    int s;
    
    s = tcsetattr(fd, opt, termptr);
    if (s < 0) {
        err_warn("tcsetattr", errno);
    }
    
    return s;
}


/*
 * POSIX "system call" wrappers.
 * Returning 0 indicates success, -1 indicates failure.
 * The errno should be properly set by the system calls.
 */
int
Accept(int sockfd, SA *sockaddr, socklen_t *addrlen)
{
    int s;
    s = accept(sockfd, sockaddr, addrlen);
    if (s < 0) {
        err_warn("accept", errno);
    }
    return s;
}

int
Access(const char *path, int amode)
{
    int s;
    s = access(path, amode);
    if (s < 0 || errno != ENOENT) {
        err_warn("accept", errno);
    }
    return s;
}

int
Chdir(const char *pathname)
{
    int s;
    s = chdir(pathname);
    if (s < 0) {
        err_fatal("chdir", errno);
    }
    return s;
}

int
Clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    int s;
    s = clock_gettime(clock_id, tp);
    if (s < 0) {
        err_fatal("clock_gettime", errno);
    }
    return s;
    
}

int
Clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain)
{
    int s = 0;
#ifdef LINUX
    s = clock_nanosleep(clock_id, flags, request, remain);
    if (s < 0) {
        err_warn("close", errno);
    }
#else
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    struct timespec t;
    if (flags != TIMER_ABSTIME) {
        struct timespec t_current;
        clock_gettime(clock_id, &t_current);
        t.tv_sec = request->tv_sec + t_current.tv_sec;
        t.tv_nsec = request->tv_nsec + t_current.tv_nsec;
        if (t.tv_nsec > 1000000000) {
            t.tv_sec++;
            t.tv_nsec -= 1000000000;
        }
    } else {
        memcpy(&t, request, sizeof(struct timespec));
    }
    pthread_mutex_lock(&mtx);
    s = pthread_cond_timedwait(&cond, &mtx, &t);
    pthread_mutex_unlock(&mtx);
    
    if (s == ETIMEDOUT) {
        s = 0;
    } else {
        err_fatal("clock_nanosleep", errno);
    }
#endif
    return s;
}

int
Close(int fd)
{
    int s;
    s = close(fd);
    if (s < 0) {
        err_warn("close", errno);
    }
    return s;
}

int
Dup(int fd)
{
    int s;
    s = dup(fd);
    if (s < 0) {
        err_warn("dup", errno);
    }
    return s;
}

#ifdef LINUX
int
Epoll_create(int size)
{
    int s;
    s = epoll_create(size);
    if (s < 0) {
        err_warn("epoll_create", errno);
    }
    
    return s;
}

int
Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    int s;
    s = epoll_ctl(epfd, op, fd, event);
    if (s < 0) {
        err_warn("epoll_ctl", errno);
    }
    
    return s;
}

int
Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    int s;
    s = epoll_wait(epfd, events, maxevents, timeout);
    if (s < 0) {
        err_warn("epoll_wait", errno);
    }
    
    return s;
}
#endif
/*
#ifdef MACOSX
int
Epoll_create(int size)
{
    int s;
    s = kqueue();
    if (s < 0) {
        err_log("epoll_create", errno);
    }
}

int
Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    
}
#endif
*/

int
Fcntl(int fd, int cmd, ...)
{
    int s;
    va_list ap;
    long arg;
    struct flock *fl;
    
    va_start(ap, cmd);
    switch (cmd) {
        case F_DUPFD:
        case F_SETFD:
        case F_SETFL:
        case F_SETOWN:
            arg = va_arg(ap, long);
            s = fcntl(fd, cmd, arg);
            break;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
            fl = va_arg(ap, struct flock *);
            s = fcntl(fd, cmd, fl);
            break;
        default:
            s = fcntl(fd, cmd);
    }
    va_end(ap);
    
    if (s < 0) {
        err_warn("fcntl", errno);
    }
    return s;
}

pid_t
Fork(void)
{
    pid_t s;
    s = fork();
    if (s < 0) {
        err_warn("fork", errno);
    }
    return s;
}

int
Ftruncate(int fd, off_t length)
{
    int s;
    s = ftruncate(fd, length);
    if (s < 0) {
        err_warn("ftruncate", errno);
    }
    return s;
}

int
Getrlimit(int resource, struct rlimit *rl)
{
    int s;
    s = getrlimit(resource, rl);
    if (s < 0) {
        err_warn("getrlimit", errno);
    }
    return s;
}

int
Kill(pid_t pid, int sig)
{
    int s;
    s = kill(pid, sig);
    if (s < 0 && errno != ESRCH) {
        err_warn("kill", errno);
    }
    return s;
}

int
Listen(int sockfd, int backlog)
{
    int s;
    s = listen(sockfd, backlog);
    if (s < 0) {
        err_warn("listen", errno);
    }
    return s;
}

void *
Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    void *s;
    s = mmap(addr, len, prot, flags, fd, offset);
    if (s == MAP_FAILED) {
        err_fatal("mmap", errno);
    }
    return s;
}

int
Munmap(void *addr, size_t len)
{
    int s;
    s = munmap(addr, len);
    if (s < 0) {
        err_fatal("munmap", errno);
    }
    return s;
}

int
Open(const char *pathname, int flags, ...)
{
    int s;
    
    va_list ap;
    
    va_start(ap, flags);
    if (flags & O_CREAT) {
        int mode;
        mode = va_arg(ap, int);
        s = open(pathname, flags, mode);
    } else {
        s = open(pathname, flags);
    }
    va_end(ap);
    if (s < 0) {
        err_warn("open", errno);
    }
    return s;
}

DIR *
Opendir(const char *filename)
{
    DIR *s;
    s = opendir(filename);
    if (s < 0) {
        err_warn("opendir", errno);
    }
    return s;
}

ssize_t
Read(int fd, void *buf, size_t n)
{
    ssize_t s;
    s = read(fd, buf, n);
    if (s < 0) {
        err_warn("read", errno);
    }
    return s;
}

struct dirent *
Readdir(DIR *dirp)
{
    struct dirent *s;
    errno = 0;
    s = readdir(dirp);
    if (s == NULL && errno != 0) {
        err_warn("readdir", errno);
    }
    return s;
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    int s;
    struct timeval save_timeout;
    
    if (timeout) {
        memcpy(&save_timeout, timeout, sizeof(struct timeval));
    }
    
    s = select(nfds, readfds, writefds, errorfds, timeout);
    if (s < 0) {
        err_warn("select", errno);
    }
    
    if (timeout) {
        memcpy(timeout, &save_timeout, sizeof(struct timeval));
    }
    
    return s;
}

int
Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    int s;
    s = setsockopt(sockfd, level, optname, optval, optlen);
    if (s < 0) {
        err_warn("setsockopt", errno);
    }
    return s;
}

int
Stat(const char *pathname, struct stat *buf)
{
    int s;
    s = stat(pathname, buf);
    if (s < 0) {
        err_warn("stat", errno);
    }
    return s;
}

int
Unlink(const char *path)
{
    int s;
    s = unlink(path);
    if (s < 0) {
        err_warn("unlink", errno);
    }
    return s;
}

ssize_t
Write(int fd, const void *buf, size_t n)
{
    ssize_t s;
    s = write(fd, buf, n);
    if (s < 0) {
        err_warn("write", errno);
    }
    return s;
}

ssize_t
Writev(int fd, struct iovec *iov, int iovcnt)
{
    ssize_t s;
    s = writev(fd, iov, iovcnt);
    if (s < 0) {
        err_warn("writev", errno);
    }
    return s;
}

/*
 * POSIX thread function wrappers.
 * Returning 0 indicates success, returning a positive errno integer indicates failure.
 */

int
Pthread_cond_broadcast(pthread_cond_t *cond)
{
    int s;
    s = pthread_cond_broadcast(cond);
    if (s != 0) {
        err_warn("pthread_cond_broadcast", s);
    }
    return s;
}

int
Pthread_cancel(pthread_t tid)
{
    int s;
    s = pthread_cancel(tid);
    if (s != 0) {
        err_warn("pthread_cancel", s);
    }
    return s;
}

int
Pthread_cond_destroy(pthread_cond_t *cond) {
    int s;
    s = pthread_cond_destroy(cond);
    if (s != 0) {
        err_warn("pthread_cond_destroy", s);
    }
    return s;
}

int
Pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
    int s;
    s = pthread_cond_init(cond, attr);
    if (s != 0) {
        err_warn("pthread_cond_init", s);
    }
    return s;
}

int
Pthread_cond_signal(pthread_cond_t *cond)
{
    int s;
    s = pthread_cond_signal(cond);
    if (s != 0) {
        err_warn("pthread_cond_singal", s);
    }
    return s;
}

int
Pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *tp)
{
    int s;
    s = pthread_cond_timedwait(cond, mutex, tp);
    if (s != 0) {
        err_warn("pthread_cond_wait", s);
    }
    return s;
}

int
Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    int s;
    s = pthread_cond_wait(cond, mutex);
    if (s != 0) {
        err_warn("pthread_cond_wait", s);
    }
    return s;
}

int
Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    int s;
    s = pthread_create(thread, attr, start_routine, arg);
    if (s != 0) {
        err_warn("pthread_create", s);
    }
    return s;
}

int
Pthread_create2(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), ...)
{
    int s;
    
    va_list ap;
    va_start(ap, start_routine);
    bool *flag;
#ifdef va_copy
    va_list ap2;
    va_copy(ap2, ap);
    flag = va_arg(ap, bool *);
    __atomic_store_n(flag, true, __ATOMIC_SEQ_CST);
    s = pthread_create(thread, attr, start_routine, &ap2);
    if (s == 0) {
        while (__atomic_test_and_set(flag, __ATOMIC_SEQ_CST)) {
        }
    } else {
        __atomic_clear(flag, __ATOMIC_SEQ_CST);
        err_warn("pthread_create", s);
    }
    va_end(ap2);
#else
    va_list *app;
    app = &ap;
    flag = va_arg(ap, bool *);
    __atomic_store_n(flag, true, __ATOMIC_SEQ_CST);
    s = pthread_create(thread, attr, start_routine, app);
    s = pthread_create(thread, attr, start_routine, &ap2);
    if (s == 0) {
        while (__atomic_test_and_set(flag, __ATOMIC_SEQ_CST)) {
        }
    } else {
        __atomic_clear(flag, __ATOMIC_SEQ_CST);
        err_warn("pthread_create", s);
    }
#endif
    va_end(ap);

    return s;
}

int
Pthread_detach(pthread_t thread)
{
    int s;
    s = pthread_detach(thread);
    if (s != 0) {
        err_warn("pthread_detach", s);
    }
    return s;
}

int
Pthread_join(pthread_t thread, void **value_ptr)
{
    int s;
    s = pthread_join(thread, value_ptr);
    if (s != 0) {
        err_warn("pthread_join", s);
    }
    return s;
}

int
Pthread_mutex_destroy(pthread_mutex_t *mutex) {
    int s;
    s = pthread_mutex_destroy(mutex);
    if (s != 0) {
        err_warn("pthread_mutex_destroy", s);
    }
    return s;
}


int
Pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
    int s;
    s = pthread_mutex_init(mutex, attr);
    if (s != 0) {
        err_warn("pthread_mutex_init", s);
    }
    return s;
}

int
Pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int s;
    s = pthread_mutex_lock(mutex);
    if (s != 0) {
        err_warn("pthread_mutex_lock", s);
    }
    return s;
}

int
Pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    int s;
    s = pthread_mutex_trylock(mutex);
    if (s == EINVAL) {
        err_warn("pthread_mutex_trylock", errno);
    }
    return s;
}

#ifdef LINUX
int
Pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
{
    int s;
    s = pthread_mutex_timedlock(mutex, abstime);
    if (s == EINVAL || EDEADLK || ENOTRECOVERABLE || EOWNERDEAD) {
        err_warn("pthread_mutex_trylock", errno);
    }
    return s;
}
#endif

int
Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    int s;
    s = pthread_mutex_unlock(mutex);
    if (s != 0) {
        err_warn("pthread_mutex_unlock", s);
    }
    return s;
}

int
Pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    int s;
    s = pthread_once(once_control, init_routine);
    if (s != 0) {
        err_warn("pthread_once", s);
    }
    return s;
}

int
Pthread_setcancelstate(int state, int *oldstate)
{
    int s;
    s = pthread_setcancelstate(state, oldstate);
    if (s != 0) {
        err_warn("pthread_setcancelstate", s);
    }
    return s;
}

int
Pthread_sigmask(int how, const sigset_t *set, sigset_t *oset)
{
    int s;
    s = pthread_sigmask(how, set, oset);
    if (s != 0) {
        err_warn("pthread_once", s);
    }
    return s;
}

/*
 * User library functions.
 */

int
get_uid_by_name(const char *username, uid_t *uid)
{
    struct passwd pwd, *res;
    char *buf;
    size_t bufsize;
    int ret;
    
    if (uid == NULL) {
        return -1;
    }
    
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) {
        bufsize = 16384;
    }
    
    buf = (char *) Malloc(bufsize);
    
    ret = getpwnam_r(username, &pwd, buf, bufsize, &res);
    
    if (res == NULL) {
        ret = -1;
    } else {
        ret = 0;
        *uid = pwd.pw_uid;
    }
    
    free(buf);
    return ret;
}

int
Getidbyname(const char *username, uid_t *uid, gid_t *gid)
{
    struct passwd pwd, *res;
    char *buf;
    size_t bufsize;
    
    if (uid == NULL || gid == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) {
        bufsize = 16384;
    }
    
    if (!(buf = (char *) Malloc(bufsize))) {
        return -1;
    }
    
    if (getpwnam_r(username, &pwd, buf, bufsize, &res) < 0) {
        free(buf);
        return -1;
    }
    
    memcpy(uid, &pwd.pw_uid, sizeof(uid_t));
    memcpy(gid, &pwd.pw_gid, sizeof(gid_t));
    
    free(buf);
    
    return 0;
}

void
mkdirp(const char *dir)
{
    char *dir_tmp = (char *) Malloc(FILENAMESIZE);
    const char *idx = dir, *idx2;
    size_t n;
    struct stat buf;
    
    do {
        if (idx[0] == '/') {
            idx++;
            continue;
        }
        idx2 = strchr(idx, '/');
        if (idx2 != NULL) {
            n = min(idx2 - dir + 1, FILENAMESIZE);
            snprintf(dir_tmp, n, "%s", dir);
            if (access(dir_tmp, F_OK) < 0)
                mkdir(dir_tmp, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            else {
                stat(dir_tmp, &buf);
                if (S_ISREG(buf.st_mode))
                    break;
            }
            if (idx2[1] == '\0')
                break;
            idx = idx2 + 1;
        } else {
            if (access(dir, F_OK) < 0)
                mkdir(dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            break;
        }
        
    } while (1);
    
    free(dir_tmp);
}

int
Nanosleep(double seconds)
{
    if (seconds < 0) {
        return EINVAL;
    }
    struct timespec rqt;
    rqt.tv_sec = floor(seconds);
    rqt.tv_nsec = (seconds - rqt.tv_sec) * 1000000000.;
#ifdef LINUX
    return clock_nanosleep(CLOCK_REALTIME, 0, &rqt, NULL);
#else
    return nanosleep(&rqt, NULL);
    
#endif
}

ssize_t
Readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;
    
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
            } else {
                return -1;
            }
        } else if (nread == 0) {
            break;
        }
        
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}

ssize_t
Readn2(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;
    
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            return -1;
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}

static int
tcp_connect(const char *hostname, const char *servname, SA *sockaddr, socklen_t *addrlen)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd;
    int ret;
    
    memset(&hint, '\0', sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    
    if ((ret = getaddrinfo(hostname, servname, &hint, &ailist)) != 0) {
        fprintf(stderr, "%s", gai_strerror(ret));
        err_warn("getaddrinfo", 0);
        return -1;
    }
    
    aip = ailist;
    do {
        sockfd = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
        if (sockfd < 0)
            continue;
        if (connect(sockfd, aip->ai_addr, aip->ai_addrlen) == 0) {
            if (addrlen != NULL)
                *addrlen = aip->ai_addrlen;
            if (sockaddr != NULL)
                memcpy(sockaddr, aip->ai_addr, aip->ai_addrlen);
            break;
        }
        close(sockfd);
    } while ((aip = aip->ai_next) != NULL);
    
    if (aip == NULL) {
        sockfd = -1;
    }
    
    freeaddrinfo(ailist);
    
    return sockfd;
}


int
Tcp_connect(const char *hostname, const char *servname, SA *sockaddr, socklen_t *addrlen)
{
    int s = tcp_connect(hostname, servname, sockaddr, addrlen);
    if (s < 0) {
        err_warn("tcp_connect", errno);
        if (errno == ETIMEDOUT || errno == EHOSTUNREACH) {
            int i;
            for (i = 0; i < MAX_RECONNECT_TIMES; i++) {
                sleep(SLEEP_TIME);
                s = tcp_connect(hostname, servname, sockaddr, addrlen);
                if (s > 0) {
                    break;
                } else {
                    err_warn("tcp_connect retry", errno);
                }
            }
        }
    }
    return s;
}

static int
tcp_listen(const char *hostname, const char *servname, SA *sockaddr, socklen_t *addrlen)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    const int optval = 1;
    int lfd, ret;
    
    memset(&hint, '\0', sizeof(struct addrinfo));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    
    if ((ret = getaddrinfo(hostname, servname, &hint, &ailist)) != 0) {
        err_warn("getaddrinfo", 0);
        return -1;
    }
    
    aip = ailist;
    do {
        lfd = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
        if (lfd < 0)
            continue;
        Setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if (bind(lfd, aip->ai_addr, aip->ai_addrlen) == 0) {
            if (addrlen != NULL) {
                *addrlen = aip->ai_addrlen;
            }
            if (sockaddr != NULL && addrlen != NULL)
                memcpy(sockaddr, aip->ai_addr, *addrlen);
            break;
        }
        Close(lfd);
    } while ((aip = aip->ai_next) != NULL);
    
    Listen(lfd, BACKLOG);
    freeaddrinfo(ailist);
    return lfd;
}

int
Tcp_listen(const char *hostname, const char *servname, SA *sockaddr, socklen_t *addrlen)
{
    int s;
    s = tcp_listen(hostname, servname, sockaddr, addrlen);
    if (s < 0) {
        err_warn("tcp_listen", errno);
    }
    return s;
}

ssize_t
Writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    
    ptr = vptr;
    
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

ssize_t
Writen2(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    
    ptr = vptr;
    
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

ssize_t
Sendfile(int fd, int sockfd, off_t offset, off_t *len, struct sf_hdtr *hdtr, int flags)
{
    ssize_t s = 0;
#ifdef LINUX
    int optval = 1;
    Setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval));
    optval = 0;
    if (hdtr != NULL && hdtr->headers != NULL) {
        Writev(sockfd, hdtr->headers, hdtr->hdr_cnt);
    }
    s = sendfile(sockfd, fd, &offset, *len);
    if (hdtr != NULL && hdtr->trailers != NULL) {
        Writev(sockfd, hdtr->trailers, hdtr->trl_cnt);
    }
    Setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval));
#else
    s = sendfile(fd, sockfd, offset, len, hdtr, flags);
    if (s < 0) {
        err_warn("sendfile", errno);
    } else {
        s = (size_t) *len;
    }
#endif
    
    return s;
}
