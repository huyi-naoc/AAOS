% detector\_disable\_cooling(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_disable\_cooling - disable cooling function

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int   
**detector_disable_cooling**(void *\*\_self*)

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_disable_cooling**() function diable cooling function of the underline detector.  

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_EDEVMAL
-------------

The underline detector is in *DETECTOR_STATE_MALFUNCTION* state.

AAOS\_ENOTSUP
-------------

The underline detetcor does not support this operation.

AAOS\_EPWROFF
-------------

The underline detector is not powered.

AAOS\_EUNINT
------------

The underline detector is uninitialized.

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
**detector_enable_cooling**(3), **detector_get_temperature**(), **detector_set_temperature**()

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

