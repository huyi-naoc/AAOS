//
//  pdu_client.c
//  AAOS
//
//  Created by Hu Yi on 2020/7/27.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "def.h"
#include "device.h"
#include "pdu_rpc.h"
#include "rpc.h"
#include "wrapper.h"

static uint16_t idx = 1;
static uint16_t chan;
static const char *pdu_name, *switch_name;
static double timeout;

static struct option longopts[] = {
    {"channel",     required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"index",       required_argument,  NULL,       'i' },
    {"name",        optional_argument,  NULL,       'n' },
    {"pdu",         required_argument,  NULL,       'p' },
    {"switch",      optional_argument,  NULL,       's' },
    {"timeout",     optional_argument,  NULL,       't' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

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
            fprintf(stderr, "The serial server is absent.\n");
            break;
        case -1 * AAOS_ETIMEDOUT:
            fprintf(stderr, "No response from the serial server.\n");
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
    int ch, ret;
    char address[ADDRSIZE], port[PORTSIZE], *s;
    void *client, *pdu;
    
    snprintf(address, ADDRSIZE, "localhost");
    snprintf(port, PORTSIZE, AWS_RPC_PORT);
    
    while ((ch = getopt_long(argc, argv, "c:i:n:p:s:t:v", longopts, NULL)) != -1) {
        switch (ch) {
            
            case 'c':
                chan = atoi(optarg);
            break;
            case 'i':
                idx = atoi(optarg);
                break;
            case 'n':
                pdu_name = optarg;
            break;
            case 'p':
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
                break;
            case 's':
                switch_name = optarg;
                break;
            case 't':
                timeout = atof(optarg);
                break;
            default:
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    client = new(PDUClient(), address, port);
    if ((ret = rpc_client_connect(client, &pdu)) != AAOS_OK) {
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
    
    if (pdu_name) {
        if ((ret = pdu_get_index_by_name(pdu, pdu_name)) != AAOS_OK) {
            fatal_handler(ret, "PDU `%s` is not found in the pdu server's configuration file.\n", pdu_name);
        }
    } else {
        protobuf_set(pdu, PACKET_INDEX, idx);
    }
    
    if (switch_name) {
        if ((ret = pdu_get_channel_by_name(pdu, switch_name)) != AAOS_OK) {
            fprintf(stderr, "Switch `%s` is not found in the pdu server's configuration file.\n", switch_name);
            exit(EXIT_FAILURE);
        }
    } else {
        protobuf_set(pdu, PACKET_CHANNEL, chan);
    }
    
    while (argc != 0) {
        if (strcmp(argv[0], "on") == 0) {
            if ((ret = pdu_turn_on(pdu)) == AAOS_OK) {
                fprintf(stderr, "OK\n");
            } else {
                fatal_handler(ret, "`turn on` failed\n");
            }
        } else if (strcmp(argv[0], "off") == 0) {
            if ((ret = pdu_turn_off(pdu)) == AAOS_OK) {
                fprintf(stderr, "OK\n");
            } else {
                fatal_handler(ret, "`turn off` failed\n");
            }
        } else if (strcmp(argv[0], "voltage") == 0) {
            double voltage;
            if ((ret = pdu_get_voltage(pdu, &voltage)) == AAOS_OK) {
                printf("%.2f\n", voltage);
            } else {
                fatal_handler(ret, "`get voltage` failed\n");
            }
        } else if (strcmp(argv[0], "current") == 0) {
            double current;
            if ((ret = pdu_get_voltage(pdu, &current)) == AAOS_OK) {
                printf("%.2f\n", current);
            } else {
                fatal_handler(ret, "`get voltage` failed\n");
            }
        } else if (strcmp(argv[0], "volcur") == 0) {
            double voltage, current;
            if ((ret = pdu_get_voltage_current(pdu, &voltage, &current)) == AAOS_OK) {
                printf("%.2f %.2f\n", voltage, current);
            } else {
                fatal_handler(ret, "`get voltage` failed\n");
            }
        } else if (strcmp(argv[0], "inspect") == 0) {
            if ((ret = device_inspect(pdu)) == AAOS_OK) {
                if (pdu_name != NULL) {
                    printf("PDU `%s` is OK\n", pdu_name);
                } else {
                    printf("PDU `%d` is OK\n", idx);
                }
            } else {
                if (pdu_name != NULL) {
                    printf("PDU `%s` is malfunction\n", pdu_name);
                } else {
                    printf("PDU `%d` is malfunction\n", idx);
                }
            }
        } else if (strcmp(argv[0], "wait") == 0) {
            if ((ret=device_wait(pdu, timeout)) == AAOS_OK) {
                if (pdu_name != NULL) {
                    printf("PDU `%s` is OK now\n", pdu_name);
                } else {
                    printf("PDU `%d` is OK now\n", idx);
                }
            } else {
                if (pdu_name != NULL) {
                    printf("Waiting for PDU `%s` recovery is timed out or failed\n", pdu_name);
                } else {
                    printf("Wait for PDU `%d` recovery is timed out or failed\n", idx);
                }
            }
        }
        argc--;
        argv++;
    }
    
    delete(client);
    delete(pdu);
    return 0;
}
