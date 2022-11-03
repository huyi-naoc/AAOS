% telescope\_init(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_init - initialize the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_init**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_init**() function initializes the telescope referenced by *\*\_self*. By calling this funcntion, **runtime** parameters of the telescope, such as time, location, PID, and so on, are approriately set up.  

If the telescope has already been initialized, calling this function does nothing and return successfully. Otherwise, it will change the telescope state to *TELESCOPE_STATE_TRACKING*.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

AAOS\_EPWROFF
------------

The underline telescope is not powered.

CONFORMING TO
=============

AAOS-draft-2022

EXAMPLES
========

None.

THREAD-SAFE
===========

This function is thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.

RATIONALE
=========

Many telescopes cannot remain their runtime parameters when powered off. Thus, an initialization operation is needed after it is powerd on. 

SEE ALSO
========

**telescope_power_off**(3), **telescope_power_on**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

