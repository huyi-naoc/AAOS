% telescope\_inspect(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_inspect - inspect the telescope

# SYNOPSIS

**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_inspect**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **telescope_inspect**() function inspects on the telescope referenced by *\*\_self*. If the telescope passes the inspection, the **TELESCOPE_STATE_MALFUNCTION** flag in the state is cleared, and all the threads blocked for waiting for the telescope's recovery are resumed.  Otherwise,  the  **TELESCOPE_STATE_MALFUNCTION** flag is set.  

The **telescope_inspect**() function inspects on the telescope referenced by *\*\_self*. If the detctor passes the inspection, the **TELESCOPE_STATE_MALFUNCTION** flag in the telescope's state is cleared, and all the threads that were blocked waiting for the underlying telescope to recover are resumed. If the inspection fails, the  **TELESCOPE_STATE_MALFUNCTION** flag is set.

## Parameters

*\_self*
:   Pointer to the telescope instance to be inspected.

# RETURN VALUE

On success, **telescope_inspect**() returns **0**.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The functions may fail with any of the following error codes:

# AAOS\_ENOTSUP

The underlying telescope does not support this operation.

# AAOS\_EMALDEV

The underlying telescope failed to pass the inspection.


# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_inspect**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# RATIONALE

A failure of other operation functions (e.g. slew, move) does not necessarily indicate a hardware malfunction. A dedicated function that performs a hardware‑specific inspection and decides whether to set or clear the **TELESCOPE_STATE_MALFUNCTION** flag is therefore required. **telescope_inspect**() fulfills this role and also notifies all threads waiting for the clearance of the malfunction flag, allowing them to resume operation as soon as the telescope recovers. Together with **telescope_register**(), it lets an automatic operation pipeline continue without needing expert‑level hardware knowledge. 

# SEE ALSO

**telescope**(1), **telescope_register**(3), **telescope**(7)

# BUGS


Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.