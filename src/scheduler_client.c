//
//  scheduler_client.c
//  AAOS
//
//  Created by huyi on 2024/11/11.
//  Copyright © 2024 NAOC. All rights reserved.
//

#include "def.h"
#include "rpc.h"
#include "scheduler_def.h"
#include "scheduler_rpc.h"
#include "wrapper.h"


static struct option longopts[] = {
    {"help",        no_argument,        NULL,       'h' },
    {"scheduler",   required_argument,  NULL,       's' },
    {"version",     no_argument,        NULL,       'v' },
    { NULL,         0,                  NULL,       0 }
};

static const char *help_string = "\
Usage:  scheduler [-s|--scheduler] <address:[port]> COMMAND [COMMAND PARAMETERS]\n\
        scheduler [-h|--help]\n\
        -h, --help        print help doc and exit\n\
        -s, --scheduler   address of schedulerd\n\n\
Commands:\n\
    get_task_by_telescope_id      ID\n\
    get_task_by_telescope_name    NAME\n\
    list_site\n\
    add_site                      INFO (in JSON format)\n\
    delete_site_by_id ID\n\
    delete_site_by_name           NAME\n\
    mask_site_by_id               ID\n\
    mask_site_by_name             NAME\n\
    unmask_site_by_id             ID\n\
    unmask_site_by_name           NAME\n\
    list_telescope\n\
    add_telescope                 INFO (in JSON format)\n\
    delete_telescope_by_id        ID\n\
    delete_telescope_by_name      NAME\n\
    mask_telescope_by_id          ID\n\
    mask_telescope_by_name        NAME\n\
    unmask_telescope_by_id        ID\n\
    unmask_telescope_by_name      NAME\n\
    list_target\n\
    add_target                    INFO (in JSON format)\n\
    delete_target_by_id           ID NSIDE\n\
    delete_target_by_name         NAME\n\
    mask_target_by_id             ID NSIDE\n\
    mask_target_by_name           NAME\n\
    unmask_target_by_id           ID NSIDE\n\
    unmask_target_by_name         NAME\n\
    add_task_record               INFO (in JSON format)\n\
    update_task_record            INFO (in JSON format)\n\
";

