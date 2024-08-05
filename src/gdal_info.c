#include "tidygdal.h"

#include <cpl_conv.h>
#include <cpl_port.h>
#include <cpl_string.h>

SEXP gdal_c_version(void) {
    return Rf_mkString(GDALVersionInfo("--version"));
}

SEXP gdal_c_build_info(void) {
    return Rf_mkString(GDALVersionInfo("BUILD_INFO"));
}

SEXP gdal_c_initialize(void) {
    GDALAllRegister();

    return R_NilValue;
}

SEXP gdal_c_set_config_option(SEXP key, SEXP value) {
    if (!Rf_isString(key)) {
        Rf_error("Argument `key` is not a string");
    }

    if (!Rf_isString(value)) {
        Rf_error("Argument `value` is not a string");
    }

    CPLSetConfigOption(
        Rf_translateCharUTF8(Rf_asChar(key)),
        Rf_translateCharUTF8(Rf_asChar(value))
    );

    return R_NilValue;
}

SEXP gdal_c_get_config_options() {
    CSLConstList opts = CPLGetConfigOptions();
    const int nopts = CSLCount(opts);

    SEXP output = PROTECT(Rf_allocVector(STRSXP, nopts));
    for (int i = 0; i < nopts; ++i)
        SET_STRING_ELT(output, i, Rf_mkChar(opts[i]));

    CPLFree(opts);
    UNPROTECT(1);
    return output;
}
