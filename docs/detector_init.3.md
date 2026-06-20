% detector\_init(3) | Library Functions Manual
%
% May 2022

# NAME


detector\_init - initialize the detector

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_init**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.


# DESCRIPTION

The **detector_init**() function initializes the detector referenced by *\*\_self*. By calling this funcntion, **runtime** parameters of the detector, such as capture mode, pixel format, trigger mode, and so on, are approriately set up.  

If the detector has already been initialized, calling this function does nothing and return successfully. Otherwise, it sets the detector state to **DETECTOR_STATE_IDLE**.

## Parameters

*\_self*
:   Pointer to the detector instance to be initialized.

# RETURN VALUE

On success, **detector_init**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).

## AAOS\_EPWROFF

The underlying detector is not powered.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_init**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# RATIONALE

Detectors may not retain their runtime parameters when powered off. Consequently an explicit initialization step is required after a power‑on to restore capture mode, pixel format, trigger settings, etc., before the detector can be used.

# SEE ALSO

**detector**(1), **detector_power_off**(3), **detector_power_on**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
