% detector\_clear\_option(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_clear\_option - clear the detector's option

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_clear_option**(void *\*\_self*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_clear_option**() function clear the option of the detector referenced by *\*\_self*. 

Clear option will set to the default options. It will disable **DETECTOR_OPTION_IGNORE_DEVMAL**, **DETECTOR_OPTION_NOWAIT**, and **DETECTOR_NOTIFY_LAST_FILLING**, but enable **DETECTOR_OPTION_NOTIFY_EACH_COMPLETION**. It will also set the output format for **detector_status**() amd **detector_info**() to **DETECTOR_OPTION_FORMAT_JSON**.

*\_self*
:   Pointer to the detector instance whose option is to be clear.

# RETURN VALUE

On success, **detector_enable_cooling**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_ENOTSUP

The underline detector does not support this operation.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_binning**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector_get_option**(3), **detector_set_option**(3)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

