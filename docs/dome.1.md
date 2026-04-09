% dome(1) | General Commands Manual
%
% March 2026

# NAME

dome — command‑line client for the **domed** daemon in AAOS

# SYNOPSIS

**dome** [options] *command* [*arguments*]

# DESCRIPTION

**dome** is the command‑line client for the **domed** daemon in the Automated Astronomical Observatory Operating System (AAOS). It is used to operate a dome and to get or set its runtime parameters.  

Currently just one dome type is supported: virtual domes conformed to AAOS standard.  

# OPTIONS

`-h, --help`
:   Display the help message and exit.

`-v, --version`
:   Display the program version and exit.

`-f, --format` *FORMAT*
:   Set the input format for coordinate arguments of the **slew** command. Accepted values are `degree` or `string`.  The default is `string`, e.g., 22h24m15.25s.

`-i, --index` *INDEX*
:   Select the dome by numeric index.  This option overrides any previous `-i` or `-n` option.  The default index is **1**.

`-n, --name` *NAME*
:   Select the dome by its configured name.  This option overrides any previous `-i` or `-n` option.

`-d, --dome` *ADDRESS:PORT*
:   Specify the dome server’s address and port. If omitted, `localhost:13006` is used.  If *address* ends with `.sock`, a Unix Domain Socket is used instead of a TCP socket.

# COMMANDS

`get` *parameter*
:   Retrieve *parameter* from the dome and write it to standard output. Supported *parameter* values are:

    - `window_open_speed` - speed at which the window of the dome opens.
    - `window_close_speed` - speed at which the window of the dome closes.
    - `window_position` - the current window position, (0% closed to 100% fully open).
    - `position` — the current azumith of the dome (degrees).
    - `slew_speed` — slew speed of the dome.
    - `track_rate` — track rate of the dome.

`info`
:   Print the capabilities of the dome.

`init`
:   Initialise the dome (e.g., set time, location, etc.).

`park`
:   Park the dome at its current position, regardless of its current activity.

`park_off`
:   Disable parking; the dome will resume normal tracking.

`open_window`
:   Open the dome's window.

`close_window`
:   Close the dome's windows.

`set` *parameter* [*value1* [*value2* …]]
:   Set a runtime parameter of the dome. Supported *parameter* values are:

    - `window_open_speed` - set the window-open speed.
    - `window_close_speed` - set the window-close speed.
    - `slew_speed` — set the slew speed of the dome.
    - `track_rate` - set the track rate of the dome.

`slew` *ra* *dec*
:   Slew the dome to the celestial coordinates (*ra*, *dec*). The coordinate format is controlled by **-f, --format**. After the slew completes the dome resumes tracking. If the dome is already slewing, the previous command is aborted.

`status`
:   Print the current status of the dome to standard output. The status includes at least the current location of the window and the *STATE* of the dome.

`stop`
:   Stop any ongoing operation (e.g., slew the dome, open window). If the dome is already parked, it will not resume normal tracking. If the dome is slewing, it will resume tracking immediately. 

# EXIT STATUS
The dome utility exits with one of the following values:

`0`
:   Success.

`>0` 
:   Failure. The exact non‑zero value corresponds to the error reported by the daemon (e.g. `1` for a generic error, other values for specific conditions).

# CONFORMING TO

AAOS-draft-2022

# SEE ALSO

**dome_close_window**(3), **dome_get_index_by_name**(3), **dome_get_position**(3), **dome_get_slew_speed**(3), **dome_get_track_rate**(3), **dome_get_window_close_speed**(3), **dome_get_window_position**(3), **dome_get_window_open_speed**(3), **dome_info**(3), **dome_init**(3), **dome_open_window**(3), **dome_park**(3), **dome_park_off**(3), **dome_slew**(3), **dome_status**(3), **dome_stop**(3), **dome_stop_window**(3), **domed**(8)

# BUGS

Bugs can be reported and filed at https://github.com/huyi-naoc/AAOS/issues.