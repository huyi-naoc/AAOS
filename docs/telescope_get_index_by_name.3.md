% telescope_init(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope_init - initialize the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_get_index_by_name**(void *\*\_self*, const char *\*name*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========
The **telescope_get_index_by_name**() function gets the index of the telescoope identified by its *name*. The index will be stored in *\_self* object and opaque to the user.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_ENOTFOUND
-------------

The telescope named *name* is not found.

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

None.

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

