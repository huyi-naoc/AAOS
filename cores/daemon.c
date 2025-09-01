//
//  daemon.c
//  AAOS
//
//  Created by Hu Yi on 2019/6/28.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "daemon.h"
#include "daemon_r.h"
#include "wrapper.h"

void
daemon_set(void *_self, unsigned int field, ...)
{
    const struct DaemonClass *class = (const struct DaemonClass*) classOf(_self);
    va_list ap;
    va_start(ap, field);
    
    if (isOf(class, DaemonClass()) && class->set.method) {
        ((void (*)(void *, unsigned int, va_list)) class->set.method)( _self, field, ap);
    } else {
        forward(_self, 0, (Method) daemon_set, "set", _self, field, &ap);
    }
    va_end(ap);
}

static void
Daemon_set(void *_self, unsigned int field, va_list ap)
{
    struct Daemon *self = cast(Daemon(), _self);
    const char *s;
    
    switch (field) {
        case DAEMON_DAEMONIZE_FIELD:
        {
            int is_daemonzied;
            is_daemonzied = va_arg(ap, int);
            if (is_daemonzied) {
                self->is_daemonized = true;
            } else {
                self->is_daemonized = false;
            }
        }
            break;
        case DAEMON_USERNAME_FIELD:
            s = va_arg(ap, const char *);
            self->username = Realloc(self->username, strlen(s) + 1);
            snprintf(self->username, strlen(s) + 1, "%s", s);
            break;
        case DAEMON_NAME_FIELD:
            s = va_arg(ap, const char *);
            self->daemon_name = Realloc(self->daemon_name, strlen(s) + 1);
            snprintf(self->daemon_name, strlen(s) + 1, "%s", s);
            break;
        case DAEMON_ROOTDIR_FIELD:
            s = va_arg(ap, const char *);
            self->root_directory = Realloc(self->root_directory, strlen(s) + 1);
            snprintf(self->root_directory, strlen(s) + 1, "%s", s);
            break;
        case DAEMON_LOCKFILE_FIELD:
            s = va_arg(ap, const char *);
            self->lock_file = Realloc(self->lock_file, strlen(s) + 1);
            snprintf(self->lock_file, strlen(s) + 1, "%s", s);
            break;
            
        default:
            break;
    }
    
}


void
daemon_start(const void *_self)
{
    const struct DaemonClass *class = (const struct DaemonClass *) classOf(_self);
    
    if (isOf(class, DaemonClass()) && class->start.method) {
        return ((void (*)(const void *)) class->start.method)(_self);
    } else {
        return forward(_self, 0, (Method) daemon_start, "start", _self);
    }
}

static int
lockfile(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return (Fcntl(fd, F_SETLK, &fl));
}

static int
already_running(const void *_self)
{
    const struct Daemon *self = cast(Daemon(), _self);
    int fd;
    pid_t pid;
    fd = Open(self->lock_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        syslog(LOG_ERR, "Cannot open or create %s.\n", self->lock_file);
        exit( AAOS_EACCES);
    }
    
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            Close(fd);
            return 1;
        } else {
            syslog(LOG_ERR, "%s %d: lockfile error.\n", __FILE__, __LINE__);
            exit(1);
        }
    }
    
    Ftruncate(fd, 0);
    pid = getpid();
    Write(fd, &pid, sizeof(pid_t));
    return 0;
}


static void
Daemon_start(const void *_self)
{
    const struct Daemon *self = cast(Daemon(), _self);
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    uid_t uid, current_uid;
    gid_t gid, current_gid;
    
    umask(0);
    
    Chdir(self->root_directory);
    
    if (self->is_daemonized) {
        if (already_running(self)) {
            fprintf(stderr, "%s is already running.\n", self->daemon_name);
            exit(AAOS_EALREADY);
        }
    } else {
        fprintf(stderr, "%s is running in a non-daemon mode.\n", self->daemon_name);
        return;
    }
    
    Getidbyname(self->username, &uid, &gid);
    current_uid = getuid();
    current_gid = getgid();
    
    if (uid != current_uid && setuid(uid) < 0) {
        fprintf(stderr, "Cannot run %s as %s, permission denied.\n", self->daemon_name, self->username);
        exit(AAOS_EPERM);
    }
    
    if (gid != current_gid && setgid(gid) < 0) {
        fprintf(stderr, "Cannot run %s as %s, permission denied.\n", self->daemon_name, self->username);
        exit(AAOS_EPERM);
    }
    
    pid = Fork();
    switch (pid) {
        case 0:
            break;
        default:
            _exit(0);
    }
    setsid();
    
    pid = Fork();
    switch (pid) {
        case 0:
            break;
        default:
            _exit(0);
    }
    
    Getrlimit(RLIMIT_NOFILE, &rl);
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (i = 0; i < rl.rlim_max; i++) {
        Close(i);
    }
    fd0 = Open("/dev/null", O_RDWR);
    fd1 = Dup(0);
    fd2 = Dup(0);
    
    openlog(self->daemon_name, LOG_CONS, LOG_DAEMON);
    
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        _exit(1);
    }
    
    if (already_running(self)) {
        syslog(LOG_ERR, "%s is already running.\n", self->daemon_name);
        _exit(AAOS_EALREADY);
    }
}

