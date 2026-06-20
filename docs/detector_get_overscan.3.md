% detector\_get\overscan(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_overscan - retrieve the detector’s overscan size

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_overscan**(void *\*\_self*, uint32\_t *\*x\_overscan*, uint32\_t *\*y\_overscan*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_overscan**() function retrieves the current overscan size of the detector referenced by *\*\_self*. The values are returned in pixels and written to the locations pointed to by *x\_overscan* and *y\_overscan*.

For a simple, single-readout‑channel detector the two values represent the number of overscan pixels in the horizontal (X) and vertical (Y) directions. For large‑format or mosaic detectors that have multiple readout channels, the returned values correspond to the overscan size of a single channel.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get overscan sizes despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameter

*\_self*
:   Pointer to the detector instance whose overscan is being queried.

*x\_overscan*
:   Pointer to a *uint32\_t* variable that will receive the overscan size in the X (horizontal) direction. The pointer must be valid (non‑NULL).

*y\_overscan*
:   Pointer to a *uint32\_t* variable that will receive the overscan size in the Y (vertival) direction. The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **detector_get_overscan**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

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

**detector_get_overscan**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_overscan**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
