% telescope_move(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope_move - move the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_move**(void *\*\_self*, unsined int *direction*, 
$~~~~~~~~~~~~~~~$double *duration*);  
int  
**telescope_timed_move**(void *\*\_self*, unsined int *direction*,  
$~~~~~~~~~~~~~~~~~~~~~$double *duration*, double *timeout*);  
int  
**telescope_try_slew**(void *\*\_self*, unsined int *direction*, 
$~~~~~~~~~~~~~~~~~~~$double *duration*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_move**(), **telescope_timed_move**() and **telescope_try_move** functions move one of axis of the telescope referenced by *\*\_self*. The move direction are duration are specified by *direction* and *duration* respectively. *direction* can be *TELESCOPE_MOVE_EAST*, *TELESCOPE_MOVE_WEST*, *TELESCOPE_MOVE_NORTH*, *TELESCOPE_MOVE_NORTH*, which are defined in **telescope_def.h**. The functions will block the calling thread until it returns. 

The meaning of *direction* depends on the underline telescope. Usaually *TELESCOPE_MOVE_EAST* and *TELESCOPE_MOVE_WEST* mean the RA axis, and the other two mean the DEC axis for a equatorial mount. 

The moving speed can be retrieved and set by **telescope_get_move_speed**() and **telescope_set_move_speed**(). 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS_ECANCELED
--------------

The current execution is cancelled by a new call of *telescope_go_home*(), *telescope_move*(), *telescope_park*(), *telescope_slew*(), and *telescope_stop*() that operate the same underline telescope. 

AAOS_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

AAOS_EINVAL
-----------

The *ra* or *dec* value is out of the underline telescope's setting limits.

AAOS_EPWROFF
------------

The underline telescope is not powered.

AAOS_EUNINT
-----------

The underline telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

*telescope_timed_move*() function shall also fail if:

AAOS_ETIMEDOUT
--------------

The telescope is still moving or slewing until *timeout*.

*telescope_try_move*() function shall also fail if:

AAOS_EBUSY
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

**telescope_get_move_speed**(3), **telescope_init**(3), **telescope_park**(3), **telescope_power_off**(3), **telescope_power_on**(3), **telescope_get_move_speed**(3), **telescope_slew**(3), **telescope_stop**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

