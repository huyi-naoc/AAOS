% detector\_get\_template(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_prefix - retrieve the FITS header template used for detector image files

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_template**(void *\*\_self*, char *\*template*, size\_t *size*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_template**() function obtains the filename of the FITS template that the detector referenced by *\_self* uses to initialize the primary header of each image it creates.

If the supplied buffer (*template*) is too small to hold the entire template filename, only the first *size‑1* bytes are copied and the string is NUL‑terminated; the excess characters are silently discarded.

## Parameter

*\_self*
:   Pointer to the detector instance.

*termplate*
:   User‑supplied buffer that receives the template filename. The result is always NUL‑terminated. If the actual filename length exceeds size bytes, the string is truncated as described above.

*size*
:   Size of *template* in bytes.

# RETURN VALUE

Always return **AAOS\_OK** (zero).

# ERRORS

**detector_get_template**() shall never faill; therefore no error codes are defined for this call.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_template**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_template**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
