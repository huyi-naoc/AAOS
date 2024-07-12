//
//  serial_client_test.c
//  AAOS
//
//  Created by Hu Yi on 2019/8/21.
//  Copyright © 2019 NAOC. All rights reserved.
//

#include "def.h"
#include "serial_rpc.h"
#include "wrapper.h"

extern size_t n_serial;
extern void **serials;

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

static void
usage()
{
    exit(EXIT_FAILURE);
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

static size_t
str2hex(const char *str, void *hex, size_t size)
{
    size_t i, len = strlen(str);
    unsigned char *s = (unsigned char *) hex;
    unsigned char h, l;
    
    if (str == NULL || hex == NULL) {
        return 0;
    }
    
    for (i = 0; i < len && i/2 < size; i+=2) {
        if (isxdigit(str[i]) && isxdigit(str[i + 1])) {
            if (isdigit(str[i])) {
                h = str[i] - '0';
            } else if (isupper(str[i])) {
                h = str[i] - 'A' + 10;
            } else {
                h = str[i] - 'a' + 10;
            }
            if (isdigit(str[i + 1])) {
                l = str[i + 1] - '0';
            } else if (isupper(str[i + 1])) {
                l = str[i + 1] - 'A' + 10;
            } else {
                l = str[i + 1] - 'a' + 10;
            }
            s[i/2] = (h << 4) | l;
        } else {
            return 0;
        }
    }
    return MIN((size_t) len / 2, size);
}

int
main(int argc, char *argv[])
{
    const char *devname = NULL, *devpath = "/dev/ttyS0";
    void *client, *serial;
    int ret;
    char buf[BUFSIZE], address[ADDRSIZE], port[PORTSIZE], *s;
    int ch;
    int index = 0;
    int binary = 0;
    int check = 0;
    int waiting = 0;
    int reg = 0;
    double delay = 0;
    
    snprintf(address, ADDRSIZE, "localhost");
    snprintf(port, PORTSIZE, SERIAL_RPC_PORT);
    
    while ((ch = getopt_long(argc, argv, "bcC:d:hi:n:p:rs:w", longopts, NULL)) != -1) {
        switch (ch) {
            case 'b':
                binary = 1;
                break;
            case 'c':
                check = 1;
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
                devname = optarg;
                index = 0;
                break;
            case 'p':
                devpath = optarg;
                break;
            case 's':
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
            case 'r':
                reg = 1;
                break;
            case 'w':
                waiting = 1;
                break;
            default:
                usage();
                break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    client = new(SerialClient(), address, port);
    
    if ((ret = rpc_client_connect(client, &serial)) != AAOS_OK) {
        delete(serial);
        delete(client);
        fatal_handler(ret, "%s", "Cannot connect to serial server.\n");
    }
    
    if (binary) {
        protobuf_set(serial, PACKET_OPTION, SERIAL_OPTION_BINARY);
    }
    
    if (index == 0) {
        if (devname != NULL) {
            if ((ret = serial_get_index_by_name(serial, devname)) != AAOS_OK) {
                delete(serial);
                delete(client);
                fatal_handler(ret, "`%s` is not found in the device list in the serial server's configuration file.\n", devname);
            } else {
                protobuf_set(serial, PACKET_STR, devname);
            }
        } else {
            if (devpath != NULL) {
                if ((ret = serial_get_index_by_path(serial, devpath)) != AAOS_OK) {
                    delete(serial);
                    delete(client);
                    fatal_handler(ret, "`%s` is not found in the device list in the serial server's configuration file.\n", devpath);
                } else {
                    protobuf_set(serial, PACKET_STR, devpath);
                }
            } else {
                delete(serial);
                delete(client);
                fprintf(stderr, "No serial device is specified.\n");
                fprintf(stderr, "Exit...");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        uint16_t idx = (uint16_t) index;
        protobuf_set(serial, PACKET_INDEX, idx);
    }
    
    if (check) {
        ret = rpc_inspect(serial);
        delete(serial);
        delete(client);
        switch (ret) {
            case AAOS_OK:
                if (index == 0) {
                    if (devname == NULL) {
                        fprintf(stderr, "`%s` is OK.\n", devpath);
                    } else {
                        fprintf(stderr, "`%s` is OK.\n", devname);
                    }
                } else {
                    fprintf(stderr, "%d is OK", index);
                }
                exit(EXIT_SUCCESS);
                break;
            default:
                if (index == 0) {
                    if (devname == NULL) {
                        fprintf(stderr, "`%s` failed.\n", devpath);
                    } else {
                        fprintf(stderr, "`%s` failed.\n", devname);
                    }
                } else {
                    fprintf(stderr, "%d is failed.\n", index);
                }
                exit(EXIT_SUCCESS);
                break;
        }
    }
    
    if (reg) {
        ret = rpc_register(serial, delay);
        delete(serial);
        delete(client);
        switch (ret) {
            case AAOS_OK:
                if (index == 0) {
                    if (devname == NULL) {
                        fprintf(stderr, "`%s` is OK, you can operate it.\n", devpath);
                    } else {
                        fprintf(stderr, "`%s` is OK, you can operate it..\n", devname);
                    }
                } else {
                    fprintf(stderr, "%d is OK, you can operate it.\n", index);
                }
                exit(EXIT_SUCCESS);
                break;
            default:
                if (index == 0) {
                    if (devname == NULL) {
                        fprintf(stderr, "waiting for `%s` recovery timed out.\n", devpath);
                    } else {
                        fprintf(stderr, "waiting for `%s` recovery  timed out.\n", devname);
                    }
                } else {
                    fprintf(stderr, "waiting for %d recovery timed out.\n", index);
                }
                exit(EXIT_SUCCESS);
                break;
        }
    }
    
    while (argc != 0) {
        memset(buf, '\0', BUFSIZE);
        unsigned char command[COMMANDSIZE];
        size_t length = strlen(argv[0]);
        if (binary) {
            length = str2hex(argv[0], command, COMMANDSIZE);
        } else {
            snprintf((char *)command, COMMANDSIZE, "%s", argv[0]);
        }
        size_t res_len;
        if ((ret = serial_raw(serial, command, length, buf, BUFSIZE, &res_len)) != AAOS_OK) {
            
            if (ret == AAOS_ENOTFOUND) {
                delete(serial);
                delete(client);
                fatal_handler(AAOS_ERROR, "Device ID `%d` is not found.\n", index);
            } else if (ret == AAOS_EBADCMD) {
                delete(serial);
                delete(client);
                fatal_handler(ret, "`%s` is an illegal command for the serial port device.\n", argv[0]);
            } else if (ret == AAOS_ETIMEDOUT) {
                if (waiting) {
                    if (index == 0) {
                        if (devname == NULL) {
                            fprintf(stderr, "read `%s` timed out, it may not be connected to a serial device.\n", devpath);
                        } else {
                            fprintf(stderr, "read `%s` timed out, it may not be connected to a serial device.\n", devname);
                        }
                    } else {
                        fprintf(stderr, "read `%d` timed out, it may not be connected to a serial device.\n", index);
                    }
                    fprintf(stderr, "wait for recover....\n");
                    if ((ret = rpc_register(serial, delay) == AAOS_OK)) {
                        fprintf(stderr, "serial device is recovered, please try again.\n");
                    } else {
                        fprintf(stderr, "waiting for recovery timed out.\n");
                    }
                    delete(serial);
                    delete(client);
                    exit(EXIT_FAILURE);
                } else {
                    delete(serial);
                    delete(client);
                    if (index == 0) {
                        if (devname == NULL) {
                            fatal_handler(ret, "read `%s` timed out, it may not be connected to a serial device.\n", devpath);
                        } else {
                            fatal_handler(ret, "read `%s` timed out, it may not be connected to a serial device.\n", devname);
                        }
                    } else {
                        fatal_handler(ret, "read `%d` timed out, it may not be connected to a serial device.\n", index);
                    }
                }
            } else if (ret == AAOS_EDEVMAL) {
                if (waiting) {
                    if (index == 0) {
                        if (devname == NULL) {
                            fprintf(stderr, "`%s` is in malfunction state.\n", devpath);
                        } else {
                            fprintf(stderr, "`%s` is in malfunction state.\n", devname);
                        }
                    } else {
                        fprintf(stderr, "%d` is in malfunction state.\n", index);
                    }
                    fprintf(stderr, "wait for device recovery....\n");
                    if ((ret = rpc_register(serial, delay) == AAOS_OK)) {
                        fprintf(stderr, "serial device is recovered, please try again.\n");
                    } else {
                        fprintf(stderr, "waiting for device recovery timed out.\n");
                    }
                    delete(serial);
                    delete(client);
                    exit(EXIT_FAILURE);
                } else {
                    delete(serial);
                    delete(client);
                    if (index == 0) {
                        if (devname == NULL) {
                            fatal_handler(ret, "`%s` is in malfunction state.\n", devpath);
                        } else {
                            fatal_handler(ret, "`%s` is in malfunction state.\n", devname);
                        }
                    } else {
                        fatal_handler(ret, "`%d` is in malfunction state.\n", index);
                    }
                }
            } else {
                fatal_handler(ret, "");
            }
        }
        
        if (binary) {
            size_t i;
            for (i = 0; i < res_len; i++) {
                printf("%02X", (unsigned char) buf[i]);
            }
            printf("\n");
        } else {
            printf("%s\n", buf);
        }
        argc--;
        argv++;
    }
    
    delete(serial);
    delete(client);
    
    return 0;
}
