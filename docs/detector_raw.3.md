% detector\_raw(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_raw - execute raw command on a detector

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_raw**(void *\*\_self*, const void *\*command*,  
$~~~~~~~~~~$size\_t *command_size*, void *\*results*,  
$~~~~~~~~~~$size\_t *results_size*, size\_t *\*return_size*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

**detector_raw**() function sends a low‑level (raw) command directly to the detector referenced by *\_self*.

The call does not modify the detector’s state (e.g. *reading* or *exposing*, etc.). It is intended for expert use, testing, or for implementing higher‑level library functions that need direct access to the underlying protocol.


## Parameters

*\_self*
:   Pointer to the detector.

*command*
:   Pointer to a buffer that contains the raw command to be sent.

*command\_size*
:   Length of the *command* in bytes, including the trailing '\0'. 

*results*
:   Destination buffer that receives the response from the detector. Whether *results* is binary or pure text string is unspecified.    

*results\_size*
:   Size of the *results* in bytes.

*return\_size*
:    If non‑NULL, receives the number of bytes actually written into *results*. If the response is larger than *results\_size*, only the first *results\_size* bytes are copied; the excess is silently truncated.

# RETURN VALUE

On success, **detector_raw**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

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

**detector_raw**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# RATIONALE

Different detector vendors provide a variety of interfaces for operating their detectors, such as proprietary protocols over Ethernet, or through their closed-source SDKs. The AAOS standard supports common detector operations, including expose, abort, and stop. For hardware-specific operations, the standard provides a mechanism to send low-level, vendor-specific instructions or to invoke the vendor’s SDK by calling the **detector_raw**() function. Other AAOS standard library functions, such as **detector_expose**(), may be implemented using **detector_raw**() for certain detectors.

# NOTES

**detector_raw**() does **not** change the detector’s state. Consequently, it can leave the detector in an inconsistent state if used incorrectly. This function should be used only by experts for low‑level testing or when implementing other library functions that need direct protocol access.

# SEE ALSO

**detector**(1), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
