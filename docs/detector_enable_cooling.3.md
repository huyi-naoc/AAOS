% detector\_enable\_cooling(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_enable\_cooling - enable the detector's cooling function

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int   
**detector_enable_cooling**(void *\*\_self*)

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_enable_cooling**() function enable the cooling system of the underlying detector.

If the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to enable the cooling system despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

*\_self*
:   Pointer to the detector instance whose cooling is to be enabled.

# RETURN VALUE

On success, **detector_enable_cooling**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

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

**detector_enable_cooling**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_disable_cooling**(3), **detector_get_temperature**(3), **detector_set_temperature**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
