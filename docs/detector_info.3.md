% detector\_info(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_info -  retrieve static information about a detector

# SYNOPSIS

**#include <detector_rpc.h>**  

int  
**detector_info**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_info()** function obtains the static information of the detector referenced by *\_self*. The caller supplies a destination buffer (*res*) and its capacity in bytes (*res\_size*). On success the function copies the information into the buffer. If *res\_len* is not *NULL*, the total length of the information (in bytes) is stored at *res\_len*.  When the actual data size exceeds *res\_size*, only the first *res\_size* bytes are written; *res\_len* (when reported) still contains the full length of the data.

Typical contents include the vendor name, model, serial number and other immutable identifiers of the detector.  Although this data are essentially constant for a given device, they are often obtained directly from the hardware rather than from configuration files. Consequently the call may fail with any of the error codes listed in the **ERRORS** section. The format of the returned data can be controlled with **detector_set_option**(). 

## Parameters

*\_self*
:   Pointer to the detector instance whose status is to be retrieved.

*res*
:   Destination buffer that receives the information.

*res\_size*
:   Size of *res* in bytes.

*res\_len*
:   If non‑*NULL*, receives the total length of the information. May be *NULL* if the length is not needed.

# RETURN VALUE

On success, **detector_info**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).

## AAOS\_ENOTSUP

The underlying detetcor does not support this operation.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_info**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# RATIONALE

Many detectors do not implement all possible operations (e.g., cooling control, readout‑rate adjustment). The **detector_info**() function provides a generic mechanism to query the static capabilities and identifiers of a specific hardware implementation. If a particular feature is not supported, the corresponding function returns **AAOS_ENOTSUP**. An **AAOS_EINVAL** error indicates that the supplied arguments are invalid or out of range; such information may also be reflected in *res* returned by **detector_info**().

# SEE ALSO

**detector**(1), **detector_get_binning**(3), **detector_get_directory**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detector_get_gain**(3), **detectory_get_prefix**(3), **detector_get_readout_rate**(3), **detector_get_region**(3), **detector_get_temperature**(3), **detctor_set_option**(3), **detector_status**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
