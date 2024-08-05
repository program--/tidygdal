#pragma once

#include <gdal.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#define GDAL_R_UNWRAP(xptr) (GDALDatasetH) R_ExternalPtrAddr((xptr))

#define GDAL_R_UNWRAP_OR_NULL(nm, xptr) nm = GDAL_R_UNWRAP((xptr)); if (nm == NULL) return R_NilValue

SEXP gdal_c_version(void);
SEXP gdal_c_build_info(void);
SEXP gdal_c_initialize(void);
void gdal_c_release_dataset(SEXP externalPtr);

/* Common API */
SEXP gdal_c_open_dataset(SEXP path, SEXP openFlags);
SEXP gdal_c_set_config_option(SEXP key, SEXP value);
SEXP gdal_c_get_config_options();


/* Raster API */
SEXP gdal_c_is_raster(SEXP externalPtr);


/* Vector API */
SEXP gdal_c_is_vector(SEXP externalPtr);
SEXP gdal_c_vector_layer_geomtype(SEXP externalPtr, SEXP layer);
SEXP gdal_c_vector_layer_feature_count(SEXP externalPtr, SEXP layer);
SEXP gdal_c_vector_layer_field_count(SEXP externalPtr, SEXP layer);
SEXP gdal_c_vector_layer_srs_name(SEXP externalPtr, SEXP layer);
SEXP gdal_c_vector_layers_info(SEXP externalPtr);
SEXP gdal_c_vector_translate(SEXP input, SEXP output, SEXP options, SEXP quiet);
