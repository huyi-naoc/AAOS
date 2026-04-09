//
//  thread_server.c
//  AAOS
//
//  Created by huyi on 2024/11/17.
//

#include "daemon.h"
#include "thread.h"
#include "thread_rpc.h"
#include "wrapper.h"

#include <libconfig.h>

extern void **threads;
extern size_t n_thread;

static void *d;
static void *server;
static const char *config_path = "/opt/aaos/etc/threadd.cfg";
static bool daemon_flag = true;
static config_t cfg;

static struct option longopts[] = {
    {"config-file", required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static const char *help_string = "\
Usage:  telescoped [options] \n\
        -c, --config-file <path>, configuration file\n\
        -h, --help        print help doc and exit\n\
        -D                do not enter into daemon process mode\n\
";

static void
usage(void)
{
    fprintf(stderr, "%s\n", help_string);
    exit(EXIT_FAILURE);
}

static void
read_daemon(void)
{
    config_setting_t *setting;
    
    setting = config_lookup(&cfg, "daemon");
    if (setting != NULL) {
        const char *name = NULL, *username = "aaos", *rootdir = "/", *lockfile = NULL;
        int daemonized = 1;
        config_setting_lookup_string(setting, "name", &name);
        config_setting_lookup_string(setting, "rootdir", &rootdir);
        config_setting_lookup_string(setting, "lockfile", &lockfile);
        config_setting_lookup_string(setting, "username", &username);
        config_setting_lookup_int(setting, "daemonized", &daemonized);
        if (daemonized && daemon_flag) {
            d = new(Daemon(), name, rootdir, lockfile, username, true);
        } else {
            d = new(Daemon(), name, rootdir, lockfile, username, false);
        }
    }
}


static void
read_configuration(void)
{
    config_setting_t *setting = NULL, *thread_setting = NULL, *scheduler_setting = NULL, *telescope_setting = NULL, *dome_setting = NULL, *detector_setting = NULL, *aws_setting = NULL, *pipeline_setting = NULL;
    int i;

    if ((setting = config_lookup(&cfg, "threads")) == NULL) {
        fprintf(stderr, "`threads` section does not exist in configuration file.\n");   
    }
    n_thread = config_setting_length(setting);
    if ((threads = (void **) Malloc(n_thread * sizeof(void *))) == NULL) {
        exit(EXIT_FAILURE);
    }
    memset(threads, '\0', n_thread * sizeof(void *));

    for (i = 0; i < n_thread; i++) {
        const char *name = NULL, *description = NULL;
        thread_setting = config_setting_get_elem(setting, (unsigned int) i);
        config_setting_lookup_string(thread_setting, "name", &name);
        config_setting_lookup_string(thread_setting, "description", &description);
        threads[i] = new(ObservationThread(), name, "description", description, (void *) 0);
        if ((scheduler_setting = config_setting_lookup(thread_setting, "scheduler_setting")) != NULL) {
            const char *scheduler_address  = NULL, *scheduler_port = NULL;
            config_setting_lookup_string(scheduler_setting, "address", &scheduler_address);
            config_setting_lookup_string(scheduler_setting, "port", &scheduler_port);
            __observation_thread_set_member(threads[i], "scheduler", scheduler_address, scheduler_port);
        }
        if ((dome_setting = config_setting_lookup(thread_setting, "dome")) != NULL) {
            const char *dome_address  = NULL, *dome_port = NULL, *dome_name = NULL;
            config_setting_lookup_string(dome_setting, "address", &dome_address);
            config_setting_lookup_string(dome_setting, "port", &dome_port);
            config_setting_lookup_string(dome_setting, "name", &dome_name);
            __observation_thread_set_member(threads[i], "dome", dome_address, dome_port, dome_name);
        }
        if ((telescope_setting = config_setting_lookup(thread_setting, "telescope")) != NULL) {
            const char *telescope_address  = NULL, *telescope_port = NULL, *telescope_name = NULL;
            long long value = 0;
            uint64_t telescope_identifier;
            config_setting_lookup_string(telescope_setting, "address", &telescope_address);
            config_setting_lookup_string(telescope_setting, "port", &telescope_port);
            config_setting_lookup_string(telescope_setting, "name", &telescope_name);
            config_setting_lookup_int64(telescope_setting, "identifier", &value);
            telescope_identifier = (uint64_t) value;
            __observation_thread_set_member(threads[i], "telescope", telescope_address, telescope_port, telescope_name, telescope_identifier);
        }
        if ((detector_setting = config_setting_lookup(thread_setting, "detetcor")) != NULL) {
            const char *detetcor_address  = NULL, *detector_port = NULL, *detector_name = NULL;
            config_setting_lookup_string(detector_setting, "address", &detetcor_address);
            config_setting_lookup_string(detector_setting, "port", &detector_port);
            config_setting_lookup_string(detector_setting, "name", &detector_name);
            __observation_thread_set_member(threads[i], "detector", detetcor_address, detector_port, detector_name);
        }
        if ((aws_setting = config_setting_lookup(thread_setting, "aws")) != NULL) {
            config_setting_t *keyname_setting, *keyvalue_setting;
            const char *aws_address  = NULL, *aws_port = NULL, *aws_name = NULL, *key = NULL, *value = NULL;
            char **keyname, **keyvalue;
            
            config_setting_lookup_string(detector_setting, "address", &aws_address);
            config_setting_lookup_string(detector_setting, "port", &aws_port);
            config_setting_lookup_string(detector_setting, "name", &aws_name);
            if ((keyname_setting = config_setting_lookup(aws_setting, "keyname")) != NULL && (keyvalue_setting = config_setting_lookup(aws_setting, "keyvalue")) != NULL) {
                int n_keyname = config_setting_length(keyname_setting), n_keyvalue = config_setting_length(keyvalue_setting);
                size_t j, n_keypair = min(n_keyname, n_keyvalue);
                keyname = (char **) Malloc(n_keypair * sizeof(char *));
                keyvalue = (char **) Malloc(n_keypair * sizeof(char *));
                for (j = 0; j < n_keypair; j++) {
                    key = config_setting_get_string_elem(keyname_setting, (int) j);
                    value = config_setting_get_string_elem(keyvalue_setting, (int) j);
                    keyname[i] = (char *) Malloc(strlen(key) + 1);
                    snprintf(keyname[i], strlen(key) + 1, "%s", key);
                    keyvalue[i] = (char *) Malloc(strlen(value) + 1);
                    snprintf(keyvalue[i], strlen(value) + 1, "%s", value);
                }
                __observation_thread_set_member(threads[i], "aws", aws_address, aws_port, aws_name, keyname, keyvalue, n_keypair);
            }
        }
        if ((pipeline_setting = config_setting_lookup(thread_setting, "pipeline")) != NULL) {
            const char *pipeline_address  = NULL, *pipeline_port = NULL;
            config_setting_lookup_string(pipeline_setting, "address", &pipeline_address);
            config_setting_lookup_string(pipeline_setting, "port", &pipeline_port);
             __observation_thread_set_member(threads[i], "pipeline", pipeline_address, pipeline_port);
        }
    }
}

static void
init(void)
{
    read_configuration();
    
    size_t i;
    
    for (i = 0; i < n_thread; i++) {
        if (threads[i] != NULL) {
            __observation_thread_start(threads[i]);
        }
    }
    rpc_server_start(server);
}

static void
destroy(void)
{
    size_t i;
    for (i = 0; i < n_thread; i++) {
        if (threads[i] != NULL) {
            __observation_thread_terminate(threads[i]);
            delete(threads[i]);
        }
    }
    free(threads);

    if (d != NULL) {
        delete(d);
    }
    
    if (server != NULL) {
        delete(server);
    }
}

/*
static void
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
*/

int
main(int argc, char *argv[])
{
    int ch, ret;
    //char config[PATHSIZE];
    
    while ((ch = getopt_long(argc, argv, "c:Dhv", longopts, NULL)) != -1) {
        switch (ch) {
            case 'c':
                config_path = optarg;
                break;
            case 'D':
                daemon_flag = false;
                break;
            case 'h':
                usage();
                break;
            case 'v':
                
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    if ((ret = Access(config_path, F_OK)) < 0) {
        if ((ret = Access("threadd.cfg", F_OK)) == 0) {
            config_path = "threadd.cfg";
        } else if ((ret = Access("etc/threadd.cfg", F_OK)) == 0) {
            config_path = "etc/threadd.cfg";
        } else if ((ret = Access("/opt/aaos/etc/threadd.cfg", F_OK)) == 0) {
            config_path = "/opt/aaos/etc/threadd.cfg";
        } else if ((ret = Access("/usr/local/aaos/etc/threadd.cfg", F_OK)) == 0) {
            config_path = "/usr/local/aaos/etc/threadd.cfg";
        } else if ((ret = Access("/etc/aaos/threadd.cfg", F_OK)) == 0) {
            config_path = "/etc/aaos/threadd.cfg";
        } else if ((ret = Access("/etc/threadd.cfg", F_OK)) == 0) {
            config_path = "/etc/threadd.cfg";
        } else {
            fprintf(stderr, "configuration file does not exist.\n");
            exit(EXIT_FAILURE);
        }
    }
    
    config_init(&cfg);

    if(config_read_file(&cfg, config_path) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        exit(EXIT_FAILURE);
    }
    
    read_daemon();
    
    if (argc == 0) {
        daemon_start(d);
        init();
    } else {
        if (strcmp(argv[0], "start") == 0) {
            daemon_start(d);
            init();
        } else if (strcmp(argv[0], "restart") == 0) {
            daemon_stop(d);
            daemon_start(d);
            init();
        } else if (strcmp(argv[0], "reload") == 0) {
            daemon_stop(d);
            daemon_reload(d);
            init();
        } else if (strcmp(argv[0], "stop") == 0) {
            daemon_stop(d);
        } else {
            fprintf(stderr, "Unknow argument.\n");
            fprintf(stderr, "Exit...");
            destroy();
            config_destroy(&cfg);
            exit(EXIT_FAILURE);
        }
    }
    destroy();
    config_destroy(&cfg);

    return 0;
}
