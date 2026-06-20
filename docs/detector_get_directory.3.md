% detector\_get\_directory(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_directory - retrieve the directory in which detector images are stored

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_directory**(void *\*\_self*, char *\*directory*, size\_t *size*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The detector_get_directory() function retrieves the directory associated with the detector instance referenced by *_self* – i.e. the directory where the detector stores its image files.

If the supplied buffer (*directory*) is too small to hold the entire path, only the first *size‑1* bytes are copied and the string is NUL‑terminated; the excess characters are silently discarded.

## Parameter

*\_self*
:   Pointer to the detector instance.

*directory*
:   User supplied buffer that receives the directory path. It will be NUL‑terminated. If the actual directory length exceeds *size* bytes, the result is truncated as described above.

*size*
:   Size of *directory* in bytes.

# RETURN VALUE

Always return **AAOS\_OK** (zero).

# ERRORS

**detector_get_directory**() shall never faill; therefore no error codes are defined for this call.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_directory**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_directory**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.