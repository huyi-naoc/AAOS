% detector\_set\_readout\_rate(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_readout\_rate - set detector‘s readout rate

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_readout_rate**(void *\*\_self*, double *readout_rate*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_readout_rate**() function sets the readout speed of the detector referenced by *\*\_self*. The readout rate is expressed in kilobit per second (kbps).

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to set readout rate despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose temperature is being queried.

*temperature*
:   Desired chip temperature (in Celsius degree) for the cooling system to achieve. 

# RETURN VALUE

On success, **detector_set_readout_rate**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a setting request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

# AAOS\_EINVAL

The supplied *readout_rate* value is outside the allowed range.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_set_readout_rate**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_option**(3), **detector_set_option**(3),  **detector_get_readout_rate**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
