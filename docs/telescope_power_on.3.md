% telescope\_power_on(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_power\_on - power on the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_power_on**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_power_on**() function powers on the telescope referenced by *\*\_self*.   

If the telescope is already powered, the call has no effect and returns success.  If the telescope was powered off, the call turns the power on and sets the telescope state to **TELESCOPE_STATE_UNINITIALIZED**.  The telescope must be initialized (e.g. via `telescope_init(3)`) before it can be used for tracking, slewing, or other operations.

## Parameters

*\_self*
:   Pointer to the telescope instance.

# RETURN VALUE

On success, **telescope_set_track_rate**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

# AAOS\_ENOTSUP

The underlying telescope does not support this operation.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_power_on**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_power_init**(3), **telescope_power_off**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

