% detector\_abort(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_abort - abort current exposure

# SYNOPSIS


**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int   
**detector_abort**(void *\*\_self*)

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_abort**() function abort the current exposure operation of the underlying detector referenced by *\_self* immediately. 

* If the detector is not in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state, it does nothing and returns instantly. 
* If the detector is in either of those states, the function forces the detector’s state to **DETECTOR_STATE_IDLE**, terminating the exposure sequence. 
* If the underlying hardware does not support an explicit abort operation, the implementation may fall back to the behaviour of **detector_stop**().

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to abort exposure despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose exposure is to be aborted.

# RETURN VALUE

On success, **detector_abort**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

## AAOS\_ENOTSUP

The underlying detetcor does not support this operation.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**detector_abort**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_expose**(3), **detector_stop**(3), **detector_wait_for_completion**(3), **detectord**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
