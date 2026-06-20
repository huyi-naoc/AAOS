% telescope\_get\_index\_by\_name(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_get\_index\_by\_name - get the index of the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_get_index_by_name**(void *\*\_self*, const char *\*name*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_get_index_by_name**() function looks up the telescope that has the specified *name* and stores its internal index inside the telescope instance referenced by *\_self*. The index is used internally by the library and is opaque to the application programmer; the caller does not retrieve the index directly.

## Parameters
*\_self*
:   Pointer to a telescope object that will receive the internal index.

*name*
:   Pointer to a NUL‑terminated string containing the name of the telescope to be looked up. The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **telescope_get_index_by_name**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ENOTFOUND

The telescope named *name* is not found.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_get_index_by_name**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

None.

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

