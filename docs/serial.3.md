% serial\_raw(3) | Library Functions Manual
%
% May 2022

NAME
====

serial\_raw - execute raw command to the serial

SYNOPSIS
========

**#include <serial_rpc.h>**  

int  
**serial_raw**(void *\*\_self*, const void *\*command*,  
$~~~~~~~~~~$size\_t *command_size*, void *\*results*,  
$~~~~~~~~~~$size\_t *results_size*, size\_t *\*return_size*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **serial_raw**() function executes a raw command of the serial referenced by *\*\_self*. The *command* is the content of the command, and the *command_size* is the length of the size. The executing results will be filled into the buffer pointed by *results*, the *results_size* is the size of the *results* in bytes, the *return_size* is the length of *results*. If the actual length of results is larger than *results_size*, only the first *results_size* bytes will be filled in. If *return_size* is **NULL**, the length results will not be returned.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EBADCMD
-------------

The *command* is illegal or not supported.

AAOS\_ETIMEDOUT
---------------

Receiving result timed out.

AAOS\_EUNINT
------------

The underline serial is uninitialized, e.g., baud rate, canonical mode, etc., have not been set yet by **serial_init**().

CONFORMING TO
=============

AAOS-draft-2022

EXAMPLES
========

None.

THREAD-SAFE
===========

This function is thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.


BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

