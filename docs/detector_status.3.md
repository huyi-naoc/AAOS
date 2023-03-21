% detector\_status(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_status - retrieve the detector status

SYNOPSIS
========

**#include <detector_rpc.h>**  

int  
**detector_status**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_status**() function retrieve the status of the detector referenced by *\*\_self*. The content of status is filled in *res* pointer provided by the user, and its size in bytes is *res_size*. The length of the content is returned in *res_len*. If the actual length of the status content is larger than *res_size*, only the first *res_size* bytes will be filled in. If *res_len* is **NULL**, the length content will not be returned.

The content must include the current detector state. If **detector_get_binning**(), **detector_get_exposure_time**(), **detector_get_frame_rate**(), **detector_get_gain**(), **detector_get_readout_rate**(), **detector_get_region**(), **detector_get_temperature**(), **detector_get_directory**(), **detectory_get_prefix**() do not return *AAOS\_ENOTSUP*, those values should also be filled in *res*.  

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_EDEVMAL
------------

The underline detector is in *MALFUNCTION* state.

AAOS\_EPWROFF
------------

The underline detector is not powered.

AAOS\_EUNINT
-----------

The underline detector is uninitialized, e.g., clock time and/or location have not been set yet by **detector_init**().

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

**detector_get_binning**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detector_get_gain**(3), **detector_get_readout_rate**(3), **detector_get_region**(3), **detector_get_temperature**(3), **detector_get_directory**(3), **detectory_get_prefix**()

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

