PHP_ARG_ENABLE(pilouloader, whether to enable My Extension support,
    [ --enable-pilouloader    Enable My Extension support])
if test "$PHP_PILOULOADER" = "yes"; then
    AC_MSG_CHECKING([for pkg-config ])
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
    if test "$PHPIZE" = "no"; then
        AC_MSG_ERROR([pkg-config is required but not found.])
    fi
    AC_MSG_CHECKING([for libsodium])
    if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists libsodium; then
      LIBSODIUM_CFLAGS=`$PKG_CONFIG libsodium --cflags`
      LIBSODIUM_LIBS=`$PKG_CONFIG libsodium --libs`
      LIBSODIUM_VERSION=`$PKG_CONFIG --modversion libsodium`
      AC_MSG_RESULT(version $LIBSODIUM_VERSION found using pkg-config)
      PHP_EVAL_LIBLINE($LIBSODIUM_LIBS, SODIUM_SHARED_LIBADD)
      PHP_EVAL_INCLINE($LIBSODIUM_CFLAGS)
    fi
    AC_DEFINE(HAVE_PILOULOADER 1, [Whether you have My Extension])
    PHP_EVAL_INCLINE($LIBSODIUM_CFLAGS)
  PHP_EVAL_LIBLINE($LIBSODIUM_LIBS, PILOULOADER_SHARED_LIBADD)
  PHP_SUBST(PILOULOADER_SHARED_LIBADD)
PHP_ADD_EXTENSION_DEP(pilouloader, json, true)
    PHP_NEW_EXTENSION(pilouloader, [ src/pilouloader.c  src/piloucustomhandler.c src/pilousignature.c], $ext_shared)
    
fi