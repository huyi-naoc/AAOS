% telescope(1) | General Commands Manual
%
% May 2022

# NAME

telescope — command‑line client for the **telescoped** daemon in AAOS

# SYNOPSIS

**telescope** [options] *command* [*arguments*]

# DESCRIPTION

**telescope** is the command‑line client for the **telescoped** daemon in the Automated Astronomical Observatory Operating System (AAOS). It is used to operate a telescope and to get or set its runtime parameters. The telescope is assumed to be an equatorial mount capable of pointing to and tracking celestial objects. 

Currently four telescope types are supported: virtual telescopes conformed to AAOS standard, the Astro‑Physics Mount 1600GTO, the Sun Yat-Sen 80 cm infared telescope, and telescopes using Sitech controller. Support for the ASA DDM100 and ASCOM compatible telescopes are planned.  

# OPTIONS

`-h, --help`
:   Display the help message and exit.

`-v, --version`
:   Display the program version and exit.

`-f, --format` *FORMAT*
:   Set the input format for coordinate arguments of the **slew** command. Accepted values are `degree` or `string`.  The default is `string`, e.g., 22h24m15.25s.

`-i, --index` *INDEX*
:   Select the telescope by numeric index.  This option overrides any previous `-i` or `-n` option.  The default index is **1**.

`-n, --name` *NAME*
:   Select the telescope by its configured name.  This option overrides any previous `-i` or `-n` option.

`-t, --telescope` *ADDRESS:PORT*
:   Specify the telescope server’s address and port. If omitted, `localhost:13000` is used. If *address* ends with `.sock`, a Unix Domain Socket is used instead of a TCP socket.


`-u, --unit` *UNIT*
:   Choose the unit for the **set move_speed** and **set slew_speed** commands. Valid units are `nature`, `second`, `minute`, and `degree`. `nature` represents 15 arcseconds per second.  The default is `nature`.

# COMMANDS

`enable` *parameter*
:   Enable a telescope parameter. Supported *parameter* values are:
     
    - `derotator` - enable the derotator of the current detetctor (horizontal mount or Nasmyth focus). 

`disable` *parameter*
:   Enable a telescope parameter. Supported *parameter* values are:
    
    - `derotator` - disable the derotator of the current detetctor, (horizontal mount or Nasmyth focus). 

`focus` *absolute* *step*
:   Adjust the focus. If *absolute* is `0`, the *step* is interpreted as a relative offset from the current focus position; otherwise *step* is taken as an absolute position.   

`get` *parameter*
:   Retrieve *parameter* from the telescope and write it to standard output.  Supported *parameter* values are:

    - `derotator_angle` - current angle of the derotator for the active detector.
    - `focus_length` - current length of the focus for the active detector.
    - `move_speed` — move speed of a single axis.
    - `slew_speed` — slew speed of the two axes (RA/EW and DEC/Alt).
    - `track_rate` — track rate of the two axes.
  
`go_home`
:   Slew the telescope to its home position and park it there. Subsequent operations (e.g., closing the dome) are then safe.

`info`
:   Print the capabilities of the telescope.  Output is JSON by default.

`init`
:   Initialise the telescope (e.g., set time, location, etc.).

`move` *direction* *duration*
:   Move a single axis of the telescope.  *direction* may be `east`, `west`, `north`, or `south`.  *duration* specifies how long the axis should move. After the move completes the telescope resumes tracking. If the telescope is already moving or slewing, the previous command is aborted.

`timed_move` *direction* *duration* *timeout*
:   Same as **move**, but blocks for up to *timeout* seconds while a previous move or slew operation finishes.  If the timeout expires, the command fails.

`try_move` *direction* *duration*
:   Same as **move**, but returns immediately if the telescope is busy; no movement occurs in that case.

`open_cover`
:   Open the optical‑tube cover.

`close_cover`
:   Close the optical‑tube cover.

`park`
:   Park the telescope at its current position, regardless of its current activity.

`park_off`
:   Disable parking; the telescope will resume normal tracking.

`power_off`
:   Power off the telescope.

`power_on`
:   Power on the telescope.

`raw` *cmd*
:   Send a raw ASCII command *cmd* directly to the telescope.

`slew` *ra* *dec*
:   Slew the telescope to the celestial coordinates (*ra*, *dec*). The coordinate format is controlled by **-f, --format**. After the slew completes the telescope resumes tracking. If the telescope is already moving or slewing, the previous command is aborted.

`timed_slew` *ra* *dec* *timeout*
:   Same as **slew**, but blocks for up to *timeout* seconds while a previous move or slew operation finishes.

`try_slew` *ra* *dec*
:   Same as **slew**, but returns immediately if the telescope is busy; no movement occurs in that case.

`set` *parameter* [*value1* [*value2* …]]
:   Set a runtime parameter of the telescope. Supported *parameter* values are:

    - `move_speed` — set the move speed of a single axis.
    - `slew_speed` — set the slew speed of the two axes (RA/EW and DEC/Alt).
    - `track_rate` — set the track rate of the two axes (RA/EW and DEC/Alt).
  
`status`
:   Print the current status of the telescope to standard output. The status includes at least the current location and the telescope’s *STATE*.

`stop`
:   Stop any ongoing operation (move, slew, etc.) and then resume tracking.

`switch` *device_type* *device_name*
:   Switch devices. Supported *device_type* values are:

    - `instrument` - select an instrument.
    - `detector` - select a detector.
    - `filter` - select a filter, grism, or slit.

# EXIT STATUS

The telescope utility exits with one of the following values:

`0`
: Success.

`>0` 
: Failure. The exact non‑zero value corresponds to the error reported by the daemon (e.g. `1` for a generic error, other values for specific conditions).

# CONFORMING TO

AAOS-draft-2022

# SEE ALSO

**telescope_close_cover**, **telescope_enable_derotator**, **telescope_disable_derotator**, **telescope_focus**(3), **telescope_get_derotator_angle**(3), **telescope_get_index_by_name**(3), **telescope_get_move_speed**(3), **telescope_get_slew_speed**(3), **telescope_get_track_rate**(3), **telescope_go_home**(3), **telescope_info**(3), **telescope_init**(3), **telescope_move**(3), **telescope_open_cover**(3), **telescope_park**(3), **telescope_park_off**(3), **telescope_power_off**(3), **telescope_power_on**(3), **telescope_raw**(3), **telescope_set_move_speed**(3), **telescope_set_slew_speed**(3), **telescope_set_track_rate**(3), **telescope_slew**(3), **telescope_stop**(3), **telescope_status**(3), **telescope_switch_detector**(3), **telescope_switch_filter**(3), **telescope_switch_instrument**(3), **telescoped**(8)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.