void
daemon_stop(const void *_self)
{
    const struct DaemonClass *class = (const struct DaemonClass *) classOf(_self);
    
    if (isOf(class, DaemonClass()) && class->stop.method) {
        return ((void (*)(const void *)) class->stop.method)(_self);
    } else {
        return forward(_self, 0, (Method) daemon_stop, "stop", _self);
    }
}

static void
Daemon_stop(const void *_self)
{
    const struct Daemon *self = cast(Daemon(), _self);
    pid_t pid;
    int fd, ret;
    
    fd = Open(self->lock_file, O_RDONLY);
    if (errno == ENOENT && fd < 0) {
        fprintf(stderr, "Lock file \"%s\" does not exist.\n", self->lock_file);
        exit(AAOS_ENOENT);
    } else if (errno == EACCES && fd < 0) {
        fprintf(stderr, "Has not permission to open lock file \"%s\".\n", self->lock_file);
        exit(AAOS_EACCES);
    }
    
    Read(fd, &pid, sizeof(pid_t));
    
    ret = Kill(pid, SIGTERM);
    if (ret < 0) {
        switch (errno) {
            case ESRCH:
                fprintf(stderr, "Daemon process %s does not running.\n", self->daemon_name);
                exit(AAOS_ESRCH);
                break;
            case EPERM:
                fprintf(stderr, "Has not permission to stop daemon process %s.\n", self->daemon_name);
                exit(AAOS_EPERM);
                break;
            default:
                break;
        }
    } else {
        fprintf(stderr, "%s is stopped.\n", self->daemon_name);
    }
}

void
daemon_restart(const void *_self)
{
    const struct DaemonClass *class = (const struct DaemonClass *) classOf(_self);
    
    if (isOf(class, DaemonClass()) && class->restart.method) {
        return ((void (*)(const void *)) class->restart.method)(_self);
    } else {
        return forward(_self, 0, (Method) daemon_restart, "restart", _self);
    }
}

static void
Daemon_restart(const void *_self)
{
    const struct Daemon *self = cast(Daemon(), _self);
    fprintf(stderr, "Restart %s ...\n", self->daemon_name);
    daemon_reload(self);
}

void
daemon_reload(const void *_self)
{
    const struct DaemonClass *class = (const struct DaemonClass *) classOf(_self);
    
    if (isOf(class, DaemonClass()) && class->reload.method) {
        return ((void (*)(const void *)) class->reload.method)(_self);
    } else {
        return forward(_self, 0, (Method) daemon_reload, "restart", _self);
    }
}

static void
Daemon_reload(const void *_self)
{
    const struct Daemon *self = cast(Daemon(), _self);
    pid_t pid;
    int fd, ret;
    
    fd = Open(self->lock_file, O_RDONLY);
    if (errno == ENOENT && fd < 0) {
        fprintf(stderr, "Lock file \"%s\" does not exist.\n", self->lock_file);
        exit(AAOS_ENOENT);
    } else if (errno == EACCES && fd < 0) {
        fprintf(stderr, "Has not permission to open lock file \"%s\".\n", self->lock_file);
        exit(AAOS_EACCES);
    }
    
    Read(fd, &pid, sizeof(pid_t));
    fprintf(stderr, "Running daemon process %s's PID is: %d\n", self->daemon_name, pid);
    
    ret = Kill(pid, SIGHUP);
    if (ret < 0) {
        switch (errno) {
            case ESRCH:
                fprintf(stderr, "Daemon process %s does not running.\n", self->daemon_name);
                exit(AAOS_ESRCH);
                break;
            case EPERM:
                fprintf(stderr, "Permission debied when reload daemon process %s.\n", self->daemon_name);
                exit(AAOS_EPERM);
                break;
            default:
                break;
        }
    }
}

/*
 * new(Daemon(), name, rootdir, lockfile, username, daemonized)
 */