static void
usage(void)
{
    fprintf(stderr, "%s", help_string);
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
            fprintf(stderr, "The scheduler server is absent.\n");
            break;
        case -1 * AAOS_ETIMEDOUT:
            fprintf(stderr, "No response from the scheduler server.\n");
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
    int ch;
    void *client;
    char address[ADDRSIZE], port[PORTSIZE];
    int ret;
    uint64_t site_id, tel_id, target_id, task_id;
    uint32_t nside;
    const char *sitename = NULL, *telescop = NULL, *targname = NULL;
    char *s;

    snprintf(address, ADDRSIZE, "localhost");
    snprintf(port, PORTSIZE, SCHEDULER_RPC_SITE_PORT);

    void *scheduler;

    while ((ch = getopt_long(argc, argv, "hs:v", longopts, NULL)) != -1) {
        switch (ch) {
            case 'h':
                usage();
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
            default:
                usage();
                break;
        }
    }
    argc -= optind;
    argv += optind;

    client = new(SchedulerClient(), address, port);

    if ((ret = rpc_client_connect(client, &scheduler)) != AAOS_OK) {
        delete(scheduler);
        delete(client);
        fatal_handler(ret, "%s", "Cannot connect to scheduler server.\n");
    }

    while (argc != 0) {

        if (strcmp("get_task_by_telescope_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`get_task_by_telescope_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            tel_id = strtoull(argv[1], NULL, 0);
            char buf[BUFSIZE];
            if ((ret = scheduler_get_task_by_telescope_id(scheduler, tel_id, buf, BUFSIZE, NULL, NULL)) == AAOS_OK) {
                fprintf(stderr, "`get_task_by_telescope_id` successed.\n");
            } else {
                fprintf(stderr, "`get_task_by_telescope_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("get_task_by_telescope_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`get_task_by_telescope_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            char buf[BUFSIZE];
            if ((ret = scheduler_get_task_by_telescope_name(scheduler, argv[1], buf, BUFSIZE, NULL, NULL)) == AAOS_OK) {
                fprintf(stderr, "`get_task_by_telescope_name` successed.\n");
            } else {
                fprintf(stderr, "`get_task_by_telescope_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("list_site", argv[0]) == 0) {
            char *buf = (char *) Malloc(BUFSIZE * 4096);
            if ((ret = scheduler_list_site(scheduler, buf, BUFSIZE * 4096, NULL, NULL)) == AAOS_OK) {
                printf("%s", buf);
            } else {
                fprintf(stderr, "list_site failed.\n");
            }
            free(buf);
            argc--;
            argv++;
            continue; 
        }
        if (strcmp("add_site", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`add_site` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_add_site(scheduler, argv[1], SCHEDULER_FORMAT_JSON)) == AAOS_OK) {
                fprintf(stderr, "`add_site` successed.\n");
            } else {
                fprintf(stderr, "`add_site` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("delete_site_by_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`delete_site_by_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            site_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_delete_site_by_id(scheduler, site_id)) == AAOS_OK) {
                fprintf(stderr, "`delete_site_by_id` successed.\n");
            } else {
                fprintf(stderr, "`delete_site_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("delete_site_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`delete_site_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_delete_site_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`delete_site_by_name` successed.\n");
            } else {
                fprintf(stderr, "`delete_site_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("mask_site_by_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`mask_site_by_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            site_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_mask_site_by_id(scheduler, site_id)) == AAOS_OK) {
                fprintf(stderr, "`mask_site_by_id` successed.\n");
            } else {
                fprintf(stderr, "`mask_site_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("mask_site_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`mask_site_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_mask_site_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`mask_site_by_name` successed.\n");
            } else {
                fprintf(stderr, "`mask_site_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("unmask_site_by_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`unmask_site_by_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            site_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_unmask_site_by_id(scheduler, site_id)) == AAOS_OK) {
                fprintf(stderr, "`unmask_site_by_id` successed.\n");
            } else {
                fprintf(stderr, "`unmask_site_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("unmask_site_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`unmask_site_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_unmask_site_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`unmask_site_by_name` successed.\n");
            } else {
                fprintf(stderr, "`unmask_site_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("list_telescope", argv[0]) == 0) {
            char *buf = (char *) Malloc(BUFSIZE * 4096);
            if ((ret = scheduler_list_telescope(scheduler, buf, BUFSIZE * 4096, NULL, NULL)) == AAOS_OK) {
                printf("%s", buf);
            } else {
                fprintf(stderr, "list_telescope failed.\n");
            }
            free(buf);
            argc--;
            argv++;
            continue;
        }
        if (strcmp("add_telescope", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`add_telescope` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_add_telescope(scheduler, argv[1], SCHEDULER_FORMAT_JSON)) == AAOS_OK) {
                fprintf(stderr, "`add_telescope` successed.\n");
            } else {
                fprintf(stderr, "`add_telescope` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("delete_telescope_by_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`delete_telescope_by_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            tel_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_delete_telescope_by_id(scheduler, tel_id)) == AAOS_OK) {
                fprintf(stderr, "`delete_telescope_by_id` successed.\n");
            } else {
                fprintf(stderr, "`delete_telescope_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("delete_telescope_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`delete_telescope_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_delete_telescope_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`delete_telescope_by_name` successed.\n");
            } else {
                fprintf(stderr, "`delete_telescope_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("mask_telescope_by_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`mask_telescope_by_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            tel_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_mask_telescope_by_id(scheduler, tel_id)) == AAOS_OK) {
                fprintf(stderr, "`mask_telescope_by_id` successed.\n");
            } else {
                fprintf(stderr, "`mask_telescope_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("mask_telescope_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`mask_telescope_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_mask_telescope_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`mask_telescope_by_name` successed.\n");
            } else {
                fprintf(stderr, "`mask_telescope_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("unmask_telescope_by_id", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`unmask_telescope_by_id` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            tel_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_unmask_telescope_by_id(scheduler, tel_id)) == AAOS_OK) {
                fprintf(stderr, "`unmask_telescope_by_id` successed.\n");
            } else {
                fprintf(stderr, "`unmask_telescope_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("unmask_telescope_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`unmask_telescope_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_unmask_telescope_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`unmask_telescope_by_name` successed.\n");
            } else {
                fprintf(stderr, "`unmask_telescope_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("list_target", argv[0]) == 0) {
            char *buf = (char *) Malloc(BUFSIZE * 4096);
            if ((ret = scheduler_list_target(scheduler, buf, BUFSIZE * 4096, NULL, NULL)) == AAOS_OK) {
                printf("%s", buf);
            } else {
                fprintf(stderr, "list_target failed.\n");
            }
            argc--;
            argv++;
            continue;
        }
        if (strcmp("add_target", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`add_target` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_add_target(scheduler, argv[1], SCHEDULER_FORMAT_JSON) == AAOS_OK)) {
                fprintf(stderr, "`add_target` successed.\n");
            } else {
                fprintf(stderr, "`add_target` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("delete_target_by_id", argv[0]) == 0) {
            if (argc < 3) {
                fprintf(stderr, "`delete_target_by_id` needs two arguments.\n");
                exit(EXIT_FAILURE);
            }
            target_id = strtoull(argv[1], NULL, 0);
            nside = (uint32_t) strtoul(argv[2], NULL, 0);
            if ((ret = scheduler_delete_target_by_id(scheduler, target_id, nside)) == AAOS_OK) {
                fprintf(stderr, "`delete_target_by_id` successed.\n");
            } else {
                fprintf(stderr, "`delete_target_by_id` failed.\n");
            }
            argc -= 3;
            argv += 3;
            continue; 
        }
        if (strcmp("delete_target_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`delete_target_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_delete_target_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`delete_target_by_name` successed.\n");
            } else {
                fprintf(stderr, "`delete_target_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("mask_target_by_id", argv[0]) == 0) {
            if (argc < 3) {
                fprintf(stderr, "`mask_target_by_id` needs two arguments.\n");
                exit(EXIT_FAILURE);
            }
            target_id = strtoull(argv[1], NULL, 0);
            nside = (uint32_t) strtoul(argv[2], NULL, 0);
            if ((ret = scheduler_mask_target_by_id(scheduler, target_id, nside)) == AAOS_OK) {
                fprintf(stderr, "`mask_target_by_id` successed.\n");
            } else {
                fprintf(stderr, "`mask_target_by_id` failed.\n");
            }
            argc -= 3;
            argv += 3;
            continue; 
        }
        if (strcmp("mask_target_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`mask_target_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_mask_target_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`mask_target_by_name` successed.\n");
            } else {
                fprintf(stderr, "`mask_target_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("unmask_target_by_id", argv[0]) == 0) {
            if (argc < 3) {
                fprintf(stderr, "`unmask_target_by_id` needs two arguments.\n");
                exit(EXIT_FAILURE);
            }
            target_id = strtoull(argv[1], NULL, 0);
            nside = (uint32_t) strtoul(argv[2], NULL, 0);
            if ((ret = scheduler_unmask_target_by_id(scheduler, target_id, nside)) == AAOS_OK) {
                fprintf(stderr, "`unmask_target_by_id` successed.\n");
            } else {
                fprintf(stderr, "`unmask_target_by_id` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("unmask_target_by_name", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`unmask_target_by_name` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_unmask_target_by_name(scheduler, argv[1])) == AAOS_OK) {
                fprintf(stderr, "`unmask_target_by_name` successed.\n");
            } else {
                fprintf(stderr, "`unmask_target_by_name` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("add_task_record", argv[0]) == 0) {
            if (argc < 2) {
                fprintf(stderr, "`add_task_record` needs an argument.\n");
                exit(EXIT_FAILURE);
            }
            if ((ret = scheduler_add_task_record(scheduler, argv[1], SCHEDULER_FORMAT_JSON)) == AAOS_OK) {
                fprintf(stderr, "`add_task_record` successed.\n");
            } else {
                fprintf(stderr, "`add_task_record` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue; 
        }
        if (strcmp("update_task_record", argv[0]) == 0) {
            if (argc < 3) {
                fprintf(stderr, "`update_task_record` needs two arguments.\n");
                exit(EXIT_FAILURE);
            }
            task_id = strtoull(argv[1], NULL, 0);
            if ((ret = scheduler_update_task_record(scheduler, task_id, argv[2], SCHEDULER_FORMAT_JSON)) == AAOS_OK) {
                fprintf(stderr, "`update_task_record` successed.\n");
            } else {
                fprintf(stderr, "`update_task_record` failed.\n");
            }
            argc -= 2;
            argv += 2;
            continue;
        }
        argc--;
        argv++;
    }
    delete(client);
    delete(scheduler);

    return 0;
}
