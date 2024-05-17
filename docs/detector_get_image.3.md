% detector\_get\_image(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_get\_image - get image

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_image**(void *\*\_self*, const char *\*filename*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_get_image**() function copies the image in the image directory to the current work directory.


RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EEXIST
------------

The *filename* does not exist in the directory.

AAOS\_ENOTSUP
-------------

The underline detector does not support this operation.

AAOS\_EPERM
-----------

The calling user has not the write permission of current directory.


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

**detector_get_directory**(3), **detector_set_directory**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

