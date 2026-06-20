% telescope\_get\_slew\_speed(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_get\_slew\_speed - get slew speeds of both axes of telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_get_slew_speed**(void *\*\_self*, double *\*slew_speed_x*, double *\*slew_speed_y*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_get_slew_speed**() function get the slew speed of the telescope referenced by *\*\_self*. The speeds are both  expressed in *arcseconds per second*.

The *slew_speed_x* could be the slew speed of either RA axis for a equatorial mount or east-west axis for a horizontal mount, whereas the *slew_speed_y* could be the slew speed of either DEC axis for a equatorial mount or north-south axis for a horizontal mount. 

## Parameters

*\_self*
:   Pointer to the telescope instance.

*slew_speed_x*
:   Pointer to a double variable that will receive the slew speed (arcsecond per second) of the X axis. The pointer must be valid (non‑NULL).

*slew_speed_y*
:   Pointer to a double variable that will receive the slew speed (arcsecond per second) of the Y axis. The pointer must be valid (non‑NULL).

# RETURN VALUE

On success, **telescope_get_slew_speed**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_ENOTSUP

The operation is not supported.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underline telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_get_slew_speed**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_set_slew_speed**(3), **telescope_slew**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

