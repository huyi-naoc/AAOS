//
//  thermal_client.c
//  AAOS
//
//  Created by Hu Yi on 2023/3/23.
//  Copyright © 2023 NAOC. All rights reserved.
//

#include "def.h"
#include "thermal_def.h"
#include "thermal_rpc.h"
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
    {"thermal",     required_argument,  NULL,       't' },
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
usage(void)
{
    fprintf(stderr, "Usage:\ttelescope [-h|--help][-v|--version]\n");
    fprintf(stderr, "\t\t[-n|--name <name>] [-i|--index <index>]\n");
    fprintf(stderr, "\t\t[-t|--telescope <`address`:`port`>]\n");
    fprintf(stderr, "\t\t[-f|--format [string | degree]] (default: string)\n");
    fprintf(stderr, "\t\t[-u|--unit [nature | second | minute | degree]] (default: nature)\n");
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
    void *thermal;
    
    
    while ((ch = getopt_long(argc, argv, "f:hi:n:t:", longopts, NULL)) != -1) {
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
                    snprintf(port, PORTSIZE, THM_RPC_PORT);
                }
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    client = new(ThermalUnitClient(), address, port);
   
    if ((ret = rpc_client_connect(client, &thermal)) != AAOS_OK) {
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
        ret = thermal_unit_get_index_by_name(thermal, name);
    } else {
        uint16_t idx = index;
        protobuf_set(thermal, PACKET_INDEX, idx);
    }
    
    if (argc == 0) {
        char command[COMMANDSIZE];
        while (fscanf(stdin, "%s", command) != EOF) {
            if (strcmp(command, "status") == 0) {
                uint16_t status;
                ret = thermal_unit_status(thermal, &status, sizeof(status), NULL);
                if (name != NULL) {
                    if (status == THERMAL_UNIT_STATE_ON) {
                        fprintf(stdout, "thermal unit %s: on\n", name);
                    } else {
                        fprintf(stdout, "thermal unit %s: off\n", name);
                    }
                } else {
                    if (status == THERMAL_UNIT_STATE_ON) {
                        fprintf(stdout, "thermal unit %02d: on\n", index);
                    } else {
                        fprintf(stdout, "thermal unit %02d: off\n", index);
                    }
                }
                continue;
            }
            if (strcmp(command, "info") == 0) {
                ret = thermal_unit_info(thermal, stdout);
                continue;
            }
            
        }
        
        while (argc > 0) {
            if (strcmp(argv[0], "status") == 0) {
                uint16_t status;
                ret = thermal_unit_status(thermal, &status, sizeof(status), NULL);
                if (name != NULL) {
                    if (status == THERMAL_UNIT_STATE_ON) {
                        fprintf(stdout, "thermal unit %s: on\n", name);
                    } else {
                        fprintf(stdout, "thermal unit %s: off\n", name);
                    }
                } else {
                    if (status == THERMAL_UNIT_STATE_ON) {
                        fprintf(stdout, "thermal unit %02d: on\n", index);
                    } else {
                        fprintf(stdout, "thermal unit %02d: off\n", index);
                    }
                }
                continue;
            }
            if (strcmp(argv[0], "info") == 0) {
                ret = thermal_unit_info(thermal, stdout);
                continue;
            }
            
            fprintf(stderr, "Unknown command `%s`.\n", argv[0]);
            argc-- ;
            argv++;
        }
        
        delete(client);
        delete(thermal);
    }
    
    return 0;
}
