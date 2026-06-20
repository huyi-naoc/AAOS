% detector\_set\_exposure\_time(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_exposure\_time - set detector's exposure time

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_exposure_time**(void *\*\_self*, double *exposure\_time*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_exposure_time**() function sets the exposure time of the detector referenced by *\*\_self*. The *exposure\_time* parameter is expressed in seconds.

If the detector also supports setting its frame rate, the implementation may automatically adjust the frame rate so that its inverse (the minimum possible exposure time) is not shorter than the requested exposure time. AAOS chooses to perform this automatic adjustment; other implementations may instead return **AAOS_EINVAL**. However, if the inverse of the requested exposure time is still larger than the maximum allowed frame rate, **detector_set_exposure_time**() will return **AAOS_EINVAL**.  

The minimum exposure time is also affected by other runtime parameters such as binning, region of interest (ROI), and readout rate; the caller does not need to know these internal details, but can retrieve them by calling **detector_info**().

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to set exposure time despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose exposure time is to be set.

*exposure\_time*
:   Desired exposure time in seconds. The value must be within the range supported by the underlying hardware. 

# RETURN VALUE

On success, **detector_set_exposure_time**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section..

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a setting request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).

## AAOS\_EINVAL

The value of *exposure\_time* is invalid, e.g., out of range. 

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_set_exposure_time**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_expose**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detecctor_info**(), **detector_set_frame_rate**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
