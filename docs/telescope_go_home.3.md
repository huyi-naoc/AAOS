% telescope\_go\_home(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_go\_home - slew then park the telescope at a proper position

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_go_home**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_go_home**() function slews the telescope referenced by *\*\_self* to a safe home position and parks it there. The function blocks the calling thread until the operation completes.

Regardless of the current telescope's state, **telescope_go_home**() changes the telescope state to **TELESCOPE_STATE_PARKED**, unless current state is **TELESCOPE_STATE_MALFUNCTION**, **TELESCOPE_STATE_PWROFF* or *TELESCOPE_STATE_EUNINIT**.

## Parameters

*\_self*
:   Pointer to the telescope instance to be parked at home position.
 
# RETURN VALUE

On success, **telescope_go_home**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_ECANCELED

The operation was cancelled because another function call of *telescope_go_home*(), *telescope_move*(), *telescope_park*(), *telescope_slew*(), or *telescope_stop*() by another thread on the same physical telescope was invoked for the same underlying telescope. The state of the telescope depends on how the slew operation was canceled. 

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

**telescope_go_home**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# RATIONALE

Most professional telescopes have an enclosure, such as a dome, to protect them from adverse weather conditions. However, closing the dome can be dangerous if the telescope is pointing to certain positions. **telescope_go_home**() slews the telescope to a predefined safe position and parks it there. As such, subsequent operations to close the dome are guaranteed to be safe.

# SEE ALSO

**telescope**(1), **telescope_park**(3), **telescope_slew**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

