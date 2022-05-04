# AAOS

Automatic Astronomical Observatory Operating System

# Build Requirements

The following packages are needed to build the software

* autotools (autoconf, automake, libtool)
* doxygen (for creating the documentation)
* tar
* libconfig >= 1.5 (for daemon servers)
* libcjson >= 1.7 (optional, if your want ASCOM device support)
* libcurl >= 7.6 (optional, if your want ASCOM device support)
* libgsl >= 2.5 (optional, for cubic spline interpolation)
* libsofa_c (optional, external IAU fundamental astronomy library for celetial coordinates transformation)  
* libudev >= 245 (optional, only available for Linux hosts)
