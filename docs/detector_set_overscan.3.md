% detector\_set\_binning(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_binning - set the detector’s overscan size

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_overscan**(void *\*\_self*, uint32\_t *x\_overscan*, uint32\_t *y\_overscan*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_overscan**() function sets the overscan of the detector referenced by *\_self*. For a simple, single-readout‑channel detector the two values represent the number of overscan pixels in the horizontal (X) and vertical (Y) directions. For large‑format or mosaic detectors that have multiple readout channels, the returned values correspond to the overscan size of a single channel.

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to set overscan despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose overscan is to be configured.

*x\_overscan*
:   Desired overscan in the X direction (pixels).

*y\_overscan*
:   Desired overscan in the Y direction (pixels).

# RETURN VALUE

On success, **detector_set_binning**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a setting request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).

## AAOS\_EINVAL

The supplied *x\_binning* and/or *y\_binning* value is invalid (not supported by the underlying detector).  

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

**detector_set_overscan**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_overscan**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
