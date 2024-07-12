% def(3) | System Wide Definition
%
% Jun 2024

# NAME


def - system wide definition

# SYNOPSIS


**#include <def.h>**  

# DESCRIPTION


Generally,  system calls or library functions (hereafter function) on AAOS return an integer to indicate whether the function call is successful or failed. The *<def.h>* header file defines the symbolic names for each of the possible return value of any system calls and library functions (hereafter function) in AAOS.


## List of return value names

Below are tables of the symbolic names of return values defined on AAOS. The former table lists those names that have identical names to the POSIX error name (with a prefix of AAOS\_) and have similar meanings. The latter table lists names defined on AAOS.

Return values related to POSIX:

------------------------    --------------------
**AAOS\_OK**                Success.
$~~~$                       $~~~$
**AAOS\_EACCESS**           Permission denied.
$~~~$                       $~~~$
**AAOS\_EAGAIN**            Resource temporarily unavailable (e.g., nonblocking I/O).
$~~~$                       $~~~$
**AAOS\_EBADF**             Bad file descriptor.
$~~~$                       $~~~$
**AAOS\_EBADMSG**           Bad message.
$~~~$                       $~~~$
**AAOS\_EBUSY**             Device or resource busy.
$~~~$                       $~~~$
**AAOS\_ECONNREFUSED**      Connection refused (e.g., the host is alive but the server daemon has not been started).
$~~~$                       $~~~$
**AAOS\_ECONNRESET**        Connection reset.
$~~~$                       $~~~$
**AAOS\_EEXIST**            File exists.
$~~~$                       $~~~$
**AAOS\_EHOSTUNREACH**      Host is unreachable.
$~~~$                       $~~~$
**AAOS\_EIO**               I/O error (e.g., failure on storage media).
$~~~$                       $~~~$
**AAOS\_EINVAL**            Invalid argument (e.g., input value is out of range).
$~~~$                       $~~~$
**AAOS\_EINTR**             Interrupted function call.
$~~~$                       $~~~$
**AAOS\_ENETDOWN**          Network is down.
$~~~$                       $~~~$
**AAOS\_ENETUNREACH**       Network unreachable.
$~~~$                       $~~~$
**AAOS\_ENOENT**            No such resource, including device, file, directory, etc.
$~~~$                       $~~~$
**AAOS\_ENOMEM**            Not enough memory.
$~~~$                       $~~~$
**AAOS\_ENOSPC**            No space left on device.
$~~~$                       $~~~$
**AAOS\_ENOTDIR**           Not a directory.
$~~~$                       $~~~$
**AAOS\_ENOTSUP**           Operation not supported.
$~~~$                       $~~~$
**AAOS\_EPERM**             Operation not permitted.
$~~~$                       $~~~$
**AAOS\_EPIPE**             Broken pipe (e.g., TCP server endpoint read/write a connected socket, but the client has been aborted earlier).
$~~~$                       $~~~$
**AAOS\_EPROTOWRONG**       Wrong protocol.
$~~~$                       $~~~$
**AAOS\_ESRCH**             No such process.
$~~~$                       $~~~$
**AAOS\_ETIMEDOUT**         Operation timed out. 
------------------------    --------------------

Return values defined by AAOS:

------------------------    --------------------
**AAOS\_ECLOSED**           Connection is closed by the peer endpoint.
$~~~$                       $~~~$
**AAOS\_EALREADY**          Daemon process already started.
$~~~$                       $~~~$
**AAOS\_EBADCMD**           Bad raw command.
$~~~$                       $~~~$
**AAOS\_ECMDNOSUPPORT**     Raw command not suported.
$~~~$                       $~~~$
**AAOS\_ECMDTOOLONG**       Raw command too long.
$~~~$                       $~~~$
**AAOS\_ECANCELED**         Operation canceled.
$~~~$                       $~~~$
**AAOS\_EDEVNOTLOADED**     Device not loaded.
$~~~$                       $~~~$
**AAOS\_EDEVMAL**           Device malfunctioned.
$~~~$                       $~~~$
**AAOS\_EEMPTY**            Empty raw command.
$~~~$                       $~~~$
**AAOS\_EFAILED**           Command failed.
$~~~$                       $~~~$
**AAOS\_ENOTFOUND**         Device not found.
$~~~$                       $~~~$
**AAOS\_EMOREPACK**         More data to be transfer.
$~~~$                       $~~~$
**AAOS\_EPWROFF**           Device powered off.
$~~~$                       $~~~$
**AAOS\_EUNINIT**           Device not initialized.
$~~~$                       $~~~$
**AAOS\_ERROR**             Other unknown or uncategorized error.
------------------------    --------------------

# SEE ALSO

**error**(3)

# CONFORMING TO

AAOS-draft-2022

