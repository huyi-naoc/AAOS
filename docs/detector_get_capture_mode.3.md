% detector\_get\_capture\_mode(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_capture\_mode - retrieve the detector's capture mode

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_capture_mode**(void *\*\_self*, uint32\_t *\*capture\_mode*);
 

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_capture_mode**() function obtains the current capture mode of the underlying detector referenced by referenced by *\*\_self*. The mode is written to the location pointed to by *capture\_mode*. The value stored in *capture\_mode* will be one of the following constants: 

* **DETECTOR_CAPTURE_MODE_SNAPSHOT**
* **DETECTOR_CAPTURE_MODE_MULIFRAME**
* **DETECTOR_CAPTURE_MODE_VIDEO** 

The meaning of each constant is self‑explanatory from its name.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get capture mode despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameter

*\_self*
:   Pointer to the detector instance whose capture mode is being queried.

*capture\_mode*
:   Pointer to a uint32\_t variable that will receive the capture mode. The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **detector_get_capture_mode**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

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

**detector_get_capture_mode**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_capture_mode**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
