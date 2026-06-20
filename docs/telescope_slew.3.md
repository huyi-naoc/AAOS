% telescope\_slew(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_slew - slew the telescope to a celestial target

telescope\_timed\_slew - slew a telescope with a timeout 

telescope\_try\_slew - attemp a slew

# SYNOPSIS

**#include <telescope_rpc.h>**

int  
**telescope_slew**(void *\*\_self*, double *ra*, double *dec*);  
int  
**telescope_timed_slew**(void *\*\_self*, double *ra*, double *dec*, 
$~~~~~~~~~~~~~~~~~~~~~$double *timeout*);  
int  
**telescope_try_slew**(void *\*\_self*, double *ra*, double *dec*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

All three functions slew the telescope referenced by *\_self* to the celestial target specified by *ra* (right‑ascension‑like coordinate) and *dec* (declination‑like coordinate).  The coordinates are expressed in **degrees**. 

**telescope_slew**() blocks the calling thread until the slew finishes (or fails). If the telescope is already in a slewing or moving state, the call aborts the previous any **telescope_move**() or **telescope_slew**() family function. If the telescope is parked, it will be automatically un‑parked before the slew begins. During the slewing period the telescope is in a slewing state.  After a successful completion the telescope resumes normal tracking. 

**telescope_timed_slew**()  works like **telescope_slew**() but returns immediately if it does not complete within *timeout* seconds.  The function returns an error if the timeout expires. If *timeout* is negative, the behavior is identical to telescope_slew().

**telescope_try_slew**() attempts to start a slew; if the telescope is already moving or slewing the call fails immediately with **AAOS_EBUSY**. 

## Parameters
*\_self*
:   Pointer to the telescope instance.

*ra*
:   Right ascension of the target in degrees, 0 ≤ ra < 360. In practice the usable range may be narrower because each instrument has hardware and/or software limits for safety.

*dec*
:   Declination of the target in degrees, –90 ≤ dec ≤ +90. In practice the usable range may be narrower because each instrument has hardware and/or software limits for safety.

*timeout*
:   Expired time in seconds. If timeout less than zero, the behavior of **telescope_timed_slew**() is the same as **telescope_slew**().

# RETURN VALUE

On success, *telescope_slew*() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ECANCELED

The operation was cancelled because another function call of *telescope_go_home*(), *telescope_move*(), *telescope_park*(), *telescope_slew*(), or *telescope_stop*() by another thread on the same physical telescope was invoked for the same underlying telescope. The state of the telescope depends on how the slew operation was canceled.  

## AAOS\_EDEVMAL

The underlying telescope is in a *malfunction* state.

## AAOS\_EINVAL

The supplied *ra* and/or *dec* values are outside the defined range or exceed the telescope’s safety limits.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_ETIMEDOUT

The execution time of these functions exceeds the maximum telescope slew time (i.e., the duration of traversing longest slew path). 

## AAOS\_EUNINT

The underlying telescope has not been initialized (e.g. its clock time or location has not been set via *telescope_init*()).

*telescope_timed_slew*() function shall also fail if:

## AAOS\_ETIMEDOUT

The slew does not finish before *timeout* seconds elapse.

*telescope_try_slew*() function shall also fail if:

## AAOS\_EBUSY

The telescope is already moving or slewing.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

*telescope_slew*(), *telescope_slew*(), and *telescope_timed_slew*() are all thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_get_slew_speed**(3), **telescope_init**(3), **telescope_move**(3), **telescope_park**(3), **telescope_power_off**(3), **telescope_power_on**(3), **telescope_set_slew_speed**(3), **telescope_status**(3), **telescope_stop**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

