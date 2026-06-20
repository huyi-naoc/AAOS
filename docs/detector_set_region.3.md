% detector\_set\_region(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_region - set the detector's region of interest

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_region**(void *\*\_self*, uint32\_t *x\_offset*, uint32\_t *y\_offset*, uint32\_t *width*, uint32\_t *height*);
 

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_region**() function sets the region of interest (ROI) of the detector referenced by *\_self*. The ROI is defined by:

* *​x\_offset* and *​y\_offset*, the coordinates of the ROI’s origin, measured from the top‑left corner of the detector;
* ​*width* and *​height*, the size of the ROI in pixels.

If the detector’s binning factors are set to values other than 1, the interpretation of the four parameters (physical pixel vs. binned pixel) is hardware‑specific.

Changing the ROI may affect the maximum allowable frame rate and, consequently, the minimum exposure time.

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately;
* **without** the option, the function attempts to set ROI despite the malfunction state.

## Parameters

*\_self*
:   Pointer to the detector instance whose ROI is to be configured.

*x\_offset*
:   Desired ROI's offset in the X direction.

*y\_offset*
:   Desired ROI's offset in the X direction.

*width*
:   Desired ROI's width.

*height*
:   Desired ROI's height.

# RETURN VALUE

On success, **detector_set_region**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a setting request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

# AAOS\_EINVAL

The supplied *trigger\_mode* value is invalid (not supported by the underlying detector).  

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

**detector_set_trigger_mode**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_option**(3), **detector_set_option**(3), **detector_get_region**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
