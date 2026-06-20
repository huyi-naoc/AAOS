% detector\_get\_image(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_image - download an image file from the detector’s image directory

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_image**(void *\*\_self*, const char *\*filename*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_image**() function copies the image file named *filename* from the detector’s image directory to the caller’s current working directory.

If the client and the detectord daemon run on the same host (e.g., the client connects to the daemon via 127.0.0.1, localhost, or a UNIX‑domain socket), the daemon simply copies the file locally. Otherwise, If the client is on a different host, the daemon transmits the file over the network using the sendfile() system call.

## Parameters

*\_self*
:   Pointer to the detector instance whose image directory is being accessed.

*fiename*
:   Name of the image file to be retrieved. The name is interpreted relative to the underlying detector’s image directory and must be NUL‑terminated.

# RETURN VALUE

On success, **detector_get_gain**() returns `0`.  On failure, a non‑zero error code is returned.  The error codes are listed in the **ERRORS** section.

# ERRORS

The function may fail with any of the following error codes:

# AAOS\_EEXIST

The specified *filename* does not exist in the detector's directory.

# AAOS\_ENOTSUP

The underlying detector does not support this operation.

# AAOS\_EPERM

The caller does not have write permission in the current working directory.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None. 

# THREAD-SAFETY

**detector_get_image**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# NOTE

This function is primarily intended for integration with a data‑pipeline system. Frequent manual use for downloading images is discouraged because it can consume a large portion of the Ethernet bandwidth.

# SEE ALSO

**sendfile**(2), **detector**(1), **detector_get_directory**(3), **detector_list_image**(3), **detector_set_directory**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.