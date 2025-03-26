//
//  thermal_server.c
//  AAOS
//
//  Created by Hu Yi on 2023/3/21.
//  Copyright © 2023 NAOC. All rights reserved.
//

#include "def.h"
#include "daemon.h"
#include "thermal.h"
#include "thermal_rpc.h"
#include "wrapper.h"
#include <libconfig.h>

extern void **units;
extern size_t n_unit;
pthread_t *tids;

static void *d, *server;
static const char *conf_path = "/usr/local/aaos/etc/telescopes.cfg";
static config_t cfg;
static bool daemon_flag = true;


static struct option longopts[] = {
    {"config-file", required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static void
usage(void)
{
    fprintf(stderr, "\t\t[-n|--name <name>] [-i|--index <index>]");
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
    config_setting_t *setting;
    
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        const char *port;
        config_setting_lookup_string(setting, "port", &port);
        server = new(ThermalUnitServer(), port);
    }
    
    setting = config_lookup(&cfg, "units");
    if (setting == NULL) {
        fprintf(stderr, "`units` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_unit = config_setting_length(setting);
        units = (void **) Malloc(n_unit * sizeof(void *));
        memset(units, '\0', n_unit * sizeof(void *));
        if (units == NULL) {
            exit(EXIT_FAILURE);
        }
        size_t i;
        for (i = 0; i < n_unit; i++) {
            config_setting_t *unit_setting;
            unit_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name, *description, *type;
            double highest, lowest, period;
            if (config_setting_lookup_string(unit_setting, "name", &name) != CONFIG_TRUE) {
                name = NULL;
            }
            if (config_setting_lookup_string(unit_setting, "description", &description) != CONFIG_TRUE) {
                description = NULL;
            }
            if (config_setting_lookup_string(unit_setting, "type", &type) != CONFIG_TRUE) {
                type = NULL;
            }
            if (config_setting_lookup_float(unit_setting, "highest", &highest) != CONFIG_TRUE) {
                highest = 10.;
            }
            if (config_setting_lookup_float(unit_setting, "lowest", &lowest) != CONFIG_TRUE) {
                lowest = 0.;
            }
            if (config_setting_lookup_float(unit_setting, "period", &period) != CONFIG_TRUE) {
                period = 300.;
            }
            if (strcmp(type, "klcam_simple") == 0) {
                const char *temp_cmd, *turn_on_cmd, *turn_off_cmd, *temp_cmd2, *turn_on_cmd2, *turn_off_cmd2, *temp_cmd3;
                double threshold;
                
                if (config_setting_lookup_string(unit_setting, "temp_cmd", &temp_cmd) != CONFIG_TRUE) {
                    temp_cmd = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "turn_on_cmd", &turn_on_cmd) != CONFIG_TRUE) {
                    turn_on_cmd = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "turn_off_cmd", &turn_off_cmd) != CONFIG_TRUE) {
                    turn_off_cmd = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "temp_cmd2", &temp_cmd2) != CONFIG_TRUE) {
                    temp_cmd2 = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "turn_on_cmd2", &turn_on_cmd2) != CONFIG_TRUE) {
                    turn_on_cmd2 = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "turn_off_cmd2", &turn_off_cmd2) != CONFIG_TRUE) {
                    turn_off_cmd2 = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "temp_cmd3", &temp_cmd3) != CONFIG_TRUE) {
                    temp_cmd3 = NULL;
                }
                if (config_setting_lookup_float(unit_setting, "threshold", &threshold) != CONFIG_TRUE) {
                    threshold = -40.;
                }
                units[i] = new(KLCAMSimpleThermalUnit(), name, highest, lowest, period, "description", description, '\0', temp_cmd, turn_on_cmd, turn_off_cmd, temp_cmd2, turn_on_cmd2, turn_off_cmd2, temp_cmd3, threshold);
            } else if (strcmp(type, "simple") == 0) {
                const char *temp_cmd, *turn_on_cmd, *turn_off_cmd;
                if (config_setting_lookup_string(unit_setting, "temp_cmd", &temp_cmd) != CONFIG_TRUE) {
                    temp_cmd = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "turn_on_cmd", &turn_on_cmd) != CONFIG_TRUE) {
                    turn_on_cmd = NULL;
                }
                if (config_setting_lookup_string(unit_setting, "turn_off_cmd", &turn_off_cmd) != CONFIG_TRUE) {
                    turn_off_cmd = NULL;
                }
                units[i] = new(SimpleThermalUnit(), name, highest, lowest, period, "description", description, '\0', temp_cmd, turn_on_cmd, turn_off_cmd);
            }
        }
    }
    config_destroy(&cfg);
}

static void
init(void)
{
    read_configuration();
    tids = (pthread_t *) Malloc(sizeof(pthread_t));
    size_t i;
    void *retval;

    for (i = 0; i < n_unit; i++) {
        if (units[i] != NULL) {
            Pthread_create(&tids[i], NULL, __thermal_unit_thermal_control, units[i]);
        }
    }
    rpc_server_start(server);
    for (i = 0; i < n_unit; i++) {
        if (units[i] != NULL) {
            Pthread_join(tids[i], &retval);
        }
    }
}

static void
destroy(void)
{
    size_t i;
    if (units != NULL) {
        for (i = 0; i < n_unit; i++) {
            if (units[i] != NULL) {
                delete(units[i]);
            }
        }
    }
    free(units);
    free(tids);
    
    if (server != NULL) {
        delete(server);
    }
    
    if (d != NULL) {
        delete(d);
    }
}

int
main(int argc, char *argv[])
{
    int ch, ret;
    
    while ((ch = getopt_long(argc, argv, "c:Dv", longopts, NULL)) != -1) {
        switch (ch) {
            case 'c':
                conf_path = optarg;
                break;
            case 'D':
                daemon_flag = false;
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    config_init(&cfg);
    
    if ((ret = Access(conf_path, F_OK)) < 0) {
        fprintf(stderr, "configuration file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    
    if(config_read_file(&cfg, conf_path) == CONFIG_FALSE) {
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
            exit(EXIT_FAILURE);
        }
    }
    destroy();
    config_destroy(&cfg);
    return 0;
}
