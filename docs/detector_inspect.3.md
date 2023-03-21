% detector\_inspect(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_inspect - inspect the detector

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_inspect**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_inspect**() function inspects on the detector referenced by *\*\_self*. If the detector is malfunction, it will change the state of the detector to *DETECTOR_STATE_MALFUNCTION*. 


RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error. If the detector works correctly, it will notify all the processes calling **detector_register** to wait for the recovery of the detector.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTSUP
------------

The underline detector does not support this operation.

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

**detector_register**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

