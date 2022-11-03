% detector\_set\_exposure\_time(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_set\_exposure\_time - set exposure time

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_exposure_time**(void *\*\_self*, double *exposure\_time*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_set_exposure_time**() function set the exposure time of the detector referenced by *\*\_self*. The unit of *exposure\_time* is in second.

If the detector supports setting its frame rate, and the inverse of the frame rate is shorter than *exposure\_time*, wether this function returns *AAOS\_EINVAL* or sets the frame rate to the inverse of *exposure_time* is implementation specific. In AAOS, it specified to the  latter.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_EDEVMAL
------------

The underline detector is in *MALFUNCTION* state.

AAOS\_EINVAL
------------

The value of *exposure\_time* is invalid. 

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

**detector_expose**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detector_set_frame_rate**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

