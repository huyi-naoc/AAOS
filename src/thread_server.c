//
//  thread_server.c
//  AAOS
//
//  Created by huyi on 2024/11/17.
//

#include "thread.h"
#include "wrapper.h"

#ifdef LINUX
#include <sys/epoll.h>
#endif

#ifdef MACOSX
#include <sys/event.h>
#endif

static void 
thread_manage(void **threads, size_t n)
{
    void *scheduler, *thread;
    size_t i;
#ifdef LINUX
    int efd, sockfd, n_events;
    struct epoll_event ev, *events;

    if ((events = (struct epoll_event *) Malloc(sizeof(struct epoll_event) * n)) == NULL) {

    }
    if ((efd = epoll_create(1)) < 0) {

    }
    for (i = 0; i < n; i++) {
        scheduler = __observation_thread_get_member(threads[i], "scheduler");
        if (scheduler != NULL) {
            sockfd = tcp_socket_get_fd(scheduler);
        }
        ev.events = EPOLLIN;
        ev.data.ptr = threads[i];
        if ((epoll_ctl(efd, EPOLL_CTL_ADD, sockfd, &ev)) < 0) {

        }
    }
    for (; ;) {
        n_events = epoll_wait(efd, events, n, 100000000);
        if (n_events > 0) {
            for (i = 0; i < n_events; i++) {
                thread = events[i].data.ptr;
                if (thread != NULL) {
                    scheduler = __observation_thread_get_member(threads, "scheduler");
                    if (scheduler != NULL) {
                        if ((ret = rpc_process(scheduler)) != AAOS_OK) {
                            delete(scheduler);
                            scheduler = NULL;
                        }
                    }
                }
            }
        } else if (n_events == 0) {

        } else {

        }
    }
    free(events);
#endif
#ifdef MACOSX
    int kq, sockfd, n_events;
    struct kevent *changelist, *eventlist;
    struct timespec tp;
    size_t j;

    tp.tv_sec = 1000000;
    tp.tv_nsec = 0;

    if ((kq = kqueue()) < 0) {

    }

    if ((changelist = (struct kevent *) Malloc(sizeof(struct kevent) * n * 2)) == NULL) {

    }

    if ((eventlist = (struct kevent *) Malloc(sizeof(struct kevent) * n)) == NULL) {

    }

    for (i = 0; i < n; i++) {
        scheduler = __observation_thread_get_member(threads[i], "scheduler");
        if (scheduler != NULL) {
            sockfd = tcp_socket_get_fd(scheduler);
            EV_SET(changelist + i, sockfd, EVFILT_READ, EV_ADD, 0, 0, threads[i]);
        }
    }

    for (; ;) {
        /*
         * EV_ADD. 
         */
        for (i = 0; i < n; i++) {
            scheduler = __observation_thread_get_member(threads[i], "scheduler");
            if (scheduler == NULL) {
                __observation_thread_set_member(threads[i], NULL, NULL);
                scheduler = __observation_thread_get_member(threads[i], "scheduler");
                if (scheduler != NULL) {
                    sockfd = tcp_socket_get_fd(scheduler);
                    EV_SET(changelist + i, sockfd, EVFILT_READ, EV_ADD, 0, 0, threads[i]);
                }
            }
        }
        n_events = kevent(kq, changelist, n, eventlist, n, &tp);
        if (n_events > 0) {
            for (i = 0; i < n_events; i++) {
                thread = eventlist[i].udata;
                if (thread != NULL) {
                    scheduler = __observation_thread_get_member(threads, "scheduler");
                    if (scheduler != NULL) {
                        if ((ret = rpc_process(scheduler)) != AAOS_OK) {
                            for (j = 0; j < n; j++) {
                                if (thread == changelist[j].udata) {
                                    sockfd = tcp_socket_get_fd(scheduler);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } else if (n_events == 0) {

        } else {

        }
    }

    free(eventlist);

#endif
}

int
main(int argc, char *argv[])
{
    return 0;
}