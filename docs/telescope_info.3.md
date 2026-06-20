% telescope\_info(3) | Library Functions Manual
%
% May 2022

# NAME


telescope\_info - retrieve static information from telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  

int  
**telescope_info**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res\_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_info()** function retrieves the static information of the telescope referenced by *\_self*. The caller supplies a destination buffer (*res*) and its capacity in bytes (*res\_size*). On success the function copies the information into the buffer. If *res\_len* is not *NULL*, the total length of the information (in bytes) is stored at *res\_len*.  When the actual data size exceeds *res\_size*, only the first *res\_size* bytes are written; *res\_len* (when reported) still contains the full length of the data.

Although this data are essentially constant for a given device, they are often obtained directly from the hardware rather than from configuration files. Consequently the call may fail with any of the error codes listed in the **ERRORS** section. 

## Parameters

*\_self*
:   Pointer to the telescope instance whose information is to be retrieved.

*res*
:   Destination buffer that receives the information.

*res\_size*
:   Size of *res* in bytes.

*res\_len*
:   If non‑*NULL*, receives the total length of the information. May be *NULL* if the length is not needed.

# RETURN VALUE

**AAOS_OK** is returned on success. On failure an error code is returned; the absolute value corresponds to one of the identifiers described in **ERRORS**. Retruning a negative value indicates network error, while positive value indicate hardware error.

# ERRORS

The function may fail with any of the following error codes:

## AAOS_EDEVMAL
the underlying telescope is in a *malfunction* state.

## AAOS_EPWROFF
the telescope is not powered.

## AAOS_EUINIT
the telescope has not been initialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_info**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# RATIONALE

Many telescopes do not implement all possible operations (e.g., move, track‑rate adjustment). The **telescope_info**() function provides a generic mechanism to query the static capabilities and identifiers of a specific hardware implementation. If a particular feature is not supported, the corresponding function returns **AAOS_ENOTSUP**. An **AAOS_EINVAL** error indicates that the supplied arguments are invalid or out of range; such information may also be reflected in *res* returned by **telescope_info**().

# SEE ALSO

**telescope**(1), **telescope_get_move_speed**(3), **telescope_get_slew_speed**(3), **telescope_get_track_rate**(3), **telescope_status**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.