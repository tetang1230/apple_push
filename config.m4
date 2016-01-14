dnl $Id$
dnl config.m4 for extension apple_push

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(apple_push, for apple_push support,
dnl Make sure that the comment is aligned:
dnl [  --with-apple_push             Include apple_push support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(apple_push, whether to enable apple_push support,
[  --enable-apple_push           Enable apple_push support])
dnl Make sure that the comment is aligned:
dnl [  --enable-apple_push           Enable apple_push support])

if test "$PHP_APPLE_PUSH" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-apple_push -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/apple_push.h"  # you most likely want to change this
  dnl if test -r $PHP_APPLE_PUSH/$SEARCH_FOR; then # path given as parameter
  dnl   APPLE_PUSH_DIR=$PHP_APPLE_PUSH
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for apple_push files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       APPLE_PUSH_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$APPLE_PUSH_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the apple_push distribution])
  dnl fi

  dnl # --with-apple_push -> add include path
  dnl PHP_ADD_INCLUDE($APPLE_PUSH_DIR/include)

  dnl # --with-apple_push -> check for lib and symbol presence
  dnl LIBNAME=apple_push # you may want to change this
  dnl LIBSYMBOL=apple_push # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $APPLE_PUSH_DIR/$PHP_LIBDIR, APPLE_PUSH_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_APPLE_PUSHLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong apple_push lib version or lib not found])
  dnl ],[
  dnl   -L$APPLE_PUSH_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  PHP_SUBST(APPLE_PUSH_SHARED_LIBADD)
  PHP_NEW_EXTENSION(apple_push, apple_push.c, $ext_shared)
fi
