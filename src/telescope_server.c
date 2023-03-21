//
//  telescope_server_test.c
//  AAOS
//
//  Created by Hu Yi on 2019/7/24.
//  Copyright © 2019 NAOC. All rights reserved.
//

#include "def.h"
#include "daemon.h"
#include "telescope.h"
#include "telescope_rpc.h"
#include "wrapper.h"
#include <libconfig.h>

extern void **telescopes;
extern size_t n_telescope;

static void *d;
static void *server;
static const char *conf_path = "/usr/local/aaos/etc/telescopes.cfg";
static bool daemon_flag = true;
static config_t cfg;

static struct option longopts[] = {
    {"config-file", required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static void
usage()
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
        if (daemonized) {
            d = new(Daemon(), name, rootdir, lockfile, username, true);
        } else {
            d = new(Daemon(), name, rootdir, lockfile, username, false);
        }
    }
}

static void
read_configuration(void)
{
    config_setting_t *setting, *telescope_setting;
    
    size_t i;
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        const char *port;
        config_setting_lookup_string(setting, "port", &port);
        server = new(TelescopeServer(), port);
    }
    
    setting = config_lookup(&cfg, "telescopes");
    if (setting == NULL) {
        fprintf(stderr, "`telescopes` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_telescope = config_setting_length(setting);
        telescopes = (void **) Malloc(n_telescope * sizeof(void *));
        memset(telescopes, '\0', n_telescope * sizeof(void *));
        if (telescopes == NULL) {
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < n_telescope; i++) {
            telescope_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name = NULL, *description = NULL, *type = NULL;
            double lon, lat, gmt_offset = -8.;
            config_setting_lookup_string(telescope_setting, "name", &name);
            config_setting_lookup_string(telescope_setting, "type", &type);
            config_setting_lookup_string(telescope_setting, "description", &description);
            config_setting_lookup_float(telescope_setting, "longitude", &lon);
            config_setting_lookup_float(telescope_setting, "latitude", &lat);
            config_setting_lookup_float(telescope_setting, "gmt_offset", &gmt_offset);
            if (type == NULL) {
                telescopes[i] = NULL;
                break;
            }
            if (strcmp(type, "VIRTUAL") == 0) {
                telescopes[i] = new(VirtualTelescope(), name, "description", description, "longitude", lon, "latitude", lat, "gmt_offset", gmt_offset, '\0');
            } else if (strcmp(type, "APMOUNT") == 0){
                config_setting_t *ap_mount_setting;
                const char *serial_port = "5002";
                const char *serial_address = "localhost";
                const char *serial_name;
                int mount_type = TELESCOPE_EQUATORIAL;
                if ((ap_mount_setting = config_setting_lookup(telescope_setting, "ap_mount")) != NULL) {
                    const char *mount_type_string;
                    config_setting_lookup_string(ap_mount_setting, "serial_address", &serial_address);
                    config_setting_lookup_string(ap_mount_setting, "serial_port", &serial_port);
                    config_setting_lookup_string(ap_mount_setting, "serial_name", &serial_name);
                    config_setting_lookup_string(ap_mount_setting, "mount_type", &mount_type_string);
                    if (strcmp(mount_type_string, "EQUATORIAL") == 0) {
                        mount_type = TELESCOPE_EQUATORIAL;
                    } else if (strcmp(mount_type_string, "HORIZONTAL") == 0) {
                        mount_type = TELESCOPE_HORIZONTAL;
                    }
                }
                telescopes[i] = new(APMount(), name, "description", description, "description", description, "longitude", lon, "latitude", lat, "gmt_offset", gmt_offset, '\0', mount_type, serial_address, serial_port, serial_name);
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
    for (i = 0; i < n_telescope; i++) {
        __telescope_power_on(telescopes[i]);
        __telescope_init(telescopes[i]);
    }
    
    rpc_server_start(server);
}

static void
destroy(void)
{
    size_t i;
    if (telescopes != NULL) {
        for (i = 0; i < n_telescope; i++) {
            if (telescopes[i] != NULL) {
                delete(telescopes[i]);
            }
        }
    }
    free(telescopes);

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
