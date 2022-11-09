% telescope\_park\_off(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_park - park off the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_park_off**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_park_off**() function park off the telescope referenced by *\*\_self*, which means the two axis are both energized by this function.    

If the telescope is not parked, calling this function does nothing and return successfully. Otherwise, if the current telescope state is *TELESCOPE_STATE_PARKED*, the state will change to *TELESCOPE_STATE_TRACKING*.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

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

SEE ALSO
========

**telescope_init**(), **telescope_park**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

