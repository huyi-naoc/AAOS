% detector\_wait\_for\_completion(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_wait\_for\_completion - wait for completion of last image 

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

*typedef void (\*image\_callback_t)(void \*, const char \*, va\_list \*)*;

int   
**detector_wait_for_completion**(void *\*\_self*, 
$~~~~~~~~~~~~~~~~~~~~~~~~~~$image\_callback\_t *image\_callback*, *...*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_wait_for_completion**() function wait for the last image has been grabbed and save to the file. If the underline detector is not in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state, it does nothing and returns immediately. Otherwise, it will also set the detector state to **DETECTOR_STATE_IDLE**. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
-------------

The underline detector is in *MALFUNCTION* state.

AAOS\_ENOTSUP
-------------

The underline detetcor does not support this operation.

AAOS\_EPWROFF
-------------

The underline detector is not powered.

AAOS\_EUNINT
------------

The underline detector is uninitialized.

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
**detector_expose**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

