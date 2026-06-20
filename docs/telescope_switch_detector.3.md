% telescope\_switch\_detector(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_switch\_detector - switch the active detector of the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  

int  
**telescope_switch_detector**(void *\*\_self*, const char *\*name*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

**telescope_switch_detector()** function changes the active detctor of  the underlying telescope referenced by *\_self* to the detector whose name is given by *name*. The name must match one of the detector identifiers known to the telescope daemon; otherwise the call fails with **AAOS_ENOTFOUND**.

## Parameters

*\_self*
:   Pointer to the telescope instance whose detector is to be switched.

*name*
:   null‑terminated string naming the target detector.


# RETURN VALUE

On success, **telescope_switch_detector**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_ENOTFOUND

The instrument is not found.

## AAOS\_ENOTSUP

The underlying telescope does not support detector switching. 

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_switch_detector**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_switch_filter**(3), **telescope_switch_instrument**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.



