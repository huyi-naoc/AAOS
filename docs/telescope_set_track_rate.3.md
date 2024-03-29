% telescope\_set\_track\_rate(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_set\_track\_rate - set track rates of both axes of telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_set_track_rate**(void *\*\_self*, double *track_rate_x*, double *track_rate_y*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_set_track_rate**() function set the track rate of the telescope referenced by *\*\_self*. The unit of *track_rate_x* and *track_rate_y* are both in arsec per second. This setting will be applied immediate if the telescope state is *TELESCOPE_STATE_TRACKING*, or after the returning of **telescope_move**() family function, **telescope_park_off**() function, **telescope_slew**() family functions, **telescope_stop**() function.

The *track_rate_x* could be the track rate of either RA axis for a equatorial mount or east-west axis for a horizontal mount, whereas the *track_rate_y* could be the track rate of either DEC axis for a equatorial mount or north-south axis for a horizontal mount. 

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

The value of  *track\_rate\_x* and/or *track\_rate\_y* is invalid.

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

Thiis function is thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.

SEE ALSO
========

**telescope_get_track_rate**(3), **telescope_move**(3), **telescope_park_off**(3), **telescope_slew**(3),  **telescope_stop**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

