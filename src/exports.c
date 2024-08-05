#include "tidygdal.h"
#include <ogr_core.h>

#define CALL_METHOD_DEFS_BEGIN static const R_CallMethodDef CallEntries[] = {

#define CALL_METHOD_DEF(fname, argc) \
    { (#fname), (DL_FUNC) &(fname), (argc) },

#define CALL_METHOD_DEFS_END {NULL, NULL, 0}};

CALL_METHOD_DEFS_BEGIN
    CALL_METHOD_DEF(gdal_c_initialize, 0)
    CALL_METHOD_DEF(gdal_c_version, 0)
    CALL_METHOD_DEF(gdal_c_build_info, 0)
    CALL_METHOD_DEF(gdal_c_open_dataset, 2)
    CALL_METHOD_DEF(gdal_c_is_raster, 1)
    CALL_METHOD_DEF(gdal_c_is_vector, 1)
    CALL_METHOD_DEF(gdal_c_vector_layers_info, 1)
    CALL_METHOD_DEF(gdal_c_vector_translate, 4)
    CALL_METHOD_DEF(gdal_c_vector_layer_geomtype, 2)
    CALL_METHOD_DEF(gdal_c_vector_layer_feature_count, 2)
    CALL_METHOD_DEF(gdal_c_vector_layer_field_count, 2)
    CALL_METHOD_DEF(gdal_c_vector_layer_srs_name, 2)
    CALL_METHOD_DEF(gdal_c_set_config_option, 2)
    CALL_METHOD_DEF(gdal_c_get_config_options, 0)
CALL_METHOD_DEFS_END

void R_init_tidygdal(DllInfo* dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
