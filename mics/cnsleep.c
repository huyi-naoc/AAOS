#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <getopt.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

static struct option long_options[] = {
    {NULL, no_argument, NULL, 'a'}, \
    {0, no_argument, NULL, 0}
};

int
main(int argc, char *argv[])
{
    signed char ch;
    clockid_t clock_id = CLOCK_REALTIME;
    int flag = 0;
    struct timespec request, remain;
    int longindex = 0;
    const char *name = argv[0];
    while ((ch = getopt_long(argc, argv, "a", long_options, &longindex)) != -1) {
        switch (ch) {
            case 'a':
#ifdef LINUX
                flag = TIMER_ABSTIME;
#else
                flag = 1;
#endif
                break;
            default:
                fprintf(stderr, "?? getopt returned character code 0%x ??\n", ch);
                fprintf(stderr, "Illegal option.\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    argc -= optind;
    argv += optind;

    if (optind == 0) {
        if (argc == 2) {
            double tp = atof(argv[1]);
            request.tv_sec = floor(tp);
            request.tv_nsec = (tp - request.tv_sec) * 1000000000;
        } else if (argc >= 3) {
            request.tv_sec = atoi(argv[1]);
            request.tv_nsec = atoi(argv[2]);
        } else if (argc == 1) {
            fprintf(stderr, "%s: lack arguments.\n", name);
            exit(EXIT_FAILURE);
        }
    } else {
        if (argc == 1) {
            double tp = atof(argv[0]);
            request.tv_sec = floor(tp);
            request.tv_nsec = (tp - request.tv_sec) * 1000000000;
        } else if (argc >= 2) {
            request.tv_sec = atoi(argv[0]);
            request.tv_nsec = atoi(argv[1]);
        } else if (argc == 0) {
            fprintf(stderr, "%s: lack arguments.\n", name);
            exit(EXIT_FAILURE);
        }
    }
#ifdef LINUX
    clock_nanosleep(clock_id, flag, &request, &remain);
#else
    if (flag == 0) {
        nanosleep(&request, &remain);
    } else {
        struct timespec now, req;
        double now_d, req_d, diff;
        clock_gettime(clock_id, &now);
        now_d = now.tv_sec + now.tv_nsec / 1000000000;
        req_d = request.tv_sec + request.tv_nsec / 1000000000;
        diff = req_d - now_d;
        if (diff < 0) {
            exit(EXIT_SUCCESS);
        }
        req.tv_sec = floor(diff);
        req.tv_nsec = (diff - req.tv_sec) * 1000000000;
        nanosleep(&req, &remain);
    }
#endif
    exit(EXIT_SUCCESS);
}
