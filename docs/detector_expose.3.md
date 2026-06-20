% detector\_expose(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_expose - start an exposure sequence on a detector

# SYNOPSIS

**#include <stdarg.h>**

**#include <detector_rpc.h>**  
**#include <detector_def.h>**
<!--**typedef void (\*image_callback_t)(void \*, const char \*, va_list \*);**-->
*typedef void (\*image\_callback_t)(void \*, const char \*, va\_list \*)*;

int   
**detector_expose**(void *\*\_self*, double *exposure\_time*, uint32\_t *n\_frames*,
$~~~~~~~~~~~~~$image\_callback\_t *image\_callback*, *...*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_expose**() function initiates an exposure sequence on the detector referenced by *\_self*.  The detector’s state progresses as follows:

* **DETECTOR_STATE_EXPOSING** – the exposure(s) are being taken,
* **DETECTOR_STATE_READING** – each frame is read out (if the underlying hardware supports it),
* **DETECTOR_STATE_IDLE** – the sequence has completed.

Each acquired frame is written to a FITS file. The file name follows the pattern:

```
prefix_YYYYmmdd_HHMMSS_nn.fits
```

where the prefix, the output directory, and the FITS header template are set with **detector_set_prefix**(), **detector_set_directory**(), and **detector_set_template**() respectively. Whether the frames are stored in a single FITS file or in separate files depends on the **DETECTOR_OPTION_NOTIFY_EACH_COMPLETION** bit in the detector’s option set (see **detector_set_option**(3)).

Callback handling:

* If *image\_callback* is **NULL**, a built‑in default callback prints the name of the generated FITS file to standard output.
* If a non‑**NULL** *image\_callback* is supplied, that function is invoked instead. The callback receives three arguments:
    1. the detector instance *\_self*;
    1. the name of the FITS file that has just been created;
    1. a *va_list \** containing any additional arguments supplied to **detector_expose**().

The call blocks until all requested frames have been saved (or until the exposure is aborted or stopped). If the detector supports it and the **DETECTOR_OPTION_NOTIFY_LAST_FILLING** option is set, the function returns as soon as the last frame is ready to be read, leaving the detector in the **DETECTOR_STATE_READING** state; in that case the caller should invoke **detector_wait_for_completion**() to wait for the final image acquisition to finish.

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before starting a new exposure.

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option, the function attempts to start the exposure sequence despite the malfunction state;
* **without** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately.

## Parameter

*\_self*
:   Pointer to the detector instance on which the exposure will be performed.

*exposure\_time*
:   Exposure time for each frame, in second.

*n\_frames*
:   Number of frames to be acquired.

*image\_callback*
:    User‑supplied function of type *image_callback_t* that is called after each image file is written. If NULL, the file name is printed to stdout.

... (ellipsis) 
:   Optional arguments that may be interpreted by the detector implementation to modify exposure settings (e.g., gain, binning, readout mode). It also can be used to supply additional information that is to be written into the image file's FITS header. The exact meaning is detector‑specific. 

# RETURN VALUE

On success, **detector_expose**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a new exposure request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_ECANCELED

The exposure is stopped or aborted before all the requested frames are completed.

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is not set).

## AAOS\_EINVAL

The value of  *exposure\_time*  is invalid, e.g., out of allowed range. 

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**detector_expose**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_abort**(3), **detector_get_option**(3), **detector_set_directory**(3), **detector_set_exposure_time**(3), **detector_set_option**(3),**detector_set_prefix**(3), **detector_set_template**(3), **detector_set_trigger_mode**(3), **detector_stop**(3), **detector_wait_for_completion**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
