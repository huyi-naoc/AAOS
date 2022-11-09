% telescope\_power_on(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_power\_on - power on the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_power_on**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_power_on**() function powers on the telescope referenced by *\*\_self*.   

If the telescope has already been powered, calling this function does nothing and return successfully. Otherwise, it will change the state of the telescope to *TELESCOPE_STATE_UNINITIALIZED*. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTSUP
------------

The underline telescope does not support this operation.

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

**telescope_power_init**(3), **telescope_power_off**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

