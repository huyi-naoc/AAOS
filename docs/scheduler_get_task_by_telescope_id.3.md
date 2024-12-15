% scheduler\_get\_task\_by\_telescope\_id(3) | Library Functions Manual
%
% Dec 2024

NAME
====

scheduler\_get\_task\_telescope\_id - get telescope's current observation task

SYNOPSIS
========

**#include <scheduler_rpc.h>**  
**#include <scheduler_def.h>**

int  
**scheduler_get_task_by_telescope_id**(void *\*\_self*, uint64\_t *identifier*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$char *\*result*, size\_t *size*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$sizte\_t *length*, unsigned int *type*);
int  
**scheduler_get_task_by_telescope_name**(void *\*\_self*, const char *\*name*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$char *\*result*, size\_t *size*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$sizte\_t *length*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int *type*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **scheduler_get_task_by_telescope_id**() and **scheduler_get_task_by_telescope_name**() functions get an observation task from the scheduler referenced by *\*\_self*. The telescope is identified by its *identifier* or *name*. The scheduling result is stored in *result*, whose capacity is *size* bytes and whose length is *length*. The format of the result is indicated as *type*. Only **SCHEDULER_FORMAT_JSON** is currently supported.  

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTFOUND
---------------

The telescope identified by *identifier* or *name* is not found.

AAOS\_ENOTSUP
------------

The underline scheduler *_self* is an site scheduler, thus **get_task** operation is not supported.

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
