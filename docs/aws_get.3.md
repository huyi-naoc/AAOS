% AWS(3) | Library Functions Manual
%
% May 2022

NAME
====

aws\_air\_pressure, aws\_air\_pressure\_by\_channel, aws\_air\_pressure\_by\_name, aws\_get\_precipitation, aws\_get\_precipitation\_by\_channel, aws\_get\_precipitation\_by\_name, aws\_get\_relative\_humidity, aws\_get\_relative\_humidity\_by\_channel, aws\_get\_relative\_humidity\_by\_name, aws\_get\_sky\_quality, aws\_get\_sky\_quality\_by\_channel, aws\_get\_sky\_quality\_by\_name, aws\_get\_temperature, aws\_get\_temperature\_by\_channel, aws\_get\_temperature\_by\_name, aws\_get\_wind\_direction, aws\_get\_wind\_direction\_by\_channel, aws\_get\_wind\_direction\_by\_name, aws\_get\_wind\_speed, aws\_get\_wind\_speed\_by\_channel, aws\_get\_wind\_speed\_by\_name, aws\_get\_data, aws\_get\_data\_by\_channel, aws\_get\_data\_by\_name, aws\_get\_raw\_data, aws\_get\_raw\_data\_by\_channel, aws\_get\_raw\_data\_by\_name  - retrieve data from different type of sensors

SYNOPSIS
========

**#include <aws_rpc.h>**  
**#include <aws_def.h>**

int  
**aws_get_air_pressure**(void *\*\_self*, double *\*air\_pressure*, size\_t size);
int
**aws_get_air_pressure_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*air\_pressure*, size\_t size);
int
**aws_get_air_pressure_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*air\_pressure*, size\_t size);
int  
**aws_get_precipitation**(void *\*\_self*, double *\*precipitation*, size\_t size);
int
**aws_get_precipitation_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*precipitation*, size\_t size);
int
**aws_get_precipitation_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*precipitation*, size\_t size);
int  
**aws_get_relative_humidity**(void *\*\_self*, double *\*relative\_humidity*, size\_t size);
int
**aws_get_relative_humidity_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*relative\_humidity*, size\_t size);
**aws_get_air_pressure_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*air\_pressure*, size\_t size);
int  
**aws_get_sky_quality**(void *\*\_self*, double *\*sky\_quality*, size\_t size);
int
**aws_get_sky_quality_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*sky\_quality*, size\_t size);
int
**aws_get_sky_quality_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*sky\_quality*, size\_t size);
int  
**aws_get_temperature**(void *\*\_self*, double *\*temperature*, size\_t size);
int
**aws_get_temperature_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*temperature*, size\_t size);
**aws_get_temperature_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*temperature*, size\_t size);
int  
**aws_get_wind_direction**(void *\*\_self*, double *\*wind\_direction*, size\_t size);
int
**aws_get_wind_direction_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*wind_direction*, size\_t size);
int
**aws_get_wind_direction_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*wind_direction*, size\_t size);
int  
**aws_get_wind_speed**(void *\*\_self*, double *\*wind\_speed*, size\_t size);
int
**aws_get_wind_speed_by_channel**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channle*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*wind_speed*, size\_t size);
int
**aws_get_wind_speed_by_name**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~$double *\*wind_speed*, size\_t size);

int
**aws_get_data**(void *\*\_self*,double *\*data*, size\_t size);
int
**aws_get_data_by_channel**(void *\*\_self*,
$~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channel*,
$~~~~~~~~~~~~~~~~~~~~~$double *\*data*, size\_t size);
int
**aws_get_data_by_name**(void *\*\_self*,
$~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*,
$~~~~~~~~~~~~~~~~~~~~~$double *\*data*, size\_t size);
int
**aws_get_raw_data**(void *\*\_self*,void *\*data*, size\_t size);
int
**aws_get_raw_data_by_channel**(void *\*\_self*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *index*, unsigned int *channel*,
$~~~~~~~~~~~~~~~~~~~~~~~~~~$void *\*data*, size\_t size);
int
**aws_get_raw_data_by_name**(void *\*\_self*,
$~~~~~~~~~~~~~~~~~~~~~~~$const char *\*aws_name*, const char *\*channle_name*,
$~~~~~~~~~~~~~~~~~~~~~~~$void *\*data*, size\_t size);


Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **aws_get_XXX**() functions retrieve data from the sensor identiedy by *channel* or **channel_name** of the AWS identified by *index* or *aws_name*. the index of the AWS identified by its *name*. Since a single sensor may produce more than one items, these function will retrieve the first *size* items. If the sensor does not work, an impossible value will be filled. For example, -9999.0 will be fill to *temperature*.

The unit of air pressure, precipitation, relative humidity, sky quality, temperature, wind direction, and wind speed are hundred pascal (hPa or mbar), milimeter (mm), percentage (%), Celsius degree, degree (clockwise, north is zero), meter per second (m/s), respectively.

The **aws_get_data**() functions retrieve original data, which means they may do not translate the data into their meaningful units.

 The **aws_get_raw_data**() functions retrieve raw data, which means they only read the data from the sensors. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTFOUND
-------------

The aws identified by *index* or *aws\_name*,  and/or sensor identified by *channel* or *channel_name* is not found.

CONFORMING TO
=============

AAOS-draft-2022

EXAMPLES
========

None.

THREAD-SAFE
===========

This function is thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.

SEE ALSO
========

**aws_data_field**(3), **aws_data_log**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

