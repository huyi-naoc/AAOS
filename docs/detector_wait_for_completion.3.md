% detector\_wait\_for\_completion(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_wait\_for\_completion - wait for the final frame of an exposure to be saved

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

*typedef void (\*image\_callback_t)(void \*, const char \*, va\_list \*)*;

int   
**detector_wait_for_completion**(void *\*\_self*, image\_callback\_t *image\_callback*, *...*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_wait_for_completion**() function wait for the last image of a previously started exposure has been grabbed and save to a FITS file. 

* If the underlying detector is currently **not** in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING**, the function returns immediately without doing anything. 
* Otherwise it waits for the final frame to be saved, then sets the detector state to **DETECTOR_STATE_IDLE**. 

Callback handling:

* If *image\_callback* is **NULL**, a built‑in default callback prints the name of the generated FITS file to standard output.
* If a non‑**NULL** *image\_callback* is supplied, that function is invoked instead. The callback receives three arguments:
    1. the detector instance *\_self*;
    1. the name of the FITS file that has just been created;
    1. a *va_list \** containing any additional arguments supplied to **detector_wait_for_completion**().

The function’s behaviour with respect to the detector’s capabilities:

* If the underlying detector supports it and **DETECTOR_OPTION_NOTIFY_LAST_FILLING** bit is set, the call blocks until the final frame has been saved (or until the previous exposure is aborted). 
* If the detector does not support this notification, the function returns **AAOS\_ENOTSUP** immediately; otherwise it returns **AAOS\_OK** (zero).

When the detector is in **DETECTOR_STATE_MALFUNCTION**, the return behaviour depends on the **DETECTOR_OPTION_IGNORE_DEVMAL** option:

* **with** the option (the default) set, the function returns **AAOS_EDEVMAL** immediately;
* **without** the option, the function attempts to wait for completion despite the malfunction state.

## Parameter

*\_self*
:   Pointer to the detector instance on which the wait is performed.

*image_callback*
:    User‑supplied function of type *image_callback_t* that is called after the final image file is written. If NULL, the file name is printed to stdout.

... (ellipsis) 
:   Optional arguments that may be interpreted by the detector implementation to modify exposure settings (e.g., gain, binning, readout mode). It also can be used to supply additional information that is to be written into the image file's FITS header. The exact meaning is detector‑specific.

# RETURN VALUE

On success, **detector_wait_for_completion**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_ECANCELED

The exposure is aborted before all the requested frames are completed.

## AAOS\_EDEVMAL

The underlying detector is in **DETECTOR_STATE_MALFUNCTION** (returned immediately if **DETECTOR_OPTION_IGNORE_DEVMAL** is set).
 
## AAOS\_ENOTSUP

The operation is not supported.

## AAOS\_EPWROFF

The underlying detector is not powered.

## AAOS\_EUNINT

The underlying detector is uninitialized.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_wait_for_completion**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# RATIONALE

Large‑format CCD detectors often have very slow readout speeds. To reduce the overhead time and improve overall telescope performance, it is desirable to slew the telescope to the next target while the previous frame is still being read out. Therefore a mechanism to notify the client when the final frame has already been written is required. **detector_wait_for_completion**() provides exactly this functionality.

# NOTE

Multiple threads can wait for the final frame completion on the same underlying physical detector. However, as such, the callback function may do certain writing operations on the same image file. Hence, it will introduce race condition. Therefore, calling multiple times on this function simultaneously is discouraged. 

# SEE ALSO

**detector**(1), **detector_abort**(3), **detector_expose**(3), **detector_set_directory**(3), **detector_set_exposure_time**(3), **detector_set_option**(3),**detector_set_prefix**(3), **detector_set_template**(3), **detector_stop**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.