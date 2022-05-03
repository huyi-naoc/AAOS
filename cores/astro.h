//
//  astro.h
//  AAOS
//
//  Created by huyi on 2019/3/4.
//  Copyright © 2019年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef astro_h
#define astro_h

#include <string.h>
#include <stdbool.h>

#include <time.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef PI
#define PI 3.141592653589793
#endif

#define degrad(x)       ((x)*PI/180.)
#define raddeg(x)       ((x)*180./PI)
#define hrdeg(x)        ((x)*15.)
#define deghr(x)        ((x)/15.)
#define hrrad(x)        degrad(hrdeg(x))
#define radhr(x)        deghr(raddeg(x))

#define deg2rad degrad
#define rad2deg raddeg

#define J2000 946684800
#define DAYOFFSET -0.5
#define ONEDAY 86400

double hms2deg(const char *);
char *deg2hms(double, char *, size_t, const char *);
double dms2deg(const char *);
char *deg2dms(double, char *, size_t, const char *);
char *lon_deg2dms(double, char *, size_t, const char *);
char *lat_deg2dms(double, char *, size_t, const char *);
double jd_tp(struct timespec *);
double jd(double);
#ifdef __USE_SOFA__
double dut_iers_a(double jd);
void xyp_iers_a(double jd, double *xp, double *yp);
void dxy_iers_a(double jd, double *dx, double *dy);
#endif
double equatorial_distance(double, double, double, double);
#ifndef __USE_SOFA__
void precess(double, double, double, double, bool, double *, double *);
void aberration(double, double, double, double *, double *);
#endif
void radec2altaz(double, double, double, double, double, double, double, double, double *, double *, double *);

#endif /* astro_h */
