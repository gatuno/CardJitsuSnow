AC_INIT([Card-Jitsu Snow], [0.1.0], [], [card_jitsu_snow])

AC_REVISION([Revision 1])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_SRCDIR([src/snow.c])

# For debian systems, /usr as default
AC_PREFIX_DEFAULT([/usr])

# We need Gcc
AC_PROG_CC

# and automake
AM_INIT_AUTOMAKE([-Wall -Werror])

# Translate this program
AM_GNU_GETTEXT_VERSION([0.19.3])
AM_GNU_GETTEXT([external])

ALL_LINGUAS=""
AC_SUBST(ALL_LINGUAS)

AM_PROG_CC_C_O

# Check for pkg-config
PKG_PROG_PKG_CONFIG

SDL2_VERSION=2.0.0
SDL2_IMAGE_VERSION=2.0.0
SDL2_TTF_VERSION=2.0.12
SDL2_MIXER_VERSION=2.0.0

AC_MSG_CHECKING([if you have SDL2 installed on your system])
PKG_CHECK_EXISTS([sdl2 >= $SDL2_VERSION], [AC_MSG_RESULT([yes])], [AC_MSG_FAILURE([SDL2 not found in your system])])
PKG_CHECK_MODULES(SDL, [sdl2 >= $SDL2_VERSION], [], [])

AC_MSG_CHECKING([if you have SDL2_image installed on your system])
PKG_CHECK_EXISTS([SDL2_image >= $SDL2_IMAGE_VERSION], [AC_MSG_RESULT([yes])], [AC_MSG_FAILURE([SDL2_image not found in your system])])
PKG_CHECK_MODULES(SDL_image, [SDL2_image >= $SDL2_IMAGE_VERSION], [], [])

AC_MSG_CHECKING([if you have SDL2_ttf installed on your system])
PKG_CHECK_EXISTS([SDL2_ttf >= $SDL2_TTF_VERSION], [AC_MSG_RESULT([yes])], [AC_MSG_FAILURE([SDL2_ttf not found in your system])])
PKG_CHECK_MODULES(SDL_ttf, [SDL2_ttf >= $SDL2_TTF_VERSION], [], [])

AC_MSG_CHECKING([if you have SDL2_mixer installed on your system])
PKG_CHECK_EXISTS([SDL2_mixer >= $SDL2_MIXER_VERSION], [AC_MSG_RESULT([yes])], [AC_MSG_FAILURE([SDL2_mixer not found in your system])])
PKG_CHECK_MODULES(SDL_mixer, [SDL2_mixer >= $SDL2_MIXER_VERSION], [], [])

AC_CONFIG_HEADERS([config.h])

AC_CHECK_TOOL(WINDRES, windres)

# Revisar el host
AC_CANONICAL_HOST

case $host_os in
  *mingw32* ) MINGW32=yes;;
          * ) MINGW32=no;;
esac
case $host_os in
  *cygwin* ) CYGWIN=yes;;
         * ) CYGWIN=no;;
esac
case $host_os in
  *linux* ) LINUX=yes;;
        * ) LINUX=no;;
esac
case $host_os in
 *darwin* ) MACOSX=yes;;
       * ) MACOSX=no;;
esac

AM_CONDITIONAL(MINGW32, test x$MINGW32 = xyes)
AM_CONDITIONAL(LINUX, test x$LINUX = xyes)
AM_CONDITIONAL(MACOSX, test x$MACOSX = xyes)

dnl ----------------------------------------
dnl Mac requires libintl to be linked statically and doesn't make that easy
dnl gettext project (for libintl) refuses to support use of pkg-config but doesn't provide an
dnl equivalent way to get the lib directory it is installed in. This requires use of --with-libintl-prefix configure option.
dnl ---------------------------------------
if test "x$MACOSX" = xyes; then
  LIBS="$LIBS_NOGETTEXT"
  if test -z "$with_libintl_prefix" ; then
    AC_MSG_ERROR([On MacOS --with-libintl-prefix must be specified to find lib/libintl.a for static link])
  fi
  if test -d "${with_libintl_prefix}/lib" && test -f "${with_libintl_prefix}/lib/libintl.a" ; then
    LIBINTL="${with_libintl_prefix}/lib/libintl.a"
    LIBS="$LIBINTL /usr/lib/libiconv.dylib $LIBS"
  else
    AC_MSG_ERROR([The specified --with-libintl-prefix did not contain lib/libintl.a which is required on MacOS for static link])
  fi
fi

dnl Add -DMACOSX to CXXFLAGS and CFLAGS if working under darwin
if test "x$MACOSX" = xyes; then
 CPPFLAGS="$CPPFLAGS -DMACOSX"
fi

dnl Add special hack for Mac framework SDL after all other SDL stuff has been done
dnl Note that SDL development environment must be installed for the build but SDL frameworks must also be installed for the build
dnl If we had libSDLmain.a, which is not included in the framework, we should be able to get away with only installing framework
if test "x$MACOSX" = xyes; then
 dnl AM_PATH_SDL(1.2.0)
 SDL_STATIC_PREFIX=`$SDL_CONFIG $sdl_config_args --prefix`
 SDL_LIBS="-Wl,-rpath,@loader_path/../Frameworks -framework AudioToolbox -framework AudioUnit -framework Cocoa -framework CoreAudio -framework IOKit -framework CoreFoundation -framework Carbon -framework CoreServices -framework ApplicationServices -framework Foundation -framework AppKit -framework OpenGL -framework SDL -framework SDL_mixer -framework SDL_image -framework SDL_ttf $SDL_STATIC_PREFIX/lib/libSDLmain.a"
AC_SUBST(SDL_STATIC_PREFIX)
fi

AC_CONFIG_FILES([ po/Makefile.in
                 Makefile
                 src/Makefile
                 data/Makefile
                 etc/Makefile
                 etc/Info.plist
                 data/server/Makefile
])

AC_OUTPUT
