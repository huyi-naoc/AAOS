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

<!--**typedef void (\*image_callback_t)(void \*, const char \*, va_list \*);**-->

*typedef void (\*image\_callback_t)(void \*, const char \*, va\_list \*)*;

int   
**detector_expose**(void *\*\_self*, double *exposure\_time*, uint32\_t *n\_frames*, 
$~~~~~~~~~~~~~$image\_callback\_t *image\_callback*, *...*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **detector_expose**() function execute an exposure command of the detetcor referenced by *\*\_self*. The *exposure\_time* is the exposure time in seconds, and the *n\_frames* is the number of exposures. The image will be save to a file whose format is conformed to FITS standard. Multiple frames may save to a single file or seperated files, depending on the setting options. After each file has saved to the file, a user defined function *image\_callback* will be invoked. The *image\_callback* function has three parameters, the first and the second are *\*\_self* and the filename of the generated FITS file respectively. The third is a pointer to the variable parameter of this function. If *image\_callback* is NULL, this function will print the filename to the standard ouput (stdout). The image file will be stored, the prefix of the filename and its directory can be specified by **detector_set_prefix**() and **detector_set_directory**().


During **detector_expose**(), the state of the detector is changed to EXPOSING and then READING. After the image(s) are captured and saved to the FITS files, it returns and the state of the detector is changed back to IDLE. 


The **detector_expose**() will return when all the requested frames are saved, or the last frame is ready to read. 

The directory that stored the generated FITS files can be set by **detector_set_directory**. The filenames are like "prefix\_YYYYmmdd\_HHMMSS\_nn.fits", where *prefix* can be set by **detector_set_prefix**. The keywords in the primary HDU of the file are copied from template file, whhich can be set by **detector_set_template**.

RETURN VALUE
============

Upon successful completion, a value of zero shall be returned; otherwise, an error number shall be returned to indicate the error.

ERRORS
======

This functions shall fail if:

AAOS\_EDEVMAL
-------------

The underline detector is in *MALFUNCTION* state.

AAOS\_EINTER
------------

The exposure is stopped or aborted before all the requested frames are completed.

AAOS\_EINVAL
------------

The value of  *exposure\_time*  is invalid.

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
**detector_abort**(3), **detector_set_directory**(3), **detector_set_exposure_time**(3), **detector_set_prefix**(3), **detector_set_template**(3), **detector_stop**(3)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

