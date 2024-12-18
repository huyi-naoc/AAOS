% scheduler\_update\_status(3) | Library Functions Manual
%
% Dec 2024

NAME
====

scheduler\_update\_status - update sites, telescopes, targets, and tasks status

SYNOPSIS
========

**#include <scheduler_rpc.h>**  
**#include <scheduler_def.h>**

int  
**scheduler_update_status**(void *\*\_self*, const char *\*string*, 
$~~~~~~~~~~~~~~~~~~~~~~~~$unsigned int type);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **scheduler_update_status**() function updates the status of sites, telescopes, targets and tasks. The identifiers of the sites, telescopes, targets and tasks are stored in the argument *string*, as well as their status. The format of *string* are specified by the *type* argument. Only **SCHEDULER_FORMAT_JSON** is currently supported.  

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EBADCMD
-------------

Syntax error of the *string* argument.

AAOS_EFMTNOTSUP
---------------

The *type* format of the *string* argument is not supported.


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
