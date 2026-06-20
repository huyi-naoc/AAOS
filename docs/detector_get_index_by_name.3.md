% detector\_get\_index\_by\_name(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_index\_by\_name - obtain the internal index of a detector object

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_index_by_name**(void *\*\_self*, const char *\*name*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_index_by_name**() function looks up the detector that has the specified *name* and stores its internal index inside the detector instance referenced by *\_self*. The index is used internally by the library and is opaque to the application programmer; the caller does not retrieve the index directly.

## Parameters
*\_self*
:   Pointer to a detector object that will receive the internal index.

*name*
:   Pointer to a NUL‑terminated string containing the name of the detector to be looked up. The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **detector_get_index_by_name**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ENOTFOUND

The detector named *name* is not found.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_index_by_name**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

None.

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.