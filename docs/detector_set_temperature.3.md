% detector\_set\_temperature(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_temperature - set desired temperature of detector's chip

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_temperature**(void *\*\_self*, double *temperature*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_temperature**() function sets the cooling temperature of the detector referenced by *\*\_self*. The temperature is expressed in Celsius degree.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to set temperature despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose temperature is to be set.

*temperature*
:   Desired chip temperature (in Celsius degree) for the cooling system to achieve. 

# RETURN VALUE

On success, **detector_set_temperature**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

# AAOS\_EINVAL

The supplied *temperature* value is outside the allowed range.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_temperature**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_temperature**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
