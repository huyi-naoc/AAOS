% detector\_get\_option(3) | Library Functions Manual
%
% May 2022

# NAME

detector\_get\_option - retrieve a detector option value

# SYNOPSIS

**#include <detector_rpc.h>**  
**#include <detector_def.h>**

int  
**detector_get_option**(void *\*\_self*, uint16\_t *\*option*);

Compile and link with *-laaoscore* *-laaosdriver*.

# DESCRIPTION

The **detector_get_option**() function obtains the current value of the option field associated with the detector referenced by *\_self*. The value is written to the location pointed to by *option*. The *option* field is typically a 16‑bit bitmask that controls various detector features; the exact meaning of each bit is defined by the detector implementation (see **detector**(7)). 

One of the fllowing values can be in *option* for controlling output format of **detector_status**() and **detector_status**():
* DETECTOR_OPTION_STRING_FORMART_PLAIN        
* DETECTOR_OPTION_STRING_FORMART_JSON         
* DETECTOR_OPTION_STRING_FORMART_LIBCONFIG    
* DETECTOR_OPTION_STRING_FORMART_XML          
* DETECTOR_OPTION_STRING_FORMART_YAML 

And any combination of the following values can be or'ed in *option*:

* DETECTOR_OPTION_IGNORE_DEVMAL           
* DETECTOR_OPTION_NOWAIT                                  
* DETECTOR_OPTION_NOTIFY_LAST_FILLING         
* DETECTOR_OPTION_NOTIFY_EACH_COMPLETION


# RETURN VALUE

Always return **AAOS\_OK** (zero).

# ERRORS

**detector_get_option**() shall never faill; therefore no error codes are defined for this call.

# CONFORMING TO

AAOS-draft-2022

# EXAMPLES

None.

# THREAD-SAFETY

**detector_get_option**() is thread‑safe provided that each thread uses its own *detector* object (*\_self*).  If the same *\_self* pointer is shared among threads, the caller must provide appropriate synchronization; otherwise the behaviour is **undefined**.  The `detectord` daemon permits multiple threads (and even processes on different hosts) to operate the same physical detector using distinct `detector` objects concurrently.

# SEE ALSO

**detector**(1), **detector_clear_option**(3), **detector_set_option**(3), **detector**(7)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.
