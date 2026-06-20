% detector\_get\_binning(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_binning - get binning

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_binning**(void *\*\_self*, uint32\_t *\*x\_binning*, uint32\_t *\*y\_binning*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_binning**() function retrieves the current binning factors of the detector referenced by *\*\_self*. The values returned in *x\_binning* and *y\_binning* are expressed in pixels (i.e., the number of detector pixels that are combined along each axis).

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get binning despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameter

*\_self*
:   Pointer to the detector instance whose binning is being queried.

*x\_binning*
:   User supplied buffer to store the binning factor in the X direction.

*y\_binning*
:   User supplied buffer to store the binning factor in the X direction.

# RETURN VALUE

On success, **detector_get_binning**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

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

**detector_get_binning**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_binning**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
