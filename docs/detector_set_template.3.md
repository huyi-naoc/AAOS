% detector\_set\_prefix(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_set\_template - set the FITS header template used for detector image files

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_set_template**(void *\*\_self*, const char *\*template*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_set_template**() function specifies the location of the FITS‑header template file that the detector uses when writing image files. The parameter *template* may be an absolute or a relative path. If a relative path is given, the `detectord` daemon searches for the directory in the following order:

1. the current working directory of the `detectord` daemon;
1. a path relative to the current working directory of the `detectord` daemon;
1. /opt/aaos/share/template;
1. /usr/local/aaos/share/template;
1. /usr/local/share/template/aaos;
1. /usr/local/share/template;
1. /usr/share/template/aaos;
1. /usr/share/template.

*template* must be NUL‑terminated and refer to an existing file; otherwise the call fails with **AAOS_ENOENT**.

If the detector is already exposing (e.g., in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** state), the caller's behaviour depends on the **DETECTOR_OPTION_NOWAIT** option:

* **with** the option set, the function returns **AAOS_EBUSY** immediately;
* **without** the option (the default) the call blocks until the current exposure operation completes before setting this parameter.

## Parameters

*\_self*
:   Pointer to the detector instance whose template file path is to be set.

*template*
:   Pointer to a nul‑terminated string that holds the template file's pathname. Passing NULL or an empty string is considered invalid and will result in an error.

# RETURN VALUE

On success, **detector_set_template**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EBUSY

The detector is in **DETECTOR_STATE_EXPOSING** or **DETECTOR_STATE_READING** and cannot accept a setting request at this time (the **DETECTOR_OPTION_NOWAIT** option is set).

## AAOS\_ENOENT

The template file does not exist.

## AAOS\_ENOMEM

Cannot allocate memory to hold the **template**.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY
**detector_set_template**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_get_template**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
