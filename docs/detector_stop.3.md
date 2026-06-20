% detector\_stop(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_stop - stop previous exposure

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int   
**detector_stop**(void *\*\_self*)

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_stop**() function waits for the current exposing frame written to the image file and then terminates remained frames. 

* If the detector is **not** in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING**, the function returns immediately without taking any action.
* Otherwise, the exposure is stopped, the remained frames (if any) are discarded, and the detector state is set to **DETECTOR_STATE_IDLE**.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to stop the exposure despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.


## Parameter

*\_self*
:   Pointer to the detector instance whose current exposure is to be stopped._

# RETURN VALUE

On success, **detector_stop**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).
 
## AAOS\_ENOTSUP

The operation is not supported by the underlying detector.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_stop**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_abort**(3), **detector_expose**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