static void *
Daemon_ctor(void *_self, va_list *app)
{
    struct Daemon *self = super_ctor(Daemon(), _self, app);
    const char *s;
    int is_daemonized;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->daemon_name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->daemon_name, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->root_directory = (char *) Malloc(strlen(s) + 1);
        snprintf(self->root_directory, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->lock_file = (char *) Malloc(strlen(s) + 1);
        snprintf(self->lock_file, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->username = (char *) Malloc(strlen(s) + 1);
        snprintf(self->username, strlen(s) + 1, "%s", s);
    }
    is_daemonized = va_arg(*app, int);
    if (is_daemonized) {
        self->is_daemonized = true;
    } else {
        self->is_daemonized = false;
    }
    
    return (void *) self;
}

static void *
Daemon_dtor(void *_self)
{
    struct Daemon *self = super_dtor(Daemon(), _self);
    
    free(self->daemon_name);
    free(self->root_directory);
    free(self->lock_file);
    free(self->username);
    
    return (void *) self;
}

static int
Daemon_puto(const void *_self, FILE *fp)
{
    const struct Daemon *self = cast(Daemon(), _self);
    
    if (self->daemon_name) {
        fprintf(fp, "Daemon  \t: %s\n", self->daemon_name);
    } else {
        fprintf(fp, "Daemon  \t: NULL\n");
    }
    if (self->root_directory) {
        fprintf(fp, "Root directory\t: %s\n", self->root_directory);
    } else {
        fprintf(fp, "Root directory\t: NULL\n");
    }
    if (self->lock_file) {
        fprintf(fp, "PID file\t: %s\n", self->root_directory);
    } else {
        fprintf(fp, "PID file\t: NULL\n");
    }
    if (self->username) {
        fprintf(fp, "User name\t: %s\n", self->root_directory);
    } else {
        fprintf(fp, "User name\t: NULL\n");
    }
    if (self->is_daemonized) {
        fprintf(fp, "Daemonized\t: true\n");
    } else {
        fprintf(fp, "Daemonized\t: false\n");
    }
    
    return AAOS_OK;
}


static void *
DaemonClass_ctor(void *_self, va_list *app)
{
    struct DaemonClass *self = super_ctor(DaemonClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) daemon_start) {
            if (tag) {
                self->start.tag = tag;
                self->start.selector = selector;
            }
            self->start.method = method;
            continue;
        }
        if (selector == (Method) daemon_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) daemon_restart) {
            if (tag) {
                self->restart.tag = tag;
                self->restart.selector = selector;
            }
            self->restart.method = method;
            continue;
        }
        if (selector == (Method) daemon_reload) {
            if (tag) {
                self->reload.tag = tag;
                self->reload.selector = selector;
            }
            self->reload.method = method;
            continue;
        }
        if (selector == (Method) daemon_set) {
            if (tag) {
                self->set.tag = tag;
                self->set.selector = selector;
            }
            self->set.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_DaemonClass;

static void
DaemonClass_destroy(void)
{
    free((void *) _DaemonClass);
}

static void
DaemonClass_initialize(void)
{
    _DaemonClass = new(Class(), "DaemonClass", Class(), sizeof(struct DaemonClass),
                       ctor, "ctor", DaemonClass_ctor,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DaemonClass_destroy);
#endif
}

const void *
DaemonClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, DaemonClass_initialize);
#endif
    return _DaemonClass;
}

static const void *_Daemon;

static void
Daemon_destroy(void)
{
    free((void *) _Daemon);
}

static void
Daemon_initialize(void)
{
    _Daemon = new(DaemonClass(), "Daemon", Object(), sizeof(struct Daemon),
                  ctor, "ctor", Daemon_ctor,
                  dtor, "dtor", Daemon_dtor,
                  puto, "puto", Daemon_puto,
                  daemon_start, "start", Daemon_start,
                  daemon_stop, "stop", Daemon_stop,
                  daemon_restart, "restart", Daemon_restart,
                  daemon_reload, "reload", Daemon_reload,
                  daemon_set, "set", Daemon_set,
                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Daemon_destroy);
#endif
}

const void *
Daemon(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Daemon_initialize);
#endif
    
    return _Daemon;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_DAEMON_PRIORITY_)));

static void
__destructor__(void)
{
    Daemon_destroy();
    DaemonClass_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_DAEMON_PRIORITY_)));

static void
__constructor__(void)
{
    DaemonClass_initialize();
    Daemon_initialize();
}
#endif

/*
 * Example
 *
 * gcc main.c object.c daemon.c wrapper.c -pthread -Wall -O2 -o daemon
 *
 * #include <stdio.h>
 * #include <string.h>
 *
 * #include "daemon.h"
 *
 * int
 * main(int argc, char *argv[])
 * {
 *     void *d = new(Daemon(), "mydaemon", "/Users/huyi", "/Users/huyi/mydaemon.pid", "myuser", true);
 *
 *     if (strcmp(argv[1], "start") == 0) {
 *         daemon_start(d);
 *     } else if (strcmp(argv[1], "stop") == 0) {
 *         daemon_stop(d);
 *         delete(d);
 *         return 0;
 *     } else {
 *         delete(d);
 *         return 1;
 *     }
 *
 *     for (; ; ) {
 *
 *     }
 *     delete(d);
 *     return 0;
 * }
 */
