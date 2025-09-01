% detector(1) | General Commands Manual
%
% Aug 2025

NAME
====

detector - the command-line client of **detetctord** in AAOS 

SYNOPSIS
========

**detector** [-dinu] *command* [parameter list]

**detector** [hv]

DESCRIPTION
===========

**detector** is the command-line client of **detectord** in Automated Astronomical Observatory Operating System (AAOS). It is used for operating detectors (i.e., CCD, CMOS or HgCrTe camera), get and set runtime parameters of a telescope.

It currently supports two types of detector, the virtual detector and the GenICam (using thirdparty aravis library).

OPTIONS
=======
`-d, --detector` *DETECTOR*

:   Set the detector server's address and port in format of *[address]:[port]*. If this option has not present, default localhost:13005 will be applied. If address is a path with suffix ``*.sock*'', it will try to connect Unix Domain Socket. 

`-h, --help`

:   Show the help message and exit

`-i, --index` *INDEX*

:   Set the index of the detector to operate. It will overwrite previous -n or -i option. Default is `1`.

`-n, --name` *NAME*

:   Set the name of the detector to operate. It will overwrite previous -n
 or -i option.


`-u, --unit` *UNIT*

:   Set the exposure time unit (`s`, `ms`)  of `expose` command. Deafult is `s`.

COMMANDS
========

abort
-----

Abort the current exposure command. The difference between **abort** and **stop** is that **abort** will cancel the current exposing frame while **stop** will wait the current frame complete.

disable *parameters*
--------------------

**disable** detector's *parameter* function. Available *parameter* are list as following:

**cooling**

:   disable cooling. 

enable *parameters*
-------------------

**enable** detector's *parameter* function. Available *parameter* are list as following:

**cooling**

:   enable cooling.


expose *exptime* *frames*
-------------------------

Execute exposure command, then wait for the command complete and print the filename of images on stdout. If option *DETECTOR_OPTION_NOTIFY_EACH_COMPLETION* (0x0008) is set, each frame will be stored in different FITS file, otherwise, each frame will be stored as an extention of a single FITS file. When the image file is closed, the filename will be printed on stdout immediately. If option *DETECTOR_OPTION_NOTIFY_LAST_FILLING* (0x0004) is set, **expose** command will return just after the last frame is began to read out.
  
**expose** command may change the frame rate silently.

**expose** command can be aborted or stopped by **abort** or **stop** command.

get *parameter*
---------------

**Get** *parameter*s of the underline detector and print it to stdout. Available *parameter*s are list as following: 

**binning**

:   get the x and y binning.

**directory**

:   get the directory where the images are saved.

**exptime**

:   get the most recent exposure time (in second).

**framerate**

:   get the most recent frame rate (in fps).

**prefix**

:   get the prefix of the image file.


**gain**

:   get the most recent gain (in e-/ADU).

**readout_rate**

:   get the most recent readout rate (in Mbps).

**region**

:   get the most recent region of interest (ROI, x-offset, y-offset, width, height).

**temerature**

:   get the current temerature close to the detector's chip, usually monitoring whether the chip has been cooled to the setting temperature.

**template**
:   get thre most recent template file that image files are create from.

info
----

Print the information of the detetcor on stdout, such as its name, description, manufacturer, serial number, capability (i.e., whether support ROI, different gain, cooling). 

power\_off
----------

Power off underline detector. **power\_off** is not necessarily supported by all types of detectors.

power\_on
----------

Power on underline detector. **power\_on** is not necessarily supported by all t
ypes of detectors.


raw *cmd*
---------

Send raw command to the underline detector. *cmd* is a literal ASCII string.

set *parameter* [*value1* [*value2*, [...]]]
--------------------------------------------

**set** *parameter*s of the underline detector and print it to stdout. Available *parameter*s are list as following: 

**binning**

:   set the x and y binning.

**directory**

:   set the directory where the images are saved.

**exptime**

:   set the most recent exposure time (in second or mili-second, see -u option).

**framerate**

:   set the most recent frame rate (in fps).

**prefix**

:   set the prefix of the image file.


**gain**

:   set the most recent gain (in e-/ADU).

**readout_rate**

:   set the most recent readout rate (in Mbps).

**region**

:   set the most recent region of interest (ROI, x-offset, y-offset, width, height).

**temerature**

:   set the desired cooling temerature.

**template**
:   set thre most recent template FITS file that image files are create from.

status
------

Print the status of the undeline detector on stdout. The status contains at least  the *STATE* of the detector. 

stop
----

Stop the current exposure command.

CONFORMING TO
=============

AAOS-draft-2022

SEE ALSO
========

**detector_arbot**(3), **detector_disable_cooling**(3), **detector_enable_cooling**(3), **detector_expose**(3), **detector_get_binning**(3), **detector_get_directory**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detector_get_gain**(3), **detector_get_index_by_name**(3), **detector_get_prefix**(3), **detector_get_readout_rate**(3), **detector_get_region**(3), **detector_get_temperature**(3), **detector_info**(3), **detector_power_off**(3), **detector_power_on**(3), **detector_raw**(3), **detector_set_binning**(3), **detector_set_directory**(3), **detector_set_exposure_time**(3), **detector_set_frame_rate**(3), **detector_set_gain**(3), **detector_set_index_by_name**(3), **detector_set_prefix**(3), **detector_set_readout_rate**(3), **detector_set_region**(3), **detector_set_temperature**(3), **detector_status**(3), **detector_stop**(3), **detectord**(8)

BUGS
====

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

