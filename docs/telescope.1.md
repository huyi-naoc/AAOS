% telescope(1) | General Commands Manual
%
% May 2022

NAME
====

telescope - the command-line client of **telescoped** in AAOS 

SYNOPSIS
========

**telescope** [-fhintuv] *command* [parameter list]

DESCRIPTION
===========

**telescope** is the command-line client of **telescoped** in Automated Astronomical Observatory Operating System (AAOS). It is used for operating telescope, get and set runtime parameters of a telescope. The telescope here is referred to a merely equatorial mount, which can point to and track celetial objects.  

It currently supports two types of telescope, the virtual telescope and the Astro-Physics Mount 1600GTO. ASA DDM100 will be also supported soon.  

OPTIONS
=======

`-h, --help`

:   Show the help message and exit

`-f, --format` *FORMAT*

:   Set the input format (`degree`, `string`) for the parameters of *slew* command. Default is `string`. 

`-i, --index` *INDEX*

:   Set the index of the telescope to operate. It will overwrite previous -t or -i option. Default is `1`.

`-n, --name` *NAME*

:   Set the name of the telescope to operate. It will overwrite previous -t
 or -i option.

`-t, --telescope` *TELESCOPE*

:   Set the telescope server's address and port in format of *[address]:[port]*. If this option has not present, default localhost:13000 will be applied. 

`-u, --unit` *UNIT*

:   Set the unit (`nature`, `second`, `minute`, `degree`)  of `set move_speed` or `set slew_speed` command. `nature` means in unit of 15 arcsec per second. Default is `nature`.

COMMANDS
========

get *parameter*
---------------

**Get** *parameter*s of the underline telescope and print it to stdout. Available *parameter*s are list as following: 

**move_speed**

:   get move speed of a single axis.

**slew_speed**

:   get slew speed of two axes, the first axis is RA (equatorial mount) or east-west (horizontal mount) axis, the second axis is DEC (equatorial mount) or altitude (horizontal mount) axis.

**track_rate**

:   set track rate of two axes, the first axis is RA (equatorial mount) or east-west (horizontal mount) axis, the second axis is DEC (equatorial mount) or altitude (horizontal mount) axis.


go_home
-------

Slew the underline telescope to its proper position and park at there. The subsequent operatioins, such as close the dome, will be safe.

init
----

Initialize the underline telescope, such as set up the time, location, and etc.

move  *direction* *duration*
----------------------------

**Move** the single axis of the telescope. *direction* can be `east`, `west`, `north`, `south`, *duration* is the duration of axis moving. After *move* completes, the telescope start to track. If the telescope is moving or slewing, the previous command will be aborted. 

timed_move *direction* *duration* *timeout*
-------------------------------------------

*timed_move* shall be equivalent to *move*, except that it has a third paramter *timeout* and if the telescope is moving or slewing, this command will wait until the previous *move* or *slew* command, or until it times out. 

try_move *direction* *duration*
-------------------------------

*try_move* shall be equivalent to *move*, except that if the telescope is moving or slewing, this command will return immediately and has no effect.

park
----

**Park** the telescope to the current position no matter what the telescope is doing now.

park_off
--------

**Park off** the telescope, and then it starts to track.

power_off
--------

**Power off** the underline telescope. 

power_on
---------

**Power on** the underline telescope.

raw *cmd*
---------

Send raw command to the underline telescope. *cmd* is a literal ASCII string.

slew *ra* *dec*
---------------

**Slew** the telescope to the celetial position at (*ra*, *dec*). The format of the coordinates is specified by **-f, --format** option. After *slew* completes, the telescope starts to track. If the telescope is moving or slewing, the previous command will be aborted. 

timed_slew *ra* *dec* *timeout*
-------------------------------------------

*timed_slew* shall be equivalent to *slew*, except that it has a third paramter *timeout* and if the telescope is moving or slewing, this command will wait until the previous *move* or *slew* command completes, or until it times out. 

try_slew *ra* *dec*
-------------------------------

*try_slew* shall be equivalent to *slew*, except that if the telescope is moving or slewing, this command will return immediately and has no effect.

set *parameter* [*value1* [*value2*, [...]]]
-------------------------------------------

**Set** parameters of the underline telescope. Available *parameter*s are list as following: 

**move_speed**

:   set move speed of a single axis.

**slew_speed**

:   set slew speed of two axes, the first axis is RA (equatorial mount) or east-west (horizontal mount) axis, the second axis is DEC (equatorial mount) or altitude (horizontal mount) axis.

**track_rate**

:   set track rate of two axes, the first axis is RA (equatorial mount) or east-west (horizontal mount) axis, the second axis is DEC (equatorial mount) or altitude (horizontal mount) axis.

status
------

Print the status of the undeline telescope to the stdout. The status contains at least the currrent location of the telescope, the *STATE* of the telescope. 

stop
----

Stop the telescope no matter what it is doing now, and then start track.

CONFORMING TO
=============

AAOS-draft-2022

SEE ALSO
========

**telescope_get_index_by_name**(3), **telescope_get_move_speed**(3), **telescope_get_slew_speed**(3), **telescope_get_track_rate**(3), **telescope_go_home**(3), **telescope_init**(3), **telescope_move**(3), **telescope_park**(3), **telescope_park_off**(3), **telescope_power_off**(3), **telescope_power_on**(3), **telescope_raw**(3), **telescope_set_move_speed**(3), **telescope_set_slew_speed**(3), **telescope_set_track_rate**(3), **telescope_slew**(3), **telescope_stop**(3), **telescope_status**(3), **telescoped**(8)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

