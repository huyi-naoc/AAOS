#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([aaos],[0.1],[huyi.naoc@gmail.com])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
LT_PREREQ([2.2])
LT_INIT([dlopen])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
# AC_PROG_RANLIB
AM_PROG_CC_C_O

case ${host_os} in
    linux*)
        AC_DEFINE([LINUX], , [Linux operating system])
        AC_CHECK_HEADERS([libudev.h mntent.h netinet/tcp.h sys/epoll.h sys/sendfile.h])
        AC_CHECK_LIB([udev], [main])
        ;;
    darwin*)
        AC_DEFINE([MACOSX], , [Macintosh operating system])
        AC_CHECK_HEADERS([sys/event.h uuid/uuid.h])
        ;;
    *)
        ;;
esac

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcfitsio':
AC_CHECK_LIB([cfitsio], [main])
# FIXME: Replace `main' with a function in `-lconfig':
AC_CHECK_LIB([config], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
AC_CHECK_LIB([cjson], [main])
AC_CHECK_LIB([chealpix], [main])

LIBS="$LIBS -pthread"
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h mntent.h netdb.h pthread.h stdint.h sys/mount.h sys/param.h sys/socket.h sys/statvfs.h sys/time.h sys/un.h syslog.h termios.h unistd.h   chealpix.h cjson/cJSON.h fitsio2.h libconfig.h])
GOBJECT_INTROSPECTION_CHECK([1.40.0])
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

AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [Enable debug mode [default = no]])], [debug=${enableval}], [ascom=no])
if test x"${debug}" = xyes; then
    AC_DEFINE([DEBUG], , [enable debug mode])
fi

AC_ARG_ENABLE([mysql], [AS_HELP_STRING([--enable-mysql], [Enable MySQL [default = yes]])], [mysql=${enableval}], [mysql=yes])
if test x"${mysql}" = xyes; then
    AC_CHECK_HEADERS([mysql/mysql.h], [have_mysql=yes], [have_mysql=no])
    if test x"${have_mysql}" = xyes; then
        AC_SEARCH_LIBS([mysql_init], [mysqlclient], [have_mysql=yes], [have_mysql=no])
        if test x"${have_mysql}" = xyes; then
            AC_DEFINE([__USE_MYSQL__], , [use mysql library])
        else
            AC_MSG_ERROR([libmysql is required for --enable-mysql])
        fi
    else
        AC_MSG_ERROR([mysql.h is required for --enable-mysql])
    fi
else
    AC_MSG_NOTICE([*** MySQL will not be enabled ***])
fi

AC_ARG_ENABLE([ascom], [AS_HELP_STRING([--enable-ascom], [Enable ASCOM interface [default = no]])], [ascom=${enableval}], [ascom=no])
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

AC_ARG_ENABLE([gsl], [AS_HELP_STRING([--enable-gsl], [Use GNU scientific library [default = yes]])], [gsl=${enableval}], [gsl=yes])
if test x"${gsl}" = xyes; then
    AC_CHECK_HEADERS([gsl/gsl_errno.h gsl/gsl_spline.h], [have_gsl=yes], [have_gsl=no])
    if test x"${have_gsl}" = xyes; then
        AC_SEARCH_LIBS([gsl_interp_accel_alloc], [gsl], [have_gsl=yes], [have_gsl=no])
        AC_SEARCH_LIBS([cblas_sswap], [gslcblas], [have_gsl_cblas=yes], [have_gsl_cblas=no])
        if test x"${have_gsl_cblas}" = xno; then
            have_gsl=no
        fi
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

AC_ARG_ENABLE([sofa], [AS_HELP_STRING([--enable-sofa],[Use IAU SOFA library [default = yes]])], [sofa=${enableval}], [sofa=yes])
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
    
AC_ARG_ENABLE([aravis], [AS_HELP_STRING([--enable-aravis], [Use aravis library for GiGE compatible cameras [default=yes]])], [aravis=${enableval}], [aravis=yes])
if test x"${aravis}" = xyes; then
    AC_CHECK_HEADERS([aravis-0.8/arv.h glib-2.0/glib.h], [have_aravis=yes], [have_aravis=no])
    if test x"${have_aravis}" = xyes; then
        AC_SEARCH_LIBS([arv_camera_new], [aravis-0.8], [have_aravis_lib=yes], [have_aravis_lib=no])
        AC_SEARCH_LIBS([g_malloc], [glib-2.0], [have_glib_lib=yes], [have_glib_lib=no])
	if test x"${have_aravis_lib}" = xyes && test x"${have_glib_lib}" = xyes; then
	    AC_DEFINE([__USE_ARAVIS__], , [use aravis library])
	else
	    AC_MSG_ERROR([libaravis-0.8 and libglib-2.0 required for --enable-aravis])
	fi
    else
        AC_MSG_ERROR([aravis-0.8/arv.h and glib-2.0/glib.h are required for --enable-aravis])
    fi
else
    AC_MSG_NOTICE([*** aravis library will NOT be linked, some functions may not work properly ***])
fi

AC_ARG_ENABLE([docs], [AS_HELP_STRING([--enable-doc], [Generate manpages [default = no]])], [docs=${enableval}], [docs=no])

if test x"${docs}" = xyes; then
    AC_CHECK_PROGS([PANDOC], [pandoc])
fi
AM_CONDITIONAL([WITH_PANDOC], [ test -n "$PANDOC" ])

AC_CONFIG_FILES([Makefile
		 cores/Makefile
                 docs/Makefile
		 drivers/Makefile
                 mics/Makefile
                 src/Makefile])
AC_OUTPUT
