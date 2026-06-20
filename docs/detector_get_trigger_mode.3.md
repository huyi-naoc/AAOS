% detector\_get\_trigger\_mode(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_capture\_mode - retrieve the detector's trigger mode

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_trigger_mode**(void *\*\_self*, uint32\_t *\*trigger\_mode*);
 

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_trigger_mode**() function obtains the current trigger mode of the underlying detector referenced by referenced by *\*\_self*. The mode is written to the location pointed to by *trigger\_mode*. The trigger mode determines how an exposure is started (e.g., default without trigger, software‑triggered, or hardware‑triggered).  The value stored in *trigger\_mode* will be one of the following constants defined in **<detector_def.h>**: 

* **DETECTOR_TRIGGER_MODE_DEFAULT**
* **DETECTOR_TRIGGER_MODE_SOFTWARE_LEVEL**
* **DETECTOR_TRIGGER_MODE_SOFTWARE_EDGE**
* **DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_HIGH**
* **DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_LOW**  
* **DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_RISE**
* **DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_FALL**

The meaning of each constant is self‑explanatory from its name.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get trigger mode despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameter

*\_self*
:   Pointer to the detector instance whose trigger mode is being queried.

*capture\_mode*
:   Pointer to a uint32\_t variable that will receive the trigger mode. The pointer must be valid (non‑NULL). 

# RETURN VALUE

On success, **detector_get_trigger_mode**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

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

**detector_get_trigger_mode**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_trigger_mode**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
