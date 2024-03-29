% telescope\_status(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_status - retrieve the telescope status

SYNOPSIS
========

**#include <telescope_rpc.h>**  

int  
**telescope_status**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_status**() function retrieve the status of the telescope referenced by *\*\_self*. The content of status is filled in *res* pointer provided by the user, and its size in bytes is *res_size*. The length of the content is returned in *res_len*. If the actual length of the status content is larger than *res_size*, only the first *res_size* bytes will be filled in. If *res_len* is **NULL**, the length content will not be returned.

The content must include the current telescope position and state. If **telescope_get_move_speed**(), **telescope_get_slew_speed**(), **telescope_get_track_rate**() do not return *AAOS\_ENOTSUP*, those values should also be filled in *res*.  

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

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

**telescope_get_move_speed**(3), **telescope_get_slew_speed**(3), **telescope_get_track_rate**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

