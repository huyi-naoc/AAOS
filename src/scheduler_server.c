//
//  scheduler_server_test.c
//  AAOS
//
//  Created by Hu Yi on 2024/7/31.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "daemon.h"
#include "scheduler.h"
#include "wrapper.h"
#include <libconfig.h>

extern void *site_list;
extern void *target_list;
extern void *telescope_list;
extern void *thread_list;

static void *d;
static void *server;
static const char *config_path = "/usr/local/aaos/etc/schedulerd.cfg";
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
    config_setting_t *setting = NULL;
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        const char *port;
        config_setting_lookup_string(setting, "port", &port);
        server = new(SchedulerServer(), port);
    }    
}
