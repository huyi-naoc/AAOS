% telescope\_raw(3) | Library Functions Manual
%
% May 2022

# NAME

telescope\_raw - execute raw command on a telescope

# SYNOPSIS


**#include <telescope_rpc.h>**  
**#include <telescope_def.h>**

int  
**telescope_raw**(void *\*\_self*, const void *\*command*,  
$~~~~~~~~~~~~~~$size\_t *command_size*, void *\*results*,  
$~~~~~~~~~~~~~~$size\_t *results_size*, size\_t *\*return_size*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

**telescope_raw**() function sends a low‑level (raw) command directly to the telescope referenced by *\_self*.

The call does not modify the telescope’s state (e.g. *slewing* or *tracking*, etc.). It is intended for expert use, testing, or for implementing higher‑level library functions that need direct access to the underlying protocol.


## Parameters

*\_self*
:   Pointer to the telescope instance whose status is to be retrieved.

*command*
:   Pointer to a buffer that contains the raw command to be sent.

*command\_size*
:   Length of the *command* in bytes, including the trailing '\0'. 

*results*
:   Destination buffer that receives the response from the telescope. Whether *results* is binary or pure text string is unspecified.    

*results\_size*
:   Size of the *results* in bytes.

*return\_size*
:    If non‑NULL, receives the number of bytes actually written into *results*. If the response is larger than *results\_size*, only the first *results\_size* bytes are copied; the excess is silently truncated.

# RETURN VALUE

On success, **telescope_stop**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with the following error codes:

## AAOS\_EDEVMAL

The underlying telescope is in a *MALFUNCTION* state.

## AAOS\_ENOTSUP

The underlying telescope does not support raw command execution.

## AAOS\_EPWROFF

The underlying telescope is not powered.

## AAOS\_EUNINT

The underlying telescope is uninitialized, e.g., clock time and/or location have not been set yet by **telescope_init**().

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**telescope_raw**() is thread‑safe provided that each thread uses its own *telescope* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `telescoped` daemon permits multiple threads (and even processes on different hosts) to operate the same physical telescope using distinct `telescope` objects concurrently.

# NOTES

**telescope_raw**() does not change the telescope’s operational state. Using it improperly can leave the telescope in an inconsistent state, so it should be restricted to experts performing low‑level testing or impelemting other library functions that knows how to interpret the raw protocol. 
 
# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

