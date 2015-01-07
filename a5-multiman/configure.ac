#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([multiman], [1.0.0], [[kai.selgrad@cs.fau.de]])
AM_INIT_AUTOMAKE

AC_DEFUN([AX_STDCXX11_FLAG], [
  AC_CACHE_CHECK(if we should use gnu++0x or gnu++11 for c++11 support,
  ac_cv_cxx_cxx11_spec,
  [
  	AC_LANG_SAVE
  	AC_LANG_CPLUSPLUS
  	ac_save_CXXFLAGS="$CXXFLAGS"
  	CXXFLAGS="$CXXFLAGS -std=gnu++11"
  	AC_TRY_COMPILE(
		[
  		],, ac_cv_cxx_cxx11_spec_use_11=yes, ac_cv_cxx_cxx11_spec_use_11=no)
  	CXXFLAGS="$ac_save_CXXFLAGS"
  	CXXFLAGS="$CXXFLAGS -std=gnu++0x"
  	AC_TRY_COMPILE(
		[
  		],,
  		ac_cv_cxx_cxx11_spec_use_0x=yes, ac_cv_cxx_cxx11_spec_use_0x=no)
  	CXXFLAGS="$ac_save_CXXFLAGS"
  	AC_LANG_RESTORE

  	if test "$ac_cv_cxx_cxx11_spec_use_11" = yes ; then
  		ac_cv_cxx_cxx11_spec="-std=gnu++11";
  	else if test "$ac_cv_cxx_cxx11_spec_use_0x" = yes ; then
  		ac_cv_cxx_cxx11_spec="-std=gnu++0x";
  	else
  	  AC_MSG_ERROR("Your compiler accepts neither -std=gnu++0x nor -std=gnu++11!")
  	fi fi
  ])])

# name one of your source files here
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile src/Makefile)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99

AX_STDCXX11_FLAG
CXXFLAGS="$CXXFLAGS $ac_cv_cxx_cxx11_spec"


# when using flex/bison:
# AC_PROG_LEX
# AC_PROG_YACC

# when building libraries
# AC_PROG_LIBTOOL

# Checks for libraries.

GUILE_FLAGS

AC_CHECK_LIB([mcm], [make_vec3f])
AC_SEARCH_LIBS(make_mesh, cgl)
AC_CHECK_LIB([GL], [main])
AC_CHECK_LIB([GLU], [main])
AC_CHECK_LIB([glut], [main])
#AC_CHECK_LIB([GLEW], [main])
#AC_SEARCH_LIBS(__glewDrawElementsBaseVertex, GLEW)
AC_CHECK_LIB([png], [main])
AC_CHECK_LIB([pthread], [main]) # required for boost on murillo.

PKG_CHECK_MODULES(GLEW, glew)
AC_SUBST([GLEW_CFLAGS])
AC_SUBST([GLEW_LIBS])

PKG_CHECK_MODULES(CGL, libcgl)
AC_SUBST([CGL_CFLAGS])
AC_SUBST([CGL_LIBS])

#PKG_CHECK_MODULES(CCGL, libccgl)
#AC_SUBST([CCGL_CFLAGS])
#AC_SUBST([CCGL_LIBS])

#AX_BOOST_BASE([1.40])
#AX_BOOST_SYSTEM([1.40])
#AX_BOOST_ASIO([1.40])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
