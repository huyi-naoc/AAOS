//
//  detector_server.c
//  AAOS
//
//  Created by Hu Yi on 2025/4/16.
//

#include "adt.h"
#include "def.h"
#include "daemon.h"
#include "rpc.h"
#include "detector_def.h"
#include "detector.h"
#include "detector_rpc.h"
#include "wrapper.h"

#include <libconfig.h>

extern void **detectors;
extern size_t n_detector;
static void *d;
static void *server;
static const char *config_path = "aaos/etc/detectord.cfg";
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
        const char *name = NULL, *username = "aaos", *rootdir = "/opt/aaos", *lockfile = NULL;
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
read_capability(config_setting_t *setting, void *detector)
{
    int iValue;
    double dValue;
    config_setting_t *subsetting;
       
    if (config_setting_lookup_int(setting, "width", &iValue) == CONFIG_TRUE) {
        __detector_set(detector, "width", iValue);
    }
    
    if (config_setting_lookup_int(setting, "height", &iValue) == CONFIG_TRUE) {
        __detector_set(detector, "height", iValue);
    }
    if (config_setting_lookup_int(setting, "n_chip", &iValue) == CONFIG_TRUE) {
        __detector_set(detector, "n_chip", iValue);
    } else {
        __detector_set(detector, "n_chip", 1);
    }
    if (config_setting_lookup_int(setting, "x_n_chip", &iValue) == CONFIG_TRUE) {
        __detector_set(detector, "x_n_chip", iValue);
    } else {
        __detector_set(detector, "x_n_chip", 1);
    }
    if (config_setting_lookup_int(setting, "n_y_chip", &iValue) == CONFIG_TRUE) {
        __detector_set(detector, "y_n_chip", iValue);
    } else {
        __detector_set(detector, "y_n_chip", 1);
    }
    if ((subsetting = config_setting_lookup(setting, "flip_map")) != NULL) {
        int i, n = config_setting_length(subsetting);
        bool *array = Malloc(sizeof(bool) * n);
        for (i = 0; i < n; i++) {
            array[i] = config_setting_get_bool_elem(subsetting, i);
        }
        __detector_set(detector, "flip_map", array, n);
    }
    if ((subsetting = config_setting_lookup(setting, "mirror_map")) != NULL) {
        int i, n = config_setting_length(subsetting);
        bool *array = Malloc(sizeof(bool) * n);
        for (i = 0; i < n; i++) {
            array[i] = config_setting_get_bool_elem(subsetting, i);
        }
        __detector_set(detector, "mirror_map", array, n);
    }
    if (config_setting_lookup_bool(setting, "binning_available", &iValue) == CONFIG_TRUE) {
        bool binning_available = iValue;
        __detector_set(detector, "binning_available", binning_available);
        if (binning_available) {
            if (config_setting_lookup_int(setting, "x_binning_min", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "x_binning_min", iValue);
            }
            if (config_setting_lookup_int(setting, "x_binning_max", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "x_binning_max", iValue);
            }
            if ((subsetting = config_setting_lookup(setting, "x_binning_array")) != NULL) {
                int i, n = config_setting_length(subsetting);
                int *array = Malloc(sizeof(int) * n);
                
                for (i = 0; i < n; i++) {
                    array[i] = config_setting_get_int_elem(subsetting, i);
                }
                __detector_set(detector, "x_binning_array", array, n);
            }
            if (config_setting_lookup_int(setting, "y_binning_min", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "y_binning_min", iValue);
            }
            if (config_setting_lookup_int(setting, "y_binning_max", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "y_binning_max", iValue);
            }
            if ((subsetting = config_setting_lookup(setting, "y_binning_array")) != NULL) {
                int i, n = config_setting_length(subsetting);
                int *array = Malloc(sizeof(int) * n);
                
                for (i = 0; i < n; i++) {
                    array[i] = config_setting_get_int_elem(subsetting, i);
                }
                __detector_set(detector, "y_binning_array", array, n);
            }
        }
    } else {
        __detector_set(detector, "binning_available", false);
    }
    
    if (config_setting_lookup_bool(setting, "offset_available", &iValue) == CONFIG_TRUE) {
        bool offset_available = iValue;
        __detector_set(detector, "offset_available", offset_available);
        if (offset_available) {
            if (config_setting_lookup_int(setting, "x_offset_min", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "x_offset_min", iValue);
            }
            if (config_setting_lookup_int(setting, "x_offset_max", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "x_offset_max", iValue);
            }
            if (config_setting_lookup_int(setting, "y_offset_min", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "y_offset_min", iValue);
            }
            if (config_setting_lookup_int(setting, "y_offset_max", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "y_offset_max", iValue);
            }
            if (config_setting_lookup_int(setting, "image_width_min", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "image_width_min", iValue);
            }
            if (config_setting_lookup_int(setting, "image_width_max", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "image_width_max", iValue);
            }
            if (config_setting_lookup_int(setting, "image_height_min", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "image_height_min", iValue);
            }
            if (config_setting_lookup_int(setting, "image_height_max", &iValue) == CONFIG_TRUE) {
                __detector_set(detector, "image_height_max", iValue);
            }
        }
    } else {
        __detector_set(detector, "offset_available", false);
    }
    
    if (config_setting_lookup_bool(setting, "frame_rate_available", &iValue) == CONFIG_TRUE) {
        bool frame_rate_available = iValue;
        __detector_set(detector, "frame_rate_available", frame_rate_available);
        if (frame_rate_available) {
            if (config_setting_lookup_float(setting, "frame_rate_min", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "frame_rate_min", dValue);
            }
            if (config_setting_lookup_float(setting, "frame_rate_max", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "frame_rate_max", dValue);
            }
        }
    } else {
        __detector_set(detector, "frame_rate_available", false);
    }
    
    if (config_setting_lookup_bool(setting, "exposure_time_available", &iValue) == CONFIG_TRUE) {
        bool exposure_time_available = iValue;
        __detector_set(detector, "exposure_time_available", exposure_time_available);
        if (exposure_time_available) {
            if (config_setting_lookup_float(setting, "exposure_time_min", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "exposure_time_min", dValue);
            } else {
                __detector_set(detector, "exposure_time_min", 0.);
            }
            if (config_setting_lookup_float(setting, "exposure_time_max", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "exposure_time_max", dValue);
            } else {
                __detector_set(detector, "exposure_time_min", 7200.);
            }
        }
    } else {
        __detector_set(detector, "exposure_time_available", true);
        __detector_set(detector, "exposure_time_min", 0.);
        __detector_set(detector, "exposure_time_max", 7200.);
    }
    
    if (config_setting_lookup_bool(setting, "gain_available", &iValue) == CONFIG_TRUE) {
        bool gain_available = iValue;
        __detector_set(detector, "gain_available", gain_available);
        if (gain_available) {
            if (config_setting_lookup_float(setting, "gain_min", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "gain_min", dValue);
            }
            if (config_setting_lookup_float(setting, "gain_max", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "gain_max", dValue);
            }
            if ((subsetting = config_setting_lookup(setting, "gain_array")) != NULL) {
                int i, n = config_setting_length(subsetting);
                double *array = Malloc(sizeof(double) * n);
                
                for (i = 0; i < n; i++) {
                    array[i] = config_setting_get_float_elem(subsetting, i);
                }
                __detector_set(detector, "gain_array", array, n);
            }
        }
    } else {
        __detector_set(detector, "gain_available", false);
    }

    if (config_setting_lookup_bool(setting, "pixel_format_available", &iValue) == CONFIG_TRUE) {
        bool pixel_format_available = iValue;
        __detector_set(detector, "pixel_format_available", pixel_format_available);
        
        if ((subsetting = config_setting_lookup(setting, "pixel_format_array")) != NULL) {
            int i, n = config_setting_length(subsetting);
            unsigned int *array = Malloc(sizeof(unsigned int) * n);
            for (i = 0; i < n; i++) {
                array[i] = config_setting_get_int_elem(subsetting, i);
            }
            __detector_set(detector, "pixel_format_array", array, n);
        }
    } else {
        __detector_set(detector, "pixel_format_available", false);
    }
    
    if (config_setting_lookup_bool(setting, "readout_rate_available", &iValue) == CONFIG_TRUE) {
        bool readout_rate_available = iValue;
        __detector_set(detector, "readout_rate_available", readout_rate_available);
        if (readout_rate_available) {
            if (config_setting_lookup_float(setting, "readout_rate_min", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "readout_rate_min", dValue);
            }
            if (config_setting_lookup_float(setting, "readout_rate_max", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "readout_rate_max", dValue);
            }
            if ((subsetting = config_setting_lookup(setting, "readout_rate_array")) != NULL) {
                int i, n = config_setting_length(subsetting);
                double *array = Malloc(sizeof(double) * n);
                
                for (i = 0; i < n; i++) {
                    array[i] = config_setting_get_float_elem(subsetting, i);
                }
                __detector_set(detector, "readout_rate_array", array, n);
            }
        }
    } else {
        __detector_set(detector, "readout_rate_available", false);
    }
    
    if (config_setting_lookup_bool(setting, "cooling_available", &iValue) == CONFIG_TRUE) {
        bool cooling_available = iValue;
        __detector_set(detector, "cooling_available", cooling_available);
        if (cooling_available) {
            if (config_setting_lookup_float(setting, "cooling_temperature_min", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "cooling_temperature_min", dValue);
            }
            if (config_setting_lookup_float(setting, "cooling_temperature_max", &dValue) == CONFIG_TRUE) {
                __detector_set(detector, "cooling_temperature_max", dValue);
            }
        }
    } else {
        __detector_set(detector, "cooling_available", false);
    }
}

