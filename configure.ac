#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([aaos], [0.1], [huyi.naoc@gmail.com])
AC_CONFIG_MACRO_DIRS([m4])
LT_PREREQ([2.2])
LT_INIT([dlopen])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
#AC_PROG_RANLIB
AM_PROG_CC_C_O

OS=`uname -s`
if test "x${OS}" = xLinux; then
    AC_DEFINE([LINUX], , [Linux operating system])
elif test "x${OS}" = xDarwin; then
    AC_DEFINE([MACOSX], , [Macintosh operating system])
fi

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcfitsio':
AC_CHECK_LIB([cfitsio], [main])
# FIXME: Replace `main' with a function in `-lconfig':
AC_CHECK_LIB([config], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-ludev':
if test "x${OS}" = xLinux; then
    AC_CHECK_LIB([udev], [main])
fi

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h mntent.h netdb.h stdint.h sys/mount.h sys/param.h sys/socket.h sys/statvfs.h sys/time.h syslog.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([atexit clock_gettime floor ftruncate memset mkdir munmap regcomp select socket strchr strerror strrchr strstr])

AC_ARG_ENABLE([ascom], [--enable-ascom enable ascom], [ascom=${enableval}], [ascom=no])
if test x"${ascom}" = xyes; then
    AC_CHECK_HEADERS([cjson/cJSON.h curl/curl.h], [have_ascom=yes], [have_ascom=no])
    if test x"${have_ascom}" = xyes; then
        AC_SEARCH_LIBS([cJSON_Parse], [cjson], [have_cjson=yes], [have_cjson=no])
        AC_SEARCH_LIBS([curl_easy_init], [curl], [have_curl=yes], [have_curl=no])
	if test x"${have_cjson}" = xyes && test x"${have_curl}" = xyes; then
	    have_ascom=yes
        else
	    have_ascom=no
	fi
	if test x"${have_ascom}" = xyes; then
	    AC_DEFINE([__USE_ASCOM__], , [use cjson and curl library])
	else
	    AC_MSG_ERROR([libcjson and libcurl is required for --enable-ascom])
	fi
    else
        AC_MSG_ERROR([cJSON.h.h and curl.h are required for --enable-ascom])
    fi
else
    AC_MSG_NOTICE([*** ASCOM driver will not be enabled ***])
fi

AC_ARG_ENABLE([gsl], [--disable-gsl disable gsl], [gsl=${enableval}], [gsl=yes])
if test x"${gsl}" = xyes; then
    AC_CHECK_HEADERS([gsl/gsl_errno.h gsl/gsl_spline.h], [have_gsl=yes], [have_gsl=no])
    if test x"${have_gsl}" = xyes; then
        AC_SEARCH_LIBS([gsl_interp_accel_alloc], [gsl], [have_gsl=yes], [have_gsl=no])
	if test x"${have_gsl}" = xyes; then
	    AC_DEFINE([__USE_GSL__], , [use GSL library])
	else
	    AC_MSG_ERROR([libgsl is required for --enable-gsl])
	fi
    else
        AC_MSG_ERROR([gsl_errno.h and gsl_spline.h are required for --enable-gsl])
    fi
else
    AC_MSG_NOTICE([*** GSL library will NOT be linked, some functions may not work properly ***])
fi

AC_ARG_ENABLE([sofa], [--disable-sofa disable sofa], [sofa=${enableval}], [sofa=yes])
if test x"${sofa}" = xyes; then
    AC_CHECK_HEADERS([sofa.h sofam.h], [have_sofa=yes], [have_sofa=no])
    if test x"${have_sofa}" = xyes; then
	AC_SEARCH_LIBS([iauApci13], [sofa_c], [have_sofa=yes])
	if test x"${have_sofa}" = xyes; then
	    AC_DEFINE([__USE_SOFA__], , [use IAU SOFA library])
	else
	    AC_MSG_ERROR([libsofa_c is required for --enable-sofa])
	fi
    else
        AC_MSG_ERROR([sofa.h and sofam.h are required for --enable-sofa])
    fi
fi
#AM_COND_IF(
#    [GSL],
#    [AC_SEARCH_LIBS([gsl_interp_accel_alloc], [gsl], [have_gsl=yes])
#    AC_SEARCH_LIBS([cblas_sswap], [gslcblas], [have_gsl=yes])
#    AM_CONDITIONAL([HAVE_GSL_LIB], [test "x${have_gsl}" = xyes])
#    AM_COND_IF(
#        [HAVE_GSL_LIB], 
#	[AC_CHECK_HEADERS([gsl/gsl_errno.h gsl/gsl_spline.h], [], [have_gsl=no])
#         AM_CONDITIONAL([HAVE_GSL_INC], [test "x${have_gsl}" = xyes])
#         AM_COND_IF([HAVE_GSL_INC], [AC_DEFINE([__USE_GSL__], , [use GSL library])], [AC_MSG_ERROR([gsl_errno.h and gsl_spline.h is required for --enable-gsl])])
#	], 
#	[AC_MSG_ERROR([libgsl is required for --enable-gsl])]
#    )
#    ]
#)

#AC_ARG_ENABLE([sofa], [--disable-sofa disable sofa], [sofa=${enableval}], [sofa=yes])
#AM_CONDITIONAL([SOFA], [test "x${sofa}" = xyes])
#AM_COND_IF(
#    [SOFA],
#    [AC_SEARCH_LIBS([iauApci13], [sofa_c], [have_sofa=yes])
#    AM_CONDITIONAL([HAVE_SOFA_LIB], [test "x${have_sofa}" = xyes])
#    AM_COND_IF(
#        [HAVE_SOFA_LIB], 
#	[AC_CHECK_HEADERS([sofa.h sofam.h], [], [have_sofa=no])
#         AM_CONDITIONAL([HAVE_SOFA_INC], [test "x${have_sofa}" = xyes])
#         AM_COND_IF([HAVE_SOFA_INC], [AC_DEFINE([__USE_SOFA__], , [use SOFA library])], [AC_MSG_ERROR([sofa.h and sofam.h is required for --enable-sofa])])
#	], 
#	[AC_MSG_ERROR([libsofa_c is required for --enable-sofa])]
#    )
#    ]
#)

#if test "x${sofa}" = xyes; then
#    have_sofa=no
#    AC_SEARCH_LIBS([iauApci13], [sofa_c], [have_sofa=yes])

#    if test "x${have_sofa}" = xyes; then
#        AC_CHECK_HEADERS([sofa.h sofam.h], [], [have_sofa=no])
#    fi
#
#    if test "x${have_sofa}" = xno; then
#        AC_MSG_WARN([
#    -----------------------------------------
#    Unable to find SOFA library on this system.
#    Building a native version.
#    ----------------------------------------])
#	sofa=no
#    fi
#fi

#if test "x${sofa}" = xyes; then
#    AC_DEFINE([__USE_SOFA__], , [use SOFA library])
#fi

#AC_ARG_ENABLE([gsl], [--enable-gsl enable gsl], [gsl=${enableval}], [gsl=no])

#if test "x${gsl}" = xyes; then
#    have_gsl=no
#    AC_SEARCH_LIBS([gsl_interp_accel_alloc], [gsl], [have_gsl=yes])
#    AC_SEARCH_LIBS([cblas_sswap], [gslcblas], [have_gsl=yes])

#    if test "x${have_gsl}" = xyes; then
#        AC_CHECK_HEADERS([gsl/gsl_errno.h gsl/gsl_spline.h], [], [have_gsl=no])
#    fi

#    if test "x${have_gsl}" = xno; then
#        AC_MSG_WARN([
#    -----------------------------------------
#    Unable to find GSL library on this system.
#    Building a native version.
#    ----------------------------------------])
#	gsl=no
#    fi
#fi

#if test "x${gsl}" = xyes; then
#    AC_DEFINE([__USE_GSL__], , [use gsl library])
#fi


#AC_SUBST([EXTRA_DRIVER_SOURCES])

#AC_ARG_ENABLE([ascom], [--enable-ascom enable ascom], [ascom=${enableval}], [ascom=no])

#if test "x${ascom}" = xyes; then
#    have_ascom=no
#    AC_SEARCH_LIBS([curl_easy_init], [curl], [have_ascom=yes])
#    AC_SEARCH_LIBS([cJSON_Parse], [cjson], [have_ascom=yes])
#
#    if test "x${have_ascom}" = xyes; then
#        AC_CHECK_HEADERS([cjson/cJSON.h curl/curl.h], [], [have_ascom=no])
#    fi
#
#    if test "x${have_ascom}" = xno; then
#        AC_MSG_WARN([
#    -----------------------------------------
#    Unable to find curl library on this system.
#    Building a native version.
#    ----------------------------------------])
#	ascom=no
#    fi
#fi

#if test "x${ascom}" = xyes; then
#    AC_DEFINE([__USE_ASCOM__], , [use ascom functionality])
#fi

AC_CONFIG_FILES([Makefile
		 cores/Makefile
		 drivers/Makefile
                 src/Makefile])
AC_OUTPUT
