% detector\_delete\_image(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_delete\_image - delete a single image file from the directory in which detector images are stored

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_delete_image**(void *\*\_self*, const char *\*filename*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_delete_image**() function remove the image file named *filename* from the directory associated with the underlying detector referenced by *\_self*.

## Parameters

*\_self*
:   Pointer to the detector instance whose image file is to be deleted.

*filename*
:   Name of the image file to be deleted. The name is interpreted relative to the detector’s image directory.


# RETURN VALUE

On success, **detector_delete_image**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

## AAOS\_EEXIST

The *filename* does not exist in the directory.

## AAOS\_ENOTSUP

The underlying detector does not support this operation.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFE

**detector_delete_image**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_delete_all_image**(3), **detector_get_directory**(3), **detector_set_directory**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
