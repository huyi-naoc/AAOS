//
//  aws_client_test.c
//  AAOS
//
//  Created by Hu Yi on 2020/2/25.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "aws_def.h"
#include "aws_rpc.h"
#include "def.h"
#include "rpc.h"
#include "wrapper.h"

static uint16_t idx = 1;
static uint16_t chan;
static int type;
static unsigned int raw_flag;
static unsigned int log_flag;
static unsigned int status_flag;
const char *aws_name, *sensor_name;

static struct option longopts[] = {
    {"aws",         required_argument,  NULL,       'a' },
    {"channel",     required_argument,  NULL,       'c' },
    {"help",        no_argument,        NULL,       'h' },
    {"index",       required_argument,  NULL,       'i' },
    {"name",        optional_argument,  NULL,       'n' },
    {"raw",         no_argument,        NULL,       'r' },
    {"sensor",      optional_argument,  NULL,       's' },
    {"status",      optional_argument,  NULL,       'S' },
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
    void *client, *aws;
    char *buf = NULL;
    size_t i, n_field = 1, size;
    
    snprintf(address, ADDRSIZE, "localhost");
    snprintf(port, PORTSIZE, AWS_RPC_PORT);
    
    while ((ch = getopt_long(argc, argv, "a:c:hi:ln:N:rs:St:v", longopts, NULL)) != -1) {
        switch (ch) {
            case 'a':
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
            case 'c':
                chan = atoi(optarg);
                break;
            case 'i':
                idx = atoi(optarg);
                break;
            case 'l':
                log_flag = 1;
            break;
            case 'n':
                aws_name = optarg;
            break;
            case 'N':
                n_field = atoi(optarg);
                break;
            case 'r':
                raw_flag = 1;
                break;
            case 's':
                sensor_name = optarg;
                break;
            case 'S':
                status_flag = 1;
                break;
            case 't':
                if (strcmp(optarg, "temperature") == 0) {
                    type = SENSOR_TYPE_TEMEPRATURE;
                } else if (strcmp(optarg, "windspeed") == 0) {
                    type = SENSOR_TYPE_WIND_SPEED;
                } else if (strcmp(optarg, "winddirection") == 0) {
                    type = SENSOR_TYPE_WIND_DIRECTION;
                } else if (strcmp(optarg, "humidity") == 0) {
                    type = SENSOR_TYPE_RELATIVE_HUMIDITY;
                } else if (strcmp(optarg, "pressure") == 0) {
                    type = SENSOR_TYPE_AIR_PRESSURE;
                } else if (strcmp(optarg, "precipitation") == 0) {
                    type = SENSOR_TYPE_PRECIPITATION;
                } else if (strcmp(optarg, "sqm") == 0) {
                    type = SENSOR_TYPE_SKY_QUALITY;
                }
                break;
            default:
                break;
        }
    }
    
