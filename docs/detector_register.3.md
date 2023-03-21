% detector\_register(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_register - wait until the recovery of the detector

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_register**(void *\*\_self*, double *timeout*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_regiter**() function waits until the recovery of the detector referenced by *\*\_self* or timed out. If *timeout* equals zero, it just returns immediately, equivalent to **detector_inspect**(); and if *timeout* is negative, it will block until the recovery of the detector. 


RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTSUP
------------

The underline detector does not support this operation.

AAOS\_ETIMEOUT
--------------

Waiting timed out.

AAOS\_ERROR
-----------

The underline detector is in malfunction state.


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

**detector_inspect**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

