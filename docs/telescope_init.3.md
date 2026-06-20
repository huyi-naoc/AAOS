% telescope\_init(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_init - initialize the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_init**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_init**() function initializes the telescope referenced by *\*\_self*. By calling this funcntion, **runtime** parameters of the telescope, such as time, location, PID, and so on, are approriately set up.  

If the telescope has already been initialized, calling this function does nothing and return successfully. Otherwise, it sets the telescope state to **TELESCOPE_STATE_TRACKING**.

## Parameters

*\_self*
:   Pointer to the telescope instance to be initialized.

# RETURN VALUE

On success, **telescope_inspect**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_EPWROFF

The underlying telescope is not powered.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_init**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# RATIONALE

Many telescopes cannot retain their runtime parameters when powered off. Thus, an initialization operation is needed after the system is powered on.

# SEE ALSO

**telescope**(1), **telescope_power_off**(3), **telescope_power_on**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.