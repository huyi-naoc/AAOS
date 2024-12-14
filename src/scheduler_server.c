//
//  scheduler_server.c
//  AAOS
//
//  Created by Hu Yi on 2024/7/31.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "daemon.h"
#include "rpc.h"
#include "scheduler_def.h"
#include "scheduler.h"
#include "scheduler_rpc.h"
#include "wrapper.h"
#include <libconfig.h>

static void *d;
static void *server;
static const char *config_path = "/usr/local/aaos/etc/schedulerd.cfg";
static bool daemon_flag = true;
static bool feed_dog_flag = true;
static config_t cfg;

extern void *scheduler;

static struct option longopts[] = {
    {"config", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}};

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
    config_setting_t *setting = NULL, *global_setting = NULL, *site_setting = NULL, *telescope_setting = NULL, *db_setting = NULL;

    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        const char *port;
        config_setting_lookup_string(setting, "port", &port);
        server = new (SchedulerServer(), port);
    }

    setting = config_lookup(&cfg, "scheduler");
    if (setting == NULL) {
        fprintf(stderr, "`scheduler` section does not exist in configuration file.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    unsigned int type;
    const char *s = NULL, *description = NULL;

    if (config_setting_lookup_string(setting, "type", &s) == CONFIG_TRUE) {
        if (strcasecmp(s, "site") == 0) {
            type = SCHEDULER_TYPE_SITE;
        } else if (strcasecmp(s, "global") == 0) {
            type = SCHEDULER_TYPE_GLOBAL;
        } else if (strcasecmp(s, "unit") == 0 || strcasecmp(s, "telescope") == 0) {
            type = SCHEDULER_TYPE_UNIT;
        } else {
            type = SCHEDULER_TYPE_UNKNOWN;
            fprintf(stderr, "`%s` type scheduler is not supported.\n", s);
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "type of scheduler is not specified.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    if (config_setting_lookup_string(setting, "description", &description) == CONFIG_TRUE) {
    }

    /*
     * Create scheduler object.
     */
    scheduler = new(__Scheduler(), type, description, (void *) 0);

    if (type == SCHEDULER_TYPE_SITE) {
        const char *address = NULL, *port = NULL, *name = NULL;
        const char *ipc_model = NULL, *algorithm = NULL, *sock_file = NULL;
        uint64_t site_id;
        int64_t s_site_id;
        double site_lon, site_lat, site_alt;
        
        if ((site_setting = config_setting_lookup(setting, "global")) == NULL) {
            fprintf(stderr, "`site` scheduler needs `global` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(site_setting, "address", &address) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "global_addr", address);
        } else {
            fprintf(stderr, "`site` scheduler needs `global` scheduler's address in `global` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(site_setting, "port", &port) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "global_port", port);
        } else {
            fprintf(stderr, "`site` scheduler needs `global` scheduler's port in `global` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }   
        if ((site_setting = config_setting_lookup(setting, "module")) == NULL) {
            fprintf(stderr, "`site` scheduler needs `module` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(site_setting, "ipc_model", &ipc_model) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "ipc_model", ipc_model);
        } else {
            fprintf(stderr, "`site` scheduler needs `ipc_model` in `moddule` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(site_setting, "algorithm", &algorithm) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "algorithm", algorithm);
        } else {
            fprintf(stderr, "`site` scheduler needs `algorithm` in `moddule` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(site_setting, "sock_file", &sock_file) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "sock_file", sock_file);
        } else {
            fprintf(stderr, "`site` scheduler needs `sock_file` in `moddule` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }

        site_setting = config_setting_lookup(setting, "site");
        if (config_setting_lookup_int64(site_setting, "site_id", &s_site_id) == CONFIG_TRUE) {
            site_id = s_site_id<0?~s_site_id + 1: s_site_id;
            __scheduler_set_member(scheduler, "site_id", site_id);
        } else {
            fprintf(stderr, "`site` scheduler needs `site_id` configuration setting in `site` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(site_setting, "name", &name) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_name", name);
        }
        if (config_setting_lookup_float(site_setting, "site_lon", &site_lon) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_lon", site_lon);
        }
        if (config_setting_lookup_float(site_setting, "site_lat", &site_lat) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_lat", site_lat);
        }
        if (config_setting_lookup_float(site_setting, "site_alt", &site_alt) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_alt", site_alt);
        }
    } else if (type == SCHEDULER_TYPE_UNIT) {
        uint64_t tel_id, site_id;
        int64_t s_tel_id, s_site_id;;
        const char *address = NULL, *port = NULL, *name = NULL, *description = NULL;
        if ((telescope_setting = config_setting_lookup(setting, "site")) == NULL) {
            fprintf(stderr, "`telescope` scheduler needs `site` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(telescope_setting, "address", &address) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_addr", address);
        } else {
            fprintf(stderr, "`unit` scheduler needs `site` scheduler's address in `site` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(telescope_setting, "port", &port) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_port", port);
        } else {
            fprintf(stderr, "`unit` scheduler needs `site` scheduler's address in `site` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if ((telescope_setting = config_setting_lookup(setting, "telescope")) == NULL) {
            fprintf(stderr, "`unit` scheduler needs `telescope` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_int64(telescope_setting, "tel_id", &s_tel_id) == CONFIG_TRUE && config_setting_lookup_int64(telescope_setting, "site_id", &s_site_id) == CONFIG_TRUE) {
            site_id = s_site_id<0?~s_site_id + 1: s_site_id;
            tel_id = s_tel_id<0?~s_tel_id + 1: s_tel_id;
            __scheduler_set_member(scheduler, "tel_id", tel_id, site_id);
        } else {
            fprintf(stderr, "`unit` scheduler needs `tel_id` and `site_id` configuration settings in `telescope` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(telescope_setting, "name", &name) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "telescop", name);
        }
        if (config_setting_lookup_string(telescope_setting, "description", &description) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "tel_des", description);
        }
    } else if (type == SCHEDULER_TYPE_GLOBAL) {
        const char *ipc_model = NULL, *algorithm = NULL, *sock_file = NULL;
        if ((global_setting = config_setting_lookup(setting, "module")) == NULL) {
            fprintf(stderr, "`global` scheduler needs `module` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(global_setting, "ipc_model", &ipc_model) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "ipc_model", ipc_model);
        } else {
            fprintf(stderr, "`global` scheduler needs `ipc_model` in `moddule` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(global_setting, "algorithm", &algorithm) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "algorithm", algorithm);
        } else {
            fprintf(stderr, "`global` scheduler needs `algorithm` in `moddule` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(global_setting, "sock_file", &sock_file) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "sock_file", sock_file);
        } else {
            fprintf(stderr, "`global` scheduler needs `sock_file` in `moddule` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
    }

    setting = config_lookup(&cfg, "database");
    const char *db_host = NULL,  *db_user = NULL, *db_passwd = NULL, *db_name = NULL;
    const char *site_db_table = NULL, *telescope_db_table = NULL, *target_db_table = NULL, *task_db_table = NULL;
    if (setting == NULL) {
        fprintf(stderr, "`database` section does not exist in configuration file.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    if (config_setting_lookup_string(setting, "db_host", &db_host) == CONFIG_TRUE) {
        __scheduler_set_member(scheduler, "db_host", db_host);
    } else {
        fprintf(stderr, "`db_host` needs to exist in `database` section.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    if (config_setting_lookup_string(setting, "db_user", &db_user) == CONFIG_TRUE) {
        __scheduler_set_member(scheduler, "db_user", db_user);
    } else {
        fprintf(stderr, "`db_user` needs to exist in `database` section.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    if (config_setting_lookup_string(setting, "db_passwd", &db_passwd) == CONFIG_TRUE) {
        __scheduler_set_member(scheduler, "db_passwd", db_passwd);
    } else {
        fprintf(stderr, "`db_passwd` needs to exist in `database` section.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    if (config_setting_lookup_string(setting, "db_name", &db_name) == CONFIG_TRUE) {
        __scheduler_set_member(scheduler, "db_name", db_name);
    } else {
        fprintf(stderr, "`db_name` needs to exist in `database` section.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }

    if (type == SCHEDULER_TYPE_GLOBAL) {
        if (config_setting_lookup_string(setting, "site_db_table", &site_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "site_db_table", site_db_table);
        } else {
            fprintf(stderr, "Global scheduler must have `site_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(setting, "telescope_db_table", &telescope_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "telescope_db_table", telescope_db_table);
        } else {
            fprintf(stderr, "Global scheduler must have `telescope_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(setting, "target_db_table", &target_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "target_db_table", target_db_table);
        } else {
            fprintf(stderr, "Global scheduler must have `target_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(setting, "task_db_table", &task_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "task_db_table", task_db_table);
        } else {
            fprintf(stderr, "Global scheduler must have `task_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
    } else if (type == SCHEDULER_TYPE_SITE) {
        if (config_setting_lookup_string(setting, "telescope_db_table", &telescope_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "telescope_db_table", telescope_db_table);
        } else {
            fprintf(stderr, "Site scheduler must have `telescope_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(setting, "target_db_table", &target_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "target_db_table", target_db_table);
        } else {
            fprintf(stderr, "Site scheduler must have `target_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
        if (config_setting_lookup_string(setting, "task_db_table", &task_db_table) == CONFIG_TRUE) {
            __scheduler_set_member(scheduler, "task_db_table", task_db_table);
        } else {
            fprintf(stderr, "Site scheduler must have `task_db_table` in `database` section.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
    }
    __scheduler_init(scheduler);
}

static void
init(void)
{
    read_configuration();
    
    rpc_server_start(server);
}

static void
destroy(void)
{
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
            fprintf(stderr, "Exit...\n");
            destroy();
            exit(EXIT_FAILURE);
        }
    }
    destroy();
    config_destroy(&cfg);
    
    return 0;
}
