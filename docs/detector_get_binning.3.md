% detector\_get\_binning(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_get\_binning - get binning

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_binning**(void *\*\_self*, uint32\_t *\*x\_binning*, uint32\_t *\*y\_binning*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_get_binning**() function gets the binning of the detector referenced by *\*\_self*. The unit of *x_binning* and *y_binning* is in pixel.


RETURN VALUE
============

Upon successful completion, **detetcor_get_binning**() shall return zero; otherwise, it shall return a non-zero integer to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
------------

The underline detector is in *MALFUNCTION* state.

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

**detector_set_binning**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

