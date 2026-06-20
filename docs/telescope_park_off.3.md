% telescope\_park\_off(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_park - park off the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_park_off**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_park_off**() function park off the telescope referenced by *\_self*, which means the two axis are both energized by this function.    

If the telescope is not parked, calling this function does nothing and return successfully. Otherwise, if the current telescope state is **TELESCOPE_STATE_PARKED**, the state will change to **TELESCOPE_STATE_TRACKING**.

## Parameters

*\_self*
:   Pointer to the telescope instance.

# RETURN VALUE

On success, **telescope_park**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underline telescope is in a *malfunction* state.

## AAOS\_EPWROFF

The underline telescope is not powered.

## AAOS\_EUNINT

The underline telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

## CONFORMING TO

AAOS-draft-2022

## EXAMPLES

None.

## THREAD-SAFE

**telescope_park**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO


**telescope**(1), **telescope_init**(3), **telescope_park**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

