% detector\_status(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_status - retrieve the detector status

# SYNOPSIS

**#include <detector_rpc.h>**  

int  
**detector_status**(void *\*\_self*, char *\*res*, size\_t *res_size*, size\_t *\*res_len*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_status**() obtains the current status of the detector referenced by *\_self*. The caller supplies a buffer (res) and its capacity (res_size). On success the function copies the status information into *res*. If *res\_len* is not **NULL**, the total length of the status data (in bytes) is stored at that address, even when the buffer is too small to hold the entire result.

When the data to be written exceeds *res\_size*, only the first *res\_size* bytes are copied; *res\_len* (when requested) still contains the full length of the information.

The status always includes at least the detector’s current state. If any of the following accessor functions succeed (i.e., they do not return **AAOS_ENOTSUP**), their values are also incorporated into the status string:

* **detector_get_binning**()
* **detector_get_directory**() 
* **detector_get_exposure_time**
* **detector_get_frame_rate**()
* **detector_get_gain**()
* **detectory_get_prefix**()
* **detector_get_readout_rate**()
* **detector_get_region**()
* **detector_get_temperature**()
* **detector_get_template**()

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to retrieve status despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

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

On success, **detector_status**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_status**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_binning**(3), **detector_get_directory**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detector_get_gain**(3), **detectory_get_prefix**(3), **detector_get_readout_rate**(3), **detector_get_region**(3), **detector_get_temperature**(3), **detector_status**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
