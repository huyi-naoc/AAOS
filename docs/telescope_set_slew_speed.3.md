% telescope\_set\_slew\_speed(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_set\_slew\_speed - set slew speeds of both axes of telescope

SYNOPSIS
========

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_set_slew_speed**(void *\*\_self*, double *slew_speed_x*, double *slew_speed_y*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_set_slew_speed**() function set the slewing speed of the telescope referenced by *\*\_self*. The speeds are expressed in *arcseconds per second*. They take effect the nexttime a slew operation is started (e.g. via **telescope_slew**() or any of its family functions).

## Parameters
*\_self*
:   Pointer to the telescope instance.

*slew_speed_x*
:   Slewing speed for the primary axis (right ascension for an equatorial mount or azimuth for a horizontal/alt‑az mount), in arcsecond per second.

*slew_speed_y*
:   Slewing speed for the secondary axis (declination for an equatorial mount or altitude for a horizontal/alt‑az mount), in arcsecond per second.

# RETURN VALUE

On success, **telescope_set_track_rate**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_EINVAL

One or both of the supplied slewing speeds are outside the allowed range.

## AAOS\_ENOTSUP

The underlying telescope does not support this operation.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_set_slew_speed**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO


**telescope**(1), **telescope_get_slew_speed**(3), **telescope_slew**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

