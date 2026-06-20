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
static const char *config_path = "/opt/aaos/etc/telescoped.cfg";
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
    config_setting_t *setting, *telescope_setting, *instruments_setting, *instrument_setting, *detectors_setting, *detector_setting, *filters_setting;
    
    size_t i;
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        fprintf(stderr, "use default telescope RPC port.\n");
        server = new(TelescopeServer(), TEL_RPC_PORT);
    } else {
        const char *port = NULL, *path = NULL;
        config_setting_lookup_string(setting, "port", &port);
        config_setting_lookup_string(setting, "path", &path);
        if (port == NULL) {
            fprintf(stderr, "`port` does not exist in configuration file.\n");
            fprintf(stderr, "use default telescope RPC port.\n");
            server = new(TelescopeServer(), TEL_RPC_PORT);
        } else {
            server = new(TelescopeServer(), port);
        }
        if (path != NULL) {
            tcp_server_set_path(server, path);
        }
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
            const char *name = NULL, *description = NULL, *type = NULL, *instrument = NULL, *detector = NULL, *filter = NULL;
            char **instruments = NULL, ***detectors = NULL, ****filters = NULL;
            size_t n_instrument = 0, *n_detector = NULL, **n_filter = NULL, j, k, l;
            double lon, lat, ele, gmt_offset = -8.;
            config_setting_lookup_string(telescope_setting, "name", &name);
            config_setting_lookup_string(telescope_setting, "type", &type);
            config_setting_lookup_string(telescope_setting, "description", &description);
            config_setting_lookup_float(telescope_setting, "longitude", &lon);
            config_setting_lookup_float(telescope_setting, "latitude", &lat);
            config_setting_lookup_float(telescope_setting, "elevation", &ele);
            config_setting_lookup_float(telescope_setting, "gmt_offset", &gmt_offset);
            if (type == NULL) {
                telescopes[i] = NULL;
                break;
            }
            if ((instruments_setting = config_setting_lookup(telescope_setting, "instruments")) != NULL) {
                if ((n_instrument = config_setting_length(instruments_setting)) > 0) {
                    instruments = (char **) Malloc(n_instrument * sizeof(char *));
                    memset(instruments, '\0', n_instrument * sizeof(char *));
                    detectors = (char ***) Malloc(n_instrument * sizeof(char **));
                    memset(detectors, '\0', n_instrument * sizeof(char **));
                    filters =  (char ****) Malloc(n_instrument * sizeof(char ***));
                    memset(filters, '\0', n_instrument * sizeof(char ***));
                    n_detector = (size_t *) Malloc( n_instrument * sizeof(size_t));
                    memset(n_detector, '\0', n_instrument * sizeof(size_t));
                    n_filter = (size_t **) Malloc(n_instrument * sizeof(size_t *));
                    memset(n_filter, '\0', n_instrument * sizeof(size_t *));
                    for (j = 0; j < n_instrument; j++) {
                        if ((instrument_setting = config_setting_get_elem(instruments_setting, (unsigned int) j)) != NULL) {
                            config_setting_lookup_string(instrument_setting, "name", &instrument);
                            if (instrument != NULL) {
                                instruments[j] = (char *) Malloc(strlen(instrument) + 1);
                                snprintf(instruments[j], strlen(instrument) + 1, "%s", instrument);
                            }
                            if ((detectors_setting = config_setting_lookup(instrument_setting, "detectors")) != NULL) {
                                if ((n_detector[j] = config_setting_length(detectors_setting)) > 0) {
                                    detectors[j] = (char **) Malloc(n_detector[j] * sizeof(char *));
                                    filters[j] = (char ***) Malloc(n_detector[j] * sizeof(char **));
                                    n_filter[j] = (size_t *) Malloc(n_detector[j] * sizeof(size_t));
                                    memset(n_filter[j], '\0', n_detector[j] * sizeof(size_t));
                                    for (k = 0; k < n_detector[j]; k++) {
                                        if ((detector_setting = config_setting_get_elem(detectors_setting, (unsigned int) k)) != NULL) {
                                             config_setting_lookup_string(detector_setting, "name", &detector);
                                             if (detector != NULL) {
                                                detectors[j][k] = (char *) Malloc(strlen(detector) + 1);
                                                snprintf(detectors[j][k], strlen(detector) + 1, "%s", detector);
                                                if ((filters_setting = config_setting_lookup(detector_setting, "filters")) != NULL) {
                                                    if ((n_filter[j][k] = config_setting_length(filters_setting)) > 0) {
                                                        filters[j][k] = (char **) Malloc(n_filter[j][k] * sizeof(char *));
                                                        for (l = 0; l < n_filter[j][k]; l++) {
                                                            filter = config_setting_get_string_elem(filters_setting, (unsigned int) l);
                                                            if (filter != NULL) {
                                                                filters[j][k][l] = (char *) Malloc(strlen(filter) + 1);
                                                                snprintf(filters[j][k][l], strlen(filter) + 1, "%s", filter);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                    }
                }
            }

            if (strcmp(type, "VIRTUAL") == 0) {
                telescopes[i] = new(VirtualTelescope(), name, "description", description, "longitude", lon, "latitude", lat, "gmt_offset", gmt_offset, '\0');
                if (instruments != NULL && n_instrument != 0) {
                    __telescope_set(telescopes[i], "instruments", instruments, n_instrument, (void *) 0);
                }
                if (detectors != NULL && n_detector != NULL) {
                    __telescope_set(telescopes[i], "detectors", detectors, n_detector, (void *) 0);
                }
                if (filters != NULL && n_filter != NULL) {
                    __telescope_set(telescopes[i], "detectors", filters, n_filter, (void *) 0);
                }
            } else if (strcmp(type, "APMOUNT") == 0){
                config_setting_t *ap_mount_setting;
                const char *serial_port = "5002";
                const char *serial_address = "localhost";
                const char *serial_name = NULL;
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
                telescopes[i] = new(APMount(), name, "description", description, "longitude", lon, "latitude", lat, "gmt_offset", gmt_offset, '\0', mount_type, serial_address, serial_port, serial_name);
            } else if (strcmp(type, "SYSU80") == 0) {
                config_setting_t *sysu80_setting, *instrument_setting;
                size_t j, n_instrument = 0;
                int dfl_inst = 0;
                size_t default_instrument = 0;
                const char *windows_address = NULL, *windows_port = NULL;
                char **instruments = NULL;
                const char *instrument;
                double home_ra = 37.95, home_dec = 89.26;
                
                if ((sysu80_setting = config_setting_lookup(telescope_setting, "sysu80")) != NULL) {
                    config_setting_lookup_string(sysu80_setting, "windows_address", &windows_address);
                    config_setting_lookup_string(sysu80_setting, "windows_port", &windows_port);
                    config_setting_lookup_int(sysu80_setting, "default_instrument", &dfl_inst);
                    config_setting_lookup_float(sysu80_setting, "home_ra", &home_ra);
                    config_setting_lookup_float(sysu80_setting, "home_dec", &home_dec);
                    
                    default_instrument = dfl_inst;
                    instrument_setting = config_setting_lookup(sysu80_setting, "instruments");
                    if (instrument_setting != NULL) {
                        n_instrument = config_setting_length(instrument_setting);
                        instruments = (char **) Malloc(n_instrument * sizeof(char *));
                        for (j = 0; j < n_instrument; j++) {
                            instrument = config_setting_get_string_elem(instrument_setting, (int) j);
                            instruments[j] = (char *) Malloc(strlen(instrument) + 1);
                            snprintf(instruments[j], strlen(instrument) + 1, "%s", instrument);
                        }
                    }
                }
                telescopes[i] = new(SYSU80(), name, "description", description, "longitude", lon, "latitude", lat,  "elevation", ele, (void *) 0, windows_address, windows_port, "instruments", instruments, "n_instrument", n_instrument, "default_instrument", default_instrument, "home_ra", home_ra, "home_dec", home_dec, (void *) 0);
            } else if (strcmp(type, "AICMOUNT") == 0) {
                config_setting_t *aic_mount_setting;
                const char *mount_address = NULL, *mount_port = NULL, *aux_address = NULL, *aux_port = NULL, *spectra_address = NULL, *spectra_port = NULL;
                double home_ra = 37.95, home_dec = 89.26;
                double slew_polling_interval = 2., focus_polling_interval = 1., derotator_polling_interval = 1., cover_polling_interval = 1.;
                unsigned int max_slew_polling_times = 200, max_focus_polling_times = 20, max_cover_polling_times = 20, max_derotator_polling_times = 20.;
                double focus_threshold = 1.2;
                
                if ((aic_mount_setting = config_setting_lookup(telescope_setting, "aic_mount")) != NULL) {
                    config_setting_lookup_string(aic_mount_setting, "mount_address", &mount_address);
                    config_setting_lookup_string(aic_mount_setting, "mount_port", &mount_port);
                    config_setting_lookup_string(aic_mount_setting, "aux_address", &aux_address);
                    config_setting_lookup_string(aic_mount_setting, "aux_port", &aux_port);
                    config_setting_lookup_string(aic_mount_setting, "spectra_address", &spectra_address);
                    config_setting_lookup_string(aic_mount_setting, "spectra_port", &spectra_port);
                    config_setting_lookup_float(aic_mount_setting, "home_ra", &home_ra);
                    config_setting_lookup_float(aic_mount_setting, "home_dec", &home_dec);
                    config_setting_lookup_float(aic_mount_setting, "slew_polling_interval", &slew_polling_interval);
                    config_setting_lookup_int(aic_mount_setting, "max_slew_polling_times", &max_slew_polling_times);
                    config_setting_lookup_float(aic_mount_setting, "focus_polling_interval", &focus_polling_interval);
                    config_setting_lookup_int(aic_mount_setting, "max_focus_polling_times", &max_focus_polling_times);
                    config_setting_lookup_float(aic_mount_setting, "derotator_polling_interval", &derotator_polling_interval);
                    config_setting_lookup_int(aic_mount_setting, "max_derotator_polling_times", &max_derotator_polling_times);
                    config_setting_lookup_float(aic_mount_setting, "cover_polling_interval", &cover_polling_interval);
                    config_setting_lookup_int(aic_mount_setting, "max_cover_polling_times", &max_cover_polling_times);
                    config_setting_lookup_float(aic_mount_setting, "focus_threshold", &focus_threshold);
                }
                telescopes[i] = new(AICMount(), name, "description", description, "longitude", lon, "latitude", lat,  "elevation", ele, "instruments", instruments, n_instrument, "detector", detectors, n_detector, "filters", filters, n_filter, (void *) 0, mount_address, mount_port, aux_address, aux_port, spectra_address, spectra_port, "home_ra", home_ra, "home_dec", home_dec, "slew_polling_interval", slew_polling_interval, "max_slew_polling_times", max_slew_polling_times, "focus_polling_interval", focus_polling_interval, "max_focus_polling_times", max_focus_polling_times, "focus_threshold", focus_threshold, "derotator_polling_interval", derotator_polling_interval, "max_derotator_polling_times", max_derotator_polling_times, "cover_polling_interval", cover_polling_interval, "max_cover_polling_times", max_cover_polling_times, (void *) 0);
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
        if (telescopes[i] != NULL) {
            __telescope_power_on(telescopes[i]);
            __telescope_init(telescopes[i]);
        }
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
    
    if ((ret = Access(config_path, F_OK)) < 0) {
        if ((ret = Access("telescoped.cfg", F_OK)) == 0) {
            config_path = "telescoped.cfg";
        } else if ((ret = Access("etc/telescoped.cfg", F_OK)) == 0) {
            config_path = "etc/telescoped.cfg";
        } else if ((ret = Access("/opt/aaos/etc/telescoped.cfg", F_OK)) == 0) {
            config_path = "/opt/aaos/etc/telescoped.cfg";
        } else if ((ret = Access("/usr/local/aaos/etc/telescoped.cfg", F_OK)) == 0) {
            config_path = "/usr/local/aaos/etc/telescoped.cfg";
        } else if ((ret = Access("/etc/aaos/telescoped.cfg", F_OK)) == 0) {
            config_path = "/etc/aaos/telescoped.cfg";
        } else if ((ret = Access("/etc/telescoped.cfg", F_OK)) == 0) {
            config_path = "/etc/telescoped.cfg";
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
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        }
    }
    destroy();
    config_destroy(&cfg);
    return 0;
}
