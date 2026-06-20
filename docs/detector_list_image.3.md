% detector\_list\_image(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_list\_image - list image 

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_list_image**(void *\*\_self*, char *\*filelist*, size\_t, *size*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

 The **detector_list_image**() function retrieves the names of all image files present in the directory associated with the detector instance referenced by *\_self*. The function copies the list into the user‑supplied buffer *filelist*. The list is formatted as a sequence of file names separated by newline characters ('\n'); the whole string is NUL‑terminated.

 If the *filelist* size *size* is insufficient to hold the complete list, at most *size‑1* bytes are copied, the result is NUL‑terminated, and the remaining file names are silently discarded.

 ## Parameters

*\_self*
:   Pointer to the detector instance whose image directory is being queried.

*filelist*
:   User supplied buffer that receives the list of image file names. It will be NUL‑terminated. If the actual directory length exceeds *size* bytes, the result is truncated as described above.

*size*
:   Size of *filelists* in bytes.


# RETURN VALUE

On success, **detector_list_image**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ENOENT

The image directory associated with the underlying detector does not exist. 

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY


**detector_list_image**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_directory**(3), **detector_set_directory**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
