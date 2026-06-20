% detector\_set\_pixel\_format(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_binning - set the detector’s binning factors

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_pixel_format**(void *\*\_self*, uint32\_t *pixel\_format*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_pixel_format**() function sets the pixel format of the detector referenced by *\_self*

Only format values that are supported by the underlying detector may be used. If an unsupported value is supplied, the call fails with **AAOS_EINVAL**.  The supported values are constants defined in **<detector_def.h>**: 

* **DETECTOR_PIXEL_FORMAT_MONO_8**
* **DETECTOR_PIXEL_FORMAT_MONO_10**
* **DETECTOR_PIXEL_FORMAT_MONO_10_PACKED** 
* **DETECTOR_PIXEL_FORMAT_MONO_12** 
* **DETECTOR_PIXEL_FORMAT_MONO_12_PACKED** 
* **DETECTOR_PIXEL_FORMAT_MONO_14**
* **DETECTOR_PIXEL_FORMAT_MONO_14_PACKED**
* **DETECTOR_PIXEL_FORMAT_MONO_16** 
* **DETECTOR_PIXEL_FORMAT_MONO_18** 
* **DETECTOR_PIXEL_FORMAT_MONO_18_PACKED**
* **DETECTOR_PIXEL_FORMAT_MONO_24**
* **DETECTOR_PIXEL_FORMAT_MONO_24_PACKED**
* **DETECTOR_PIXEL_FORMAT_MONO_32**
* **DETECTOR_PIXEL_FORMAT_MONO_64**
* **DETECTOR_PIXEL_FORMAT_RGB_24** 
* **DETECTOR_PIXEL_FORMAT_YUV422**

A particular detector may support only a subset of them.

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to set pixel format despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameters

*\_self*
:   Pointer to the detector instance whose binning factor is to be configured.

*pixel\_format*
:   Desired pixel format (one of the constants listed above).

# RETURN VALUE

On success, **detector_set_pixel_format**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a setting request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).

# AAOS\_EINVAL

The supplied *pixel\_format* value is invalid (not supported by the underlying detector).  

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_set_binning**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_binning**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
