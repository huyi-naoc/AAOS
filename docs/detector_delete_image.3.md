% detector\_delete\_image(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_delete\_image - delete image 

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_delete_image**(void *\*\_self*, const char *\*filename*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_delete_image**() function delete the image named *filename* in the directory.


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

**detector_delete_all_image**(3), **detector_get_directory**(3), **detector_set_directory**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

