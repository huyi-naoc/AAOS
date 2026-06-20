% detector\_power\_on(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_power\_on - power on the detector

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_power_on**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_power_on**() function powers on the detector referenced by *\*\_self*.   

If the detector is already powered, the call has no effect and returns success.  If the detector was powered off, the call turns the power on and sets the detector state to **DETECTOR_STATE_UNINITIALIZED**.  The detector must be initialized (e.g. via `detector_init(3)`) before it can be used for exposing or other operations.

## Parameters

*\_self*
:   Pointer to the detector instance whose power supply to be switched on.

# RETURN VALUE

On success, **detector_power_on**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_power_on**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_init**(3), **detector_power_off**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
