//
//  telescope_client_test.c
//  AAOS
//
//  Created by Hu Yi on 2019/7/24.
//  Copyright © 2019 NAOC. All rights reserved.
//

#include "astro.h"
#include "def.h"
#include "telescope_rpc.h"
#include "wrapper.h"

#include <getopt.h>

#define SPEED_UNIT_NATURE   1
#define SPEED_UNIT_SECOND   2
#define SPEED_UNIT_MINUTE   3
#define SPEED_UNIT_DEGREE   4

#define COORDINATE_FORMAT_DEGREE 1
#define COORDINATE_FORMAT_STRING 2

static struct option longopts[] = {
    {"format",      required_argument,  NULL,       'f' },
    {"help",        no_argument,        NULL,       'h' },
    {"index",       required_argument,  NULL,       'i' },
    {"name",        required_argument,  NULL,       'n' },
    {"telescope",   optional_argument,  NULL,       't' },
    {"unit",        required_argument,  NULL,       'u' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static void
error_handler(int e)
{
    switch (e) {
        case AAOS_ETIMEDOUT:
            fprintf(stderr, "Execution is timed out.\n");
            break;
        case AAOS_ECANCELED:
            fprintf(stderr, "Execution is canceled by other command.\n");
            break;
        case AAOS_EPWROFF:
            fprintf(stderr, "Telescope has been powered off.\n");
            break;
        case AAOS_EUNINIT:
            fprintf(stderr, "Telescope has NOT been initialized.\n");
            break;
        case AAOS_EINVAL:
            fprintf(stderr, "Input parameter is illegal.\n");
            break;
        default:
            fprintf(stderr, "Unknown error happened.\n");
            break;
    }
}

static void
usage()
{
    fprintf(stderr, "Usage:\ttelescope [-h|--help][-v|--version]\n");
    fprintf(stderr, "\t\t[-n|--name <name>] [-i|--index <index>]\n");
    fprintf(stderr, "\t\t[-t|--telescope <`address`:`port`>]\n");
    fprintf(stderr, "\t\t[-f|--format [string | degree]] (default: string)\n");
    fprintf(stderr, "\t\t[-u|--unit [nature | second | minute | degree]] (default: nature)\n");
    exit(EXIT_FAILURE);
}

static double
speed_unit_transformation(const char *s, unsigned int unit)
{
    double speed;
    speed = atof(s);
    switch (unit) {
        case SPEED_UNIT_NATURE:
            speed *= 15. / 3600.;
            break;
        case SPEED_UNIT_SECOND:
            speed /= 3600.;
            break;
        case SPEED_UNIT_MINUTE:
            speed /= 60.;
            break;
        default:
            break;
    }
    return speed;
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
    void *telescope;
    
    unsigned int unit = SPEED_UNIT_NATURE;
    unsigned int format = COORDINATE_FORMAT_STRING;
    
    while ((ch = getopt_long(argc, argv, "f:hi:n:t:Nu:", longopts, NULL)) != -1) {
        switch (ch) {
            case 'f':
                if (strcmp(optarg, "degree") == 0) {
                    format = COORDINATE_FORMAT_DEGREE;
                } else if (strcmp(optarg, "string") == 0) {
                    format = COORDINATE_FORMAT_STRING;
                } else {
                    fprintf(stderr, "Wrong format.\n");
                    fprintf(stderr, "Exit...\n");
                }
                break;
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
            case 't':
                memset(address, '\0', ADDRSIZE);
                memset(port, '\0', PORTSIZE);
                if (optarg != NULL) {
                    char *s = strrchr(optarg, ':');
                    if (s - optarg < ADDRSIZE) {
                        memcpy(address, optarg, s - optarg);
                    } else {
                        fprintf(stderr, "Address is too long.\n");
                        fprintf(stderr, "Exit...\n");
                        exit(EXIT_FAILURE);
                    }
                    if (strlen(s + 1) < PORTSIZE) {
                        snprintf(port, PORTSIZE, "%s", s + 1);
                    } else {
                        fprintf(stderr, "Port is too long.\n");
                        fprintf(stderr, "Exit...\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    snprintf(address, ADDRSIZE, "localhost");
                    snprintf(port, PORTSIZE, "5001");
                }
                break;
            case 'u':
                if (strcmp(optarg, "nature") == 0) {
                    unit = SPEED_UNIT_NATURE;
                } else if (strcmp(optarg, "second") == 0) {
                    unit = SPEED_UNIT_SECOND;
                } else if (strcmp(optarg, "minute") == 0) {
                    unit = SPEED_UNIT_MINUTE;
                } else if (strcmp(optarg, "degree") == 0) {
                    unit = SPEED_UNIT_DEGREE;
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
    
    client = new(TelescopeClient(), address, port);
   
    if ((ret = rpc_client_connect(client, &telescope)) != AAOS_OK) {
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
        ret = telescope_get_index_by_name(telescope, name);
    } else {
        uint16_t idx = index;
        protobuf_set(telescope, PACKET_INDEX, idx);
    }
    
    if (argc == 0) {
        char command[COMMANDSIZE];
        while (fscanf(stdin, "%s", command) != EOF) {
            if (strcmp(command, "power_on") == 0) {
                ret = telescope_power_on(telescope);
                continue;
            }
            if (strcmp(command, "power_off") == 0) {
                ret = telescope_power_off(telescope);
                continue;
            }
            if (strcmp(command, "init") == 0) {
                ret = telescope_init(telescope);
                continue;
            }
            if (strcmp(command, "park") == 0) {
                ret = telescope_park(telescope);
                continue;
            }
            if (strcmp(command, "park_off") == 0) {
                ret = telescope_park_off(telescope);
                continue;
            }
            if (strcmp(command, "stop") == 0) {
                ret = telescope_stop(telescope);
                continue;
            }
            if (strcmp(command, "go_home") == 0) {
                ret = telescope_go_home(telescope);
                continue;
            }
            if (strcmp(command, "move") == 0) {
                char buf[COMMANDSIZE];
                unsigned int direction;
                double duration;
                fscanf(stdin, "%s", buf);
                if (strcmp(buf, "east") == 0) {
                    direction = TELESCOPE_MOVE_EAST;
                } else if (strcmp(buf, "west") == 0) {
                    direction = TELESCOPE_MOVE_WEST;
                } else if (strcmp(buf, "north") == 0) {
                    direction = TELESCOPE_MOVE_NORTH;
                } else if (strcmp(buf, "south") == 0) {
                    direction = TELESCOPE_MOVE_SOUTH;
                } else {
                    fprintf(stderr, "Incorrect moving direction.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                fscanf(stdin, "%lf", &duration);
                if ((ret = telescope_move(telescope, direction, duration)) != AAOS_OK) {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "try_move") == 0) {
                char buf[COMMANDSIZE];
                unsigned int direction;
                double duration;
                fscanf(stdin, "%s", buf);
                if (strcmp(buf, "east") == 0) {
                    direction = TELESCOPE_MOVE_EAST;
                } else if (strcmp(buf, "west") == 0) {
                    direction = TELESCOPE_MOVE_WEST;
                } else if (strcmp(buf, "north") == 0) {
                    direction = TELESCOPE_MOVE_NORTH;
                } else if (strcmp(buf, "south") == 0) {
                    direction = TELESCOPE_MOVE_SOUTH;
                } else {
                    fprintf(stderr, "Incorrect moving direction.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                fscanf(stdin, "%lf", &duration);
                if ((ret = telescope_try_move(telescope, direction, duration)) != AAOS_OK) {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "timed_move") == 0) {
                char buf[COMMANDSIZE];
                unsigned int direction;
                double duration, timeout;
                fscanf(stdin, "%s", buf);
                if (strcmp(buf, "east") == 0) {
                    direction = TELESCOPE_MOVE_EAST;
                } else if (strcmp(buf, "west") == 0) {
                    direction = TELESCOPE_MOVE_WEST;
                } else if (strcmp(buf, "north") == 0) {
                    direction = TELESCOPE_MOVE_NORTH;
                } else if (strcmp(buf, "south") == 0) {
                    direction = TELESCOPE_MOVE_SOUTH;
                } else {
                    fprintf(stderr, "Incorrect moving direction.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                fscanf(stdin, "%lf %lf", &duration, &timeout);
                if ((ret = telescope_timed_move(telescope, direction, duration, timeout)) != AAOS_OK) {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "slew") == 0) {
                double ra, dec;
                if (format == COORDINATE_FORMAT_STRING) {
                    char ra_string[COMMANDSIZE], dec_string[COMMANDSIZE];
                    fscanf(stdin, "%s %s", ra_string, dec_string);
                    ra = hms2deg(ra_string);
                    dec = dms2deg(dec_string);
                } else {
                    fscanf(stdin, "%lf %lf", &ra, &dec);
                }
                if ((ret = telescope_slew(telescope, ra, dec)) != AAOS_OK) {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "try_slew") == 0) {
                double ra, dec;
                if (format == COORDINATE_FORMAT_STRING) {
                    char ra_string[COMMANDSIZE], dec_string[COMMANDSIZE];
                    fscanf(stdin, "%s %s", ra_string, dec_string);
                    ra = hms2deg(ra_string);
                    dec = dms2deg(dec_string);
                } else {
                    fscanf(stdin, "%lf %lf", &ra, &dec);
                }
                if ((ret = telescope_try_slew(telescope, ra, dec)) != AAOS_OK) {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "timed_slew") == 0) {
                double ra, dec, timeout;
                if (format == COORDINATE_FORMAT_STRING) {
                    char ra_string[COMMANDSIZE], dec_string[COMMANDSIZE];
                    fscanf(stdin, "%s %s %lf", ra_string, dec_string, &timeout);
                    ra = hms2deg(ra_string);
                    dec = dms2deg(dec_string);
                } else {
                    fscanf(stdin, "%lf %lf %lf", &ra, &dec, &timeout);
                }
                if ((ret = telescope_timed_slew(telescope, ra, dec, timeout)) != AAOS_OK) {
                    error_handler(ret);
                }
                continue;
            }
            if (strcmp(command, "set") == 0) {
                char field[COMMANDSIZE];
                fscanf(stdin, "%s", field);
                if (strcmp(field, "move_speed") == 0) {
                    double move_speed;
                    char value[COMMANDSIZE];
                    fscanf(stdin, "%s", value);
                    move_speed = speed_unit_transformation(value, unit);
                    ret = telescope_set_move_speed(telescope, move_speed);
                    goto label_set2;
                }
                if (strcmp(field, "slew_speed") == 0) {
                    double slew_speed_x, slew_speed_y;
                    char value1[COMMANDSIZE], value2[COMMANDSIZE];
                    fscanf(stdin, "%s %s", value1, value2);
                    slew_speed_x = speed_unit_transformation(value1, unit);;
                    slew_speed_y = speed_unit_transformation(value2, unit);;
                    ret = telescope_set_slew_speed(telescope, slew_speed_x, slew_speed_y);
                    goto label_set2;
                }
                if (strcmp(field, "track_rate") == 0) {
                    double track_rate_x, track_rate_y;
                    fscanf(stdin, "%lf %lf", &track_rate_x, &track_rate_y);
                    ret = telescope_set_track_rate(telescope, track_rate_x, track_rate_y);
                    goto label_set2;
                }
                fprintf(stderr, "Unknown setting entry `%s`.\n", field);
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            label_set2:
                continue;
            }
            if (strcmp(command, "get") == 0) {
                char field[COMMANDSIZE];
                fscanf(stdin, "%s", field);
                if (strcmp(field, "move_speed") == 0) {
                    double move_speed;
                    ret = telescope_get_move_speed(telescope, &move_speed);
                    printf("move speed\t: %.2f\n", move_speed);
                    goto label_get2;
                }
                if (strcmp(field, "slew_speed") == 0) {
                    double slew_speed_x, slew_speed_y;
                    ret = telescope_get_slew_speed(telescope, &slew_speed_x, &slew_speed_y);
                    printf("slew speed\t: %.2f, %.2f\n", slew_speed_x, slew_speed_y);
                    goto label_get2;
                }
                if (strcmp(field, "track_rate") == 0) {
                    double track_rate_x, track_rate_y;
                    ret = telescope_get_track_rate(telescope, &track_rate_x, &track_rate_y);
                    printf("track rate\t: %.2f, %.2f\n", track_rate_x, track_rate_y);
                    goto label_get2;
                }
                fprintf(stderr, "Unknown getting entry.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            label_get2:
                continue;
            }
            if (strcmp(command, "raw") == 0) {
                char input[BUFSIZE], output[BUFSIZE];
                fscanf(stdin, "%s", input);
                ret = telescope_raw(telescope, input, strlen(input), output, BUFSIZE, NULL);
                if (ret == AAOS_OK) {
                    printf("%s\n", output);
                } else {
                    printf("wrong command.\n");
                }
                continue;
            }
            if (strcmp(command, "status") == 0) {
                char buf[BUFSIZE];
                ret = telescope_status(telescope, buf, BUFSIZE, NULL);
                if (ret == AAOS_OK) {
                    printf("%s\n", buf);
                }
                continue;
            }
            if (strcmp(command, "inspect") == 0) {
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
                continue;
            }
            if (strcmp(command, "register") == 0) {
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
                continue;
            }
        }
    }
    
    while (argc > 0) {
        if (strcmp(argv[0], "power_on") == 0) {
            ret = telescope_power_on(telescope);
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "power_off") == 0) {
            ret = telescope_power_off(telescope);
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "init") == 0) {
            ret = telescope_init(telescope);
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "park") == 0) {
            ret = telescope_park(telescope);
            argc--;
            argv++;
            continue;
        }
        
        if (strcmp(argv[0], "park_off") == 0) {
            ret = telescope_park_off(telescope);
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "stop") == 0) {
            ret = telescope_stop(telescope);
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "go_home") == 0) {
            ret = telescope_go_home(telescope);
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "inspect") == 0) {
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
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "register") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"register\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            
            double timeout = atof(argv[1]);
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
            argc -= 2;
            argv += 2;
            continue;
        }
        if (strcmp(argv[0], "move") == 0) {
            unsigned int direction;
            double duration;
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"move\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "east") == 0) {
                direction = TELESCOPE_MOVE_EAST;
            } else if (strcmp(argv[1], "west") == 0) {
                direction = TELESCOPE_MOVE_WEST;
            } else if (strcmp(argv[1], "north") == 0) {
                direction = TELESCOPE_MOVE_NORTH;
            } else if (strcmp(argv[1], "south") == 0) {
                direction = TELESCOPE_MOVE_SOUTH;
            } else {
                fprintf(stderr, "Incorrect moving direction.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            duration = atof(argv[2]);
            if ((ret = telescope_move(telescope, direction, duration)) != AAOS_OK) {
                error_handler(ret);
            }
            argc -= 3;
            argv += 3;
            continue;
        }
        if (strcmp(argv[0], "try_move") == 0) {
            unsigned int direction;
            double duration;
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"try_move\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "east") == 0) {
                direction = TELESCOPE_MOVE_EAST;
            } else if (strcmp(argv[1], "west") == 0) {
                direction = TELESCOPE_MOVE_WEST;
            } else if (strcmp(argv[1], "north") == 0) {
                direction = TELESCOPE_MOVE_NORTH;
            } else if (strcmp(argv[1], "south") == 0) {
                direction = TELESCOPE_MOVE_SOUTH;
            } else {
                fprintf(stderr, "Incorrect moving direction.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            duration = atof(argv[2]);
            if ((ret = telescope_try_move(telescope, direction, duration)) != AAOS_OK) {
                error_handler(ret);
            }
            argc -= 3;
            argv += 3;
            continue;
        }
        if (strcmp(argv[0], "timed_move") == 0) {
            unsigned int direction;
            double duration;
            double timeout;
            if (argc < 4) {
                fprintf(stderr, "Too few parameters for \"timed_move\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "east") == 0) {
                direction = TELESCOPE_MOVE_EAST;
            } else if (strcmp(argv[1], "west") == 0) {
                direction = TELESCOPE_MOVE_WEST;
            } else if (strcmp(argv[1], "north") == 0) {
                direction = TELESCOPE_MOVE_NORTH;
            } else if (strcmp(argv[1], "south") == 0) {
                direction = TELESCOPE_MOVE_SOUTH;
            } else {
                fprintf(stderr, "Incorrect moving direction.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            duration = atof(argv[2]);
            timeout = atof(argv[3]);
            if ((ret = telescope_timed_move(telescope, direction, duration, timeout)) != AAOS_OK) {
                error_handler(ret);
            }
            argc -= 4;
            argv += 4;
            continue;
        }
        if (strcmp(argv[0], "slew") == 0) {
            double ra, dec;
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"slew\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (format == COORDINATE_FORMAT_STRING) {
                ra = hms2deg(argv[1]);
                dec = dms2deg(argv[2]);
            } else {
                ra = atof(argv[1]);
                dec = atof(argv[2]);
            }
            if ((ret = telescope_slew(telescope, ra, dec)) != AAOS_OK) {
                error_handler(ret);
            }
            argc -= 3;
            argv += 3;
            continue;
        }
        if (strcmp(argv[0], "try_slew") == 0) {
            double ra, dec;
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"slew\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (format == COORDINATE_FORMAT_STRING) {
                ra = hms2deg(argv[1]);
                dec = dms2deg(argv[2]);
            } else {
                ra = atof(argv[1]);
                dec = atof(argv[2]);
            }
            if ((ret = telescope_slew(telescope, ra, dec)) != AAOS_OK) {
                error_handler(ret);
            }
            argc -= 3;
            argv += 3;
            continue;
        }
        if (strcmp(argv[0], "timed_slew") == 0) {
            double ra, dec, timeout;
            if (argc < 4) {
                fprintf(stderr, "Too few parameters for \"timed_slew\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (format == COORDINATE_FORMAT_STRING) {
                ra = hms2deg(argv[1]);
                dec = dms2deg(argv[2]);
            } else {
                ra = atof(argv[1]);
                dec = atof(argv[2]);
            }
            timeout = atof(argv[3]);
            if ((ret = telescope_timed_slew(telescope, ra, dec, timeout)) != AAOS_OK) {
                error_handler(ret);
            }
            argc -= 4;
            argv += 4;
            continue;
        }
        if (strcmp(argv[0], "set") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Too few parameters for \"set\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            argc--;
            argv++;
            if (strcmp(argv[0], "move_speed") == 0) {
                double move_speed;
                move_speed = speed_unit_transformation(argv[1], unit);
                ret = telescope_set_move_speed(telescope, move_speed);
                argc -= 2;
                argv += 2;
                goto label_set;
            }
            if (strcmp(argv[0], "slew_speed") == 0) {
                if (argc < 3) {
                    fprintf(stderr, "Too few parameters for setting slew speed.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                double slew_speed_x, slew_speed_y;
                slew_speed_x = speed_unit_transformation(argv[1], unit);;
                slew_speed_y = speed_unit_transformation(argv[2], unit);;
                ret = telescope_set_slew_speed(telescope, slew_speed_x, slew_speed_y);
                argc -= 3;
                argv += 3;
                goto label_set;
            }
            if (strcmp(argv[0], "track_rate") == 0) {
                if (argc < 3) {
                    fprintf(stderr, "Too few parameters for setting slew speed.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                double track_rate_x, track_rate_y;
                track_rate_x = speed_unit_transformation(argv[1], unit);;
                track_rate_y = speed_unit_transformation(argv[2], unit);;
                ret = telescope_set_track_rate(telescope, track_rate_x, track_rate_y);
                argc -= 3;
                argv += 3;
                goto label_set;
            }
            fprintf(stderr, "Unknown setting entry `%s`.\n", argv[0]);
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        label_set:
            continue;
        }
        if (strcmp(argv[0], "get") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"get\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "move_speed") == 0) {
                double move_speed;
                ret = telescope_get_move_speed(telescope, &move_speed);
                printf("move speed\t: %.2f\n", move_speed);
                goto label_get;
            }
            if (strcmp(argv[1], "slew_speed") == 0) {
                double slew_speed_x, slew_speed_y;
                ret = telescope_get_slew_speed(telescope, &slew_speed_x, &slew_speed_y);
                printf("slew speed\t: %.2f, %.2f\n", slew_speed_x, slew_speed_y);
                goto label_get;
            }
            if (strcmp(argv[1], "track_rate") == 0) {
                double track_rate_x, track_rate_y;
                ret = telescope_get_track_rate(telescope, &track_rate_x, &track_rate_y);
                printf("track rate\t: %.2f, %.2f\n", track_rate_x, track_rate_y);
                goto label_get;
            }
            fprintf(stderr, "Unknown getting entry.\n");
            fprintf(stderr, "Exit...\n");
            exit(EXIT_FAILURE);
        label_get:
            argc -= 2;
            argv += 2;
            continue;
        }
        if (strcmp(argv[0], "raw") == 0) {
            if (argc < 2) {
                fprintf(stderr, "Too few parameters for \"raw\" command.\n");
                fprintf(stderr, "Exit...\n");
                exit(EXIT_FAILURE);
            }
            char buf[BUFSIZE];
            ret = telescope_raw(telescope, argv[1], strlen(argv[1]), buf, BUFSIZE, NULL);
            if (ret == AAOS_OK) {
                printf("%s\n", buf);
            } else {
                printf("wrong command.\n");
            }
            argc -= 2;
            argv += 2;
            continue;
        }
        if (strcmp(argv[0], "status") == 0) {
            char buf[BUFSIZE];
            ret = telescope_status(telescope, buf, BUFSIZE, NULL);
            if (ret == AAOS_OK) {
                printf("%s\n", buf);
            }
            argc-- ;
            argv++;
            continue;
        }
        fprintf(stderr, "Unknown command `%s`.\n", argv[0]);
        argc-- ;
        argv++;
    }
    
    delete(client);
    delete(telescope);
    return 0;
}

