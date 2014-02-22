###############################################################################
###
### OpenStopMotion Configure
###
###############################################################################
#
# This file is part of OpenStopMotion by Patrick Fedick
#
# $Author: pafe1234@gmail.com $
# $Revision: 483 $
# $Date: 2014-02-20 20:04:03 +0100 (Do, 20 Feb 2014) $
# $Id: build_package.sh 483 2014-02-20 19:04:03Z pafe1234@gmail.com $
#
#
# Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.63)
AC_INIT([openstopmotion], [0.6.5])
AC_CONFIG_AUX_DIR(autoconf)

AC_CONFIG_SRCDIR([ppl7/include/ppl7.h])
AC_CONFIG_SRCDIR([ppl7/include/config.h.in])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([ppl7/Makefile])
AC_CONFIG_FILES([ppl7/tests/Makefile])
AC_CONFIG_FILES([ppl7/ppl7-config])
AC_CONFIG_HEADERS([ppl7/include/config.h])
AC_CONFIG_HEADERS([ppl7/include/ppl7-config.h])

###############################################################################
# Includes
###############################################################################
sinclude(autoconf/ax_check_openssl.m4)
sinclude(autoconf/ax_lib_curl.m4)
sinclude(autoconf/ax_lib_mysql.m4)
sinclude(autoconf/ax_lib_postgresql.m4)
sinclude(autoconf/ax_lib_sqlite.m4)
sinclude(autoconf/freetype.m4)
sinclude(autoconf/ax_path_lib_pcre.m4)
sinclude(autoconf/png.m4)
sinclude(autoconf/jpeg.m4)
sinclude(autoconf/ax_pthread.m4)
sinclude(autoconf/sdl2.m4)
sinclude(autoconf/sdl.m4)
sinclude(autoconf/freetds.m4)
sinclude(autoconf/nasm.m4)
sinclude(autoconf/lib-link.m4)
sinclude(autoconf/iconv.m4)
sinclude(autoconf/bzip2.m4)
sinclude(autoconf/ax_check_zlib.m4)
sinclude(autoconf/libmcrypt.m4)
sinclude(autoconf/libmhash.m4)
sinclude(autoconf/ax_lib_crypto.m4)
sinclude(autoconf/ax_lib_gcrypt.m4)
sinclude(autoconf/checkfuncs.m4)
sinclude(autoconf/ax_gcc_x86_cpuid.m4)
sinclude(autoconf/ax_check_compiler_flags.m4)
sinclude(autoconf/ax_gcc_archflag.m4)
sinclude(autoconf/ldns.m4)
sinclude(autoconf/mpg123.m4)
sinclude(autoconf/lame.m4)
sinclude(autoconf/ogg.m4)
sinclude(autoconf/shout.m4)
sinclude(autoconf/imagemagick.m4)
sinclude(autoconf/libtiff.m4)
sinclude(autoconf/libmicrohttpd.m4)
sinclude(autoconf/libidn.m4)
sinclude(autoconf/libbind.m4)
sinclude(autoconf/gtest.m4)



report_have_pthreads="no"
report_have_jpeg="no"
report_have_libmicrohttpd="no"
report_have_libmcrypt="no"
report_have_libcrypto="no"
report_have_libmhash="no"
report_have_libidn="no"
report_have_libtiff="no"
report_have_curl="no"
report_have_mpg123="no"
report_have_lame="no"
report_have_libogg="no"
report_have_libshout="no"
report_have_libmad="no"
report_have_iconv="no"
report_have_bzip2="no"
report_have_zlib="no"
report_have_pcre="no"
report_have_openssl="no"
report_have_freetype2="no"
report_have_sdl1="no"
report_have_sdl2="no"
report_have_png="no"
report_have_libimlib="no"
report_have_mysql="no"
report_have_postgres="no"
report_have_sqlite3="no"
report_have_ldns="no"
report_have_nasm="no"


AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

OLD_CFLAGS=$CFLAGS
# Checks for programs.
AC_PROG_CC
CFLAGS=$OLD_CFLAGS
AC_PROG_CXX
dnl AC_LANG(C++)

