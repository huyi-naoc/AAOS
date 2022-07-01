% detector\_expose(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_expose - do expose of the detector

SYNOPSIS
========

**#include <stdarg.h>**

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

**typedef void (\*image\_callback\_t)(void \*, const char \*, va_list \*);**

int   
**detector\_expose**(void *\*\_self*, double *exposure\_time*, uint32\_t *n\_frames*, 
$~~~~~~~~~~~~~$image\_callback\_t *image\_callback*, *...*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector\_expose**() function execute an exposure command of the detetcor referenced by *\*\_self*. The *exposure\_time* is the exposure time in seconds, and the *n\_frames* is the number of exposures. The image will be save to a file whose format is conformed to FITS standard. Multiple frames may save to a single file or seperated files, depending on the setting options. After each file has saved to the file, a user defined function *image\_callback* will be invoked. The *image\_callback* function has three parameters, the first and the second are *\*\_self* and the filename of the generated FITS file respectively. The third is a pointer to the variable parameter of this function.

 The **detector\_expose**() will return when all the requested frames are saved, or the last frame is ready to read. 

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

These functions shall fail if:

AAOS\_EDEVMAL
-------------

The underline detector is in *MALFUNCTION* state.

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

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

