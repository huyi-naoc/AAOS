% detector\_info(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_info - retrieve the detector information

SYNOPSIS
========

**#include <detector_rpc.h>**  

int  
**detector_info**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_info**() function retrieve the information of the detector referenced by *\*\_self*. The content of info is filled in *res* pointer provided by the user, and its size in bytes is *res_size*. The length of the content is returned in *res_len*. If the actual length of the info content is larger than *res_size*, only the first *res_size* bytes will be filled in. If *res_len* is **NULL**, the length content will not be returned.

The information generally contains the vendor, model, serial number, and etc of thhe the detector, which is constant. However, it is not necessarily retrieved from configuration files. Therefore, *AAOS\_EDEVMAL*, *AAOS\_EPWROFF*, *AAOS\_EUINIT* may return too.

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

AAOS\_EUNINIT
-------------

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

**detector_status**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

