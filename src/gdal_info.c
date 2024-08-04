#include "tidygdal.h"

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
