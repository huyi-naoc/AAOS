% detector\_set\_region(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_set\_region - set grabbing region

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_region**(void *\*\_self*, uint32\_t *x\_offset*, uint32\_t *y\_offset*,
$~~~~~~~~~~~~~~~~~$uint32\_t *width*, uint32\_t *height*);
 

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_set_region**() function set the grabbing region of the detector referenced by *\*\_self*. The region start at *x\_offset* and *y\_offset* from the corner of the detector with *width* and *height*.


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

The value of *x\_offset*, *y\_offset*, *width* and/or *height* is invalid. 

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

**detector_get_region**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

