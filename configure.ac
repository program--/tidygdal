dnl Adapted primarily from `sf` <https://github.com/r-spatial/sf>

AC_INIT
AC_CONFIG_SRCDIR([src/gdal_info.c])

: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
    echo "could not determine R_HOME"
    exit 1
fi

R_BIN="${R_HOME}/bin/R"
RSCRIPT_BIN="${R_HOME}/bin/Rscript"

AC_MSG_CHECKING([R version > 3.3.0])

R_VERSION=`${RSCRIPT_BIN} -e 'cat(base::version$major, base::version$minor, sep=".")'`
R_VERSION_MAJOR=`echo ${R_VERSION} | cut  -f1 -d"."`
R_VERSION_MINOR=`echo ${R_VERSION} | cut  -f2 -d"."`
R_VERSION_PATCH=`echo ${R_VERSION} | cut  -f3 -d"."`

if test [ "$R_VERSION_MAJOR" = "development" ]; then
    CC=`"${R_BIN}" CMD config CC`
else
    if test [ $R_VERSION_MAJOR -lt 3 ] -o [ $R_VERSION_MAJOR -eq 3 -a $R_VERSION_MINOR -lt 3 ]; then
        AC_MSG_RESULT(no)
        AC_MSG_ERROR([tidygdal is not compatible with R versions before 3.3.0])
    else
        CC=`"${R_BIN}" CMD config CC`
    fi
fi
AC_MSG_RESULT(yes)

: ${CC=`"${R_BIN}" CMD config CC`}
: ${CFLAGS=`"${R_BIN}" CMD config CFLAGS`}
: ${LDFLAGS=`"${R_BIN}" CMD config LDFLAGS`}
AC_MSG_NOTICE([CC: ${CC}])

# ==> GDAL configuration
GDAL_CONFIG="gdal-config"
GDAL_CONFIG_SET="no"
AC_ARG_WITH([gdal-config],
    AS_HELP_STRING([--with-gdal-config=GDAL_CONFIG],[the location of gdal-config]),
           [gdal_config=$withval])

if test [ -n "$gdal_config" ] ; then
    GDAL_CONFIG_SET="yes"
    AC_SUBST([GDAL_CONFIG],["${gdal_config}"])
    AC_MSG_NOTICE(gdal-config set to $GDAL_CONFIG)
fi

if test ["$GDAL_CONFIG_SET" = "no"] ; then
  AC_PATH_PROG([GDAL_CONFIG], ["$GDAL_CONFIG"],["no"])
  if test ["$GDAL_CONFIG" = "no"] ; then
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([gdal-config not found or not executable.])
  fi
else
  AC_MSG_CHECKING(gdal-config exists)
  if test -r "${GDAL_CONFIG}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([gdal-config not found - configure argument error.])
  fi
  AC_MSG_CHECKING(gdal-config executable)
  if test -x "${GDAL_CONFIG}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([gdal-config not executable.])
  fi
fi

AC_MSG_CHECKING(gdal-config usability)
if test `${GDAL_CONFIG} --version`;
then
	GDAL_CFLAGS=`${GDAL_CONFIG} --cflags`
	GDAL_VERSION=`${GDAL_CONFIG} --version`
	GDAL_LIBS=`${GDAL_CONFIG} --libs`
	GDAL_DEP_LIBS=`${GDAL_CONFIG} --dep-libs`
	GDAL_DATADIR=`${GDAL_CONFIG} --datadir`
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	echo "Error: gdal-config not found"
	echo "The gdal-config script distributed with GDAL could not be found."
	echo "If you have not installed the GDAL libraries, you can"
	echo "download the source from  http://www.gdal.org/"
	echo "If you have installed the GDAL libraries, then make sure that"
	echo "gdal-config is in your path. Try typing gdal-config at a"
	echo "shell prompt and see if it runs. If not, use:"
	echo " --configure-args='--with-gdal-config=/usr/local/bin/gdal-config'"
	echo "with appropriate values for your installation."
	echo ""

	exit 1
fi

