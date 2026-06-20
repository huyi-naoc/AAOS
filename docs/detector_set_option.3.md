% detector\_set\_option(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_set\_option - set runtime options for the detector

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_option**(void *\*\_self*, uint16\_t *option*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION


The **detector_set_option**() function sets the runtime options for the detector referenced by *\*\_self*.  The option parameter must be one of the **DETECTOR_OPTION_\*** constants defined in **<detector_def.h>**. Options affect the behaviour of subsequent detector operations such as **detector_expose**(), **detector_status**(), and **detector_info**().

One of the following options can be set for controlling output format for **detector_status**() and **detector_info**():

**DETECTOR_OPTION_STRING_FORMART_PLAIN**
:   Select plain‑text output for **detector_status**() and **detector_info**().

**DETECTOR_OPTION_STRING_FORMART_JSON**
:   Select JSON output for **detector_status**() and **detector_info**(). JSON is the default format and is currently the only format supported by the library.

**DETECTOR_OPTION_STRING_FORMART_LIBCONFIG**
:   Select *libconfig*‑compatible output for **detector_status**() and **detector_info**().

**DETECTOR_OPTION_STRING_FORMART_XML**
:   Select *XML*‑compatible output for **detector_status**() and **detector_info**().

**DETECTOR_OPTION_STRING_FORMART_YAML**
:   Select *YAML*‑compatible output for **detector_status**() and **detector_info**().

And any combination of the following values can be or'ed in *option* for controlling those functions that do exposusre and modify the detector's runtime parameters :

**DETECTOR_OPTION_IGNORE_DEVMAL**
:   When this option is set, functions that would normally return with **AAOS_EDEVMAL** if the detector is in a malfunction state will instead attempt to carry out the operation. By default the option is not set, and the functions return **AAOS_EDEVMAL** immediately.

**DETECTOR_OPTION_NO_WAIT**
:  When set, calls that would block while the detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** return immediately with **AAOS_EBUSY** instead of waiting for the exposure or readout to finish. The default behaviour is to wait.

**DETECTOR_OPTION_NOTIFY_LAST_FILLING**
:   If the underlying hardware supports it and this option is set, **detector_expose**() returns as soon as the last frame begins to be read (i.e., when the detector enters **DETECTOR_STATE_READING**). By default the option is **not** set.

**DETECTOR_OPTION_NOTIFY_EACH_COMPLETION**
:   If the detector supports it, **detector_expose**() creates a separate file for each frame rather than appending each frame to a single FITS file (the original behaviour is to create one file and add subsequent frames as image extensions). After each frame is written the *image_callback* (see **detector_expose**(3)) is invoked. By default the option is not set.

## Parameters

*\_self*
:   Pointer to the detector instance whose option is to be modified.

*option*
:   Option value.
 
# RETURN VALUE

On success, **detector_set_option**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EINVAL

The supplied *option* value is invalid.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_set_option**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector_clear_option**(3), **detector_get_option**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
