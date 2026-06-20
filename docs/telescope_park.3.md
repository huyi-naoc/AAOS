% telescope\_park(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_park - park the telescope (de‑energise both axes)

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_park**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_park**() function stops any motion the telescope referenced by *\*\_self* and de‑energises both axes, leaving it at its current position. 

If the telescope is already parked, the function does nothing and returns success. If the telescope is in a **TELSCOPE_STATE_MOVING**, **TELESCOPE_STATE_SLEWING** or **TELESCOPE_STATE_TRACKING** state, the state is changed to **TELESCOPE_STATE_PARKED**.

## Parameters

*\_self*
:   pointer to the telescope instance to be parked.

# RETURN VALUE

On success, **telescope_park**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_park**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_park_off**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
