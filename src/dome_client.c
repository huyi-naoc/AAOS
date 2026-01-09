//
//  dome_client.c
//  AAOS
//
//  Created by Hu Yi on 2025/5/12.
//

#include "def.h"
#include "dome_rpc.h"
#include "wrapper.h"

extern size_t n_dome;
extern void **domes;

static struct option longopts[] = {
    {"binary",      no_argument,        NULL,       'b' },
    {"check",       no_argument,        NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"index",       required_argument,  NULL,       'i' },
    {"name",        required_argument,  NULL,       'n' },
    {"path",        required_argument,  NULL,       'p' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};


static const char *help_string = "\
Usage:  dome [options]    [COMMAND1, COMMAND2, ...\n\
        -b, --binary      set how seriald inteprets the input COMMANDs as hex\n\
                          strings rather than ASCII character strings\n\
        -c, --check       check whether the serial device works\n\
        -d, --delay       <timeout>, set timeout of -r option\n\
        -h, --help        print help doc and exit\n\
        -i, --index       <index>, specify serial's index\n\
        -n, --name        <name>, specify serial's name\n\
        -p, --path        <path>, specify serial's devpath\n\
        -s, --serial      <address:[port]> address (and port) of seriald\n\
        -u, --unit        <unit>, specify parameter unit for setting command\n\
                          supported formats are nature, second, minute,\n\
                          and degree, default is nature\n\n\
Commands:\n\
    init\n\
    open\n\
    close\n\
    stop\n\
    status\n\
    inspect\n\
    register    TIMEOUT\n\
    set         NAME\n\
                NAME can be open_speed and close_speed\n\
    get         NAME\n\
                NAME can be position, open_speed and close_speed\n\
";


static void
usage(void)
{
    exit(EXIT_FAILURE);
}

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
            fprintf(stderr, "Telescope has been powered off.\n");
            break;
        case AAOS_EUNINIT:
            fprintf(stderr, "Telescope has NOT been initialized.\n");
            break;
        case AAOS_EINVAL:
            fprintf(stderr, "Input parameter is illegal.\n");
            break;
        case AAOS_ENOTSUP:
            fprintf(stderr, "Operation is NOT supported.\n");
            break;
        case AAOS_ENOTFOUND:
            fprintf(stderr, "Dome is NOT found.\n");
            break;
        default:
            fprintf(stderr, "Unknown error happened.\n");
            break;
    }
}

