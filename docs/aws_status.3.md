% aws\_status(3) | Library Functions Manual
%
% May 2022

NAME
====

aws\_status - get the status of the AWS

SYNOPSIS
========

**#include <stdio.h>**
**#include <aws_rpc.h>**  
**#include <aws_def.h>**

int  
**aws_status**(void *\*\_self*, FILE *\*fp*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **aws_status**() function gets the status of the AWS. The status will be write to file that opened as *fp* in ASCII code.


At least the state of the AWS must be included in the status.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTFOUND
---------------

The aws indexed in *\_self* is not found.

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

**aws_inspect**(3), **aws_register**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

