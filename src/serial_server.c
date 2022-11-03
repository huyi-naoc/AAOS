//
//  serial_server_test.c
//  AAOS
//
//  Created by Hu Yi on 2019/5/22.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "daemon.h"
#include "serial.h"
#include "serial_rpc.h"
#include "wrapper.h"
#include <libconfig.h>

extern void **serials;
extern size_t n_serial;
extern void *serial_list;

static void *d;
static void *server;
static const char *config_path = "/usr/local/aaos/etc/seriald.cfg";
static bool daemon_flag = true;
static bool feed_dog_flag = true;
static config_t cfg;

static struct option longopts[] = {
    {"config",      required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static void
read_daemon(void)
{
    config_setting_t *setting = NULL;
    
    setting = config_lookup(&cfg, "daemon");
    
    if (setting != NULL) {
        const char *name = NULL, *username = "aag", *rootdir = "/", *lockfile = NULL;
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
    config_setting_t *setting = NULL, *serial_setting = NULL;
    
    size_t i;
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        const char *port;
        config_setting_lookup_string(setting, "port", &port);
        server = new(SerialServer(), port);
    }
    
    setting = config_lookup(&cfg, "serials");
    if (setting == NULL) {
        fprintf(stderr, "`serials` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_serial = config_setting_length(setting);
        serials = (void **) Malloc(n_serial * sizeof(void *));
        memset(serials, '\0', n_serial * sizeof(void *));
        if (serials == NULL) {
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < n_serial; i++) {
            serial_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name = NULL, *path = NULL, *description = NULL, *type = NULL, *inspect = NULL;
            config_setting_lookup_string(serial_setting, "name", &name);
            config_setting_lookup_string(serial_setting, "path", &path);
            config_setting_lookup_string(serial_setting, "description", &description);
            config_setting_lookup_string(serial_setting, "type", &type);
            if (type == NULL) {
                break;
            }
            if (strcmp(type, "JZD") == 0) {
                serials[i] = new(JZDSerial(), name, path, "description", description, NULL);
            } else if (strcmp(type, "APMOUNT") == 0){
                serials[i] = new(APMountSerial(), name, path, "description", description, NULL);
            } else if (strcmp(type, "SQM") == 0){
                serials[i] = new(SQMSerial(), name, path, "description", description, NULL);
            } else if (strcmp(type, "WS100UMB") == 0){
                serials[i] = new(WS100UMBSerial(), name, path, "description", description, NULL);
            } else if (strcmp(type, "AAGPDU") == 0){
                serials[i] = new(AAGPDUSerial(), name, path, "description", description, NULL);
            } else if (strcmp(type, "RDSS") == 0) {
                serials[i] = new(RDSSSerial(), name, path, "description", description, NULL);
            } else if (strcmp(type, "KLTP") == 0) {
                serials[i] = new(KLTPSerial, name, path, "description", description, NULL);
            } else {
            }
            config_setting_lookup_string(serial_setting, "inspect", &inspect);
            if (inspect != NULL) {
                __serial_set_inspect(serials[i], inspect, strlen(inspect));
            }
        }
    }
}

static void *
feed_dog_thr(void *arg)
{
    Pthread_detach(pthread_self());
    size_t i;
    for (; ;) {
        Nanosleep(500.);
        for (i = 0; i < n_serial; i++) {
            __serial_feed_dog(serials[i]);
        }
    }
    return NULL;
}

static void
serial_list_cleanup(void *serial)
{
    if (serial != NULL) {
        delete(serial);
    }
}

static void
init(void)
{
    read_configuration();
    
    size_t i;
    for (i = 0; i < n_serial; i++) {
        __serial_init(serials[i]);
    }
    
    serial_list = new(ThreadsafeList(), serial_list_cleanup);
    
    if (feed_dog_flag) {
        pthread_t tid;
        Pthread_create(&tid, NULL, feed_dog_thr, NULL);
    }
    
    rpc_server_start(server);
}

static void
destroy(void)
{
    size_t i;
    for (i = 0; i < n_serial; i++) {
        if (serials[i] != NULL) {
            delete(serials[i]);
        }
    }
    free(serials);

    if (serial_list != NULL) {
        delete(serial_list);
    }
    
    if (d != NULL) {
        delete(d);
    }
    
    if (server != NULL) {
        delete(server);
    }
}

static void
usage(void)
{
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int ch, ret;
    
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
    
    config_init(&cfg);

    if ((ret = Access(config_path, F_OK)) < 0) {
        fprintf(stderr, "configuration file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    
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
            exit(EXIT_FAILURE);
        }
    }
    destroy();
    config_destroy(&cfg);
    
    return 0;
}
