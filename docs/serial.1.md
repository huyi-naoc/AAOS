% serial(1) | General Commands Manual
%
% May 2022

NAME
====

serial - the command-line client of **seriald** in AAOS 

SYNOPSIS
========

**serial** [-bcdhinprsw] *cmd list* 

DESCRIPTION
===========

**serial** is the command-line client of **seriald** in Automated Astronomical Observatory Operating System (AAOS). It is used for send raw commands to RS232 devices and print the result to stdout.  

OPTIONS
=======

`-h, --help`

:   Show the help message and exit.

`-b, --binary`

:   *cmd* is binary rather than ASCII string.

`-c, --check`

:   Check whether the underline serial device works correctly and exit.

`-d, --delay` *DELAY*

:   Set the timeout of waiting for the serial recovering.

`-i, --index` *INDEX*

:   Set the index of the serial to operate. It will overwrite previous -i, -n or -p option. Default is `1`.

`-n, --name` *NAME*

:   Set the name of the serial to operate. It will overwrite previous -i, -n
 or -p option.

`-n, --path` *PATH*

:   Set the path (e.g., /dev/ttyS0) of the serial to operate. It will overwrite previous -i, -nor -p option.

`-r`  

:   Check whether the underline serial device works correctly. If not, wait until the serial device recoverying or timed out.

`-s` *SERIAL*

:   Set the serial server's address and port in format of *[address]:[port]*. If this option has not present, default localhost:12000 will be applied. 

`-w`
:   Wait wait until the serial device recoverying or timed out when excuting serial raw comman failed.

CONFORMING TO
=============

AAOS-draft-2022

SEE ALSO
========

**serial_get_index_by_name**(3), **serial_get_index_by_path**(3),  **serial_inspect**(3), **serial_raw**(3), **serial_register**(3), **seriald**(8)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

