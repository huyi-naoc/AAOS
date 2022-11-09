% telescope\_set\_slew\_speed(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_set\_slew\_speed - set slew speeds of both axes of telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_set_slew_speed**(void *\*\_self*, double *slew_speed_x*, double *slew_speed_y*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_set_slew_speed**() function set the slew speed of the telescope referenced by *\*\_self*. The unit of *slew_speed_x* and *slew_speed_y* are both in arsec per second. This setting will be applied next time calling **telescope_slew**() family functions. 

The *slew_speed_x* could be the slew speed of either RA axis for a equatorial mount or east-west axis for a horizontal mount, whereas the *slew_speed_y* could be the slew speed of either DEC axis for a equatorial mount or north-south axis for a horizontal mount. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

AAOS\_EINVAL
------------

The value of  *slew\_speed\_x* and/or *slew\_speed\_y* is invalid. 

AAOS\_ENOTSUP
------------

The underline telescope does not support this operation.

AAOS\_EPWROFF
------------

The underline telescope is not powered.

AAOS\_EUNINT
-----------

The underline telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

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

**telescope_get_slew_speed**(3), **telescope_slew**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

