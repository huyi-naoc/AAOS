//
//  astro.c
//  AAOS
//
//  Created by huyi on 2019/3/4.
//  Copyright © 2019年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

/*
 * convert string format to degree and vice versa.
 * The ONLY rationale is for a thread safe library, do not use static storage as other library.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <regex.h>
#include <unistd.h>
#include "astro.h"

#ifdef __USE_SOFA__
#include <sofa.h>
#include <sofam.h>
#ifdef __USE_GSL__
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
static double *mjd, *pm_x_a, *pm_y_a, *ut1_utc_a, *dx_2000a_a, *dy_2000a_a;

static gsl_interp_accel *pm_x_a_acc;
static gsl_spline *pm_x_a_spline;
static gsl_interp_accel *pm_y_a_acc;
static gsl_spline *pm_y_a_spline;
static gsl_interp_accel *ut1_utc_a_acc;
static gsl_spline *ut1_utc_a_spline;
static gsl_interp_accel *dx_2000a_a_acc;
static gsl_spline *dx_2000a_a_spline;
static gsl_interp_accel *dy_2000a_a_acc;
static gsl_spline *dy_2000a_a_spline;
#endif
#endif

/*
 * legal expression
 * 02h or 2h or 02*, 12h23.2m, 12h23m55s 12h23m55.222s
 */

static regex_t preg_hms_1;
static const char *pattern_hms_1 = "^((([0-9]|[0-1][0-9]|2[0-3])(\\.[0-9]*)?)|\\.[0-9]+)[hH\\*]?$";
static regex_t preg_hms_2;
static const char *pattern_hms_2 = "^([0-9]|[0-1][0-9]|2[0-3])[:hH\\*]((([0-9]|[0-5][0-9])(\\.[0-9]*)?)|\\.[0-9]+)[mM]?$";
static regex_t preg_hms_3;
static const char *pattern_hms_3 = "^([0-9]|[0-1][0-9]|2[0-3])[:hH\\*]([0-9]|[0-5][0-9])[:mM]((([0-9]|[0-5][0-9])(\\.[0-9]*)?)|\\.[0-9]+)[sS]?$";
static regex_t preg_hms_4;
static const char *pattern_hms_4 = "^24(\\.0*)?[hH\\*]?$|^24[:hH\\*](0{1,2}(\\.0*)?|\\.0+)[mM]?$|^24[:hH\\*]0{1,2}[:mM](0{1,2}(\\.0*)?|\\.0+)[sS]?$";

static regex_t preg_dms_1;
static const char *pattern_dms_1 = "^[+-]?((([0-9]|[0-8][0-9])(\\.[0-9]*)?)|\\.[0-9]+)[dD\\*]?$";
static regex_t preg_dms_2;
static const char *pattern_dms_2 = "^[+-]?([0-9]|[0-8][0-9])[:dD\\*]((([0-9]|[0-5][0-9])(\\.[0-9]*)?)|\\.[0-9]+)[mM]?$";
static regex_t preg_dms_3;
static const char *pattern_dms_3 = "^[+-]?([0-9]|[0-8][0-9])[:dD\\*]([0-9]|[0-5][0-9])[:mM]((([0-9]|[0-5][0-9])(\\.[0-9]*)?)|\\.[0-9]+)[sS]?$";
static regex_t preg_dms_4;
static const char *pattern_dms_4 = "^[+-]?90(\\.0*)?[dD\\*]?$|^[+-]?90[:dD\\*](0{1,2}(\\.0*)?|\\.0+)[mM]?$|^[+-]?90[:dD\\*]0{1,2}[:mM](0{1,2}(\\.0*)?|\\.0+)[sS]?$";

static regex_t preg_fmt;
static const char *pattern_fmt = "^%[+-]*[0-9]*\\.[0-9]*[HhMmSsDd]";

void
astro_init(void)
{
    regcomp(&preg_hms_1, pattern_hms_1, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_hms_2, pattern_hms_2, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_hms_3, pattern_hms_3, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_hms_4, pattern_hms_4, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_1, pattern_dms_1, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_2, pattern_dms_2, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_3, pattern_dms_3, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_4, pattern_dms_4, REG_EXTENDED | REG_NOSUB);
    
    regcomp(&preg_fmt, pattern_fmt, REG_EXTENDED);
}

void
astro_destroy(void)
{
    regfree(&preg_hms_1);
    regfree(&preg_hms_2);
    regfree(&preg_hms_3);
    regfree(&preg_hms_4);
    regfree(&preg_dms_1);
    regfree(&preg_dms_2);
    regfree(&preg_dms_3);
    regfree(&preg_dms_4);
    regfree(&preg_fmt);
}

static int
is_hms_string_legal(const char *string)
{
    int ret;
    
    if ((ret = regexec(&preg_hms_3, string, 0, NULL, 0)) == 0) {
        return 3;
    }
    
    if ((ret = regexec(&preg_hms_2, string, 0, NULL, 0)) == 0) {
        return 2;
    }
    
    if ((ret = regexec(&preg_hms_1, string, 0, NULL, 0)) == 0) {
        return 1;
    }
    
    if ((ret = regexec(&preg_hms_4, string, 0, NULL, 0)) == 0) {
        return 4;
    }
    
    return 0;
}

static double
hms2deg_1(const char *string)
{
    char *buf;
    size_t size = strlen(string);
    
    if ((buf = (char *) malloc(size + 1)) == NULL) {
        return -1.0;
    }
    
    memcpy(buf, string, size + 1);
    switch (buf[size - 1]) {
        case 'H':
        case 'h':
        case '*':
            buf[size - 1] = '\0';
            break;
        default:
            break;
    }
    
    free(buf);
    
    return atof(buf) * 15.;
}

static double
hms2deg_2(const char *string)
{
    char *buf;
    const char *s, *s2;
    size_t size = strlen(string);
    double ret;
    
    if ((buf = (char *) malloc(size + 1)) == NULL) {
        return -1.0;
    }
    
    s = string;
    s2 = s;
    while (*s >= '0' && *s <= '9') {
        s++;
    }
    memset(buf, '\0', size + 1);
    memcpy(buf, s2, s - s2);
    s++;
    ret = atof(buf) * 15.;
    
    memcpy(buf, s, strlen(s) + 1);
    switch (buf[strlen(s) - 1]) {
        case 'M':
        case 'm':
        case ':':
            buf[strlen(s) - 1] = '\0';
            break;
        default:
            break;
    }
    ret += atof(buf) / 4.;
    
    free(buf);
    
    return ret;
}

static double
hms2deg_3(const char *string)
{
    char *buf;
    const char *s, *s2;
    size_t size = strlen(string);
    double ret;
    
    if ((buf = (char *) malloc(size + 1)) == NULL) {
        return -1.0;
    }
    
    s = string;
    s2 = s;
    while (*s >= '0' && *s <= '9') {
        s++;
    }
    memset(buf, '\0', size + 1);
    memcpy(buf, s2, s - s2);
    s++;
    ret = atof(buf) * 15.;
    
    s2 = s;
    while (*s >= '0' && *s <= '9') {
        s++;
    }
    memset(buf, '\0', size + 1);
    memcpy(buf, s2, s - s2);

    ret += atof(buf) / 4.;
    
    s++;
    memcpy(buf, s, strlen(s) + 1);
    switch (buf[strlen(s) - 1]) {
        case 'S':
        case 's':
        case ':':
            buf[strlen(s) - 1] = '\0';
            break;
        default:
            break;
    }
    ret += atof(buf) / 240.;
    
    free(buf);
    
    return ret;
}

double
hms2deg(const char *string)
{
    int ret = is_hms_string_legal(string);
    
    switch (ret) {
        case 1:
            return hms2deg_1(string);
            break;
        case 2:
            return hms2deg_2(string);
            break;
        case 3:
            return hms2deg_3(string);
            break;
        case 4:
            return 360.;
            break;
        default:
            break;
    }
    return -1.0;
}

