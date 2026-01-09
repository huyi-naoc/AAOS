//
//  dome_server.c
//  AAOS
//
//  Created by huyi on 2025/5/11.
//

#include "adt.h"
#include "def.h"
#include "daemon.h"
#include "rpc.h"
#include "dome_def.h"
#include "dome.h"
#include "dome_rpc.h"
#include "wrapper.h"

#include <libconfig.h>


void **domes;
size_t n_dome;
static void *d;
static void *server;
static const char *config_path = "/opt/aaos/etc/domed.cfg";
static bool daemon_flag = true;
static config_t cfg;

static struct option longopts[] = {
    {"config", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
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
            d = new (Daemon(), name, rootdir, lockfile, username, true);
        } else {
            d = new (Daemon(), name, rootdir, lockfile, username, false);
        }
    }
}

static void
read_configuration(void)
{
    config_setting_t *setting = NULL, *dome_setting = NULL;
    size_t i;
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        fprintf(stderr, "use default detector RPC port.\n");
        server = new (DomeServer(), DOM_RPC_PORT);
    } else {
        const char *port = NULL, *path = NULL;
        config_setting_lookup_string(setting, "port", &port);
        config_setting_lookup_string(setting, "path", &path);
        if (port == NULL) {
            server = new(DomeServer(), DOM_RPC_PORT);
        } else {
            server = new(DomeServer(), port);
        }
        if (path != NULL) {
            tcp_server_set_path(server, path);
        }
    }
    
    setting = config_lookup(&cfg, "domes");
    if (setting == NULL) {
        fprintf(stderr, "`domes` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_dome = config_setting_length(setting);
        domes = (void **) Malloc(n_dome * sizeof(void *));
        if (domes == NULL) {
            exit(EXIT_FAILURE);
        }
        memset(domes, '\0', n_dome * sizeof(void *));
        for (i = 0; i < n_dome; i++) {
            dome_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name = NULL, *description = NULL, *type = NULL;
            double window_open_speed = 1./60., window_close_speed = 1./60.;
            
            config_setting_lookup_string(dome_setting, "name", &name);
            config_setting_lookup_string(dome_setting, "type", &type);
            config_setting_lookup_string(dome_setting, "description", &description);
            config_setting_lookup_float(dome_setting, "window_open_speed", &window_open_speed);
            config_setting_lookup_float(dome_setting, "close_speed", &window_close_speed);
            if (type == NULL) {
                domes[i] = NULL;
                continue;
            }
            if (strcmp(type, "VIRTUAL") == 0) {
                domes[i] = new(VirtualDome(), name, "description", description, "window_open_speed", window_open_speed, "window_close_speed", window_close_speed, '\0');
            } else {
                
            }
        }
    }
    
}

static void
init(void)
{
    read_configuration();
    size_t i;
    for (i = 0; i < n_dome; i++) {
        if (domes[i] != NULL) {
            __dome_init(domes[i]);
        }
    }
    
    rpc_server_start(server);
}

static void
destroy(void)
{
    size_t i;
    if (domes != NULL) {
        for (i = 0; i < n_dome; i++) {
            if (domes[i] != NULL) {
                delete(domes[i]);
            }
        }
    }
    free(domes);

    if (server != NULL) {
        delete(server);
    }

    if (d != NULL) {
        delete(d);
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
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    
    if(config_read_file(&cfg, config_path) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        fprintf(stderr, "Exit...\n");
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