static void
read_configuration(void)
{
    config_setting_t *setting = NULL, *detetcor_setting = NULL;
    size_t i;
    
    setting = config_lookup(&cfg, "server");
    if (setting == NULL) {
        fprintf(stderr, "`server` section does not exist in configuration file.\n");
        fprintf(stderr, "use default detector RPC port.\n");
        server = new (DetectorServer(), DETECTOR_RPC_PORT);
    } else {
        const char *port = NULL, *path = NULL;
        int option = 0;
        
        config_setting_lookup_string(setting, "port", &port);
        if (port == NULL) {
            server = new(DetectorServer(), DETECTOR_RPC_PORT);
        } else {
            server = new(DetectorServer(), port);
        }
        
        config_setting_lookup_string(setting, "path", &path);
        if (path != NULL) {
            tcp_server_set_path(server, path);
        }
        config_setting_lookup_int(setting, "option", &option);
        tcp_server_set_option(server, option);
    }
    
    setting = config_lookup(&cfg, "detectors");
    if (setting == NULL) {
        fprintf(stderr, "`detectors` section does not exist in configuration file.\n");
        exit(EXIT_FAILURE);
    } else {
        n_detector = config_setting_length(setting);
        detectors = (void **) Malloc(n_detector * sizeof(void *));
        memset(detectors, '\0', n_detector * sizeof(void *));
        if (detectors == NULL) {
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < n_detector; i++) {
            detetcor_setting = config_setting_get_elem(setting, (unsigned int) i);
            const char *name = NULL, *description = NULL, *type = NULL, *prefix = NULL, *directory = NULL, *template = NULL;
            
            config_setting_lookup_string(detetcor_setting, "name", &name);
            config_setting_lookup_string(detetcor_setting, "type", &type);
            config_setting_lookup_string(detetcor_setting, "description", &description);
            config_setting_lookup_string(detetcor_setting, "prefix", &prefix);
            config_setting_lookup_string(detetcor_setting, "directory", &directory);
            config_setting_lookup_string(detetcor_setting, "template", &template);
            if (type == NULL) {
                detectors[i] = NULL;
                continue;
            }
            if (strcmp(type, "VIRTUAL") == 0) {
                config_setting_t *capability_setting;
                detectors[i] = new(VirtualDetector(), name, "description", description, "directory", directory, "prefix", prefix, '\0');
		__detector_set_template(detectors[i], template);
                if ((capability_setting = config_setting_lookup(detetcor_setting, "capability")) != NULL) {
                    read_capability(capability_setting, detectors[i]);
                }
            } else if (strcmp(type, "USTCCAMERA") == 0){
                config_setting_t *ustc_camera_setting = NULL;
                const char *so_path = NULL;
                int level = 0, which = 0;
                if ((ustc_camera_setting = config_setting_lookup(detetcor_setting, "ustc_camera")) != NULL) {
                    config_setting_lookup_string(ustc_camera_setting, "so_path", &so_path);
                    config_setting_lookup_int(ustc_camera_setting, "log_level", &level);
                    config_setting_lookup_int(ustc_camera_setting, "which", &which);
                }
                detectors[i] = new(USTCCamera(), name, "description", description, "directory", directory, "prefix", prefix, '\0', so_path, level, which);
		__detector_set_template(detectors[i], template);
            }
#ifdef __USE_ARAVIS__
            else if (strcmp(type, "GenICam") == 0) {
                config_setting_t *genicam_setting = NULL;
                const char *genicam_name = NULL;
                if ((genicam_setting = config_setting_lookup(detetcor_setting, "genicam")) != NULL) {
                    config_setting_lookup_string(ustc_camera_setting, "name", &genicam_name);
                    detectors[i] = new(GenICam(), name, "description", description, "directory", directory, "prefix", prefix, '\0', genicam_name);
		__detector_set_template(detectors[i], template);
                }
            }
#endif
            else {
                
            }
        }
    }
}

