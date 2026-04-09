% detector(1) | General Commands Manual
%
% Aug 2025

# NAME

detector - the command-line client of **detectord** in AAOS 

# SYNOPSIS


**detector** [-dinu] *command* [*arguments*]  

**detector** -h | -v

# DESCRIPTION

**detector** is the command-line client of **detectord** in Automated Astronomical Observatory Operating System (AAOS). It is used for operating detectors (i.e., CCD, CMOS or HgCrTe camera), get and set runtime parameters of a detector.

Currently four detector types are supported: virtual detector that conforms to the AAOS standard, GenICam compatible cameras accessed through the third‑party *aravis* library, ZWO ASI series cameras, and QHY cameras.

# OPTIONS

`-d, --detector` *ADDRESS:PORT*
:  Specify the detector server’s address and port. If this option is not supplied, the default `localhost:13005` is used.  If *address* ends with `.sock`, a Unix Domain Socket is used instead of a TCP socket.

`-h, --help`

: Display the help message and exit

`-i, --index` *INDEX*

: Select the detector by numeric index.  This option overrides any previous `-i` or `-n` option. The default index is **1**.

`-n, --name` *NAME*

:  Select the detector by its configured name.  This option overrides any previous `-i` or `-n` option.

`-u, --unit` *UNIT*

:   Set the exposure time unit (`s`, `ms`)  of `expose` command. Deafult is `s`.

`-v, --version`
: Display the program version and exit.

# COMMANDS

`abort`
:   Abort the current exposure command.  The difference between **abort** and **stop** is that **abort** cancels the current exposing frame, whereas **stop** waits for the current frame to finish.

`disable` *parameters*
:   Disable a detector function.  Currently the only supported *parameter* is:
    
    - `cooling` – disable cooling.

`enable` *parameters*
:   Enable a detector function.  Currently the only supported *parameter* is:
    
    - `cooling` – enable cooling.

`expose` *exptime* *frames*
:   Start an exposure of length *exptime* (in the unit selected by **-u**) and acquire *frames* frames.  The command waits until the exposure sequence completes and prints the name(s) of the generated FITS file(s) on **stdout**.
  
    If the option `DETECTOR_OPTION_NOTIFY_EACH_COMPLETION` (0x0008) is set, each frame is stored in a separate FITS file. Otherwise all frames are stored as extensions of a single FITS file. When a FITS file is closed, its name is printed immediately. If the option `DETECTOR_OPTION_NOTIFY_LAST_FILLING` (0x0004) is set, **expose** returns as soon as the last frame begins read‑out.

    The **expose** command may silently change the frame rate. It can be aborted or stopped with the **abort** or **stop** commands.
  
`get` *parameter*

:   Retrieve *parameter* from the detector and print it to **stdout**.  Supported parameters are:

    - `binning` – current X and Y binning factors.
    - `capture_mode` – current capture mode.
    - `directory` – directory where images are saved.
    - `exptime` – most recent exposure time (seconds).
    - `framerate` – most recent frame rate (frames s⁻¹).
    - `gain` – most recent gain (e⁻/ADU).
    - `pixel_format` - current pixel format.
    - `prefix` – filename prefix for saved images.
    - `readout_rate` – most recent read‑out rate (Mbps).
    - `region` – current region of interest (ROI) as `x‑offset,y‑offset,width,height`.
    - `temperature` – current detector‑chip temperature.
    - `template` – most recent template FITS file used for image creation.
    - `trigger_mode` - current trigger mode.

`info`
:   Print information about the detector on **stdout**, including its name, description, manufacturer, serial number, and capabilities (e.g., ROI support, multiple gain settings, cooling). 

`power_off`
:   Power off the detector.  This command is not necessarily supported by all detector types.

`power_on`
:   Power on the detector.  This command is not necessarily supported by all detector types.

`raw` *cmd*
:   Send a raw command *cmd* (a literal ASCII string) directly to the detector.

`set` *parameter* [*value1* [*value2* …]]
:   Set *parameter* of the underlying detector.  Supported parameters are:
  
    - `binning` – set X and Y binning factors.  
    - `capture_mode` – set capture mode; valid values are `sanphot`, `multiframe`, and `video`.
    - `directory` – set the directory where images are saved.
    - `exptime` – set exposure time (seconds or milliseconds, depending on **-u**).
    - `framerate` – set frame rate (frames s⁻¹).
    - `gain` – set gain (e⁻/ADU).
    - `pixel_format` – set pixel format; valid values are `mono8`, `mono10`, `mono10_packed`, `mono12`, `mono12_packed`, `mono14`, `mono14_packed`, `mono16`, `mono16_packed`, `mono24`, `mono24_packed`, `mono32`, `mono64`, `RGB24`, `YUV422`, or the numbers **1**‑**16** corresponding to the above list.
    - `prefix` – set the filename prefix for saved images.
    - `readout_rate` – set read‑out rate (Mbps).
    - `region` – set region of interest as `x‑offset,y‑offset,width,height`.
    - `temperature` – set the desired detector‑chip temperature.
    - `template` – set the template FITS file used for image creation.
    - `trigger_mode` – set trigger mode; valid values are `default`, `software_level`, `software_edge`, `hardware_level_high`, `hardware_level_low`, `hardware_edge_rise`, `hardware_edge_fall`, or the numbers **1**‑**7** corresponding to the above list.

`status`
:   Print the current status of the underlying detector on **stdout**. The output always includes at least the detector *STATE*.

`stop`
:   Stop the current exposure command.  Unlike **abort**, this command waits for the current frame to finish before returning.

# EXIT STATUS

The **detector** utility exits with one of the following values:

`0`
:   Success.

`>0`
:   Failure.  The non‑zero value corresponds to the error reported by the daemon (e.g. `1` for a generic error, other values for specific conditions).

# CONFORMING TO

AAOS‑draft‑2022

# SEE ALSO

**detector_abort**(3), **detector_disable_cooling**(3), **detector_enable_cooling**(3), **detector_expose**(3), **detector_get_binning**(3), **detector_get_directory**(3), **detector_get_exposure_time**(3), **detector_get_frame_rate**(3), **detector_get_gain**(3), **detector_get_index_by_name**(3), **detector_get_prefix**(3), **detector_get_readout_rate**(3), **detector_get_region**(3), **detector_get_temperature**(3), **detector_info**(3), **detector_init**(3), **detector_power_off**(3), **detector_power_on**(3), **detector_raw**(3), **detector_set_binning**(3), **detector_set_directory**(3), **detector_set_exposure_time**(3), **detector_set_frame_rate**(3), **detector_set_gain**(3), **detector_set_index_by_name**(3), **detector_set_prefix**(3), **detector_set_readout_rate**(3), **detector_set_region**(3), **detector_set_temperature**(3), **detector_status**(3), **detector_stop**(3), **detectord**(8)

# BUGS

Bugs can be reported and filed at <https://github.com/huyi-naoc/AAOS/issues>.