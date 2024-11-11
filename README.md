#AAOS - Introduction

Automatic Astronomical Observatory Operating System

# Build Requirements

The following packages are needed to build the software

* autotools (autoconf, automake, libtool)
* doxygen (for creating the documentation, not started yet)
* pandoc (for creating the manpages, not started yet)
* tar
* libaravis >= 0.8 (optional, for GIGaE camera)
* libcfitsio >= 3.4 (for FITS file operation)
* libchealpix >= 3.3 (for scheduling system)
* libconfig >= 1.5 (for daemon servers)
* libcjson >= 1.7 (for serializtion)
* libcurl >= 7.6 (optional, if your want ASCOM device support)
* libglib == 2.0 (optional, for GenICAM compatible detector)
* libgsl >= 2.5 (optional, for cubic spline interpolation)
* libmysqlclient >= 11.5 (MariaDB, for operating with databases)
* libsofa_c (optional, external IAU fundamental astronomy library for celetial 
  coordinates transformation)  
* libudev >= 245 (optional, only available for Linux hosts)

# Build

```Bash
cd AAOS
./autogen.sh
./configure
make
make install
```
If GenICAM driver is compiled, the CFLAGS must be set properly. For example, 
if the libaravis-dev and libglib2.0-dev packages are installed using apt
under Ubuntu 22.04,

```Bash
./configure CFLAGS="-I/usr/include/aravis-0.8\ 
                    -I/usr/include/glib-2.0\ 
                    -I/usr/lib/x86_64-linux-gnu/glib-2.0/include"
```
# License

Copyright (C) 2015-2022 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA.