char *
deg2hms(double deg, char *hms, size_t size, const char *fmt)
{
    
    if (deg < 0 || deg >= 360.) {
        printf("%lf\n", deg);
        fprintf(stderr, "input out of range.\n");
        return NULL;
    }
    
    FILE *fp = fmemopen(hms, size, "w");
    const char *idx = fmt;
    
    double dhh, dmm, dss;
    int ihh, imm, iss;
    
    char *buf = (char *) malloc(strlen(fmt) + 1);
    
    dhh = deg / 15.;
    ihh = floor(dhh);
    dmm = (dhh - ihh) * 60.;
    imm = floor(dmm);
    dss = (dmm - imm) * 60.;
    iss = floor(dss);
    
    while (*idx != '\0') {
        switch (*idx) {
            case '%':
                switch (*(idx + 1)) {
                    case '%':
                        fputc('%', fp);
                        idx++;
                        break;
                    case 'H':
                        fprintf(fp, "%02d", ihh);
                        idx++;
                        break;
                    case 'h':
                        fprintf(fp, "%d", ihh);
                        idx++;
                        break;
                    case 'M':
                        fprintf(fp, "%02d", imm);
                        idx++;
                        break;
                    case 'm':
                        fprintf(fp, "%d", imm);
                        idx++;
                        break;
                    case 'S':
                        fprintf(fp, "%02d", iss);
                        idx++;
                        break;
                    case 's':
                        fprintf(fp, "%d", iss);
                        idx++;
                        break;
                    default:
                        /*
                         * something like %5.2h
                         */
                    {
                        int ret;
                        regmatch_t pmatch;
                        if ((ret = regexec(&preg_fmt, idx, 1, &pmatch, 0)) == 0) {
                            memset(buf, '\0', strlen(fmt) + 1);
                            memcpy(buf, idx, pmatch.rm_eo - pmatch.rm_so);
                            char c = buf[pmatch.rm_eo - pmatch.rm_so - 1];
                            switch (c) {
                                case 'H':
                                case 'h':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dhh);
                                    break;
                                case 'M':
                                case 'm':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dmm);
                                    break;
                                case 'S':
                                case 's':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dss);
                                    break;
                                default:
                                    fprintf(stderr, "illegal format.\n");
                                    free(buf);
                                    fclose(fp);
                                    break;
                            }
                            idx += pmatch.rm_eo - pmatch.rm_so - 1;
                        } else {
                            fprintf(stderr, "illegal format.\n");
                            free(buf);
                            fclose(fp);
                            return NULL;
                        }
                    }
                        break;
                }
                break;
            default:
                fputc(*idx, fp);
                break;
        }
        idx++;
    }
    
    free(buf);
    fclose(fp);
    
    return hms;
}

static int
is_dms_string_legal(const char *string)
{
    
    int ret;
    
    if ((ret = regexec(&preg_dms_3, string, 0, NULL, 0)) == 0) {
        return 3;
    }
    
    if ((ret = regexec(&preg_dms_2, string, 0, NULL, 0)) == 0) {
        return 2;
    }
    
    if ((ret = regexec(&preg_dms_1, string, 0, NULL, 0)) == 0) {
        return 1;
    }
    
    if ((ret = regexec(&preg_dms_4, string, 0, NULL, 0)) == 0) {
        return 4;
    }
    
    return 0;
}

static double
dms2deg_1(const char *string)
{
    char *buf;
    size_t size = strlen(string);
    
    if ((buf = (char *) malloc(size + 1)) == NULL) {
        return -91.0;
    }
    
    memcpy(buf, string, size + 1);
    switch (buf[size - 1]) {
        case 'D':
        case 'd':
        case '*':
        case ':':
            buf[size - 1] = '\0';
            break;
        default:
            break;
    }
    
    free(buf);
    
    return atof(buf);
}

static double
dms2deg_2(const char *string)
{
    char *buf;
    const char *s, *s2;
    size_t size = strlen(string);
    int sign = 1;
    double ret;
    
    if ((buf = (char *) malloc(size + 1)) == NULL) {
        return -91.0;
    }
    
    s = string;
    if (*s == '+') {
        s++;
    } else if (*s == '-') {
        sign = -1;
        s++;
    }
    s2 = s;
    while (*s >= '0' && *s <= '9') {
        s++;
    }
    memset(buf, '\0', size + 1);
    memcpy(buf, s2, s - s2);
    s++;
    ret = atof(buf);
    
    memcpy(buf, s, strlen(s) + 1);
    switch (buf[strlen(s) - 1]) {
        case 'M':
        case 'm':
        case ':':
            buf[strlen(s) - 1] = '\0';
            break;
        default:
            break;
    }
    ret += atof(buf) / 60.;
    
    free(buf);
    
    return ret * sign;
}

static double
dms2deg_3(const char *string)
{
    char *buf;
    const char *s, *s2;
    size_t size = strlen(string);
    int sign = 1;
    double ret;
    
    if ((buf = (char *) malloc(size + 1)) == NULL) {
        return -91.0;
    }
    
    s = string;
    if (*s == '+') {
        s++;
    } else if (*s == '-') {
        sign = -1;
        s++;
    }
    s2 = s;
    while (*s >= '0' && *s <= '9') {
        s++;
    }
    memset(buf, '\0', size + 1);
    memcpy(buf, s2, s - s2);
    s++;
    ret = atof(buf);
    
    s2 = s;
    while (*s >= '0' && *s <= '9') {
        s++;
    }
    memset(buf, '\0', size + 1);
    memcpy(buf, s2, s - s2);
    
    ret += atof(buf) / 60.;
    
    s++;
    memcpy(buf, s, strlen(s) + 1);
    switch (buf[strlen(s) - 1]) {
        case 'S':
        case 's':
        case ':':
            buf[strlen(s) - 1] = '\0';
            break;
        default:
            break;
    }
    ret += atof(buf) / 3600.;
    
    free(buf);
    
    return ret * sign;
}

double
dms2deg(const char *string)
{
    int ret = is_dms_string_legal(string);
    
    switch (ret) {
        case 1:
            return dms2deg_1(string);
            break;
        case 2:
            return dms2deg_2(string);
            break;
        case 3:
            return dms2deg_3(string);
            break;
        case 4:
            if (*string == '-') {
                return -90.;
            } else {
                return 90.;
            }
            break;
        default:
            break;
    }
    return -91.0;
}

char *
deg2dms(double deg, char *dms, size_t size, const char *fmt)
{
    int sign = 1;
    if (deg < -90. || deg > 90.) {
        printf("%lf\n", deg);
        fprintf(stderr, "input out of range.\n");
        return NULL;
    }
    
    if (deg < 0) {
        sign = -1;
        deg *= sign;
    }
    
    FILE *fp = fmemopen(dms, size, "w");
    const char *idx = fmt;
    
    double ddd, dmm, dss;
    int idd, imm, iss;
    
    char *buf = (char *) malloc(strlen(fmt) + 1);
    
    ddd = deg;
    idd = floor(ddd);
    dmm = (ddd - idd) * 60.;
    imm = floor(dmm);
    dss = (dmm - imm) * 60.;
    iss = floor(dss);
    
    while (*idx != '\0') {
        switch (*idx) {
            case '%':
                switch (*(idx + 1)) {
                    case '%':
                        fputc('%', fp);
                        idx++;
                        break;
                    case 'D':
                        if (sign > 0) {
                            fprintf(fp, "%02d", idd);
                        } else {
                            fprintf(fp, "-%02d", idd);
                        }
                        idx++;
                        break;
                    case 'd':
                        fprintf(fp, "%d", idd * sign);
                        idx++;
                        break;
                    case 'M':
                        fprintf(fp, "%02d", imm);
                        idx++;
                        break;
                    case 'm':
                        fprintf(fp, "%d", imm);
                        idx++;
                        break;
                    case 'S':
                        fprintf(fp, "%02d", iss);
                        idx++;
                        break;
                    case 's':
                        fprintf(fp, "%d", iss);
                        idx++;
                        break;
                    default:
                    {
                        int ret;
                        regmatch_t pmatch;
                        if ((ret = regexec(&preg_fmt, idx, 1, &pmatch, 0)) == 0) {
                            memset(buf, '\0', strlen(fmt) + 1);
                            memcpy(buf, idx, pmatch.rm_eo - pmatch.rm_so);
                            char c = buf[pmatch.rm_eo - pmatch.rm_so - 1];
                            switch (c) {
                                case 'D':
                                case 'd':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, ddd * sign);
                                    break;
                                case 'M':
                                case 'm':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dmm);
                                    break;
                                case 'S':
                                case 's':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dss);
                                    break;
                                default:
                                    fprintf(stderr, "illegal format.\n");
                                    free(buf);
                                    fclose(fp);
                                    break;
                            }
                            idx += pmatch.rm_eo - pmatch.rm_so - 1;
                        } else {
                            fprintf(stderr, "illegal format.\n");
                            free(buf);
                            fclose(fp);
                            return NULL;
                        }
                    }
                        break;
                }
                break;
            default:
                fputc(*idx, fp);
                break;
        }
        idx++;
    }
    
    free(buf);
    fclose(fp);
    
    return dms;
}

