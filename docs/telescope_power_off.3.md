% telescope\_power\_off(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_power\_off - power off the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_power_off**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_power_off**() function powers off the telescope referenced by *\*\_self*.   

If the telescope has already been powered off, calling this function does nothing and return successfully. Otherwise, it will change the state of the telescope to *TELESCOPE_STATE_PWROFF* no matter which state the telescope is right now. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

AAOS\_ENOTSUP
------------

The underline telescope does not support this operation.

CONFORMING TO
=============

AAOS-draft-2022

NOTES
=====

The **telescope_power_off**() function is usually called after **telescope_go_home**(), and before **dome_close**().

EXAMPLES
========

None.

THREAD-SAFE
===========

These functions are thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.

SEE ALSO
========

**dome_close**(3), **telescope_gpo_home**(3), **telescope_power_init**(3), **telescope_power_on**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

