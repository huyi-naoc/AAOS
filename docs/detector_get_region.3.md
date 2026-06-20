% detector\_get\_region(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_region - get the detector's region of interest

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_region**(void *\*\_self*, uint32\_t *\*x\_offset*, uint32\_t *\*y\_offset*,
$~~~~~~~~~~~~~~~~~$uint32\_t *\*width*, uint32\_t *\*height*);
 

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_region**() function retrieves the region of interest (ROI) that the underlying detector is currently using. The ROI is defined by its origin, given by *​x\_offset* and *​y\_offset* (measured from the top‑left corner of the detector), and by its ​*width* and *​height*.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get ROI despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose grabbing region is being queried.

*x\_offset*
:   Pointer to a uint32\_t variable that will receive the ROI's offset in the X direction. The pointer must be valid (non‑NULL).

*y\_offset*
:   Pointer to a uint32\_t variable that will receive the ROI's offset in the Y direction. The pointer must be valid (non‑NULL).

*width*
:   Pointer to a uint32\_t variable that will receive the ROI's width. The pointer must be valid (non‑NULL).

*height*
:   Pointer to a uint32\_t variable that will receive the ROI's height. The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **detector_get_region**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

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

**detector_set_region**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_region**(3), **detector**(7) 

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