static void
fatal_handler(int error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    
    switch (error) {
        case -1 * AAOS_ENETDOWN:
            fprintf(stderr, "Network is down.\n");
            break;
        case -1 * AAOS_ENETUNREACH:
            fprintf(stderr, "Network is unreachable\n");
            break;
        case -1 * AAOS_EPIPE:
            fprintf(stderr, "The dome server is absent.\n");
            break;
        case -1 * AAOS_ETIMEDOUT:
            fprintf(stderr, "No response from the dome server.\n");
            break;
        default:
            vfprintf(stderr, fmt, ap);
            break;
            break;
    }
    
    va_end(ap);
    fprintf(stderr, "Exit...\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    const char *dome_name = NULL;
    void *client, *dome;
    int ret;
    char buf[BUFSIZE], address[ADDRSIZE], port[PORTSIZE], *s;
    int ch;
    int index = 1;
    bool check = false;
    bool reg = false;
    double delay = 0.;
    
	
    if (Access("/opt/aaos/run/domed.sock", F_OK) == 0) {
        snprintf(address, ADDRSIZE, "/opt/aaos/run/domed.sock");
    } else if (Access("/usr/local/aaos/run/domed.sock", F_OK) == 0) {
        snprintf(address, ADDRSIZE, "/usr/local/aaos/run/domed.sock");
    } else if (Access("/run/domed.sock", F_OK) == 0) {
        snprintf(address, ADDRSIZE, "/run/domed.sock");
    } else {
        snprintf(address, ADDRSIZE, "localhost");
        snprintf(port, PORTSIZE, DOM_RPC_PORT);
    }
    
    while ((ch = getopt_long(argc, argv, "a:cd:hi:n:r", longopts, NULL)) != -1) {
        switch (ch) {
            case 'a':
                if (Access(optarg, F_OK) == 0) { // UNIX domain socket
                    snprintf(address, ADDRSIZE, "%s", optarg);
                } else {
                    s = strrchr(optarg, ':');
                    if (s == NULL) { //input like "example.com"
                        memset(address, '\0', ADDRSIZE);
                        if (strlen(optarg) >= ADDRSIZE) {
                            fprintf(stderr, "Address is too long.\n");
                            fprintf(stderr, "Exit...\n");
                            exit(EXIT_FAILURE);
                        }
                        snprintf(address, ADDRSIZE, "%s", optarg);
                    } else {
                        if (s == optarg) { // input like :8000
                            s++;
                            if (strlen(s) >= PORTSIZE) {
                                fprintf(stderr, "Port is too long.\n");
                                fprintf(stderr, "Exit...\n");
                                exit(EXIT_FAILURE);
                            }
                            snprintf(address, PORTSIZE, "%s", s);
                        } else { //input like localhost:8000
                            memset(address, '\0', ADDRSIZE);
                            memset(port, '\0', PORTSIZE);
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
                        }
                    }
                }
                break;
            case 'c':
                check = true;
                break;
            case 'd':
                delay = atof(optarg);
                break;
            case 'h':
                usage();
                break;
            case 'i':
                index = atoi(optarg);
                break;
            case 'n':
                dome_name = optarg;
                index = 0;
                break;
            case 'r':
                reg = false;
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    client = new(DomeClient(), address, port);
    
    if ((ret = rpc_client_connect(client, &dome)) != AAOS_OK) {
        delete(dome);
        delete(client);
        fatal_handler(ret, "%s", "Cannot connect to dome server.\n");
    }
        
    if (index == 0) {
        if (dome_name != NULL) {
            if ((ret = dome_get_index_by_name(dome, dome_name)) != AAOS_OK) {
                delete(dome);
                delete(client);
                fatal_handler(ret, "`%s` is not found in the device list in the dome server's configuration file.\n", dome_name);
            } else {
                protobuf_set(dome, PACKET_STR, dome_name);
            }
        } else {
            delete(dome);
            delete(client);
            fprintf(stderr, "No dome device is specified.\n");
            fprintf(stderr, "Exit...");
            exit(EXIT_FAILURE);
            
        }
    } else {
        uint16_t idx = (uint16_t) index;
        protobuf_set(dome, PACKET_INDEX, idx);
    }
    
    if (check) {
        ret = dome_inspect(dome);
        delete(dome);
        delete(client);
        switch (ret) {
            case AAOS_OK:
                if (index == 0) {
                    if (dome_name != NULL) {
                        fprintf(stderr, "`%s` is OK.\n", dome_name);
                    }
                } else {
                    fprintf(stderr, "%d is OK", index);
                }
                exit(EXIT_SUCCESS);
                break;
            default:
                exit(EXIT_FAILURE);
                break;
        }
    }
    
    if (reg) {
        ret = rpc_register(dome, delay);
        delete(dome);
        delete(client);
        switch (ret) {
            case AAOS_OK:
                if (index == 0) {
                    if (dome_name != NULL) {
                        fprintf(stderr, "`%s` is OK, you can operate it.\n", dome_name);
                    }
                } else {
                    fprintf(stderr, "%d is OK, you can operate it.\n", index);
                }
                exit(EXIT_SUCCESS);
                break;
            default:
                if (index == 0) {
                    if (dome_name != NULL) {
                        fprintf(stderr, "waiting for `%s` recovery timed out.\n", dome_name);
                    }
                } else {
                    fprintf(stderr, "waiting for %d recovery timed out.\n", index);
                }
                exit(EXIT_SUCCESS);
                break;
        }
    }
    
    while (argc != 0) {
        if (strcmp(argv[0], "open_window") == 0) {
            if ((ret = dome_open_window(dome)) == AAOS_OK) {
                fprintf(stderr, "open dome's window success.\n");
            } else {
                error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "close_window") == 0) {
            if ((ret = dome_close_window(dome)) == AAOS_OK) {
                fprintf(stderr, "close dome's window success.\n");
            } else {
                error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "stop_window") == 0) {
            if ((ret = dome_stop_window(dome)) == AAOS_OK) {
                fprintf(stderr, "stop dome's window success.\n");
            } else {
                error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "init") == 0) {
            if ((ret = dome_init(dome)) == AAOS_OK) {
                fprintf(stderr, "initialize dome success.\n");
            } else {
                error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "status") == 0) {
            char buf[BUFSIZE];
            if ((ret = dome_status(dome, buf, BUFSIZE, NULL)) == AAOS_OK) {
                fprintf(stderr, "%s\n", buf);
            } else {
                error_handler(ret);
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[0], "set") == 0) {
            if (argc < 3) {
                fprintf(stderr, "set command needs two parameters.\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "window_open_speed") == 0) {
                double speed = atof(argv[2]);
                if ((ret = dome_set_window_open_speed(dome, speed)) == AAOS_OK) {
                    fprintf(stderr, "set dome's window_open_speed success.\n");
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "window_close_speed") == 0) {
                double speed = atof(argv[2]);
                if ((ret = dome_set_window_close_speed(dome, speed)) == AAOS_OK) {
                    fprintf(stderr, "set dome's window_close_speed success.\n");
                } else {
                    error_handler(ret);
                }
            } else {
                fprintf(stderr, "Unknown set command.\n");
            }
            argc -= 3;
            argv += 3;
            continue;
        }
        if (strcmp(argv[0], "get") == 0) {
            if (argc < 2) {
                fprintf(stderr, "get command needs one parameter.\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[1], "open_speed") == 0) {
                double speed;
                if ((ret = dome_get_window_open_speed(dome, &speed)) == AAOS_OK) {
                    printf("%.4f\n", speed);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "window_close_speed") == 0) {
                double speed;
                if ((ret = dome_get_window_close_speed(dome, &speed)) == AAOS_OK) {
                    printf("%.4f\n", speed);
                } else {
                    error_handler(ret);
                }
            } else if (strcmp(argv[1], "window_position") == 0) {
                double position;
                if ((ret = dome_get_window_position(dome, &position)) == AAOS_OK) {
                    printf("%.4f\n", position);
                } else {
                    error_handler(ret);
                }
            } else {
                fprintf(stderr, "Unknown set command.\n");
            }
            argc -= 2;
            argv += 2;
            continue;
        }
        argc--;
        argv++;
    }
    
    delete(dome);
    delete(client);
    
    return 0;
}
