% telescope\_power\_off(3) | Library Functions Manual
%
% May 2022

NAME
====

telescope\_power\_off - power off the telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_power_off**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

DESCRIPTION
===========

The **telescope_power_off**() function powers off the telescope referenced by *\*\_self*.   

If the telescope has already been powered off, calling this function does nothing and return successfully. Otherwise, it will change the state of the telescope to *TELESCOPE_STATE_PWROFF* no matter which state the telescope is right now. 

## Parameters

*\_self*
:   Pointer to the telescope instance.

# RETURN VALUE

On success, **telescope_power_off**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

# AAOS\_ENOTSUP

The underlying telescope does not support this operation.

# CONFORMING TO

AAOS-draft-2022

# NOTES

The function is typically called after **telescope_go_home**() and before **dome_close_window**() to ensure the telescope is safely parked before the dome is closed.

# EXAMPLES

None.

# THREAD-SAFE

**telescope_power_off**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **dome_close_window**(3), **telescope_gpo_home**(3), **telescope_power_init**(3), **telescope_power_on**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

