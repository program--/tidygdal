#include "tidygdal.h"

void gdal_c_release_dataset(SEXP externalPtr) {
    GDALDatasetH dataset = GDAL_R_UNWRAP(externalPtr);
    if (dataset != NULL) {
        GDALClose(dataset);
    }
}

SEXP gdal_c_open_dataset(SEXP path, SEXP openFlags) {
    const char* path_ = CHAR(STRING_ELT(path, 0));
    int openFlagsSigned = INTEGER_ELT(openFlags, 0);
    unsigned int openFlags_ = (unsigned int) openFlagsSigned;
    GDALDatasetH dataset = GDALOpenEx(path_, openFlags_, NULL, NULL, NULL);
    SEXP dataset_ptr = PROTECT(R_MakeExternalPtr(dataset, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(dataset_ptr, gdal_c_release_dataset, TRUE);
    UNPROTECT(1);
    return dataset_ptr;
}

SEXP gdal_c_is_raster(SEXP externalPtr) {
    return Rf_ScalarLogical(GDALGetRasterCount(GDAL_R_UNWRAP(externalPtr)) > 0);
}
