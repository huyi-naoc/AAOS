//
//  thread_client.c
//  AAOS
//
//  Created by huyi on 2026/2/23.
//

#include "def.h"
#include "thread_rpc.h"
#include "wrapper.h"

extern size_t n_thread;
extern void **threads;


static const char *help_string = "\
Usage:  thread [options] COMMAND [PARAMETERS ... ]\n\
        -h, --help        print help doc and exit\n\
        -i, --index       <index>, specify telescope's index\n\
        -n, --name        <name>, specify telescope's name\n\
        -t, --thread      <address:[port]> address (and port) of threadd\n\
Commands:\n\
    start\n\
    terminate\n\
    resume\n\
    suspend\n\
    stop\n\
    cancel\n\
";

static void
error_handler(int e)
{
    switch (e) {
        case AAOS_ECANCELED:
            fprintf(stderr, "Operation is canceled by other command.\n");
            break;
        case AAOS_ENOTSUP:
            fprintf(stderr, "Operation is NOT supported.\n");
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

static struct option longopts[] = {
    {"help",        no_argument,        NULL,       'h' },
    {"index",       required_argument,  NULL,       'i' },
    {"name",        required_argument,  NULL,       'n' },
    {"thread",      optional_argument,  NULL,       't' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};


int
main(int argc, char *argv[])
{
    int ch;
    char address[ADDRSIZE], port[PORTSIZE];
    uint16_t index = 1;
    const char *name = NULL;
    
    int ret = AAOS_OK;
    void *client;
    void *thread;
    
    
    snprintf(address, ADDRSIZE, "localhost");
    snprintf(port, PORTSIZE, "%s", THR_RPC_PORT);
    
    while ((ch = getopt_long(argc, argv, "hi:n:t:v", longopts, NULL)) != -1) {
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
                    snprintf(port, PORTSIZE, THR_RPC_PORT);
                }
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    client = new(ObservationThreadClient(), address, port);
   
    if ((ret = rpc_client_connect(client, &thread)) != AAOS_OK) {
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
        ret = observation_thread_get_index_by_name(thread, name);
    } else {
        uint16_t idx = index;
        protobuf_set(thread, PACKET_INDEX, idx);
    }
    
    if (argc == 0) {
        
    } else {
        while (argc > 0) {
            if (strcmp(argv[0], "start") == 0) {
                if ((ret = observation_thread_start(thread)) == AAOS_OK) {
                    fprintf(stderr, "start success\n");
                } else {
                    error_handler(ret);
                }
                argc--;
                argv++;
                continue;
            }
            if (strcmp(argv[0], "terminate") == 0) {
                if ((ret = observation_thread_terminate(thread)) == AAOS_OK) {
                    fprintf(stderr, "terminate success\n");
                } else {
                    error_handler(ret);
                }
                argc--;
                argv++;
                continue;
            }
            if (strcmp(argv[0], "resume") == 0) {
                if ((ret = observation_thread_resume(thread)) == AAOS_OK) {
                    fprintf(stderr, "resume success\n");
                } else {
                    error_handler(ret);
                }
                argc--;
                argv++;
                continue;
            }
            if (strcmp(argv[0], "suspend") == 0) {
                if (argc < 2) {
                    fprintf(stderr, "Too few parameters for \"suspend\" command.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                uint32_t flag = atoi(argv[1]);
                if ((ret = observation_thread_suspend(thread, flag)) == AAOS_OK) {
                    fprintf(stderr, "suspend success\n");
                } else {
                    error_handler(ret);
                }
                argc -= 2;
                argv += 2;
                continue;
            }
            if (strcmp(argv[0], "stop") == 0) {
                if (argc < 2) {
                    fprintf(stderr, "Too few parameters for \"suspend\" command.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                uint32_t flag = atoi(argv[1]);
                if ((ret = observation_thread_stop(thread, flag)) == AAOS_OK) {
                    fprintf(stderr, "stop success\n");
                } else {
                    error_handler(ret);
                }
                argc -= 2;
                argv += 2;
                continue;
            }
            if (strcmp(argv[0], "cancel") == 0) {
                if (argc < 2) {
                    fprintf(stderr, "Too few parameters for \"suspend\" command.\n");
                    fprintf(stderr, "Exit...\n");
                    exit(EXIT_FAILURE);
                }
                uint32_t flag = atoi(argv[1]);
                if ((ret = observation_thread_cancel(thread, flag)) != AAOS_OK) {
                    fprintf(stderr, "cancel success\n");
                } else {
                    error_handler(ret);
                }
                argc -= 2;
                argv += 2;
                continue;
            }
            fprintf(stderr, "Unknown command `%s`.\n", argv[0]);
            argc--;
            argv++;
        }
    }
    
    return ret;
}