char *
lon_deg2dms(double deg, char *dms, size_t size, const char *fmt)
{
    if (deg < 0 || deg > 360) {
        fprintf(stderr, "input out of range.\n");
        return NULL;
    }
    
    FILE *fp = fmemopen(dms, size, "w");
    const char *idx = fmt;
    
    double ddd, dmm, dss;
    int idd, imm, iss;
    
    char *buf = (char *) malloc(strlen(fmt) + 1);
    
    ddd = deg;
    idd = floor(ddd);
    dmm = (ddd - idd) * 60.;
    imm = floor(dmm);
    dss = (dmm - imm) * 60.;
    iss = floor(dss);
    
    while (*idx != '\0') {
        switch (*idx) {
            case '%':
                switch (*(idx + 1)) {
                    case '%':
                        fputc('%', fp);
                        idx++;
                        break;
                    case 'D':
                        fprintf(fp, "%03d", idd);
                        idx++;
                        break;
                    case 'd':
                        fprintf(fp, "%d", idd);
                        idx++;
                        break;
                    case 'M':
                        fprintf(fp, "%02d", imm);
                        idx++;
                        break;
                    case 'm':
                        fprintf(fp, "%d", imm);
                        idx++;
                        break;
                    case 'S':
                        fprintf(fp, "%02d", iss);
                        idx++;
                        break;
                    case 's':
                        fprintf(fp, "%d", iss);
                        idx++;
                        break;
                    default:
                    {
                        int ret;
                        regmatch_t pmatch;
                        if ((ret = regexec(&preg_fmt, idx, 1, &pmatch, 0)) == 0) {
                            memset(buf, '\0', strlen(fmt) + 1);
                            memcpy(buf, idx, pmatch.rm_eo - pmatch.rm_so);
                            char c = buf[pmatch.rm_eo - pmatch.rm_so - 1];
                            switch (c) {
                                case 'D':
                                case 'd':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, ddd);
                                    break;
                                case 'M':
                                case 'm':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dmm);
                                    break;
                                case 'S':
                                case 's':
                                    buf[pmatch.rm_eo - pmatch.rm_so - 1] = 'f';
                                    fprintf(fp, buf, dmm);
                                    break;
                                default:
                                    fprintf(stderr, "illegal format.\n");
                                    free(buf);
                                    fclose(fp);
                                    break;
                            }
                            idx += pmatch.rm_eo - pmatch.rm_so - 1;
                        } else {
                            fprintf(stderr, "illegal format.\n");
                            free(buf);
                            fclose(fp);
                            return NULL;
                        }
                    }
                        break;
                }
                break;
            default:
                fputc(*idx, fp);
                break;
        }
        idx++;
    }
    
    free(buf);
    fclose(fp);
    return dms;
}

char *
lat_deg2dms(double deg, char *dms, size_t size, const char *fmt)
{
    return deg2dms(deg, dms, size, fmt);
}

#ifndef PI
#define PI 3.141592654
#endif

/*
 * Reference: Textbook on Spherical Astronomy.
 */

double
spherical_distance_rad(double theta1_rad, double phi1_rad, double theta2_rad, double phi2_rad)
{
    return acos(cos(theta1_rad - theta2_rad) - sin(theta1_rad) * sin(theta2_rad) * (1 - cos(phi1_rad - phi2_rad)));
}

double
equatorial_distance(double ra1_deg, double dec1_deg, double ra2_deg, double dec2_deg)
{
    return spherical_distance_rad(PI * (.5 - dec1_deg / 180.) , ra1_deg * PI / 180., PI * (.5 - dec2_deg / 180.) , ra2_deg * PI / 180.) * 180. / PI;
}



/*
static double
jd2000(time_t tloc)
{
    return (double)(tloc - J2000) / ONEDAY + DAYOFFSET;
}
 */

double
jd_tp(struct timespec *tp)
#ifdef __USE_SOFA__
{
    struct tm tm_buf;
    gmtime_r(&tp->tv_sec, &tm_buf);
    double date1, date2;
    tm_buf.tm_year += 1900;
    tm_buf.tm_mon++;
    iauDtf2d("UTC", tm_buf.tm_year, tm_buf.tm_mon, tm_buf.tm_mday, tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec + tp->tv_nsec / 1000000000., &date1, &date2);
    
    return date1 + date2;
}
#else
{
    struct tm tm_buf;
    
    gmtime_r(&tp->tv_sec, &tm_buf);
    tm_buf.tm_year += 1900;
    tm_buf.tm_mon++;
    
    double y, m, d;
    double A, B, C, D;
    
    if (tm_buf.tm_mon == 1 || tm_buf.tm_mon == 2) {
        y = tm_buf.tm_year - 1;
        m = tm_buf.tm_mon + 12;
    } else {
        y = tm_buf.tm_year;
        m = tm_buf.tm_mon;
    }
    
    d = tm_buf.tm_mday + (tm_buf.tm_hour + tm_buf.tm_min / 60. + tm_buf.tm_sec / 3600. + tp->tv_nsec / 1000000000.) / 24.;

    A = floor(y/100);
    if (tp->tv_sec < -3698870400) {
        B = 0.;
    } else {
        B = 2. - A + floor(A / 4);
    }
    
    if (y < 0) {
        C = floor(365.25 * y - 0.75);
    } else {
        C = floor(365.25 * y);
    }
    
    D = floor(30.6001 *(m + 1));
    return B + C + D + d + 1720994.5;
}
#endif

double
jd(double t)
{
    struct timespec tp;
    
    tp.tv_sec = floor(t);
    tp.tv_nsec = (t - tp.tv_sec) * 1000000000.;
    
    return jd_tp(&tp);
}

#ifndef __USE_SOFA__
void
precess_matrix(double equinox1, double equinox2, bool is_FK4, double matrix[3][3])
{
    double deg_to_rad = PI / 180.0, sec_to_rad = deg_to_rad / 3600.0;
    double t = 0.001 * (equinox2 - equinox1);
    double st, A, B, C;
    double cosa, cosb, cosc, sina, sinb, sinc;
    
    if (is_FK4) {
        st = 0.001 * (equinox1 - 1900.0);
        A = sec_to_rad * t * (23042.53 + st * (139.75 + 0.06 * st) + t * (30.23 - 0.27 * st + 18.0 * t));
        B = sec_to_rad * t * t * (79.27 + 0.66 * st + 0.32 * t) + A;
        C = sec_to_rad * t * (20046.85 - st * (85.33 + 0.37 * st) + t * (-42.67 - 0.37 * st - 41.8 * t));
    } else {
        st = 0.001 * (equinox1 - 2000.0);
        A = sec_to_rad * t * (23062.181 + st * (139.656 + 0.0139 * st) + t * (30.188 - 0.344 * st + 17.998 * t));
        B = sec_to_rad * t * t * (79.280 + 0.410 * st + 0.205 * t) + A;
        C = sec_to_rad * t * (20043.109 - st * (85.33 + 0.217 * st) + t * (-42.665 - 0.217 * st - 41.833 * t));
    }
    
    cosa = cos(A);
    cosb = cos(B);
    cosc = cos(C);
    sina = sin(A);
    sinb = sin(B);
    sinc = sin(C);
    
    matrix[0][0] = cosa * cosb * cosc - sina * sinb;
    matrix[0][1] = sina * cosb + cosa * sinb * cosc;
    matrix[0][2] = cosa * sinc;
    matrix[1][0] = -cosa * sinb - sina * cosb * cosc;
    matrix[1][1] = cosa * cosb - sina * sinb * cosc;
    matrix[1][2] = -sina * sinc;
    matrix[2][0] = -cosb * sinc;
    matrix[2][1] = -sinb * sinc;
    matrix[2][2] = cosc;
}