dnl AX_GCC_ARCHFLAG
#AX_CC_MAXOPT

  
case "$host_os" in
	*mingw32* )
		report_have_pthreads="yes, Win32 Threads"
  		;;
	*)
		AX_PTHREAD([report_have_pthreads="yes, pthreads"]
			AC_DEFINE(HAVE_PTHREADS,1, [Define to 1 if we have pthreads])			
		)

		;;
esac

AC_PROG_SED

AC_C_BIGENDIAN(
	report_byteorder="BIG ENDIAN"
	AC_DEFINE(__BIG_ENDIAN__),
	report_byteorder="LITTLE ENDIAN"
	AC_DEFINE(__LITTLE_ENDIAN__))

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(wchar_t)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(void*)

if test $ac_cv_sizeof_char -eq 1; then
	AC_DEFINE(ppluint8,unsigned char)
	AC_DEFINE(pplint8,signed char)
fi
if test $ac_cv_sizeof_short -eq 2; then
	AC_DEFINE(ppluint16,unsigned short)
	AC_DEFINE(pplint16,signed short)
fi

if test $ac_cv_sizeof_int -eq 4; then
	AC_DEFINE(ppluint32,unsigned int)
	AC_DEFINE(pplint32,signed int)
fi

if test $ac_cv_sizeof_long_long -eq 8; then
		AC_DEFINE(ppluint64,unsigned long long)
		AC_DEFINE(pplint64,signed long long)
else
	if test $ac_cv_sizeof_long -eq 8; then
		AC_DEFINE(ppluint64,unsigned long)
		AC_DEFINE(pplint64,signed long)
	fi
fi

if test $ac_cv_sizeof_wchar_t -eq 4; then
	if test "$report_byteorder" = "LITTLE ENDIAN"; then
		AC_DEFINE(ICONV_UNICODE,"UTF-32LE")
	else
		AC_DEFINE(ICONV_UNICODE,"UTF-32BE")
	fi
else
	if test "$report_byteorder" = "LITTLE ENDIAN"; then
		AC_DEFINE(ICONV_UNICODE,"UTF-16LE")
	else
		AC_DEFINE(ICONV_UNICODE,"UTF-16BE")
	fi
fi



if test $ac_cv_sizeof_voidp -eq 4; then
	AC_DEFINE(ppliptr,ppluint32)
elif test $ac_cv_sizeof_voidp -eq 8; then
	AC_DEFINE(ppliptr,ppluint64)
else
	AC_MSG_ERROR(size of void* is not supported)
	
fi

AC_CHECK_SIZEOF(fpos_t)


AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

# AX_HAVE_QT scheint unter FreeBSD nicht kompatibel zu sein. Der produzierte Code
# läuft zwar unter FreeBSD, aber nicht unter Linux oder MinGW 
dnl AX_HAVE_QT

AX_PROG_NASM


PKG_PROG_PKG_CONFIG()


#--------------------------------------------------------------------
#       Check for various typedefs and provide substitutes if
#       they don't exist.
#--------------------------------------------------------------------

AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_OFF_T
dnl AC_SIZEOF_TYPE(off_t)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM
# FPOS_T ist ein Struct?
AX_FPOS_ISSTRUCT()

AX_TM_HAS_GMTOFF()

AC_PATH_XTRA

AC_ARG_ENABLE([gcov],
  [AS_HELP_STRING([--enable-gcov],
    [use Gcov to test the test suite])],
    [],
    [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV],[test '!' "$enable_gcov" = no])

report_have_gtest="no"
report_codecoverage_enabled="no"
GTEST_LIB_CHECK([1.6.0],
	[GTEST_PREFIX=`$GTEST_CONFIG --prefix`
	AC_SUBST(GTEST_PREFIX)
	report_have_gtest="yes"
	if test '!' "$enable_gcov" = no
	then
		GCOV_CFLAGS="-fprofile-arcs -ftest-coverage"
		GCOV_LDFLAGS="-lgcov"
		AC_SUBST(GCOV_CFLAGS)
		AC_SUBST(GCOV_LDFLAGS)
		report_codecoverage_enabled="yes"
	
	fi
	],
	report_have_gtest="no"
)


AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([sys/file.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/poll.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([arpa/nameser.h])
AC_CHECK_HEADERS([resolv.h],,,[
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/nameser.h>
])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([strings.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_HEADERS([dirent.h])
AC_CHECK_HEADERS([fnmatch.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([wctype.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADERS([wchar.h])
AC_CHECK_HEADERS([widec.h])
AC_CHECK_HEADERS([syslog.h])
AC_CHECK_HEADERS([sys/mman.h])
AC_CHECK_HEADERS([valgrind/helgrind.h])

AC_FUNC_MKTIME
AC_CHECK_TYPE(size_t, unsigned)
AC_CHECK_TYPE(wchar_t, unsigned)

# Checks for library functions.
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strtolower strtoupper])
AC_CHECK_FUNCS([bzero bcopy gethostbyname gettimeofday inet_ntoa memmove memset mkdir socket vasprintf asprintf])
AC_CHECK_FUNCS([gethostname sethostname getdomainname setdomainname uname])
AC_CHECK_FUNCS([strcasecmp strlen strnlen strdup strerror strncasecmp strndup strstr strcasestr strcmp strncmp])
AC_CHECK_FUNCS([strncpy strlcpy strncat strlcat])
AC_CHECK_FUNCS([strtok strtok_r])
AC_CHECK_FUNCS([htol atoll atoi atol atof strtoll strtoull])
AC_CHECK_FUNCS([flock fcntl truncate ftruncate sync fsync unlink remove])
AC_CHECK_FUNCS([fgets fputs fgetws fputws fputc fgetc fputwc fgetwc mmap munmap sysconf pagesize])
AC_CHECK_FUNCS([usleep nanosleep localtime localtime_r gmtime gmtime_r])
AC_CHECK_FUNCS([signal syslog openlog closelog])
AC_CHECK_FUNCS([strtok_r])
AC_CHECK_FUNCS([getpid getppid getuid geteuid])
AC_CHECK_FUNCS([mkstemp])
AC_CHECK_FUNCS([wcscmp wcsncmp wcscasecmp wcsncasecmp wcstol wcstoll wcslen wcsstr wcstoul wcstoull wcstod wcstof])
AC_CHECK_FUNCS([wcstombs mbstowcs wcsrtombs wcsnrtombs mbsrtowcs mbsnrtowcs])
AC_CHECK_FUNCS([wprintf fwprintf swprintf vwprintf vfwprintf vswprintf])
AC_CHECK_FUNCS([vsnprintf])
AC_CHECK_FUNCS([wstol wstoll watoi watoll wstod watof])
AC_CHECK_FUNCS([poll select pselect])
AC_CHECK_FUNCS([opendir closedir readdir readdir_r stat])
AC_CHECK_FUNCS([clock_gettime])

# Resolver
AM_CHECK_LIBBIND()
report_have_libbind="no"
if test "$have_res_query" = "yes"
then
	AC_DEFINE(HAVE_LIBBIND,1,[ Define if you have libbind resolver functions ])
	report_have_libbind="yes"
fi


LDFLAGS="$LDFLAGS -L/usr/local/lib"
CFLAGS="$CFLAGS -I/usr/local/include"



##################################################################################### JPEG
AC_CHECK_LIBJPEG([6.0],
            AC_DEFINE(HAVE_JPEG, 1, [ Define if you have libjpeg. ])
            report_have_jpeg="yes"
           :)
      
##################################################################################### PNG   
AC_CHECK_LIBPNG([1.2],
            AC_DEFINE(HAVE_PNG, 1, [ Define if you have libpng. ])
            report_have_png="yes"
           :)
      

AC_CHECK_LIBTIFF([1.2],
            AC_DEFINE(HAVE_TIFF, 1, [ Define if you have libtiff. ])
            report_have_libtiff="yes"
            AC_SUBST(TIFF_CFLAGS)
			AC_SUBST(TIFF_LIBS)
           :)

AM_ICONV
if test "$am_cv_func_iconv_works" = "yes" 
then
	report_have_iconv="yes"
	ICONV_CFLAGS=$INCICONV
	ICONV_LIBS=$LIBICONV
	AC_SUBST(ICONV_CFLAGS)
	AC_SUBST(ICONV_LIBS)
fi


# Bzip2 Vorhanden?
AM_BZIP2
AX_CHECK_ZLIB(report_have_zlib="yes"
	AC_DEFINE([HAVE_LIBZ], [1], [Define to 1 if you have `z' library (-lz)])
	ZLIB_CFLAGS="-I${ZLIB_HOME}/include"
	ZLIB_LIBS="-L${ZLIB_HOME}/li -lz"
	AC_SUBST(ZLIB_CFLAGS)
	AC_SUBST(ZLIB_LIBS)
)


dnl do we need to specify -lm explicitly?
AC_CHECK_FUNC(sin,,[AC_CHECK_LIB(m,sin)])

# PCRE vorhanden?
AX_PATH_LIB_PCRE(
            AC_DEFINE(HAVE_PCRE, 1, [ Define if you have pcre. ])
            report_have_pcre="yes"
            ,
            )
       
AC_CHECK_FT2(9.0.0,
	AC_DEFINE(HAVE_FREETYPE2, 1, [ Define if you have freetype2. ])
	report_have_freetype2="yes"
	, :)

dnl Check for nasm
AC_MSG_NOTICE(checking if we can use assembly)
use_asm="none"
report_have_nasm="no"
case $target in
	i?86*|x86*|amd64*)
		AC_MSG_NOTICE(supported platform for assembly)
		AC_CHECK_NASM(2.0.0,
			report_have_nasm="yes"
		)
		;;
esac		

if test $report_have_nasm = "yes"
then
	use_asm="nasm"
	AC_DEFINE(HAVE_NASM,1,)
	AC_DEFINE(HAVE_X86_ASSEMBLER,1,)
	HAVE_X86_ASSEMBLER="ASM_RELEASE	=	\$(ASM_OBJ_RELEASE)
ASM_DEBUG	=	\$(ASM_OBJ_DEBUG)"
	AC_SUBST(HAVE_X86_ASSEMBLER)
	ASM=${NASM}
	ASMFLAGS=${NASMFLAGS}
	ASMDEBUGFLAGS=${NASMDEBUGFLAGS}
	AC_SUBST(ASM)
	AC_SUBST(ASMFLAGS)
	AC_SUBST(ASMDEBUGFLAGS)
fi

SRCDIR=`pwd`
AC_SUBST(SRCDIR)


 
 
# If we support __thread, that can speed up tcmalloc a bit.
# Note, however, that our code tickles a bug in gcc < 4.1.2
# involving TLS and -fPIC (which our libraries will use) on x86:
#   http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
AC_MSG_CHECKING([for __thread])
AC_LINK_IFELSE([AC_LANG_PROGRAM([#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)) && ((__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 1) || (__GNUC__ == 4 && __GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ < 2))
#error gcc has this bug: http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
#endif], [static __thread int p = 0])],
               [AC_DEFINE(HAVE_TLS, 1,
                      Define to 1 if compiler supports __thread)
                AC_MSG_RESULT([yes])],
               [AC_MSG_RESULT([no])])

# We also need to check if the kernel supports __thread, which requires uname()
AC_CHECK_DECLS(uname,,, [#include <sys/utsname.h>])
 

case "$host_os" in
	*mingw32* )
		MINGW32=yes
		AC_DEFINE(MINGW32, 1, [Define if you compile on Windows MINGW32])
  		;;
	* )
		MINGW32=no
		;;
esac

case "${host_cpu}-${host_os}" in
	*solaris*)
		CFLAGS=`echo $CFLAGS | $SED 's/-I\/usr\/local\/include //g' | $SED 's/-I\/usr\/include //g' `
        ;;
esac;


###############################################################################
# Features from ppl7, which we don't need/want
###############################################################################
AC_SUBST(LIBMICROHTTPD_CFLAGS)
AC_SUBST(LIBMICROHTTPD_LIBS)
AC_SUBST(LIBIDN_CFLAGS)
AC_SUBST(LIBIDN_LIBS)
AC_SUBST(MPG123_CFLAGS)
AC_SUBST(MPG123_LIBS)
AC_SUBST(LAME_CFLAGS)
AC_SUBST(LAME_LIBS)
AC_SUBST(OGG_CFLAGS)
AC_SUBST(OGG_LIBS)
AC_SUBST(SHOUT_CFLAGS)
AC_SUBST(SHOUT_LIBS)
AC_SUBST(SDL2_CFLAGS)
AC_SUBST(SDL2_LIBS)
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)
AC_SUBST(IMLIB_CFLAGS)
AC_SUBST(IMLIB_LIBS)
AC_SUBST(FREETDS_CFLAGS)
AC_SUBST(FREETDS_LIBS),
AC_SUBST(SQLITE3_CFLAGS),
AC_SUBST(SQLITE3_LIBS),
AC_SUBST(SQLITE3_LDFLAGS),
AC_SUBST(MYSQL_CFLAGS),
AC_SUBST(MYSQL_LIBS),
AC_SUBST(MYSQL_LDFLAGS),
AC_SUBST(POSTGRESQL_CFLAGS),
AC_SUBST(POSTGRESQL_LIBS),
AC_SUBST(POSTGRESQL_LDFLAGS),
AC_SUBST(LIBLDNS_CFLAGS)
AC_SUBST(LIBLDNS_LIBS)
AC_SUBST(LIBMCRYPT_CFLAGS)
AC_SUBST(LIBMCRYPT_LIBS)
AC_SUBST(LIBMHASH_LIBS)
AC_SUBST(LIBMHASH_CFLAGS)
AC_SUBST(OPENSSL_INCLUDES)
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)
AC_SUBST(OPENSSL_LDFLAGS)

			
###############################################################################
# Export variables from feature check
###############################################################################
AC_SUBST(report_have_zlib)
AC_SUBST(report_have_bzip2)
AC_SUBST(report_have_png)
AC_SUBST(report_have_jpeg)
AC_SUBST(report_have_freetype2)
AC_SUBST(report_have_sdl1)
AC_SUBST(report_have_sdl2)
AC_SUBST(report_have_lame)
AC_SUBST(report_have_libmad)
AC_SUBST(report_have_curl)
AC_SUBST(report_have_openssl)
AC_SUBST(report_have_pcre)
AC_SUBST(report_have_iconv)
AC_SUBST(report_have_libmcrypt)
AC_SUBST(report_have_libmhash)
AC_SUBST(report_have_mysql)
AC_SUBST(report_have_postgres)
AC_SUBST(report_have_freetds)
AC_SUBST(report_have_nasm)
AC_SUBST(report_have_pthreads)
AC_SUBST(report_have_ldns)
AC_SUBST(report_have_libogg)
AC_SUBST(report_have_libshout)
AC_SUBST(report_have_libimlib)
AC_SUBST(report_have_libtiff)
AC_SUBST(report_have_libmicrohttpd)
AC_SUBST(report_have_libidn)
AC_SUBST(report_have_libbind)

###############################################################################
# Output
###############################################################################

AC_OUTPUT


###############################################################################
# Report
###############################################################################

dnl report configuration
AC_MSG_RESULT([
******************************************************************************
** Configuration summary for $PACKAGE_NAME Version $PACKAGE_VERSION
** Target: $target_os        CPU: $target_cpu
** 
** Byteorder: $report_byteorder
**   Size of char         $ac_cv_sizeof_char     Size of float        $ac_cv_sizeof_float
**   Size of short        $ac_cv_sizeof_short     Size of double       $ac_cv_sizeof_double
**   Size of int          $ac_cv_sizeof_int     Size of void*        $ac_cv_sizeof_voidp
**   Size of long         $ac_cv_sizeof_long     Size of wchar_t      $ac_cv_sizeof_wchar_t
**   Size of long long    $ac_cv_sizeof_long_long     
**   Size of size_t       $ac_cv_sizeof_size_t
******************************************************************************
** Assembler:
**   Nasm (x86 assembler): $report_have_nasm
**
** Threads:                $report_have_pthreads
**
** Compression:
**   ZLIB library:         $report_have_zlib
**   BZIP2 library:        $report_have_bzip2
**
** Graphic:
**   BMP:                  yes (buildin)
**   GIF:                  yes (buildin)
**   TGA:                  yes (buildin)
**   PPM:                  yes (buildin)
**   PNG library:          $report_have_png
**   JPEG library:         $report_have_jpeg
**   Freetype 2.x library: $report_have_freetype2
**
** Other:
**   PCRE:                 $report_have_pcre
**   ICONV:                $report_have_iconv
**
** Testing:
**   Unittests:            $report_have_gtest
**   Codecoverage:         $report_codecoverage_enabled
******************************************************************************
])
