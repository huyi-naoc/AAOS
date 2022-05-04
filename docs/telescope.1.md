% telescope(1) | General Commands Manual

NAME
====

telescope - the command-line client of **telescoped** in AAOS 

SYNOPSIS
========

**telescope** [-fhintuv] <u>command</u> [parameter list]

DESCRIPTION
===========

**telescope** is the command-line client of **telescoped** in Automated Astronomical Observatory Operating System (AAOS). It is used for operating telescope, get and set runtime parameters of a telescope. The telescope here is referred to a merely equatorial mount, which can point to and track celetial objects.  

It currently supports two types of telescope, the virtual telescope and the Astro-Physics Mount 1600GTO. ASA DDM100 will be also supported soon.  

OPTIONS
=======

`-h, --help`

:   Show the help message and exit

`-c, --cli WALLPAPER_PATHS...`

:   Set wallpapers from the command line

`-m, --modes WALLPAPER_MODES...`

:   Specify the modes for the wallpapers (`zoom`, `center_black`, `center_blur`, `fit_black`, `fit_blur`)

`-r, --random`

:   Set wallpapers randomly

`-l, --lockscreen`

:   Set lockscreen wallpapers instead of desktop ones (for supported desktop environments)

BUGS
====

Bugs can be reported and filed at https://gitlab.gnome.org/gabmus/hydrapaper/issues

If you are not using the flatpak version of HydraPaper, or if you are using an otherwise out of date or downstream version of it, please make sure that the bug you want to report hasn't been already fixed or otherwise caused by a downstream patch.
