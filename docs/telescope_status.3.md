% telescope\_status(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_status - retrieve the telescope status

# SYNOPSIS

**#include <telescope_rpc.h>**  

int  
**telescope_status**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

**telescope_status()** function retrieves the current status of the telescope referenced by *\_self*.  The caller supplies a destination buffer (*res*) and its capacity in bytes (*res\_size*). On success the function copies the information into the buffer. If *res\_len* is not *NULL*, the total length of the information (in bytes) is stored at *res\_len*.  When the actual data size exceeds *res\_size*, only the first *res\_size* bytes are written; *res\_len* (when reported) still contains the full length of the data.

The status must contain at least the telescope’s current position and state. If **telescope_get_detector**(), **telescope_get_filter**(), **telescope_get_instrument**(), **telescope_get_move_speed**(), **telescope_get_slew_speed**(), or **telescope_get_track_rate**() succeed (i.e., they do not return **AAOS_ENOTSUP**), their values should also be included in the status data.

## Parameters

*\_self*
:   Pointer to the telescope instance whose status is to be retrieved.

*res*
:   Destination buffer that receives the information.

*res\_size*
:   Size of *res* in bytes.

*res\_len*
:   If non‑*NULL*, receives the total length of the information. May be *NULL* if the length is not needed.

# RETURN VALUE

On success, *telescope_status*() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS**
section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in *malfunction* state.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_status**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# SEE ALSO

**telescope**(1), **telescope_info**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

