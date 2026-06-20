% detector\_set\_binning(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_binning - set the detector’s binning factors

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_binning**(void *\*\_self*, uint32\_t *x\_binning*, uint32\_t *y\_binning*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION
 
The **detector_set_binning**() function sets the binning factors of the detector referenced by *\_self*.  The binning factor is the number of physical detector pixels that are combined (binned) along each axis, therefore the values are expressed in pixels. 

* Only binning values that are supported by the underlying hardware may be used; an unsupported value causes the call to fail with **AAOS_EINVAL**.
* Changing the binning may affect the maximum allowable frame rate and, consequently, the minimum exposure time.
* If the detector is currently exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the behaviour of the call depends on the **DETECTOR_OPTION_NOWAIT** option:
    * **with** the option set, the function returns **AAOS_EBUSY** immediately;
    * **without** the option (the default) the call blocks until the current exposure operation completes before applying the new binning.
* When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:
    * **with** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately;
    * **without** the option, the function attempts to wait for completion despite the malfunction state.

## Parameters

*\_self*
:   Pointer to the detector instance whose binning factor is to be configured.

*x\_binning*
:   Desired binning factor in the X direction (pixels).

*y\_binning*
:   Desired binning factor in the Y direction (pixels).

# RETURN VALUE

On success, **detector_set_binning**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a new binning request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

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

**detector_set_binning**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_binning**(3), **detector_get_option**(3), **detector_info**(3), **detector_set_option**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
