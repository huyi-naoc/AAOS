% detector\_register(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_register -  wait until the detector recovers (or times out)

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_register**(void *\*\_self*, double *timeout*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_register**() function blocks until the detector referenced by *\_self* has recovered from an error condition or the timeout expires. If *timeout* equals zero, it just returns immediately (no wait). If *timeout* is negative, the call blocks indefinitely until the detector recovers. Otherwise, it specifies the maximum wait time in seconds; the call returns when the detector recovers or when the timeout elapses. The function does not modify any other detector state; it merely waits for the recovery condition to become true.

## Parameters

*\_self*
:   Pointer to the detector instance whose recovery is being awaited.

*timeout*
:   Timeout (seconds).

# RETURN VALUE

On success, **detector_register**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

## AAOS\_ETIMEOUT

Waiting timed out.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_register**() is thread‑safe provided that each thread uses its own detector object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# RATIONALE

Robust automatic pipelines must be able to survive transient detector failures without direct termination. If an operation such as **detector_expose**() returns an error, the pipeline should not abort immediately. Instead it can call **detector_register**() to wait for the detector to recover. While the pipeline is blocked, a human operator can inspect the detector (**detector_inspect**()) and, if necessary, perform repairs. Once the malfunction flag has been cleared, the waiting pipeline resumes automatically. Together with **detector_inspect**(), **detector_register**() raises the level of automation achievable by AAOS applications.

# SEE ALSO

**detector**(1), **detector_inspect**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
