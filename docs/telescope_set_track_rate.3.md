% telescope\_set\_track\_rate(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_set\_track\_rate - set track rates of both axes of telescope

# SYNOPSIS


**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_set_track_rate**(void *\*\_self*, double *track_rate_x*, double *track_rate_y*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION


The **telescope_set_track_rate**() function set the track rate of the telescope referenced by *\*\_self*. The rates are expressed in *arcseconds per second*. If the telescope is currently in the **TELESCOPE_STATE_TRACKING** state, the new rates take effect immediately.  Otherwise the rates are stored and will be applied automatically after the next call to one of the following functions: **telescope_move**() (any variant), **telescope_park_off**(), **telescope_slew**() (any variant), **telescope_stop**().

## Parameters
*\_self*
:   Pointer to the telescope instance.

*track_rate_x*
:   Tracking rate for the primary axis (right ascension for an equatorial mount or azimuth for a horizontal/alt‑az mount), in arcsecond per second.

*track_rate_y*
:   Tracking rate for the secondary axis (declination for an equatorial mount or altitude for a horizontal/alt‑az mount), in arcsecond per second.

# RETURN VALUE

On success, **telescope_set_track_rate**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_EINVAL

One or both of the supplied tracking rates are outside the allowed range.

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

**telescope_set_track_rate**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_get_track_rate**(3), **telescope_move**(3), **telescope_park_off**(3), **telescope_slew**(3),  **telescope_stop**(3), **telescoped**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

