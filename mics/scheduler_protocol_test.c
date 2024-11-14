#include "def.h"
#include "scheduler_def.h"
#include "scheduler.h"
#include "utils.h"
#include "wrapper.h"


static const char *global_un_sock = NULL, *site_un_sock = NULL;
static const char *global_module = NULL, *site_module = NULL;

static struct option longopts[] = {
    {"global-un-sock", required_argument, NULL, 'g'},
    {"global-module", required_argument, NULL, 'G'},
    {"site-un-sock", required_argument, NULL, 's'},
    {"site-module", required_argument, NULL, 'S'},
    {NULL, 0, NULL, 0}};


static void
usage()
{
    exit(EXIT_FAILURE);
}

static void
test_protocol(void)
{
    char *json_string;

    json_string = __scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
    printf("An example `request` operation send to the gloabl scheduling module.\n");
    printf("%s\n\n", json_string);
    free(json_string);

    json_string = __scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
    printf("An example `acknowledge` operation send to the gloabl scheduling module.\n");
    printf("%s\n\n", json_string);
    free(json_string);

    json_string = __scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_ID, 2);
    printf("An example `requet` operation send to the site scheduling module.\n");
    printf("%s\n\n", json_string);
    free(json_string);
}

static void
pop_task_block(void)
{
    char *block_buf = NULL, *json_string;
	char buf[BUFSIZE];
    uint32_t len;
    ssize_t nread;
	int sockfd;
    FILE *fp;

    if (((fp = popen(global_module, "r+"))) == NULL) {
        fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
    }
	pclose(fp);

	if ((sockfd = Un_stream_connect(global_un_sock)) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}
	
	json_string = __scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
    snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
    len = strlen(json_string) + 1;
    free(json_string);
	memcpy(buf, &len, sizeof(uint32_t));
	if (Writen(sockfd, buf, sizeof(uint32_t) + len) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		goto end;
	}
	
	for (; ;) {
		if ((nread = Readn(sockfd, &len, sizeof(uint32_t))) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		    goto end;	
		}
		block_buf = (char *) Realloc(block_buf, len);
		if ((nread = Readn(sockfd, block_buf, len)) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
			goto end;
		}
        
		json_string = __scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
        snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
		len = strlen(json_string + sizeof(uint32_t)) + 1;
        free(json_string);
		memcpy(buf, &len, sizeof(uint32_t));
		if (Writen(sockfd, buf, sizeof(uint32_t) + len) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
			goto end;
	    }
		printf("%s\n", block_buf);
	}

end:
    free(block_buf);
    Close(sockfd);
 	exit(EXIT_FAILURE);
}

static void
pop_and_push_task_block(const char *path, const char *path2)
{
    char *block_buf = NULL, *json_string;
	char buf[BUFSIZE];
    uint32_t len;
    ssize_t nread;
	int sockfd, sockfd2;
    FILE *fp;

    if (((fp = popen(global_module, "r+"))) == NULL) {
        fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
    }
    pclose(fp);
	if ((sockfd = Un_stream_connect(global_un_sock)) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}
    if (((fp = popen(site_module, "r+"))) == NULL) {
        fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
    }
    pclose(fp);
    if ((sockfd2 = Un_stream_connect(site_un_sock)) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}
	
	json_string = __scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
    snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
    len = strlen(json_string) + 1;
    free(json_string);
	memcpy(buf, &len, sizeof(uint32_t));
	if (Writen(sockfd, buf, sizeof(uint32_t) + len) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		goto end;
	}
	
	for (; ;) {
		if ((nread = Readn(sockfd, &len, sizeof(uint32_t))) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		    goto end;	
		}
		block_buf = (char *) Realloc(block_buf, sizeof(uint32_t) + len);
        memcpy(buf, &len, sizeof(uint32_t));
		if ((nread = Readn(sockfd, block_buf + sizeof(uint32_t), len)) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
			goto end;
		}
        
		json_string = __scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
        snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
		len = strlen(json_string + sizeof(uint32_t)) + 1;
        free(json_string);
		memcpy(buf, &len, sizeof(uint32_t));
		if (Writen(sockfd, block_buf, sizeof(uint32_t) + strlen(block_buf + sizeof(uint32_t) + 1)) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
			goto end;
	    }

        printf("Task block generated by gloabl scheduling module: ");
		printf("%s\n\n", block_buf);

        if (Writen(sockfd2, block_buf, sizeof(uint32_t) + strlen(block_buf + sizeof(uint32_t) + 1)) < 0) {
		    fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
            Close(sockfd2);
			goto end;
	    }
	}

end:
    free(block_buf);
    Close(sockfd);
 	exit(EXIT_FAILURE);
}

static void
get_task(uint64_t identifier)
{
    int sockfd;
    char buf[BUFSIZE];
    char *json_string;
    ssize_t nread;
    uint32_t len;

    if ((sockfd = Un_stream_connect(site_un_sock)) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}

    json_string = __scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_ID, identifier);
    len = strlen(json_string) + 1;
    memcpy(buf, &len, sizeof(uint32_t));
    snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
    free(json_string);
    if (Writen(sockfd, buf, sizeof(uint32_t) + strlen(json_string) + 1) < 0) {
        fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
        goto end;
    }

    if ((nread = Readn(sockfd, &len, sizeof(uint32_t))) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		goto end;	
	}
    if ((nread = Readn(sockfd, buf, len)) < 0) {
		fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
		goto end;	
	}

    printf("Task block generated by site scheduling module: ");
    printf("%s\n\n", buf);

end:
    Close(sockfd);
 	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int ch;
    while ((ch = getopt_long(argc, argv, "g:G:s:S", longopts, NULL)) != -1) {
        switch (ch) {
            case 'g':
                global_un_sock = optarg;
                break;
            case 'G':
                global_module = optarg;
                break;
            default:
                usage();
                break;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc > 0) {
        if (strcasecmp(argv[0], "test_protocol") == 0) {
            test_protocol();
        } else if (strcasecmp(argv[0], "pop_task_block") == 0) {
            pop_task_block();
        } else if (strcasecmp(argv[0], "pop_and_push_task_block") == 0) {
            pop_task_block();
        } else if (strcasecmp(argv[0], "get_task") == 0) {
            if (argc > 1) {
                uint64_t identifier = strtoull(argv[1], NULL, 10);
                get_task(identifier);
            } else {
                fprintf(stderr, "`%s` failed at line %d.\n", __func__, __LINE__);
                exit(EXIT_FAILURE);
            }
            
        }
    }

    return 0;
}
