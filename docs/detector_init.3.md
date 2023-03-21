% detector\_init(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_init - initialize the detector

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_init**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_init**() function initializes the detector referenced by *\*\_self*.  

If the detector has already been initialized, calling this function does nothing and return successfully. Otherwise, it will change the telescope state to *DETECTOR_STATE_IDLE*.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
------------

The underline detector is in *MALFUNCTION* state.

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

**detector_power_off**(3), **detector_power_on**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

