% telescope\_slew(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_slew - slew the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**

int  
**telescope_slew**(void *\*\_self*, double *ra*, double *dec*);  
int  
**telescope_timed_slew**(void *\*\_self*, double *ra*, double *dec*, 
$~~~~~~~~~~~~~~~~~~~~~$double *timeout*);  
int  
**telescope_try_slew**(void *\*\_self*, double *ra*, double *dec*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_slew**(), **telescope_timed_slew**() and **telescope_try_slew** functions slew the telescope referenced by *\*\_self* to the celetial target (*ra*, *dec*). The functions will block the calling thread until it returns. 

*ra* and *dec* are both in degree unit. *ra* is from 0 to 360, and *dec* is from -90 to 90. However, since every telescope has hardware and/or software limits for the safety, the range of *ra* and *dec* will be much smaller in practice. *ra* does not necessarily mean **Right Ascension**. Neither, *dec* must mean **Declination**.  

The slewing speed can be retrieved and set by **telescope_get_slew_speed**() and **telescope_set_slew_speed**().

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_ECANCELED
--------------

The current execution is cancelled by a new call of *telescope_go_home*(), *telescope_move*(), *telescope_park*(), *telescope_slew*(), and *telescope_stop*() that operate the same underline telescope. 

AAOS\_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

AAOS\_EINVAL
-----------

The value of *ra* and/or *dec* value is out of their definition range or the underline telescope's setting limits.

AAOS\_EPWROFF
------------

The underline telescope is not powered.

AAOS\_ETIMEDOUT
--------------

The execution timed out, usually means the telescope is in trouble.

AAOS\_EUNINT
-----------

The underline telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

*telescope_timed_slew*() function shall also fail if:

AAOS\_ETIMEDOUT
--------------

The telescope is still moving or slewing until *timeout*.

*telescope_try_slew*() function shall also fail if:

AAOS\_EBUSY
----------

The telescope is moving or slewing now.

CONFORMING TO
=============

AAOS-draft-2022

EXAMPLES
========

None.

THREAD-SAFE
===========

These functions are thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.

SEE ALSO
========

**telescope_get_slew_speed**(3), **telescope_init**(3), **telescope_move**(3), **telescope_park**(3), **telescope_power_off**(3), **telescope_power_on**(3), **telescope_set_slew_speed**(3), **telescope_stop**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

