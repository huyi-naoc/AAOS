% telescope\_raw(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_raw - execute raw command to the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_raw**(void *\*\_self*, const void *\*command*,  
$~~~~~~~~~~~~~~~$size\_t *command_size*, void *\*results*,  
$~~~~~~~~~~~~~~~$size\_t *results_size*, size\_t *\*return_size*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_raw**() function execute a raw command of the telescope referenced by *\*\_self*. The *command* is the content of the ra command, and the *command_size* is the length of the size. The executing results will be filled into the buffer pointed by *results*, the *results_size* is the size of the *results* in bytes, the *return_size* is the length of *results*. If the actual length of results is larger than *results_size*, only the first *results_size* bytes will be filled in. If *return_size* is **NULL**, the length results will not be returned.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
------------

The underline telescope is in *MALFUNCTION* state.

AAOS\_ENOTSUP
------------

The underline telescope does not support this operation.

AAOS\_EPWROFF
------------

The underline telescope is not powered.

AAOS\_EUNINT
-----------

The underline telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

CONFORMING TO
=============

AAOS-draft-2022

EXAMPLES
========

None.

THREAD-SAFE
===========

This function is thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.


NOTES
=====

**telescope_raw**() will not change the telescope state. Therefore, it may lead the telescope state iinconsitency. This function should be used by expert for low-level testing purpose. Other telescope library may be implemented through **telescope_raw**().  

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

