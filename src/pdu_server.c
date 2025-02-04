//
//  pdu_server.c
//  AAOS
//
//  Created by Hu Yi on 2020/7/25.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "def.h"
#include "daemon.h"
#include "pdu.h"
#include "pdu_def.h"
#include "pdu_rpc.h"
#include "wrapper.h"
#include <libconfig.h>

extern void **pdus;
extern size_t n_pdu;

static void *d;
static void *server;
static const char *config_path = "/usr/local/aaos/etc/pdud.cfg";
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
read_daemon(const char *pathname)
{
    int ret;
    config_t cfg;
    config_init(&cfg);
    config_setting_t *setting;
    
    if ((ret = Access(pathname, F_OK)) < 0) {
        fprintf(stderr, "configuration file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    
    if(config_read_file(&cfg, pathname) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        exit(EXIT_FAILURE);
    }
    
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
    config_destroy(&cfg);
}

static void
read_configuration(const char *pathname)
{
    int ret;
    config_t cfg;
    config_init(&cfg);
    config_setting_t *setting;
    
    if ((ret = Access(pathname, F_OK)) < 0) {
        fprintf(stderr, "configuration file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    
    if(config_read_file(&cfg, pathname) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        exit(EXIT_FAILURE);
    }
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        const char *port;
        config_setting_lookup_string(setting, "port", &port);
        server = new(PDUServer(), port);
    }
    
    if ((ret = Access(pathname, F_OK)) < 0) {
        fprintf(stderr, "configuration file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    
    if(config_read_file(&cfg, pathname) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        exit(EXIT_FAILURE);
    }
    
    setting = config_lookup(&cfg, "pdus");
    if (setting == NULL) {
        fprintf(stderr, "`pdus` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_pdu = config_setting_length(setting);
        pdus = (void **) Malloc(n_pdu * sizeof(void *));
        memset(pdus, '\0', n_pdu * sizeof(void *));
        if (pdus == NULL) {
            exit(EXIT_FAILURE);
        }
        size_t i;
        for (i = 0; i < n_pdu; i++) {
            config_setting_t *pdu_setting;
            pdu_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name, *description, *type;
            if (config_setting_lookup_string(pdu_setting, "name", &name) != CONFIG_TRUE) {
                name = NULL;
            }
            if (config_setting_lookup_string(pdu_setting, "description", &description) != CONFIG_TRUE) {
                description = NULL;
            }
            if (config_setting_lookup_string(pdu_setting, "type", &type) != CONFIG_TRUE) {
                type = NULL;
            }
            size_t j, n_switches;
            config_setting_t *switches_setting;
            switches_setting = config_setting_get_member(pdu_setting, "switches");
            n_switches = config_setting_length(switches_setting);
            if (strcmp(type, "AAGPDU") == 0) {
                config_setting_t *serial_setting;
                const char *address, *port, *serial1, *inspect1, *serial2, *inspect2;
                if (config_setting_lookup_string(serial_setting, "address", &address) != CONFIG_TRUE) {
                    address = NULL;
                }
                if (config_setting_lookup_string(serial_setting, "port", &port) != CONFIG_TRUE) {
                    port = NULL;
                }
                if (config_setting_lookup_string(serial_setting, "serial1", &serial1) != CONFIG_TRUE) {
                    serial1 = NULL;
                }
                if (config_setting_lookup_string(serial_setting, "inspect1", &inspect1) != CONFIG_TRUE) {
                    inspect1 = NULL;
                }
                if (config_setting_lookup_string(serial_setting, "serial2", &serial2) != CONFIG_TRUE) {
                    serial2 = NULL;
                }
                if (config_setting_lookup_string(serial_setting, "inspect2", &inspect2) != CONFIG_TRUE) {
                    serial2 = NULL;
                }
                pdus[i] = new(AAGPDU(), name, n_switches, "description", description, "type", type, '\0', address, port, serial1, inspect1, serial2, inspect2);
            } else {
                fprintf(stderr, "Unsupported PDU type `%s`\n", type);
                exit(EXIT_FAILURE);
            }
            for (j = 0; j < n_switches; j++) {
                void *myswitch;
                config_setting_t *switch_setting = config_setting_get_elem(switches_setting, (unsigned int) j);
                const char *name, *type;
                int channel = 0;
                if (config_setting_lookup_string(switch_setting, "name", &name) != CONFIG_TRUE) {
                    name = NULL;
                }
                if (config_setting_lookup_string(switch_setting, "type", &type) != CONFIG_TRUE) {
                    type = NULL;
                }
                if (config_setting_lookup_int(switch_setting, "channel", &channel) != CONFIG_TRUE) {
                    type = NULL;
                }
                if (strcmp(type, "AAGPDU") == 0) {
                    myswitch = new(AAGSwitch(), name, "type", type, '\0');
                    switch_set_pdu(myswitch, pdus[i]);
                } else {
                    myswitch = NULL;
                    break;
                }
                if (channel != 0) {
                    switch_set_channel(myswitch, (unsigned int) channel);
                } else {
                    switch_set_channel(myswitch, (unsigned int) j + 1);
                }
            }
        }
    }
    config_destroy(&cfg);
}

static void
init(void)
{
    rpc_server_start(server);
}

static void
destroy(void)
{
    size_t i;
    for (i = 0; i < n_pdu; i++) {
        if (pdus[i] != NULL) {
            delete(pdus[i]);
        }
    }
    free(pdus);
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
    int ch;
    
    while ((ch = getopt_long(argc, argv, "c:Dv", longopts, NULL)) != -1) {
        switch (ch) {
            case 'c':
                config_path = optarg;
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
    
    read_daemon(config_path);
    if (argc == 0) {
        daemon_start(d);
        read_configuration(config_path);
        init();
    } else {
        if (strcmp(argv[0], "start") == 0) {
            daemon_start(d);
            read_configuration(config_path);
            init();
        } else if (strcmp(argv[0], "restart") == 0) {
            daemon_stop(d);
            daemon_start(d);
            read_configuration(config_path);
            init();
        } else if (strcmp(argv[0], "reload") == 0) {
            daemon_stop(d);
            daemon_reload(d);
            read_configuration(config_path);
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
    return 0;
}


