//
//  detector_client.c
//  AAOS
//
//  Created by Hu Yi on 2025/4/17.
//

#include "def.h"
#include "detector_rpc.h"
#include "utils.h"
#include "wrapper.h"

#include <getopt.h>

#define DURATION_UNIT_SECOND        1
#define DURATION_UNIT_MINISECOND    2

#define COORDINATE_FORMAT_DEGREE 1
#define COORDINATE_FORMAT_STRING 2

static struct option longopts[] = {
    {"detector",    optional_argument,  NULL,       'd' },
    {"help",        no_argument,        NULL,       'h' },
    {"header",      no_argument,        NULL,       'H' },
    {"index",       required_argument,  NULL,       'i' },
    {"name",        required_argument,  NULL,       'n' },
    {"unit",        required_argument,  NULL,       'u' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static const char *help_string = "\
Usage:  detector [options] COMMAND [PARAMETERS ... ]\n\
        -h, --help        print help doc and exit\n\
        -i, --index       <index>, specify telescope's index\n\
        -n, --name        <name>, specify telescope's name\n\
        -t, --telescope   <address:[port]> address (and port) of telescoped\n\
        -u, --unit        <unit>, specify parameter unit for expose command\n\
                          supported units can be s (second), or ms (milisecond)\n\
Commands:\n\
    abort\n\
    enable      NAME\n\
                NAME can be cooling\n\
    disable     NMAE\n\
                NAME can be cooling\n\
    expose      EXPTIME NFRAMES\n\
    info\n\
    init\n\
    inspect\n\
    status\n\
    stop\n\
    get         NAME\n\
                NAME can be binning, capture_mode, directory, exptime,\n\
                frame_rate, gain, overscan, pixel_format, prefix,\n\
                readout_rate, region, temperature, trigger_mode\n\
    set         NAME VALUE1 ... \n\
                binning parameter has two values\n\
                capture_mode parameter has one value\n\
                directory parameter has one value\n\
                exptime parameter has one value\n\
                frame_rate parameter has one value\n\
                gain parameter has one value\n\
                overscan parameter has two values\n\
                pixel_format parameter has one value\n\
                readout_rate parameter has one value\n\
                region parameter has four values\n\
                temperature parameter has one value\n\
                trigger_mode parameter has one value\n\
    register    TIMEOUT\n\
";



unsigned int unit = 1;
static const char *header;

static void
error_handler(int e)
{
    switch (e) {
        case AAOS_ETIMEDOUT:
            fprintf(stderr, "Operation is timed out.\n");
            break;
        case AAOS_ECANCELED:
            fprintf(stderr, "Operation is canceled by another command.\n");
            break;
        case AAOS_EPWROFF:
            fprintf(stderr, "Detector has been powered off.\n");
            break;
        case AAOS_EUNINIT:
            fprintf(stderr, "Detector has NOT been initialized.\n");
            break;
        case AAOS_EINVAL:
            fprintf(stderr, "Input parameter is illegal.\n");
            break;
        case AAOS_ENOTSUP:
            fprintf(stderr, "Operation is NOT supported.\n");
            break;
        case AAOS_ENOTFOUND:
            fprintf(stderr, "Detector is not found.\n");
            break;
        default:
            fprintf(stderr, "Unknown error happened.\n");
            break;
    }
}

static void
usage(void)
{
    fprintf(stderr, "%s", help_string);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int ch;
    char address[ADDRSIZE], port[PORTSIZE];
    uint16_t index = 1;
    const char *name = NULL;
    
    int ret;
    void *client;
    void *detector;
    
    struct stat stat_buf;
    
    if (Stat("/opt/aaos/run/detector.sock", &stat_buf) == 0 && S_ISSOCK(stat_buf.st_mode)) {
        snprintf(address, ADDRSIZE, "/opt/aaos/run/detectord.sock");
    } else if (Stat("/usr/local/aaos/run/detectord.sock", &stat_buf) == 0 && S_ISSOCK(stat_buf.st_mode)) {
        snprintf(address, ADDRSIZE, "/usr/local/aaos/run/detectord.sock");
    } else if (Stat("/run/detectord.sock", &stat_buf) == 0 && S_ISSOCK(stat_buf.st_mode)) {
        snprintf(address, ADDRSIZE, "/run/detectord.sock");
    } else {
        snprintf(address, ADDRSIZE, "localhost");
        snprintf(port, PORTSIZE, DET_RPC_PORT);
    }
        
    while ((ch = getopt_long(argc, argv, "d:hi:n:u:v", longopts, NULL)) != -1) {
        switch (ch) {
            case 'h':
                usage();
                break;
            case 'i':
                index = atoi(optarg);
                break;
            case 'n':
                name = optarg;
                index = 0;
                break;
            case 'd':
                memset(address, '\0', ADDRSIZE);
                memset(port, '\0', PORTSIZE);
                parse_addr_port(optarg, address, ADDRSIZE, port, PORTSIZE, "localhost", DET_RPC_PORT);
                break;
            case 'H':
                header = optarg;
                break;
            case 'u':
                if (strcmp(optarg, "s") == 0) {
                    unit = DURATION_UNIT_SECOND;
                } else if (strcmp(optarg, "ms") == 0) {
                    unit = DURATION_UNIT_MINISECOND;
                } else {
                    fprintf(stderr, "Wrong unit.\n");
                    fprintf(stderr, "Exit...\n");
                }
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    client = new(DetectorClient(), address, port);
   
    if ((ret = rpc_client_connect(client, &detector)) != AAOS_OK) {
        switch (ret) {
            case AAOS_ECONNREFUSED:
                fprintf(stderr, "Port `%s` on `%s` might not be listened.\n", port, address);
                break;
            case AAOS_ENETUNREACH:
                fprintf(stderr, "Network is unreachable.\n");
                break;
            case AAOS_ETIMEDOUT:
                fprintf(stderr, "Connecting is timeout.\n");
                break;
            default:
            {
                char buf[BUFSIZE];
                strerror_r(errno, buf, BUFSIZE);
                fprintf(stderr, "%s.\n", buf);
            }
                break;
        }
        exit(EXIT_FAILURE);
    }
    
    if (name != NULL) {
        ret = detector_get_index_by_name(detector, name);
    } else {
        uint16_t idx = index;
        protobuf_set(detector, PACKET_INDEX, idx);
    }
    
    if (argc == 0) {
        char command[COMMANDSIZE];
        while (fscanf(stdin, "%s", command) != EOF) {
            if (strcmp(command, "expose") == 0) {
                double exposure_time;
                uint32_t n_frame;
                uint16_t options;
                fscanf(stdin, "%lf %u", &exposure_time, &n_frame);
                ret = detector_get_option(detector, &options);
                ret = detector_expose(detector, exposure_time, n_frame, NULL, header);
                if (ret == AAOS_OK) {
                    fprintf(stderr, "expose success.\n");
                } else {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "power_off") == 0) {
                ret = detector_power_on(detector);
                if (ret == AAOS_OK) {
                    fprintf(stderr, "power_off success.\n");
                } else {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "power_on") == 0) {
                ret = detector_power_on(detector);
                if (ret == AAOS_OK) {
                    fprintf(stderr, "power_on success.\n");
                } else {
                    error_handler(ret);
                }
                continue;
            }
            /*
            if (strcmp(command, "init") == 0) {
                //      ret = detector_init(detector);
                if (ret == AAOS_OK) {
                    fprintf(stderr, "init success.\n");
                } else {
                    error_handler(ret);
                }
                continue;
            }
             */
            if (strcmp(command, "get") == 0) {
                char field[COMMANDSIZE];
                
                fscanf(stdin, "%s", field);
                if (strcmp(field, "binning") == 0) {
                    uint32_t x_binning, y_binning;
                    if ((ret = detector_get_binning(detector, &x_binning, &y_binning)) == AAOS_OK) {
                        fprintf(stdout, "%u %u\n", x_binning, y_binning);
                    } else {
                        error_handler(ret);
                    }
                } else if (strcmp(field, "exptime") == 0) {
                    double exposure_time;
                    if ((ret = detector_get_exposure_time(detector, &exposure_time)) == AAOS_OK) {
                        fprintf(stdout, "%.3f\n", exposure_time);
                    } else {
                        error_handler(ret);
                    }
                } else if (strcmp(field, "framerate") == 0) {
                    double frame_rate;
                    if ((ret = detector_get_frame_rate(detector, &frame_rate)) == AAOS_OK) {
                        fprintf(stdout, "%.3f\n", frame_rate);
                    } else {
                        error_handler(ret);
                    }
                } else if (strcmp(field, "gain") == 0) {
                    double gain;
                    if ((ret = detector_get_gain(detector, &gain)) == AAOS_OK) {
                        fprintf(stdout, "%.3f\n", gain);
                    } else {
                        error_handler(ret);
                    }
                } else if (strcmp(field, "readout_rate") == 0) {
                    double readout_rate;
                    
                    if ((ret = detector_get_readout_rate(detector, &readout_rate)) == AAOS_OK) {
                        fprintf(stdout, "%.3f\n", readout_rate);
                    } else {
                        error_handler(ret);
                    }
                    goto label_get2;
                }
                if (strcmp(field, "region") == 0) {
                    uint32_t x, y, width, height;
                    
                    if ((ret = detector_get_region(detector, &x, &y, &width, &height)) == AAOS_OK) {
                        fprintf(stdout, "%u %u %u %u\n", x, y, width, height);
                    } else {
                        error_handler(ret);
                    }
                    goto label_get2;
                }
                fprintf(stderr, "Unknown setting entry `%s`.\n", field);
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            label_get2:
                continue;
            }
            if (strcmp(command, "set") == 0) {
                char field[COMMANDSIZE];
                fscanf(stdin, "%s", field);
                if (strcmp(field, "binning") == 0) {
                    uint32_t x_binning, y_binning;
                    fscanf(stdin, "%ud %ud", &x_binning, &y_binning);
                    ret = detector_set_binning(detector, x_binning, y_binning);
                    goto label_set2;
                }
                if (strcmp(field, "exptime") == 0) {
                    double exposure_time;
                    fscanf(stdin, "%lf", &exposure_time);
                    ret = detector_set_exposure_time(detector, exposure_time);
                    goto label_set2;
                }
                if (strcmp(field, "framerate") == 0) {
                    double frame_rate;
                    fscanf(stdin, "%lf", &frame_rate);
                    ret = detector_set_exposure_time(detector, frame_rate);
                    goto label_set2;
                }
                if (strcmp(field, "gain") == 0) {
                    double gain;
                    fscanf(stdin, "%lf", &gain);
                    ret = detector_set_gain(detector, gain);
                    goto label_set2;
                }
                if (strcmp(field, "readout_rate") == 0) {
                    double readout_rate;
                    fscanf(stdin, "%lf", &readout_rate);
                    ret = detector_set_readout_rate(detector, readout_rate);
                    goto label_set2;
                }
                if (strcmp(field, "region") == 0) {
                    uint32_t x, y, width, height;
                    fscanf(stdin, "%ud %ud %ud %ud", &x, &y, &width, &height);
                    ret = detector_set_region(detector, x, y, width, height);
                    goto label_set2;
                }
                fprintf(stderr, "Unknown setting entry `%s`.\n", field);
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            label_set2:
                if (ret == AAOS_OK) {
                    fprintf(stderr, "set %s success.\n", field);
                } else {
                    error_handler(ret);
                }
                continue;
            }
            
            if (strcmp(command, "raw") == 0) {
                char input[BUFSIZE], *output;
                output = (char *) Malloc(BUFSIZE * 256);
                fscanf(stdin, "%s", input);
                ret = detector_raw(detector, input, strlen(input), output, BUFSIZE * 256, NULL);
                if (ret == AAOS_OK) {
                    printf("%s\n", output);
                } else {
                    printf("wrong command.\n");
                }
                free(output);
                continue;
            }
            if (strcmp(command, "status") == 0) {
                char buf[BUFSIZE];
                ret = detector_status(detector, buf, BUFSIZE, NULL);
                if (ret == AAOS_OK) {
                    printf("%s\n", buf);
                }
                continue;
            }
            if (strcmp(command, "inspect") == 0) {
                /*
                ret = telescope_inspect(telescope);
                switch (ret) {
                    case AAOS_OK:
                        printf("Telescope is OK\n");
                        break;
                    case AAOS_ENOTSUP:
                        printf("Inspect command is not supported.\n");
                        break;
                    default:
                        printf("Telescope malfunctions.\n");
                        break;
                }
                 */
                continue;
            }
            if (strcmp(command, "register") == 0) {
                /*
                double timeout;
                fscanf(stdin, "%lf", &timeout);
                ret = telescope_register(telescope, timeout);
                switch (ret) {
                    case AAOS_OK:
                        printf("Telescope is OK now.\n");
                        break;
                    case AAOS_ETIMEDOUT:
                        printf("Telescope malfunctions until timed out.\n");
                        break;
                    default:
                        printf("Telescope malfunctions.\n");
                        break;
                }
                 */
                continue;
            }
        }
    }
    
    while (argc > 0) {
        if (strcmp(argv[0], "power_on") == 0) {
            ret = detector_power_on(detector);
            if (ret == AAOS_OK) {
                fprintf(stderr, "power_on success.\n");
            } else {
                //error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "power_off") == 0) {
            ret = detector_power_off(detector);
            if (ret == AAOS_OK) {
                fprintf(stderr, "power_off success.\n");
            } else {
                error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        
        if (strcmp(argv[0], "disable") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"disable\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "cooling") == 0) {
                if ((ret = detector_disable_cooling(detector)) == AAOS_OK) {
                    fprintf(stderr, "disable_cooling success.\n");
                } else {
                    error_handler(ret);
                }
            } else {
                fprintf(stderr, "Unknown disable operation.\n");
            }
            argc -= 2;
            argv += 2;
	    continue;
        }
        if (strcmp(argv[0], "enable") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"enable\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "cooling") == 0) {
                if ((ret = detector_enable_cooling(detector)) == AAOS_OK) {
                    fprintf(stderr, "enable_cooling success.\n");
                } else {
                    error_handler(ret);
                }
            } else {
                fprintf(stderr, "Unknown disable operation.\n");
            }
            argc -= 2;
            argv += 2;
	    continue;
        }
        if (strcmp(argv[0], "expose") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"expose\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            double exposure_time;
            uint32_t n_frame;
            uint16_t options;
            
            exposure_time = atof(argv[1]);
            if (unit == DURATION_UNIT_MINISECOND) {
                exposure_time /= 1000.;
            }
            n_frame = atoi(argv[2]);
            detector_get_option(detector, &options);
            ret = detector_expose(detector, exposure_time, n_frame, NULL, header);
            if (ret == AAOS_OK) {
                fprintf(stderr, "expose success.\n");
            } else {
                error_handler(ret);
            }
            argc -= 3;
            argv += 3;
            continue;
        }
        if (strcmp(argv[0], "get") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"get\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "binning") == 0) {
                uint32_t x_binning, y_binning;
                if ((ret = detector_get_binning(detector, &x_binning, &y_binning)) == AAOS_OK) {
                    fprintf(stdout, "%u %u\n", x_binning, y_binning);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "capture_mode") == 0) {
                uint32_t capture_mode;
                if ((ret = detector_get_capture_mode(detector, &capture_mode)) == AAOS_OK) {
                    fprintf(stdout, "%un", capture_mode);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "directory") == 0) {
                char directory[PATHSIZE];
                if ((ret = detector_get_directory(detector, directory, PATHSIZE)) == AAOS_OK) {
                    fprintf(stdout, "%s\n", directory);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "exptime") == 0) {
                double exposure_time;
                if ((ret = detector_get_exposure_time(detector, &exposure_time)) == AAOS_OK) {
                    fprintf(stdout, "%.3f\n", exposure_time);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "framerate") == 0) {
                double frame_rate;
                if ((ret = detector_get_frame_rate(detector, &frame_rate)) == AAOS_OK) {
                    fprintf(stdout, "%.3f\n", frame_rate);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "gain") == 0) {
                double frame_rate;
                if ((ret = detector_get_frame_rate(detector, &frame_rate)) == AAOS_OK) {
                    fprintf(stdout, "%.3f\n", frame_rate);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "overscan") == 0) {
                uint32_t x_overscan, y_overscan;
                if ((ret = detector_get_overscan(detector, &x_overscan, &y_overscan)) == AAOS_OK) {
                    fprintf(stdout, "%u %u\n", x_overscan, y_overscan);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "pixel_format") == 0) {
                uint32_t pixel_format;
                if ((ret = detector_get_pixel_format(detector, &pixel_format)) == AAOS_OK) {
                    fprintf(stdout, "%un", pixel_format);
                } else {
                    error_handler(ret);
                }
            }  else if (strcmp(argv[1], "prefix") == 0) {
                char prefix[PATHSIZE];
                if ((ret = detector_get_prefix(detector, prefix, PATHSIZE)) == AAOS_OK) {
                    fprintf(stdout, "%s\n", prefix);
                } else {
                    error_handler(ret);
                }
            }  else if (strcmp(argv[1], "readout_rate") == 0) {
                double readout_rate;
                if ((ret = detector_get_readout_rate(detector, &readout_rate)) == AAOS_OK) {
                    fprintf(stdout, "%.3f\n", readout_rate);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "region") == 0) {
                uint32_t x, y, width, height;
                if ((ret = detector_get_region(detector, &x, &y, &width, &height)) == AAOS_OK) {
                    fprintf(stdout, "%-5u %-5u %-5u %-5u\n", x, y, width, height);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "temperature") == 0) {
                double temperature;
                if ((ret = detector_get_temperature(detector, &temperature)) == AAOS_OK) {
                    fprintf(stdout, "%.2f\n", temperature);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "template") == 0) {
                char template[PATHSIZE];
                if ((ret = detector_get_template(detector, template, PATHSIZE)) == AAOS_OK) {
                    fprintf(stdout, "%s\n", template);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "trigger_mode") == 0) {
                uint32_t trigger_mode;
                if ((ret = detector_get_trigger_mode(detector, &trigger_mode)) == AAOS_OK) {
                    fprintf(stdout, "%un", trigger_mode);
                } else {
                    error_handler(ret);
                }
            } else {
                argc--;
                argv++;
                continue;
            }
            argc -= 2;
            argv += 2;
            continue;
        }
        if (strcmp(argv[0], "info") == 0) {
            char buf[BUFSIZE];
            ret = detector_info(detector, buf, BUFSIZE, NULL);
            if (ret == AAOS_OK) {
                printf("%s\n", buf);
            } else {
                error_handler(ret);
            }
            argc-- ;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "raw") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"raw\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            char *buf = (char *) Malloc(BUFSIZE * 256);
            memset(buf, '\0', BUFSIZE * 256);
            ret = detector_raw(detector, argv[1], strlen(argv[1]), buf, BUFSIZE * 256, NULL);
            if (ret == AAOS_OK) {
                if (buf[0] != '\0') {
                    fprintf(stdout, "%s\n", buf);
                }
            } else if (ret == AAOS_EBADCMD) {
                fprintf(stderr, "Wrong command: `%s`.\n", argv[1]);
            } else {
                error_handler(ret);
            }
            free(buf);
            argc -= 2;
            argv += 2;
            continue;
        }
        if (strcmp(argv[0], "status") == 0) {
            char buf[BUFSIZE];
            ret = detector_status(detector, buf, BUFSIZE, NULL);
            if (ret == AAOS_OK) {
                printf("%s\n", buf);
            } else {
                error_handler(ret);
            }
            argc-- ;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "stop") == 0) {
            ret = detector_stop(detector);
            if (ret != AAOS_OK) {
                error_handler(ret);
            } else {
                fprintf(stderr, "stop command success.\n");
            }
            argc-- ;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "abort") == 0) {
            ret = detector_abort(detector);
            if (ret != AAOS_OK) {
                error_handler(ret);
            } else {
                fprintf(stderr, "abort command success.\n");
            }
            argc-- ;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "set") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"set\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "binning") == 0) {
                if (argc < 4) {
                    fprintf(stderr, "Too few parameters for \"set binning\" command.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                uint32_t x_binning, y_binning;
                x_binning = atoi(argv[2]);
                y_binning = atoi(argv[3]);
                
                if ((ret = detector_set_binning(detector, x_binning, y_binning)) == AAOS_OK) {
                    fprintf(stderr, "set binning success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 4;
                argv += 4;
            } else if (strcmp(argv[1], "capture_mode") == 0) {
                uint32_t capture_mode;
                capture_mode = atoi(argv[2]);
                if ((ret = detector_set_capture_mode(detector, capture_mode)) == AAOS_OK) {
                    fprintf(stderr, "set capture mode success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "directory") == 0) {
                if ((ret = detector_set_directory(detector, argv[2])) == AAOS_OK) {
                    fprintf(stderr, "set directory success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "exptime") == 0) {
                double exposure_time = atof(argv[2]);
                if ((ret = detector_set_exposure_time(detector, exposure_time)) == AAOS_OK) {
                    fprintf(stderr, "set exptime success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "framerate") == 0) {
                double frame_rate = atof(argv[2]);
                if ((ret = detector_set_frame_rate(detector, frame_rate)) == AAOS_OK) {
                    fprintf(stderr, "set framerate success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "gain") == 0) {
                double gain = atof(argv[2]);
                if ((ret = detector_set_gain(detector, gain)) == AAOS_OK) {
                    fprintf(stderr, "set gain success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "overscan") == 0) {
                if (argc < 4) {
                    fprintf(stderr, "Too few parameters for \"set overscan\" command.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                uint32_t x_overscan, y_overscan;
                x_overscan = atoi(argv[2]);
                y_overscan = atoi(argv[3]);
                
                if ((ret = detector_set_overscan(detector, x_overscan, y_overscan)) == AAOS_OK) {
                    fprintf(stderr, "set overscan success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 4;
                argv += 4;
            } else if (strcmp(argv[1], "pixel_format") == 0) {
                uint32_t pixel_format;
                pixel_format = atoi(argv[2]);
                if ((ret = detector_set_pixel_format(detector, pixel_format)) == AAOS_OK) {
                    fprintf(stderr, "set pixel format success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "prefix") == 0) {
                if ((ret = detector_set_prefix(detector, argv[2])) == AAOS_OK) {
                    fprintf(stderr, "set prefix success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "readout_rate") == 0) {
                double readout_rate = atof(argv[2]);
                if ((ret = detector_set_readout_rate(detector, readout_rate)) == AAOS_OK) {
                    fprintf(stderr, "set readout rate success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "region") == 0) {
                if (argc < 6) {
                    fprintf(stderr, "Too few parameters for \"set region\" command.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                uint32_t x, y, width, height;
                x = atoi(argv[2]);
                y = atoi(argv[3]);
                width = atoi(argv[4]);
                height = atoi(argv[5]);
                if ((ret = detector_set_region(detector, x, y, width, height)) == AAOS_OK) {
                    fprintf(stderr, "set readout rate success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 6;
                argv += 6;
            } else if (strcmp(argv[1], "temperature") == 0) {
                double temperature = atof(argv[2]);
                if ((ret = detector_set_temperature(detector, temperature)) == AAOS_OK) {
                    fprintf(stderr, "set temperature success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "template") == 0) {
                if ((ret = detector_set_template(detector, argv[2])) == AAOS_OK) {
                    fprintf(stderr, "set template success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else if (strcmp(argv[1], "trigger_mode") == 0) {
                uint32_t trigger_mode;
                trigger_mode = atoi(argv[2]);
                if ((ret = detector_set_trigger_mode(detector, trigger_mode)) == AAOS_OK) {
                    fprintf(stderr, "set trigger mode success.\n");
                } else {
                    error_handler(ret);
                }
                argc -= 3;
                argv += 3;
            } else {
                fprintf(stderr, "Unknown setting entry `%s`.\n", argv[1]);
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        fprintf(stderr, "Unknown command `%s`.\n", argv[0]);
        argc-- ;
        argv++;
    }
    delete(client);
    delete(detector);
    
    return 0;
}
