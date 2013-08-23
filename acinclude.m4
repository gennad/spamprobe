# $Id: 10_configure.in_db4.2.dpatch 8 2004-04-02 09:11:24Z bsb $
#
# Written for the Debian spamprobe package by
#
#   Bernd S. Brentrup <bsb@debian.org>
#
# Copyright (c) 2004 Software in the Public Interest
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# As a special exception, the Free Software Foundation gives unlimited
# permission to copy, distribute and modify the configure scripts that
# are the output of Autoconf.  You need not follow the terms of the GNU
# General Public License when using or distributing such scripts, even
# though portions of the text of Autoconf appear in them.  The GNU
# General Public License (GPL) does govern all other use of the material
# that constitutes the Autoconf program.
#
# Certain portions of the Autoconf source text are designed to be copied
# (in certain cases, depending on the input) into the output of
# Autoconf.  We call these the "data" portions.  The rest of the Autoconf
# source text consists of comments plus executable code that decides which
# of the data portions to output in any given case.  We call these
# comments and executable code the "non-data" portions.  Autoconf never
# copies any of the non-data portions into its output.
#
# This special exception to the GPL applies to versions of Autoconf
# released by the Free Software Foundation.  When you make and
# distribute a modified version of Autoconf, you may extend this special
# exception to the GPL to apply to your modified version as well, *unless*
# your modified version has the potential to copy into its output some
# of the text that was the non-data portion of the version that you started
# with.  (In other words, unless your change moves or copies text from
# the non-data portions to the data portions.)  If your modification has
# such potential, you must delete any notice of this special exception
# to the GPL from your modified version.

#--------------------------------------------------------------------------
# DAC_SEARCH_LIBS(NAME, PROGRAM, LIBRARIES, 
#                 [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [MORE-LIBS])
# 
# This macro has been trivially adopted from the AC_SEARCH_LIBS macro
# distibuted with GNU autoconf, providing a more flexible and robust
# method to select a needed library from a list of candidate
# libraries.
#
# DAC_SEARCH_LIBS tries to link the user-supplied PROGRAM against each
# library listed in the LIBRARIES argument (only base portions of
# library names, i.e. for libXYZ.{a,so} use XYZ.  
# ACTION-IF-FOUND is invoked when PROGRAM can be linked, either
# without using any entry from LIBRARIES or for the first successful
# one, in the latter case -lXYZ is prepended to the global LIBS
# variable.  The MORE-LIBS argument can be used to provide additional
# libraries needed to link PROGRAM.
# The NAME argument is used to differentiate between multiple invocations
# of DAC_SEARCH_LIBS
AC_DEFUN([DAC_SEARCH_LIBS],
  [AC_CACHE_CHECK([for $1 library], [dac_cv_search_$1],
    [dac_save_LIBS=$LIBS
    dac_cv_search_$1=no
    AC_LANG_CONFTEST($2)
    AC_LINK_IFELSE([],dac_cv_search_$1="present")
    if test "$dac_cv_search_$1" = no; then
      for dac_lib in $3 ; do
        LIBS="-l$dac_lib $6 $dac_save_LIBS"
        AC_LINK_IFELSE([],dac_cv_search_$1="-l$dac_lib" break)
      done
    fi
    LIBS=$dac_save_LIBS])
    AS_IF([test "$dac_cv_search_$1" != no],
          [test "$dac_cv_search_$1" = "present" || LIBS="$dac_cv_search_$1 $LIBS" ; $4],
          [$5])dnl
  ])

