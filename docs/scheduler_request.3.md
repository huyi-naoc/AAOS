% scheduler\_request(3) | Library Functions Manual
%
% April 2024

NAME
====

scheduler\_request - request an SIU from the scheduler.

SYNOPSIS
========

**#include <scheduler_rpc.h>**

int  
**scheduler_request_by_name**(void *\*\_self*, const char *\*name*, unsigned int *\*option*, void *\*result*, size\_t *size*, size\_t *\*length*);  
int  
**scheduler_request_by_id**(void *\*\_self*, const char *identifier*, unsigned int *\*option*, void *\*result*, size\_t *size*, size\_t *\*length*);  

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **scheduler_request_by_name**() and **scheduler_request_by_id**() functions are used to request an SIU (see **scheduler_siu**(7)) from the scheduler through *name* or *identifier* of the telescope, respectively. The content of the SIU will be stored in *result*. The caller must also provide the size of *result* in *size*.  The format of *result* is stored in *options*. If *length* is not NULL, the length of *result* will also be returned.

The format of *result* may be SIU\_FORMAT\_JSON, SIU\_FORMAT\_XML, SIU\_FORMAT\_LIBCONFIG, SIU\_FORMAT\_YAML, SIU\_FORMAT\_INI, currently JSON format only.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_ENOTFOUND
---------------

The telescope is not scheduled by the scheduler.

CONFORMING TO
=============

AAOS-draft-2024

EXAMPLES
========

None.

THREAD-SAFE
===========

These functions are thread-safe, as long as *\*\_self* is not shared among threads. Otherwise, it is the caller's resposibility to protect *\*\_self*. The behavior of sharing *\*\_self* without approriate guard will be **undefined**.

SEE ALSO
========

**scheduler_siu**(7)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