AC_MSG_NOTICE([GDAL: ${GDAL_VERSION}])
AC_MSG_CHECKING([GDAL version >= 2.0.1])
GDAL_MAJ_VER=`echo $GDAL_VERSION | cut -d "." -f1`
GDAL_MIN_VER=`echo $GDAL_VERSION | cut -d "." -f2`
GDAL_PATCH_VER=`echo $GDAL_VERSION | cut -d "." -f3`
if test ${GDAL_MAJ_VER} -lt 2 ; then
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([tidygdal is not compatible with GDAL versions below 2.0.1])
else
   AC_MSG_RESULT(yes)
fi

if test [${GDAL_MAJ_VER} -eq 3] -a [${GDAL_MIN_VER} -eq 6] -a [${GDAL_PATCH_VER} -eq 0] ; then
  AC_MSG_ERROR([GDAL version 3.6.0 has been withdrawn, please update GDAL])
fi

INLIBS="${LIBS}"
INCFLAGS="${CFLAGS}"
INPKG_CFLAGS="${PKG_CFLAGS}"
INPKG_LIBS="${PKG_LIBS}"

AC_SUBST([PKG_CFLAGS], ["${INPKG_CFLAGS} ${GDAL_CFLAGS}"])
AC_SUBST([PKG_LIBS], ["${INPKG_LIBS} ${GDAL_LIBS}"])

# honor PKG_xx overrides
# for CFLAGS we will superfluously double R's flags
# since we'll set PKG_CFLAGS with this, but that shouldn't hurt
CFLAGS="${INCFLAGS} ${PKG_CFLAGS}"

gdalok=yes
AC_CHECK_HEADERS(gdal.h,,gdalok=no)
if test "${gdalok}" = no; then
   AC_MSG_ERROR([gdal.h not found in given locations.])
fi

NEED_DEPS=no
LIBS="${INLIBS} ${PKG_LIBS}"
[cat > gdal_test.c <<_EOCONF
#include <gdal.h>
int main(void) {
GDALAllRegister();
}
_EOCONF]

AC_MSG_CHECKING(GDAL: linking with --libs only)
${CC} ${CFLAGS} -o gdal_test gdal_test.c ${LIBS} 2> errors.txt
if test `echo $?` -ne 0 ; then
gdalok=no
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi

if test "${gdalok}" = no; then
AC_MSG_CHECKING(GDAL: linking with --libs and --dep-libs)
LIBS="${LIBS} ${GDAL_DEP_LIBS}"
gdalok=yes
${CC} ${CFLAGS} -o gdal_test gdal_test.c ${LIBS} 2>> errors.txt
if test `echo $?` -ne 0 ; then
gdalok=no
fi
if test "${gdalok}" = yes; then
    NEED_DEPS=yes
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi
fi

if test "${gdalok}" = no; then
   cat errors.txt
   AC_MSG_NOTICE([Install failure: compilation and/or linkage problems.])
   AC_MSG_ERROR([GDALAllRegister not found in libgdal.])
fi

rm -f gdal_test errors.txt gdal_test.c

GDAL_GE_250="no"
GDAL_MAJ_VER=`echo $GDAL_VERSION | cut -d "." -f1`
GDAL_MOD_VER=`echo $GDAL_VERSION | cut -d "." -f2`
if test "${GDAL_MAJ_VER}" = 2 ; then
  if test "${GDAL_MOD_VER}" -ge 5 ; then
    GDAL_GE_250="yes" 
  fi
else 
  if test "${GDAL_MAJ_VER}" -ge 3 ; then
    GDAL_GE_250="yes" 
  fi
fi

GDAL_DATA_TEST_FILE="${GDAL_DATADIR}/pcs.csv"
AC_MSG_CHECKING(GDAL: ${GDAL_DATADIR}/pcs.csv readable)
if test -r "${GDAL_DATA_TEST_FILE}" ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  if test "${GDAL_GE_250}" = "no" ; then
    AC_MSG_ERROR([pcs.csv not found in GDAL data directory.])
  fi
fi

AC_MSG_NOTICE([Package C flags: ${PKG_CFLAGS}])
AC_MSG_NOTICE([Package LIBS: ${PKG_LIBS}])

AC_CONFIG_FILES(src/Makevars)
AC_OUTPUT
