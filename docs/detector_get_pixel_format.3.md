% detector\_get\_pixel\_format(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_pixel\_format - retrieve the detector's pixel format

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_pixel_format**(void *\*\_self*, uint32\_t *\*pixel\_format*);
 

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_pixel_format**() function retrieves the current pixel format of the detector referenced by *\*\_self*. 

The **detector_get_pixel_format**() function obtains the current pixel format of the underlying detector referenced by referenced by *\*\_self*. The pixel format is written to the location pointed to by *pixel\_format*. The value stored in *pixel\_format* will be one of the following constants defined in **<detector_def>**: 

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

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to get pixel format despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.


## Parameter

*\_self*
:   Pointer to the detector instance whose pixel format is being queried.

*pixel\_format*
:   Pointer to a uint32\_t variable that will receive the pixel format. The pointer must be valid (non‑NULL). 

# RETURN VALUE

On success, **detector_get_pixel_format**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

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

**detector_get_pixel_format**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_set_pixel_format**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
