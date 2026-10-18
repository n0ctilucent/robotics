AC_INIT([rover], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

# Initialize both C and C++ compilers
AC_PROG_CC
AC_PROG_CXX

PKG_CHECK_MODULES([GPIOD], [libgpiod >= 1.6])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
