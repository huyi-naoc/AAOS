% detector\_inspect(3) | Library Functions Manual
%
% May 2022

NAME
====

detector\_inspect - inspect the detector

SYNOPSIS
========

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_inspect**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_inspect**() function performs a hardware‑specific inspection of the detector referenced by *\*\_self*. The inspection determines whether the detector is in a functional state and updates the **DETECTOR_STATE_MALFUNCTION** flag accordingly. 

* If the inspection succeeds, the **DETECTOR_STATE_MALFUNCTION** flag is cleared and any threads that were blocked waiting for the detector to recover are resumed.
* If the inspection fails, the flag is set, indicating a hardware malfunction. The function does **not** alter any other detector state; it merely updates the malfunction flag and notifies waiting threads.

## Parameters

*\_self*
:   Pointer to the detector instance to be inspected.

# RETURN VALUE

On success, **detector_inspect**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

# AAOS\_ENOTSUP

The underlying detector does not support this operation.

# AAOS\_EMALDEV

The underlying detector failed to pass the inspection.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_inspect**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# RATIONALE

Ordinary operation functions (e.g., **detector_expose**(), **detector_set_frame_rate**()) return error codes for many reasons that do not imply a hardware malfunction (network hiccups, invalid arguments, temporary resource exhaustion, etc.). Automatically marking the detector as malfunctioning on any error would cause unnecessary downtime.

**detector_inspect**() provides a dedicated, hardware‑specific check that decides whether the **DETECTOR_STATE_MALFUNCTION** flag should be set or cleared. In addition, it wakes all threads waiting for the flag to be cleared, allowing them to resume work as soon as the detector is confirmed healthy. Together with detector_register(), it enables an automatic recovery pipeline that can handle transient hardware issues without manual intervention.

# SEE ALSO

**detector**(1), **detector_register**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
