% telescope\_register(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_register - wait until the recovery of the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_register**(void *\*\_self*, double *timeout*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_register**() function blocks until the telescope referenced by *\_self* has recovered from an error condition or the timeout expires. If *timeout* equals zero, it just returns immediately (no wait). If *timeout* is negative, the call blocks indefinitely until the telescope recovers. Otherwise, it specifies the maximum wait time in seconds; the call returns when the telescope recovers or when the timeout elapses. The function does not modify any other telescope state; it merely waits for the recovery condition to become true.

## Parameters

*\_self*
:   Pointer to the telescope instance whose recovery is being awaited.

*timeout*
:   Timeout (seconds).

# RETURN VALUE

On success, **telescope_register**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

## AAOS\_ENOTSUP

The underlying telescope does not support this operation.

## AAOS\_ETIMEOUT

Waiting timed out.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_register**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# RATIONALE

Robust automatic pipelines must be able to survive transient telescope failures without directly terminating. If an operation such as **telescope_slew**() returns an error, the pipeline should not abort immediately. Instead it can call **telescope_register**() to wait for the telescope to recover. While the pipeline is blocked, a human operator can inspect the telescope (**telescope_inspect**()) and, if necessary, perform repairs. Once the malfunction flag has been cleared, the waiting pipeline resumes automatically. Together with **telescope_inspect**(), **telescope_register**() raises the level of automation achievable by AAOS applications.

# SEE ALSO

**telescope**(1), **telescope_inspect**(3), **telescope**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

