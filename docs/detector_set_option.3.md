% detector\_set\_option(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_set\_option - set option

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_option**(void *\*\_self*, uint16\_t *option*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_set_option**() function set the option of the detector referenced by *\*\_self*. 

The available options are:

DETECTOR\_OPTION\_NOTIFY\_LAST\_FILLING
---------------------------------------

If the detector support, **detector_expose**() will return when the last frame is begun to read, i.e., the detector is in *DETECTOR\_STATE\_READING*, when this option is set. In default, this option is **NOT** set.

DETECTOR\_OPTION\_NOTIFY\_EACH\_COMPLETION
------------------------------------------

If the detector support, **detector_expose**() generate a single file and call the *image\_callback* for each frame. In default, this option is set.


RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_ENOTSUP
------------

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

**detector_clear_option**(3), **detector_get_option**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

