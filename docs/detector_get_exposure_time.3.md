% detector\_get\_exposure\_time(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_exposure\_time - retrieve the detector’s exposure time

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_exposure_time**(void *\*\_self*, double *\*exposure\_time*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_exposure_time**() function obtains the current exposure time of the underlying detector referenced by *\_self*. The value is returned in seconds and written to the location pointed to by *exposure\_time*.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get exposure time despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose exposure time is being queried.

*exposure\_time*
:   Pointer to a double variable that will receive the exposure time (in second). The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **detector_get_exposure_time**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_exposure_time**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_expose**(3), **detector_set_exposure_time**(3), **detector_set_frame_rate**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
