% telescope\_go\_home(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_go\_home - slew then park the telescope at a proper position

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_go_home**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_go_home**() function slews the telescope referenced by *\*\_self* to a proper position and park it there. The functions will block the calling thread until it returns.

No matter which state the telescope is, the **telescope_go_home**() function will change the telescope state to *TELESCOPE_STATE_PARKED*,  except for current state in *TELESCOPE_STATE_MALFUNCTION*, *TELESCOPE_STATE_PWROFF* and *TELESCOPE_STATE_EUNINIT*.
 
RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ECANCELED
--------------

The current execution is cancelled by a new call of *telescope_go_home*(), *telescope_move*(), *telescope_park*(), *telescope_slew*(), and *telescope_stop*() that operate the same underline telescope. 

AAOS\_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

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

RATIONALE
=========

Almost every professional telescope have an enclosure called dome to protect them from bad weather conditions. However, it may be dangerous to close the dome when the telescope pointing to certain positions. **telescope_go_home**() will slew the telescope to a predefined safe position and park it there.As such, the subsequent operation on closing the dome is guaranteed to be safe.    

SEE ALSO
========

**telescope_park**(3), **telescope_slew**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