void
precess(double ra, double dec, double equinox1, double equinox2, bool is_FK4, double *ra_out, double *dec_out)
{
    double ra_rad, dec_rad, a;
    double x[3], x2[3], matrix[3][3];
    
    ra_rad = ra * PI / 180.;
    dec_rad = dec * PI / 180.;
    a = cos(dec_rad);
    x[0] = a * cos(ra_rad);
    x[1] = a * sin(ra_rad);
    x[2] = sin(dec_rad);
    
    
    double deg_to_rad = PI / 180.0, sec_to_rad = deg_to_rad / 3600.0;
    double t = 0.001 * (equinox2 - equinox1);
    double st, A, B, C;
    double cosa, cosb, cosc, sina, sinb, sinc;
    
    if (is_FK4) {
        st = 0.001 * (equinox1 - 1900.0);
        A = sec_to_rad * t * (23042.53 + st * (139.75 + 0.06 * st) + t * (30.23 - 0.27 * st + 18.0 * t));
        B = sec_to_rad * t * t * (79.27 + 0.66 * st + 0.32 * t) + A;
        C = sec_to_rad * t * (20046.85 - st * (85.33 + 0.37 * st) + t * (-42.67 - 0.37 * st - 41.8 * t));
    } else {
        st = 0.001 * (equinox1 - 2000.0);
        A = sec_to_rad * t * (23062.181 + st * (139.656 + 0.0139 * st) + t * (30.188 - 0.344 * st + 17.998 * t));
        B = sec_to_rad * t * t * (79.280 + 0.410 * st + 0.205 * t) + A;
        C = sec_to_rad * t * (20043.109 - st * (85.33 + 0.217 * st) + t * (-42.665 - 0.217 * st - 41.833 * t));
    }
    
    cosa = cos(A);
    cosb = cos(B);
    cosc = cos(C);
    sina = sin(A);
    sinb = sin(B);
    sinc = sin(C);
    
    matrix[0][0] = cosa * cosb * cosc - sina * sinb;
    matrix[0][1] = sina * cosb + cosa * sinb * cosc;
    matrix[0][2] = cosa * sinc;
    matrix[1][0] = -cosa * sinb - sina * cosb * cosc;
    matrix[1][1] = cosa * cosb - sina * sinb * cosc;
    matrix[1][2] = -sina * sinc;
    matrix[2][0] = -cosb * sinc;
    matrix[2][1] = -sinb * sinc;
    matrix[2][2] = cosc;
    
    x2[0] = x[0] * matrix[0][0] + x[1] * matrix[1][0] + x[2] * matrix[2][0];
    x2[1] = x[0] * matrix[0][1] + x[1] * matrix[1][1] + x[2] * matrix[2][1];
    x2[2] = x[0] * matrix[0][2] + x[1] * matrix[1][2] + x[2] * matrix[2][2];
    
    *ra_out = atan2(x2[1], x2[0]) * 180. / PI;
    *dec_out = asin(x2[2]) * 180. / PI;
}

