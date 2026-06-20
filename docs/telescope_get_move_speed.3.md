% telescope\_get\_move\_speed(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_get\_move\_speed - get current axis move speed

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_get_move_speed**(void *\*\_self*, double *\*move_speed*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_get_move_speed**() function get the moving speed of the telescope referenced by *\*\_self*. The speed is expressed in *arcseconds per second*.

## Parameters
*\_self*
:   Pointer to the telescope instance.

*move_speed*
:   Pointer to a double variable that will receive the move speed (arcsecond per second). The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **telescope_get_move_speed**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_ENOTSUP

The operation is not supported.

## AAOS\_EPWROFF

The underlying telescope does not support this operation.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE


**telescope_get_move_speed**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_move**(3), **telescope_set_move_speed**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