static void
init(void)
{
    read_configuration();
    size_t i;
    for (i = 0; i < n_detector; i++) {
        if (detectors[i] != NULL) {
            __detector_init(detectors[i]);
        }
    }
    rpc_server_start(server);
}

static void
destroy(void)
{
    size_t i;
    if (detectors != NULL) {
        for (i = 0; i < n_detector; i++) {
            if (detectors[i] != NULL) {
                delete(detectors[i]);
            }
        }
    }
    free(detectors);

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
        if ((ret = Access("detectord.cfg", F_OK)) == 0) {
            config_path = "detectord.cfg";
        } else if ((ret = Access("etc/detectord.cfg", F_OK)) == 0) {
            config_path = "etc/detectord.cfg";
        } else if ((ret = Access("/opt/aaos/etc/detectord.cfg", F_OK)) == 0) {
            config_path = "/opt/aaos/etc/detectord.cfg";
        } else if ((ret = Access("/usr/local/aaos/etc/detectord.cfg", F_OK)) == 0) {
            config_path = "/usr/local/aaos/etc/detectord.cfg";
        } else if ((ret = Access("/etc/aaos/detectord.cfg", F_OK)) == 0) {
            config_path = "/etc/aaos/detectord.cfg";
        } else if ((ret = Access("/etc/seriald.cfg", F_OK)) == 0) {
            config_path = "/etc/detectord.cfg";
        } else {
            fprintf(stderr, "configuration file does not exist.\n");
            fprintf(stderr, "Exit...\n");
            config_destroy(&cfg);
            exit(EXIT_FAILURE);
        }
    }
    
    if(config_read_file(&cfg, config_path) == CONFIG_FALSE) {
        fprintf(stderr, "fail to read configuration file.\n");
        fprintf(stderr, "Exit...\n");
        config_destroy(&cfg);
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

