//
//  aws_server_test.c
//  AAOS
//
//  Created by Hu Yi on 2020/1/22.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "def.h"
#include "daemon.h"
#include "aws.h"
#include "aws_def.h"
#include "aws_rpc.h"
#include "wrapper.h"
#include <libconfig.h>

extern void **awses;
extern size_t n_aws;

static void *d;
static void *server;
static const char *config_path = "/usr/local/aaos/etc/awsd.cfg";
static bool daemon_flag = true;

static void
usage()
{
    fprintf(stderr, "\t\t[-n|--name <name>] [-i|--index <index>]");
    exit(EXIT_FAILURE);
}

static struct option longopts[] = {
    {"config-file", required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};


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
        server = new(AWSServer(), port);
    }
    
    if ((ret = Access(pathname, F_OK)) < 0) {
        fprintf(stderr, "configuration file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    
    if(config_read_file(&cfg, pathname) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        exit(EXIT_FAILURE);
    }
    
    setting = config_lookup(&cfg, "awses");
    if (setting == NULL) {
        fprintf(stderr, "`telescopes` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_aws = config_setting_length(setting);
        awses = (void **) Malloc(n_aws * sizeof(void *));
        memset(awses, '\0', n_aws * sizeof(void *));
        if (awses == NULL) {
            exit(EXIT_FAILURE);
        }
        size_t i;
        for (i = 0; i < n_aws; i++) {
            config_setting_t *aws_setting;
            aws_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name, *description, *type;
            if (config_setting_lookup_string(aws_setting, "name", &name) != CONFIG_TRUE) {
                name = NULL;
            }
            if (config_setting_lookup_string(aws_setting, "description", &description) != CONFIG_TRUE) {
                description = NULL;
            }
            if (config_setting_lookup_string(aws_setting, "type", &type) != CONFIG_TRUE) {
                type = NULL;
            }
            size_t j, n_controller, n_sensor = 0;
            config_setting_t *controllers_setting;
            controllers_setting = config_setting_get_member(aws_setting, "controllers");
            n_controller = config_setting_length(controllers_setting);
            for (j = 0; j < n_controller; j++) {
                config_setting_t *devices_setting;
                config_setting_t *controller_setting;
                controller_setting = config_setting_get_elem(controllers_setting, (unsigned int) j);
                size_t n_device, k;
                devices_setting = config_setting_get_member(controller_setting, "devices");
                n_device = config_setting_length(devices_setting);
                for (k = 0; k < n_device; k++) {
                    config_setting_t *device_setting, *sensors_setting;
                    device_setting = config_setting_get_elem(devices_setting, (unsigned int) k);
                    sensors_setting = config_setting_get_member(device_setting, "sensors");
                    size_t n;
                    n = config_setting_length(sensors_setting);
                    n_sensor += n;
                }
            }
            
            if (strcmp(type, "KLAWS") == 0) {
                awses[i] = new(KLAWS(), name, n_sensor, "type", type, "description", description, '\0', n_controller);
                n_sensor = 0;
                void *aws = awses[i];
                size_t j;
                controllers_setting = config_setting_get_member(aws_setting, "controllers");
                for (j = 0; j < n_controller; j++) {
                    config_setting_t *controller_setting, *devices_setting;
                    controller_setting = config_setting_get_elem(controllers_setting, (unsigned int) j);
                    void *controller;
                    size_t n_device, k;
                    devices_setting = config_setting_get_member(controller_setting, "devices");
                    const char *address, *port;
                    int critical = 1;
                    if (config_setting_lookup_string(controller_setting, "address", &address) != CONFIG_TRUE) {
                        address = NULL;
                    }
                    if (config_setting_lookup_string(controller_setting, "port", &port) != CONFIG_TRUE) {
                        port = NULL;
                    }
                    if (config_setting_lookup_int(controller_setting, "critical", &critical) != CONFIG_TRUE) {
                        critical = 1;
                    }
                    n_device = config_setting_length(devices_setting);
                    controller = new(KLAWSController(), address, port, n_device, critical);
                    klaws_set_controller(aws, controller, j);
                    for (k = 0; k < n_device; k++) {
                        void *device, *serial;
                        const char *name;
                        int critical = 1;
                        config_setting_t *device_setting, *sensors_setting;
                        device_setting = config_setting_get_elem(devices_setting, (unsigned int) k);
                        sensors_setting = config_setting_get_member(device_setting, "sensors");
                        size_t l, n;
                        n = config_setting_length(sensors_setting);
                        if (config_setting_lookup_string(device_setting, "name", &name) != CONFIG_TRUE) {
                            name = NULL;
                        }
                        if (config_setting_lookup_int(device_setting, "critical", &critical) != CONFIG_TRUE) {
                            critical = 1;
                        }
                        device = new(KLAWSDevice(), name, critical);
                        serial = klaws_controller_get_serial(controller);
                        klaws_device_set_index(device, serial);
                        klaws_controller_set_device(controller, device, k);
                        for (l = 0; l < n; l++) {
                            void *sensor = NULL;
                            config_setting_t *sensor_setting;
                            sensor_setting = config_setting_get_elem(sensors_setting, (unsigned int) l);
                            const char *name, *command, *type, *description, *model;
                            if (config_setting_lookup_string(sensor_setting, "name", &name) != CONFIG_TRUE) {
                                name = NULL;
                            }
                            if (config_setting_lookup_string(sensor_setting, "command", &command) != CONFIG_TRUE) {
                                command = NULL;
                            }
                            if (config_setting_lookup_string(sensor_setting, "model", &model) != CONFIG_TRUE) {
                                model = NULL;
                            }
                            if (config_setting_lookup_string(sensor_setting, "description", &description) != CONFIG_TRUE) {
                                description = NULL;
                            }
                            
                            if (config_setting_lookup_string(sensor_setting, "type", &type) != CONFIG_TRUE) {
                                type = NULL;
                            }
                            
                            if (strcmp(model, "Young41342") == 0) {
                                sensor = new(Young41342(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_TEMEPRATURE);
                            } else if (strcmp(model, "Young05305V") == 0 && strcmp(type, "wind speed") == 0) {
                                sensor = new(Young05305VS(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_WIND_SPEED);
                            } else if (strcmp(model, "Young05305V") == 0 && strcmp(type, "wind direction") == 0) {
                                double offset;
                                if (config_setting_lookup_float(sensor_setting, "offset", &offset) != CONFIG_TRUE) {
                                    offset = 0.;
                                }
                                sensor = new(Young05305VD(), name, command, "description", description, "model", model, '\0', "offset", offset, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_WIND_DIRECTION);
                            } else if (strcmp(model, "Young41382VC") == 0 && strcmp(type, "humidity") == 0) {
                                sensor = new(Young41382VCR(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_RELATIVE_HUMIDITY);
                            } else if (strcmp(model, "Young41382VC") == 0 && strcmp(type, "temperature") == 0) {
                                sensor = new(Young41382VCT(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_TEMEPRATURE);
                            } else if (strcmp(model, "Young61302V") == 0) {
                                sensor = new(Young61302V(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_AIR_PRESSURE);
                            } else if (strcmp(model, "WS100UMB") == 0) {
                                sensor = new(WS100UMB(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_PRECIPITATION);
                            } else if (strcmp(model, "SQM") == 0) {
                                double extinction;
                                if (config_setting_lookup_float(sensor_setting, "extinction", &extinction) != CONFIG_TRUE) {
                                    extinction = 0.;
                                }
                                sensor = new(SkyQualityMonitor(), name, command, "description", description, "model", model, '\0', "extinction", extinction, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_SKY_QUALITY);
                            } else if (strcmp(model, "PT100") == 0) {
                                sensor = new(PT100(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_TEMEPRATURE);
                            } else if (strcmp(model, "PTB210") == 0) {
                                sensor = new(PTB210(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_AIR_PRESSURE);
                            } else if (strcmp(model, "AAGPDUPT1000") == 0) {
                                sensor = new(AAGPDUPT1000(), name, command, "description", description, "model", model, '\0');
                                sensor_set_type(sensor, SENSOR_TYPE_TEMEPRATURE);
                            } else {
                            }
                            sensor_set_channel(sensor, (unsigned int) (n_sensor + l + 1));
                            sensor_set_controller(sensor, controller);
                            sensor_set_device(sensor, device);
                            __aws_set_sensor(aws, sensor, n_sensor + l);
                        }
                        n_sensor += n;
                    }
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
    for (i = 0; i < n_aws; i++) {
        if (awses[i] != NULL) {
            delete(awses[i]);
        }
    }
    free(awses);
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
