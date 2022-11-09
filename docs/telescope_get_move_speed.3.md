% telescope\_get\_move\_speed(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_get\_move\_speed - get current axis move speed

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_get_move_speed**(void *\*\_self*, double *\*move_speed*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_get_move_speed**() function get the move speed of the telescope referenced by *\*\_self*. The unit of *move_speed* is in arsec per second.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
-------------

The underline telescope is in *MALFUNCTION* state.

AAOS\_ENOTSUP
------------

The o\peration is not supported.

AAOS\_EPWROFF
------------

The underline telescope does not support this operation.

AAOS\_EUNINT
------------

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

 **telescope_move**(3), **telescope_set_move_speed**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

