% detector\_set\_frame\_rate(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_set\_frame\_rate - set frame rate

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_frame_rate**(void *\*\_self*, double *frame_rate*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_set_frame_rate**() function set the frame rate of the detector referenced by *\*\_self*. The unit of *frame\_rate* is in frame per second. This setting will be applied next time calling **detector_expose**() function. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
------------

The underline detector is in *MALFUNCTION* state.

AAOS\_EINVAL
------------

The value of  *frame\_rate* is invalid. 

AAOS\_ENOTSUP
------------

The underline detector does not support this operation.

AAOS\_EPWROFF
------------

The underline detector is not powered.

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

**detector_expose**(3), **detector_get_frame_rate**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

