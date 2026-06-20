% telescope\_move(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_move - move the telescope

# SYNOPSIS


**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_move**(void *\*\_self*, unsined int *direction*, 
$~~~~~~~~~~~~~~~$double *duration*);  
int  
**telescope_timed_move**(void *\*\_self*, unsined int *direction*,  
$~~~~~~~~~~~~~~~~~~~~~$double *duration*, double *timeout*);  
int  
**telescope_try_slew**(void *\*\_self*, unsined int *direction*, 
$~~~~~~~~~~~~~~~~~~~$double *duration*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The three functions **telescope_move**(), **telescope_timed_move**() and **telescope_try_move**() command one of axis of the telescope referenced by *\*\_self* move for a  specified *direction* in a given *duration*. They differ in blocking behaviour:

**telescope_move**() blocks until the requested move finishes (or fails). **telescope_timed_move**() blocks until the move finishes or the timeout expires, whichever occurs first. If *timeout* is negative, the behavior is identical to **telescope_move**(). **telescope_try_move**() attempts to start a move; if the telescope is already moving or slewing the call fails immediately with **AAOS_EBUSY**.

The functions will block the calling thread until it returns.  The moving speed can be retrieved and set by **telescope_get_move_speed**() and **telescope_set_move_speed**(). These functions are primarily intended for pulse‑guiding operations. 

## Parameters

*\_self*
:   Pointer to the telescope instance to be moved.

*direction* 
:   Move direction. It must be one of the constants defined in **telescope_def.h**: **TELESCOPE_MOVE_EAST**, **TELESCOPE_MOVE_WEST**, **TELESCOPE_MOVE_NORTH**, **TELESCOPE_MOVE_NORTH**. The exact meaning of each constant depends on the underlying mount. For an equatorial mount, **TELESCOPE_MOVE_EAST** and **TELESCOPE_MOVE_WEST** refer to the Right‑Ascension axis, while **TELESCOPE_MOVE_NORTH** and **TELESCOPE_MOVE_SOUTH** refer to the Declination axis. 

*timeout*
:   Expired time in seconds. If timeout less than zero, the behavior of **telescope_timed_move**() is the same as **telescope_move**().


# RETURN VALUE

On success, **telescope_move**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ECANCELED

The current execution is cancelled by a new call of *telescope_go_home*(), *telescope_move*(), *telescope_park*(), *telescope_slew*(), and *telescope_stop*() that operate the same underline telescope. 

## AAOS\_EDEVMAL

The underlying telescope is in a *MALFUNCTION* state.

## AAOS\_EINVAL

 The supplied *direction* is not a defined constant, or duration (or timeout for **telescope_timed_move**()) is outside the allowed range.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

**telescope_timed_move**() function shall also fail if:

## AAOS\_ETIMEDOUT

The telescope is still moving or slewing until *timeout*.

**telescope_try_move**() function shall also fail if:

## AAOS\_EBUSY

The underlying telescope is already moving or slewing.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_move**(), **telescope_try_move**(), and **telescope_timed_move**() are all thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_get_move_speed**(3), **telescope_init**(3), **telescope_park**(3), **telescope_power_off**(3), **telescope_power_on**(3), **telescope_get_move_speed**(3), **telescope_slew**(3), **telescope_stop**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
