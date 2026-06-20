% telescope\_stop(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_stop - halt motion of both axes of the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  

int  
**telescope_stop**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

**telescope_stop()** function halts the motion of both axes of the underlying telescope referenced by *\_self*. If the telescope currently *moving* or *slewing*,  the call stops the motion immediately and the telescope resumes normal tracking. If the telescope is already *tracking* or *parked*, the call has no effect.


## Parameters

*\_self*
:   Pointer to the telescope instance whose axis movement is to be stopped.


# RETURN VALUE

On success, **telescope_stop**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_ENOTSUP

The underlying telescope does not support a *stop* operation. 

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_stop**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_park**(3), **telescope_park_off**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

