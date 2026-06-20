% detector\_get\_prefix(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_prefix - retrieve the prefix used for detector image filenames

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_prefix**(void *\*\_self*, char *\*prefix*, size\_t *size*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_prefix**() function retrieves the filename prefix that the detector referenced by *\_self* uses when it creates image files.

If the supplied buffer (*prefix*) is too small to hold the entire prefix, only the first *size‑1* bytes are copied and the string is NUL‑terminated; the excess characters are silently discarded.

## Parameter

*\_self*
:   Pointer to the detector instance.

*prefix*
:   User supplied buffer that receives the prefix. It will be NUL‑terminated. If the actual directory length exceeds *size* bytes, the result is truncated as described above.

*size*
:   Size of *prefix* in bytes.

# RETURN VALUE

Always return **AAOS\_OK** (zero).

# ERRORS

**detector_get_prefix**() shall never faill; therefore no error codes are defined for this call.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_prefix**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_prefix**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
