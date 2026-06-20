% detector\_set\_frame\_rate(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_frame\_rate - set set detector's frame rate

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_frame_rate**(void *\*\_self*, double *frame_rate*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_frame_rate**() function sets the frame rate of the detector referenced by *\*\_self*. The *frame\_rate* parameter is expressed in frames per second (fps). 

f the detector also allows its exposure time to be set, the implementation may automatically shorten the exposure time so that its inverse (the maximum possible frame rate) does **not** exceed the requested *frame\_rate*. AAOS chooses to perform this automatic adjustment; other implementations may instead return **AAOS_EINVAL**. However, if the inverse of the requested exposure time is still larger than the maximum allowed frame rate, **detector_set_exposure_time**() will return **AAOS_EINVAL**.  

The minimum exposure time is also affected by other runtime parameters such as binning, region of interest (ROI), and readout rate; the caller does not need to know these internal details, but can retrieve them by calling **detector_info**().

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to set frame rate despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose frame rate is to be set.

*frame\_rate*
:   Desired frame rate in fps. The value must be within the range supported by the underlying hardware. 

# RETURN VALUE

On success, **detector_set_frame_rate**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section..

# ERRORS

This functions shall fail if:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).

## AAOS\_EINVAL

The value of *frame\_rate* is invalid, e.g., out of range. 

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

**detector_set_frame_rate**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_expose**(3), **detector_get_frame_rate**(3), **detector_info**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