    client = new(AWSClient(), address, port);
    if ((ret = rpc_client_connect(client, &aws)) != AAOS_OK) {
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
    
    if (aws_name) {
        if ((ret = aws_get_index_by_name(aws, aws_name)) != AAOS_OK) {
            fatal_handler(ret, "AWS `%s` is not found in the aws server's configuration file.\n", aws_name);
        }
    } else {
        protobuf_set(aws, PACKET_INDEX, idx);
    }
    
    if (log_flag || status_flag) {
        aws_data_field(aws, stdout);
        if (log_flag) {
            aws_data_log(aws, stdout);
        } else {
            aws_status(aws, stdout);
        }
        exit(EXIT_SUCCESS);
    }
    
    if (sensor_name) {
        if ((ret = aws_get_channel_by_name(aws, sensor_name)) != AAOS_OK) {
            fprintf(stderr, "Sensor `%s` is not found in the aws server's configuration file.\n", sensor_name);
            exit(EXIT_FAILURE);
        }
    } else {
        protobuf_set(aws, PACKET_CHANNEL, chan);
    }
    
    if (raw_flag) {
        buf = (char *) Malloc(COMMANDSIZE);
        ret = aws_get_raw_data(aws, buf, COMMANDSIZE);
    } else {
        double data[n_field];
        FILE *fp;
        fp = open_memstream(&buf, &size);
        switch (type) {
            case SENSOR_TYPE_TEMEPRATURE:
                ret = aws_get_temperature(aws, data, n_field);
                fprintf(fp, "%.2f", data[0]);
                break;
            case SENSOR_TYPE_WIND_SPEED:
                ret = aws_get_wind_speed(aws, data, n_field);
                fprintf(fp, "%.2f", data[0]);
                break;
            case SENSOR_TYPE_WIND_DIRECTION:
                ret = aws_get_wind_direction(aws, data, n_field);
                fprintf(fp, "%.2f", data[0]);
                break;
            case SENSOR_TYPE_RELATIVE_HUMIDITY:
                ret = aws_get_relative_humidity(aws, data, n_field);
                fprintf(fp, "%.2f", data[0]);
                break;
            case SENSOR_TYPE_AIR_PRESSURE:
                ret = aws_get_air_pressure(aws, data, n_field);
                fprintf(fp, "%.2f", data[0]);
                break;
            case SENSOR_TYPE_SKY_QUALITY:
                ret = aws_get_sky_quality(aws, data, n_field);
                for (i = 0; i < n_field - 1; i++) {
                    fprintf(fp, "%.2f ", data[i]);
                }
                fprintf(fp, "%.2f", data[n_field - 1]);
                break;
            case SENSOR_TYPE_PRECIPITATION:
                ret = aws_get_precipitation(aws, data, n_field);
                if (n_field == 1) {
                    //fprintf(fp, "%d", (int) data[0]);
                    int rain_type = (int) data[0];
                    switch (rain_type) {
                        case 0:
                            fprintf(fp, "no rain");
                            break;
                        case 40:
                            fprintf(fp, "unspecified");
                            break;
                        case 60:
                            fprintf(fp, "rain");
                            break;
                        case 67:
                            fprintf(fp, "freezing");
                            break;
                        case 69:
                            fprintf(fp, "sleet");
                            break;
                        case 70:
                            fprintf(fp, "snow");
                            break;
                        case 90:
                            fprintf(fp, "hail");
                            break;

                        default:
                            break;
                    }
                } else {
                    int rain_type = (int) data[0];
                    switch (rain_type) {
                        case 0:
                            fprintf(fp, "no rain ");
                            break;
                        case 40:
                            fprintf(fp, "unspecified ");
                            break;
                        case 60:
                            fprintf(fp, "rain ");
                            break;
                        case 67:
                            fprintf(fp, "freezing ");
                            break;
                        case 69:
                            fprintf(fp, "sleet ");
                            break;
                        case 70:
                            fprintf(fp, "snow ");
                            break;
                        case 90:
                            fprintf(fp, "hail ");
                            break;

                        default:
                            break;
                    }
                    for (i = 1; i < n_field - 1; i++) {
                        fprintf(fp, "%.2f ", data[i]);
                    }
                    fprintf(fp, "%.2f", data[n_field - 1]);
                }
                break;
            default:
                ret = aws_get_data(aws, data, n_field);
                for (i = 0; i < n_field - 1; i++) {
                    fprintf(fp, "%.2f ", data[i]);
                }
                fprintf(fp, "%.2f", data[n_field - 1]);
                break;
        }
        /*
         * close the file is mandatory.
         */
        fclose(fp);
    }
    if (ret == AAOS_OK) {
        printf("%s\n", buf);
    } else {
        if (ret < 0) {
            fprintf(stderr, "RPC failure caused by network problem.\n");
        } else {
            fprintf(stderr, "RPC failure caused by remote server.\n");
        }
        free(buf);
        delete(client);
        delete(aws);
        exit(EXIT_FAILURE);
    }
    free(buf);
    delete(client);
    delete(aws);
    return 0;
}
