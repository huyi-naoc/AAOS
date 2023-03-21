% aws\_inspect(3) | Library Functions Manual
%
% May 2022

NAME
====

aws\_inspect - inspect the aws

SYNOPSIS
========

**#include <aws_rpc.h>**  
**#include <aws_def.h>**

int  
**aws_inspect**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **aws_inspect**() function inspects on the aws referenced by *\*\_self*. If the aws is malfunction, it will change the state of the aws to *AWS_STATE_MALFUNCTION*. 


RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error. If the aws works correctly, it will notify all the processes calling **aws_register** to wait for the recovery of the aws.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTSUP
------------

The underline aws does not support this operation.

AAOS\_ERROR
-----------

The underline aws is in malfunction state.


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

**aws_register**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