static void
nutate_(double jd, double *nut_lon, double *nut_obliq)
{
    double jdcen = (jd - 2451545.0) / 36525.;
    double coef_moon[4] = {1.0/189474.0, -0.0019142, 445267.111480, 297.85036};
    double d = (coef_moon[3] + jdcen * coef_moon[2] + jdcen * jdcen * coef_moon[1] + jdcen * jdcen * jdcen * coef_moon[0]) * PI / 180.;
    double coef_sun[4] = {-1.0/3e5, -0.0001603, 35999.050340, 357.52772};
    double sun = (coef_sun[3] + jdcen * coef_sun[2] + jdcen * jdcen * coef_sun[1] + jdcen * jdcen * jdcen * coef_sun[0]) * PI / 180.;
    double coef_mano[4] = {1.0/5.625e4, 0.0086972, 477198.867398, 134.96298};
    double mano = (coef_mano[3] + jdcen * coef_mano[3] + jdcen * jdcen * coef_mano[2] + jdcen * jdcen * jdcen * coef_mano[0]) * PI / 180.;
    double coef_mlat[4] = {-1.0/3.27270e5, -0.0036825, 483202.017538, 93.27191};
    double mlat = (coef_mlat[3] + jdcen * coef_mlat[2] + jdcen * jdcen * coef_mlat[1] + jdcen * jdcen * jdcen * coef_mlat[0]) * PI / 180.;
    double coef_moe[4] = {1.0/4.5e5, 0.0020708, -1934.136261, 125.04452};
    double moe = (coef_moe[3] + jdcen * coef_moe[2] + jdcen * jdcen * coef_moe[1] + jdcen * jdcen * jdcen * coef_moe[0]) * PI / 180.;
    double d_lng[63] = {0.,-2.,0.,0.,0.,0.,-2.,0.,0.,-2,-2,-2,0,2,0,2,0,0,-2,0,2,0,0,-2,0,-2,0,0,2,-2,0,-2,0,0,2,2,0,-2,0,2,2,-2,-2,2,2,0,-2,-2,0,-2,-2,0,-1,-2,1,0,0,-1,0,0,2,0,2};
    double m_lng[63] = {0,0,0,0,1,0,1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,1,0,-1,0,0,0,1,1,-1,0,0,0,0,0,0,-1,-1,0,0,0,1,0,0,1,0,0,0,-1,1,-1,-1,0,-1};
    double mp_lng[63] = {0,0,0,0,0,1,0,0,1,0,1,0,-1,0,1,-1,-1,1,2,-2,0,2,2,1,0,0,-1,0,-1,0,0,1,0,2,-1,1,0,1,0,0,1,2,1,-2,0,1,0,0,2,2,0,1,1,0,0,1,-2,1,1,1,-1,3,0};
    double f_lng[63] = {0,2,2,0,0,0,2,2,2,2,0,2,2,0,0,2,0,2,0,2,2,2,0,2,2,2,2,0,0,2,0,0,0,-2,2,2,2,0,2,2,0,2,2,0,0,0,2,0,2,0,2,-2,0,0,0,2,2,0,0,2,2,2,2};
    double om_lng[63] = {1,2,2,2,0,0,2,1,2,2,0,1,2,0,1,2,1,1,0,1,2,2,0,2,0,0,1,0,1,2,1,1,1,0,1,2,2,0,2,1,0,2,1,1,1,0,1,1,1,1,1,0,0,0,0,0,2,0,0,2,2,2,2};
    double sin_lng[63] = {-171996, -13187, -2274, 2062, 1426, 712, -517, -386, -301, 217, -158, 129, 123, 63, 63, -59, -58, -51, 48, 46, -38, -31, 29, 29, 26, -22, 21, 17, 16, -16, -15, -13, -12, 11, -10, -8, 7, -7, -7, -7, 6,6,6,-6,-6,5,-5,-5,-5,4,4,4,-4,-4,-4,3,-3,-3,-3,-3,-3,-3,-3};
    double sdelt[63] = {-174.2, -1.6, -0.2, 0.2, -3.4, 0.1, 1.2, -0.4, 0., -0.5, 0., 0.1, 0.,0.,0.1, 0.,-0.1,0.,0.,0.,0.,0.,0.,0.,0.,0.,0., -0.1, 0., 0.1,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
    double cos_lng[63] = {92025, 5736, 977, -895, 54, -7, 224, 200, 129, -95,0,-70,-53,0,-33, 26, 32, 27, 0, -24, 16,13,0,-12,0,0,-10,0,-8,7,9,7,6,0,5,3,-3,0,3,3,0,-3,-3,3,3,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double cdelt[63] = {8.9, -3.1, -0.5, 0.5, -0.1, 0.0, -0.6, 0.0, -0.1, 0.3,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
    size_t i;
    double arg[63], sarg[63], carg[63];

    d = fmod(d, 2.0 * PI);
    sun = fmod(sun, 2.0 * PI);
    mano = fmod(mano, 2.0 * PI);
    mlat = fmod(mlat, 2.0 * PI);
    moe = fmod(moe, 2.0 * PI);
    
    *nut_lon = .0;
    *nut_obliq = .0;

    for (i = 0; i < 63; i++) {
        arg[i] = d * d_lng[i] + sun * m_lng[i] + mano * mp_lng[i] + mlat * f_lng[i] + moe * om_lng[i];
        sarg[i] = sin(arg[i]);
        carg[i] = cos(arg[i]);
        *nut_lon += 0.0001 * (sdelt[i] * jdcen + sin_lng[i]) * sarg[i];
        *nut_obliq += 0.0001 * (cdelt[i] * jdcen + cos_lng[i]) * carg[i];
    }
    
    *nut_lon /= 3600.;
    *nut_obliq /= 3600.;
}

void
nutate(double jd, double ra, double dec, double *d_ra, double *d_dec, double *eps_out, double *d_psi_out, double *d_eps_out)
{
    double jdcen = (jd - 2451545.0) / 36525.;
    double nut_lon = .0, nut_obliq = .0;
    double coef_moon[4] = {1.0/189474.0, -0.0019142, 445267.111480, 297.85036};
    double d = (coef_moon[3] + jdcen * coef_moon[2] + jdcen * jdcen * coef_moon[1] + jdcen * jdcen * jdcen * coef_moon[0]) * PI / 180.;
    double coef_sun[4] = {-1.0/3e5, -0.0001603, 35999.050340, 357.52772};
    double sun = (coef_sun[3] + jdcen * coef_sun[2] + jdcen * jdcen * coef_sun[1] + jdcen * jdcen * jdcen * coef_sun[0]) * PI / 180.;
    double coef_mano[4] = {1.0/5.625e4, 0.0086972, 477198.867398, 134.96298};
    double mano = (coef_mano[3] + jdcen * coef_mano[3] + jdcen * jdcen * coef_mano[2] + jdcen * jdcen * jdcen * coef_mano[0]) * PI / 180.;
    double coef_mlat[4] = {-1.0/3.27270e5, -0.0036825, 483202.017538, 93.27191};
    double mlat = (coef_mlat[3] + jdcen * coef_mlat[2] + jdcen * jdcen * coef_mlat[1] + jdcen * jdcen * jdcen * coef_mlat[0]) * PI / 180.;
    double coef_moe[4] = {1.0/4.5e5, 0.0020708, -1934.136261, 125.04452};
    double moe = (coef_moe[3] + jdcen * coef_moe[2] + jdcen * jdcen * coef_moe[1] + jdcen * jdcen * jdcen * coef_moe[0]) * PI / 180.;
    double d_lng[63] = {0.,-2.,0.,0.,0.,0.,-2.,0.,0.,-2,-2,-2,0,2,0,2,0,0,-2,0,2,0,0,-2,0,-2,0,0,2,-2,0,-2,0,0,2,2,0,-2,0,2,2,-2,-2,2,2,0,-2,-2,0,-2,-2,0,-1,-2,1,0,0,-1,0,0,2,0,2};
    double m_lng[63] = {0,0,0,0,1,0,1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,1,0,-1,0,0,0,1,1,-1,0,0,0,0,0,0,-1,-1,0,0,0,1,0,0,1,0,0,0,-1,1,-1,-1,0,-1};
    double mp_lng[63] = {0,0,0,0,0,1,0,0,1,0,1,0,-1,0,1,-1,-1,1,2,-2,0,2,2,1,0,0,-1,0,-1,0,0,1,0,2,-1,1,0,1,0,0,1,2,1,-2,0,1,0,0,2,2,0,1,1,0,0,1,-2,1,1,1,-1,3,0};
    double f_lng[63] = {0,2,2,0,0,0,2,2,2,2,0,2,2,0,0,2,0,2,0,2,2,2,0,2,2,2,2,0,0,2,0,0,0,-2,2,2,2,0,2,2,0,2,2,0,0,0,2,0,2,0,2,-2,0,0,0,2,2,0,0,2,2,2,2};
    double om_lng[63] = {1,2,2,2,0,0,2,1,2,2,0,1,2,0,1,2,1,1,0,1,2,2,0,2,0,0,1,0,1,2,1,1,1,0,1,2,2,0,2,1,0,2,1,1,1,0,1,1,1,1,1,0,0,0,0,0,2,0,0,2,2,2,2};
    double sin_lng[63] = {-171996, -13187, -2274, 2062, 1426, 712, -517, -386, -301, 217, -158, 129, 123, 63, 63, -59, -58, -51, 48, 46, -38, -31, 29, 29, 26, -22, 21, 17, 16, -16, -15, -13, -12, 11, -10, -8, 7, -7, -7, -7, 6,6,6,-6,-6,5,-5,-5,-5,4,4,4,-4,-4,-4,3,-3,-3,-3,-3,-3,-3,-3};
    double sdelt[63] = {-174.2, -1.6, -0.2, 0.2, -3.4, 0.1, 1.2, -0.4, 0., -0.5, 0., 0.1, 0.,0.,0.1, 0.,-0.1,0.,0.,0.,0.,0.,0.,0.,0.,0.,0., -0.1, 0., 0.1,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
    double cos_lng[63] = {92025, 5736, 977, -895, 54, -7, 224, 200, 129, -95,0,-70,-53,0,-33, 26, 32, 27, 0, -24, 16,13,0,-12,0,0,-10,0,-8,7,9,7,6,0,5,3,-3,0,3,3,0,-3,-3,3,3,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double cdelt[63] = {8.9, -3.1, -0.5, 0.5, -0.1, 0.0, -0.6, 0.0, -0.1, 0.3,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
    double d_psi = nut_lon;
    double d_eps = nut_obliq;
    double eps0, eps, ce, se;
    double x, y, z, x2, y2, z2, r, xyproj, ra2, dec2;
    double arg[63], sarg[63], carg[63];
    size_t i;

    d = fmod(d, 2.0 * PI);
    sun = fmod(sun, 2.0 * PI);
    mano = fmod(mano, 2.0 * PI);
    mlat = fmod(mlat, 2.0 * PI);
    moe = fmod(moe, 2.0 * PI);

    for (i = 0; i < 63; i++) {
        arg[i] = d * d_lng[i] + sun * m_lng[i] + mano * mp_lng[i] + mlat * f_lng[i] + moe * om_lng[i];
        sarg[i] = sin(arg[i]);
        carg[i] = cos(arg[i]);
        nut_lon += 0.0001 * (sdelt[i] * jdcen + sin_lng[i]) * sarg[i];
        nut_obliq += 0.0001 * (cdelt[i] * jdcen + cos_lng[i]) * carg[i];
    }
    
    nut_lon /= 3600.;
    nut_obliq /= 3600.;

    eps0 = 23.4392911 * 3600. - 46.8150 * jdcen - 0.00059 * jdcen * jdcen + 0.001813 * jdcen * jdcen * jdcen;
    eps = (eps0 + d_eps) /3600. * (PI / 180.);

    se = sin(eps);
    ce = cos(eps);

    x = cos(ra * PI / 180.) * cos(dec * PI / 180.);
    y = sin(ra * PI / 180.) * cos(dec * PI / 180.);
    z = sin(dec * PI / 180.);
    
    x2 = x - (y*ce + z*se)*d_psi * (PI/(180.*3600.));
    y2 = y + (x*ce*d_psi - z*d_eps) * (PI/(180.*3600.));
    z2 = z + (x*se*d_psi + y*d_eps) * (PI/(180.*3600.));
    
    r = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
    xyproj = sqrt(x2 * x2 + y2 * y2);
    
    ra2 = x2 * 0.;
    dec2 = x2 * 0.;

    if (xyproj == 0 && z != 0) {
        dec2 = asin(z2/r);
        ra2 = 0.;
    } else if (xyproj != 0) {
        ra2 = atan2(y2, x2);
        dec2 = asin(z2/r);
    }
    
    ra2 /= PI / 180.;
    dec2 /= PI / 180.;
    eps /= PI / 180.;
    d_psi /= 3600.;
    d_eps /= 3600.;
    
    if (ra2 < 0) {
        ra2 += 360.;
    }
    
    *d_ra = ra2 - ra;
    *d_dec = dec2 - dec;
    if (eps_out != NULL) {
        *eps_out = eps;
    }
    if (d_psi_out != NULL) {
        *d_psi_out = d_psi;
    }
    if (d_eps_out != NULL) {
        *d_eps_out = d_eps;
    }
}

static double
fmod_(double x, double y)
{
    double m;
    m = fmod(x, y);
    if (x < 0) {
        m -= y;
    }
    return m;
}

void
sun_position(double jd, double *ra, double *dec, double *lon_out, double *oblt_out)
{
    jd = (jd - 2415020.0) / 36525.0;
    double sun_lon  = (279.696678 + fmod_((36000.768925 * jd), 360.0)) * 3600.0;
    double me = 358.475844 + fmod_((35999.049750 * jd), 360.0);
    double ellcor = (6910.1 - 17.2 * jd) * sin(me * PI / 180.) + 72.3 * sin(2.0 * me * PI / 180.);
    double mv = 212.603219 + fmod_((58517.803875 * jd), 360.0);
    
    sun_lon += ellcor;
    
    double vencorr = 4.8 * cos((299.1017 + mv - me) * PI / 180.) + 5.5 * cos((148.3133 + 2.0 * mv - 2.0 * me) * PI / 180.) + 2.5 * cos((315.9433 + 2.0 * mv - 3.0 * me) * PI / 180.) + 1.6 * cos((345.2533 + 3.0 * mv - 4.0 * me) * PI / 180.) + 1.0 * cos((318.15 + 3.0 * mv - 5.0 * me) * PI / 180.);
    sun_lon += vencorr;
    
    double mm = 319.529425 + fmod_((19139.858500 * jd), 360.0);
    double marscorr = 2.0 * cos((343.8883 - 2.0 * mm + 2.0 * me) * PI / 180.) + 1.8 * cos((200.4017 - 2.0 * mm + me) * PI / 180.);
    sun_lon += marscorr;
    
    double mj = 225.328328 + fmod_((3034.6920239 * jd), 360.0);
    double jupcorr = 7.2 * cos((179.5317 - mj + me) * PI / 180.) + 2.6 * cos((263.2167 - mj) * PI / 180.) + 2.7 * cos((87.1450 - 2.0 * mj + 2.0 * me) * PI / 180.) + 1.6 * cos((109.4933 - 2.0 * mj + me) * PI / 180.);
    sun_lon += jupcorr;
    
    double d = 350.7376814 + fmod_((445267.11422 * jd), 360.0);
    double mooncorr = 6.5 * sin(d * PI / 180.);
    sun_lon += mooncorr;
    
    double longterm = 6.4 * sin((231.19 + 20.20 * jd) * PI / 180.);
    sun_lon += longterm;
    sun_lon = fmod_(sun_lon + 2592000.0, 1296000.0);
    
    double longmed = sun_lon / 3600.0;
    sun_lon -= 20.5;
    double omega = 259.183275 - fmod_((1934.142008 * jd), 360.0);
    sun_lon = sun_lon - 17.2 * sin(omega * PI / 180.);
    double oblt = 23.452294 - 0.0130125 * jd + (9.2 * cos(omega * PI / 180.)) / 3600.0;
    
    sun_lon /= 3600.;
    *ra = atan2(sin(sun_lon * PI / 180.) * cos(oblt * PI / 180.), cos(sun_lon * PI / 180.));
    *ra = fmod(*ra, 2 * PI) * 180. / PI;
    if (*ra < 0) {
        *ra += 360.;
    }
    *dec = asin(sin(sun_lon * PI / 180.) * sin(oblt * PI / 180.)) * 180. / PI;
    
    if (lon_out != NULL) {
        *lon_out = longmed;
    }
    
    if (oblt_out != NULL) {
        *oblt_out = oblt;
    }
}

void
aberration(double jd, double ra, double dec, double *d_ra, double *d_dec)
{
    double jdcen = ( jd - 2451545.0 ) / 36525.0;
    
    double nut_lon, nut_obliq, d_eps, eps, eps0;
    double sun_ra, sun_dec, sun_lon;
    nutate_(jd, &nut_lon, &nut_obliq);
    d_eps = nut_obliq;
    
    eps0 = 23.4392911 * 3600. - 46.8150 * jdcen - 0.00059 * jdcen * jdcen + 0.001813 * jdcen * jdcen * jdcen;
    eps = (eps0 / 3600. + d_eps) * (PI / 180.);
    
    sun_position(jd, &sun_ra, &sun_dec, &sun_lon, NULL);
    
    double e = 0.016708634 - 0.000042037 * jdcen - 0.0000001267 * jdcen * jdcen;
    double pi = 102.93735 + 1.71946 * jdcen + 0.00046 *jdcen * jdcen;
    double k = k = 20.49552;
    
    double cd = cos(dec * PI / 180.);
    double sd = sin(dec * PI / 180.);
    double ce = cos(eps);
    double te = tan(eps);
    double cp = cos(pi * PI / 180.);
    double sp = sin(pi * PI / 180.);
    double cs = cos(sun_lon * PI / 180.);
    double ss = sin(sun_lon * PI / 180.);
    double ca = cos(ra * PI / 180.);
    double sa = sin(ra * PI / 180.);
    
    double term1 = (ca*cs*ce+sa*ss)/cd;
    double term2 = (ca*cp*ce+sa*sp)/cd;
    double term3 = (cs*ce*(te*cd-sa*sd)+ca*sd*ss);
    double term4 = (cp*ce*(te*cd-sa*sd)+ca*sd*sp);
    
    *d_ra = -k * term1 + e*k * term2;
    *d_dec = -k * term3 + e*k * term4;
    *d_ra /= 3600.;
    *d_dec /= 3600;
}

static double
refract_(double alt, double pressure, double temperature)
{
    temperature += 273.15;
    
    double R;
    double tpcor = pressure / 1010. * 283. / temperature;
    
    if (alt < 15.) {
        R = 3.569 * (0.1594 + 0.0196 * alt + 0.00002 * alt * alt) / (1. + 0.505 * alt + 0.0845 * alt * alt);
    } else {
        R = 0.0166667 / tan((alt + 7.31 / (alt + 4.4)) * PI / 180.);
    }
    
    return R * tpcor;
}

static double
refract(double alt, double pressure, double temperature, bool inverse, double epsilon)
{
    if (!inverse) {
        return alt - refract_(alt, pressure, temperature);
    } else {
        double cur = alt + refract_(alt, pressure, temperature), last;
        while (true) {
            last = cur;
            cur = alt + refract_(last, pressure, temperature);
            if (fabs(cur - last) * 3600. < epsilon) {
                break;
            }
        }
        return cur;
    }
}

static void
hadec2altaz(double ha, double dec, double lat, double *alt, double *az)
{
    double sh = sin(ha * PI / 180.);
    double ch = cos(ha * PI / 180.);
    double sd = sin(dec * PI / 180.);
    double cd = cos(dec * PI / 180.);
    double sl = sin(lat * PI / 180.);
    double cl = cos(lat * PI / 180.);
    
    double x = - ch * cd * sl + sd * cl;
    double y = - sh * cd;
    double z = ch * cd * cl + sd * sl;
    double r = sqrt (x * x + y * y);
    
    *az = atan2(y, x) / (PI / 180.);
    *alt = atan2(z,r) / (PI / 180.);
    
    if (*az < 0.) {
        *az += 360.;
    }
}

static double
ct2lst(double jd, double lon)
{
    double c[4] = {280.46061837, 360.98564736629, 0.000387933, 38710000.0};
    double jd2000 = 2451545.0;
    double t0 = jd - jd2000;
    double t = t0 / 36525.0;
    double theta = c[0] + (c[1] * t0) +  ( c[2] - t / c[3] ) * t * t;
    double lst = ( theta + lon) / 15.0;
    
    if (lst < 0) {
        lst = 24.0 + fmod_(lst, 24);
    }

    return fmod_(lst, 24.0);
}
#endif

#ifdef __USE_SOFA__
double
dut_iers_a(double jd)
#ifdef __USE_GSL__
{
    double dut;
    int ret;
    if ((ret = gsl_spline_eval_e(ut1_utc_a_spline, jd - 2400000.5, ut1_utc_a_acc, &dut)) == GSL_SUCCESS) {
        return dut;
    } else {
        return +9999.;
    }
}
#else
{
    return -9999.;
}
#endif

void
xyp_iers_a(double jd, double *xp, double *yp)
#ifdef __USE_GSL__
{
    int ret;
    if ((ret = gsl_spline_eval_e(pm_x_a_spline, jd - 2400000.5, pm_x_a_acc, xp)) == GSL_SUCCESS) {
        
    } else {
        *xp = 9999.;
    }
    if ((ret = gsl_spline_eval_e(pm_y_a_spline, jd - 2400000.5, pm_y_a_acc, xp)) == GSL_SUCCESS) {
        
    } else {
        *yp = 9999.;
    }
}
#else
{
    *xp = -9999.;
    *yp = -9999.;
}
#endif

void
dxy_iers_a(double jd, double *dx, double *dy)
#ifdef __USE_GSL__
{
    int ret;
    if ((ret = gsl_spline_eval_e(dx_2000a_a_spline, jd - 2400000.5, dx_2000a_a_acc, dx)) == GSL_SUCCESS) {
        
    } else {
        *dx = 9999.;
    }
    if ((ret = gsl_spline_eval_e(dy_2000a_a_spline, jd - 2400000.5, dy_2000a_a_acc, dy)) == GSL_SUCCESS) {
        
    } else {
        *dy = 9999.;
    }
}
#else
{
    *dx = -9999.;
    *dy = -9999.;
}
#endif
#endif

void
radec2altaz(double jd, double ra, double dec, double lon, double lat, double alt, double pressure, double temperature, double *altitude, double *azumith, double *ha_out)
#ifndef __USE_SOFA__
{
    double equinox_now = (jd - 2451545.0)/365.25 + 2000.0;
    double d_ra1, d_ra2, d_dec1, d_dec2, eps, d_psi, dump;
    double ha;
    precess(ra, dec, 2000., equinox_now, false, &ra, &dec);
    
    nutate(jd, ra, dec, &d_ra1, &d_dec1, &eps, &d_psi, &dump);
    aberration(jd, ra, dec, &d_ra2, &d_dec2);
    
    ra += (d_ra1 + d_ra2);
    dec += (d_dec1 + d_dec2);
    
    double lmst = ct2lst(jd, lon) * 15.;
    double last = lmst + d_psi * cos(eps * PI / 180.);
    ha = last - ra;
    
    if (ha < 0) {
        ha += 360.;
    }
    ha = fmod_(ha, 360.);
    
    hadec2altaz(ha, dec, lat, altitude, azumith);
    
    if (pressure < 0.) {
        pressure = 1010. * pow(1.0 - 6.5 / 288000.0 * alt, 5.255);
    }
    
    if (temperature < -273.15) {
        if (alt > 11000.) {
            temperature = 211.5;
        } else {
            temperature = 283. - 0.0065 * alt;
        }
    }
    
    *altitude = refract(*altitude, pressure, temperature, false, 0.);
    if (ha_out != NULL) {
        *ha_out = ha;
    }
}
#else
{
    double dob, rob, eo;
    
    iauASTROM astrom;
    
    iauApci13(jd, 0, &astrom, &eo);
    if (ha_out != NULL) {
        iauAtco13(ra * DD2R, dec * DD2R, 0., 0., 0., 0., jd, 0., 0., lon * DD2R, lat * DD2R, alt, 0., 0., 0., 0., 0., 1., azumith, altitude, ha_out, &dob, &rob, &eo);
    } else {
        double tmp;
        iauAtco13(ra * DD2R, dec * DD2R, 0., 0., 0., 0., jd, 0., 0., lon * DD2R, lat * DD2R, alt, 0., 0., 0., 0., 0., 1., azumith, altitude, &tmp, &dob, &rob, &eo);
    }
    *azumith *= DR2D;
    *altitude *= DR2D;
    *altitude = 90. - *altitude;
    
}
#endif

/*
void
pix2world_sip(double x, double y, double crval[2], double crpix[2], double CD[2][2], double *A, double *B, size_t A_order, size_t B_order, double *ra, double *dec)
{
    double f = 0., g = 0.;
    size_t i, j;
    
    x -= crpix[0];
    y -= crpix[1];
    for (i = 0; i < A_order; i++) {
        double X;
        if (i != 0) {
            X = pow(x, i);
        } else {
            X = 1.;
        }
        for (j = 0; j < B_order; j++) {
            double Y;
            if (j != 0) {
                Y = pow(y, j);
            } else {
                Y = 1.;
            }
            if (*(A + i * A_order + j) != 0) {
                f += *(A + i * A_order + j) * X * Y;
            }
            if (*(B + i * A_order + j) != 0) {
                g += *(B + i * A_order + j) * X * Y;
            }
        }
    }
    *ra = crval[0] + CD[0][0] * (x + f) + CD[0][1] * (y + g);
    *dec = crval[1] + CD[1][0] * (x + f) + CD[1][1] * (y + g);
}

void
world2pix_sip(double ra, double dec, double crval[2], double crpix[2], double CD[2][2], double *A, double *B, size_t A_order, size_t B_order, double *x, double *y)
{
    double f = 0., g = 0.;
    double CD_[2][2];
    size_t i, j;
    
    double coeff = 1./(CD[0][0] * CD[1][1] - CD[1][0] * CD[0][1]);
    
    CD_[0][0] = CD[1][1] * coeff;
    CD_[0][1] = -1. * CD[0][1] * coeff;
    CD_[1][0] = -1 * CD[1][0] * coeff;
    CD_[1][1] = CD[0][0] * coeff;
    
    ra -= crval[0];
    dec -= crval[1];
    for (i = 0; i < A_order; i++) {
        double RA;
        if (i != 0) {
            RA = pow(ra, i);
        } else {
            RA = 1.;
        }
        for (j = 0; j < B_order; j++) {
            double DEC;
            if (j != 0) {
                DEC = pow(dec, j);
            } else {
                DEC = 1.;
            }
            if (*(A + i * A_order + j) != 0) {
                f += *(A + i * A_order + j) * RA * DEC;
            }
            if (*(B + i * A_order + j) != 0) {
                g += *(B + i * A_order + j) * RA * DEC;
            }
        }
    }

    *x = crpix[0] + CD_[0][0] * (ra + f) + CD_[0][1] * (dec + g);
    *y = crpix[1] + CD_[1][0] * (ra + f) + CD_[1][1] * (dec + g);
}
*/

double
air_mass(double z)
{
    double secz = 1 / cos(z*PI/180.), secz_1 = secz - 1;

    return secz - 0.0018167 * secz_1 - 0.002875 * secz_1 * secz_1 - 0.0008083 * secz_1 * secz_1 * secz_1;
}


#ifdef __USE_SOFA__
static size_t
read_iers_a(void)
{
    /* 
     * Download link https://datacenter.iers.org/data/9/finals2000A.all
     * IERS_A table file search order:
     * 1. current work directory
     * 2. /usr/local/aaos/share
     * 3. /usr/local/share/aaos
     * 4. /usr/share/aaos
     * 5. The pathname set in AAOS_IERS_A environment variable.
     */
    FILE *fp = NULL;
    if (access("finals2000A.all", R_OK) == 0) {
        if ((fp = fopen("finals2000A.all", "r")) != NULL) {
            goto error;
        }
    }
    if (access("/usr/local/aaos/share/finals2000A.all", R_OK) == 0) {
        if ((fp = fopen("/usr/local/aaos/share/finals2000A.all", "r")) != NULL) {
            goto error;
        }
    }
    if (access("/usr/local/share/aaos/finals2000A.all", R_OK) == 0) {
        if ((fp = fopen("/usr/local/share/aaos/finals2000A.all", "r")) != NULL) {
            goto error;
        }
    }
    if (access("/usr/share/aaos/finals2000A.all", R_OK) == 0) {
        if ((fp = fopen("/usr/share/aaos/finals2000A.all", "r")) != NULL) {
            goto error;
        }
    }
    if (access(getenv("AAOS_IERS_A"), R_OK) == 0) {
        if ((fp = fopen(getenv("AAOS_IERS_A"), "r")) != NULL) {
            goto error;
        }
    }
error:
    if (fp == NULL) {
        fprintf(stderr, "IERS_A data file `finals2000A.all` not found.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
    /*
     * The file format of finals2000A.all is referenced 
     * https://maia.usno.navy.mil/ser7/readme.finals2000A.
     * The format of the finals2000A.data, finals2000A.daily, and finals2000A.all files is:
     * Col.#    Format  Quantity
     * -------  ------  -------------------------------------------------------------
     *  1-2      I2      year (to get true calendar year, add 1900 for MJD<=51543 or add 2000 for MJD>=51544)
     *  3-4      I2      month number
     *  5-6      I2      day of month
     *  7        X       [blank]
     *  8-15     F8.2    fractional Modified Julian Date (MJD UTC)
     *  16       X       [blank]
     *  17       A1      IERS (I) or Prediction (P) flag for Bull. A polar motion values
     *  18       X       [blank]
     *  19-27    F9.6    Bull. A PM-x (sec. of arc)
     *  28-36    F9.6    error in PM-x (sec. of arc)
     *  37       X       [blank]
     *  38-46    F9.6    Bull. A PM-y (sec. of arc)
     *  47-55    F9.6    error in PM-y (sec. of arc)
     *  56-57    2X      [blanks]
     *  58       A1      IERS (I) or Prediction (P) flag for Bull. A UT1-UTC values
     *  59-68    F10.7   Bull. A UT1-UTC (sec. of time)
     *  69-78    F10.7   error in UT1-UTC (sec. of time)
     *  79       X       [blank]
     *  80-86    F7.4    Bull. A LOD (msec. of time) -- NOT ALWAYS FILLED
     *  87-93    F7.4    error in LOD (msec. of time) -- NOT ALWAYS FILLED
     *  94-95    2X      [blanks]
     *  96       A1      IERS (I) or Prediction (P) flag for Bull. A nutation values
     *  97       X       [blank]
     *  98-106   F9.3    Bull. A dX wrt IAU2000A Nutation (msec. of arc), Free Core Nutation NOT Removed
     *  107-115  F9.3    error in dX (msec. of arc)
     *  116      X       [blank]
     *  117-125  F9.3    Bull. A dY wrt IAU2000A Nutation (msec. of arc), Free Core Nutation NOT Removed
     *  126-134  F9.3    error in dY (msec. of arc)
     *  135-144  F10.6   Bull. B PM-x (sec. of arc)
     *  145-154  F10.6   Bull. B PM-y (sec. of arc)
     *  155-165  F11.7   Bull. B UT1-UTC (sec. of time)
     *  166-175  F10.3   Bull. B dX wrt IAU2000A Nutation (msec. of arc)
     *  176-185  F10.3   Bull. B dY wrt IAU2000A Nutation (msec. of arc)
     */
    size_t nlines = 0;
    char buf[256], field[32];
    while (fgets(buf, 256, fp) != NULL) {
        nlines++;
    }
    mjd = malloc(sizeof(double) * nlines);
    pm_x_a = malloc(sizeof(double) * nlines);
    pm_y_a = malloc(sizeof(double) * nlines);
    ut1_utc_a = malloc(sizeof(double) * nlines);
    dx_2000a_a = malloc(sizeof(double) * nlines);
    dy_2000a_a = malloc(sizeof(double) * nlines);

    rewind(fp);
    nlines = 0;
    while (fgets(buf, 256, fp) != NULL) {
        memset(field, '\0', 32);
        memcpy(field, buf + 6, 9);
        if (strncmp(field, "         ", 9) == 0) {
            continue;
        }
        mjd[nlines] = atof(field);
        memcpy(field, buf + 17, 10);
        if (strncmp(field, "          ", 10) == 0) {
            continue;
        }
        pm_x_a[nlines] = atof(field);
        memcpy(field, buf + 36, 10);
        if (strncmp(field, "          ", 10) == 0) {
            continue;
        }
        pm_y_a[nlines] = atof(field);
        memcpy(field, buf + 58, 10);
        if (strncmp(field, "          ", 10) == 0) {
            continue;
        }
        ut1_utc_a[nlines] = atof(field);
        memcpy(field, buf + 96, 10);
        if (strncmp(field, "          ", 10) == 0) {
            continue;
        }
        dx_2000a_a[nlines] = atof(field);
        memcpy(field, buf + 115, 10);
        if (strncmp(field, "          ", 10) == 0) {
            continue;
        }
        dy_2000a_a[nlines] = atof(field);
        nlines++;
    }
    fclose(fp);
    return nlines;
}
#endif

static void __destructor__(void) __attribute__ ((destructor(101)));

static void
__destructor__(void)
{
    regfree(&preg_hms_1);
    regfree(&preg_hms_2);
    regfree(&preg_hms_3);
    regfree(&preg_hms_4);
    regfree(&preg_dms_1);
    regfree(&preg_dms_2);
    regfree(&preg_dms_3);
    regfree(&preg_dms_4);
    regfree(&preg_fmt);
#ifdef __USE_SOFA__
#ifdef __USE_GSL__
    free(mjd);
    free(pm_x_a);
    free(pm_y_a);
    free(ut1_utc_a);
    free(dx_2000a_a);
    free(dy_2000a_a);
    gsl_spline_free(pm_x_a_spline);
    gsl_interp_accel_free(pm_x_a_acc);
    gsl_spline_free(pm_y_a_spline);
    gsl_interp_accel_free(pm_y_a_acc);
    gsl_spline_free(ut1_utc_a_spline);
    gsl_interp_accel_free(ut1_utc_a_acc);
    gsl_spline_free(dx_2000a_a_spline);
    gsl_interp_accel_free(dx_2000a_a_acc);
    gsl_spline_free(dy_2000a_a_spline);
    gsl_interp_accel_free(dy_2000a_a_acc);
#endif
#endif
}

static void __constructor__(void) __attribute__ ((constructor(101)));

static void
__constructor__(void)
{
#ifdef __USE_SOFA__
#ifdef __USE_GSL__
    size_t nlines;
    nlines = read_iers_a();
    pm_x_a_acc = gsl_interp_accel_alloc();
    pm_x_a_spline = gsl_spline_alloc(gsl_interp_cspline, nlines);
    gsl_spline_init(pm_x_a_spline, mjd, pm_x_a, nlines);
    pm_y_a_acc = gsl_interp_accel_alloc();
    pm_y_a_spline = gsl_spline_alloc(gsl_interp_cspline, nlines);
    gsl_spline_init(pm_y_a_spline, mjd, pm_y_a, nlines);
    ut1_utc_a_acc = gsl_interp_accel_alloc();
    ut1_utc_a_spline = gsl_spline_alloc(gsl_interp_cspline, nlines);
    gsl_spline_init(ut1_utc_a_spline, mjd, ut1_utc_a, nlines);
    dx_2000a_a_acc = gsl_interp_accel_alloc();
    dx_2000a_a_spline = gsl_spline_alloc(gsl_interp_cspline, nlines);
    gsl_spline_init(pm_x_a_spline, mjd, dx_2000a_a, nlines);
    dy_2000a_a_acc = gsl_interp_accel_alloc();
    dy_2000a_a_spline = gsl_spline_alloc(gsl_interp_cspline, nlines);
    gsl_spline_init(pm_x_a_spline, mjd, dy_2000a_a, nlines);
    gsl_set_error_handler_off();
#endif
#endif
    regcomp(&preg_hms_1, pattern_hms_1, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_hms_2, pattern_hms_2, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_hms_3, pattern_hms_3, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_hms_4, pattern_hms_4, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_1, pattern_dms_1, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_2, pattern_dms_2, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_3, pattern_dms_3, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_dms_4, pattern_dms_4, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_fmt, pattern_fmt, REG_EXTENDED);
}
