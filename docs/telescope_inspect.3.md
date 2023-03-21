% telescope\_inspect(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_inspect - inspect the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_inspect**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_inspect**() function inspects on the telescope referenced by *\*\_self*. If the telescope is malfunction, it will change the state of the telescope to *TELESCOPE_STATE_MALFUNCTION*. 


RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error. If the telescope works correctly, it will notify all the processes calling **telescope_register** to wait for the recovery of the telescope.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTSUP
------------

The underline telescope does not support this operation.

AAOS\_ERROR
-----------

The underline telescope is in malfunction state.


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

**telescope_inspect**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

