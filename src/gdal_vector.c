#include "tidygdal.h"

#include <gdal.h>
#include <ogr_srs_api.h>
#include <gdal_utils.h>

OGRLayerH gdal_c_get_layer(SEXP externalPtr, SEXP layer) {
    GDALDatasetH ds = NULL; 
    GDAL_R_UNWRAP_OR_NULL(ds, externalPtr);
    
    OGRLayerH l = GDALDatasetGetLayerByName(ds, CHAR(STRING_ELT(layer, 0)));
    if (l == NULL) {
        Rf_error("failed to get layer %s", CHAR(STRING_ELT(layer, 0)));
        return R_NilValue;
    }

    return l;
}

SEXP gdal_c_is_vector(SEXP externalPtr) {
    return Rf_ScalarLogical(GDALDatasetGetLayerCount(GDAL_R_UNWRAP(externalPtr)) > 0);
}

SEXP gdal_c_vector_layer_geomtype(SEXP externalPtr, SEXP layer)
{
    OGRLayerH l = gdal_c_get_layer(externalPtr, layer);
    return Rf_mkString(OGRGeometryTypeToName(OGR_L_GetGeomType(l)));
}

SEXP gdal_c_vector_layer_feature_count(SEXP externalPtr, SEXP layer)
{
    OGRLayerH l = gdal_c_get_layer(externalPtr, layer);
    return Rf_ScalarReal(OGR_L_GetFeatureCount(l, 0));
}

SEXP gdal_c_vector_layer_field_count(SEXP externalPtr, SEXP layer) {
    OGRLayerH l = gdal_c_get_layer(externalPtr, layer);
    OGRFeatureDefnH defn = OGR_L_GetLayerDefn(layer);
    if (defn == NULL) {
        Rf_error("failed to get layer %s defn\n", CHAR(STRING_ELT(layer, 0)));
        return R_NilValue;
    }

    return Rf_ScalarInteger(OGR_FD_GetFieldCount(defn));
}

SEXP gdal_c_vector_layer_srs_name(SEXP externalPtr, SEXP layer) {
    OGRLayerH l = gdal_c_get_layer(externalPtr, layer);
    OGRSpatialReferenceH srs = OGR_L_GetSpatialRef(layer);
    if (layer == NULL) {
        Rf_error("failed to get layer %s spatial reference\n", CHAR(STRING_ELT(layer, 0)));
        return R_NilValue;
    }

    return Rf_mkString(OSRGetName(srs));
}

SEXP gdal_c_vector_layers_info(SEXP externalPtr) {

    static const char* list_names[] = {"layer_name", "geometry_type", "features", "fields", "crs_name", ""};

    GDALDatasetH ds = GDAL_R_UNWRAP(externalPtr);
    if (ds == NULL) {
        return R_NilValue;
    }

    int nlayers = GDALDatasetGetLayerCount(ds);
    if (nlayers == 0) {
        return R_NilValue;
    }

    // Allocate a list with n elements.
    SEXP output = PROTECT(Rf_allocVector(VECSXP, nlayers));

    for (int i = 0; i < nlayers; ++i) {
        OGRLayerH layer = GDALDatasetGetLayer(ds, i);
        if (layer == NULL) {
            Rf_error("failed to get layer %d\n", i);
            return R_NilValue;
        }

        OGRFeatureDefnH defn = OGR_L_GetLayerDefn(layer);
        if (defn == NULL) {
            Rf_error("failed to get layer %d defn\n", i);
            return R_NilValue;
        }

        OGRSpatialReferenceH srs = OGR_L_GetSpatialRef(layer);
        if (layer == NULL) {
            Rf_error("failed to get layer %d spatial reference\n", i);
            return R_NilValue;
        }

        SEXP element = PROTECT(Rf_mkNamed(VECSXP, list_names));

        /* layer_name */
        SET_VECTOR_ELT(element, 0, Rf_mkString(OGR_L_GetName(layer)));
        /* geom_type */
        SET_VECTOR_ELT(element, 1, Rf_mkString(OGRGeometryTypeToName(OGR_L_GetGeomType(layer))));
        /* nfeatures */
        SET_VECTOR_ELT(element, 2, Rf_ScalarReal(OGR_L_GetFeatureCount(layer, 0)));
        /* nfields */
        SET_VECTOR_ELT(element, 3, Rf_ScalarInteger(OGR_FD_GetFieldCount(defn)));
        /* crs_name */
        SET_VECTOR_ELT(element, 4, Rf_mkString(OSRGetName(srs)));

        SET_VECTOR_ELT(output, i, element);
        UNPROTECT(1);
    }

    UNPROTECT(1);
    return output;
}

SEXP gdal_c_vector_translate(SEXP input, SEXP output, SEXP options) {
    int error = FALSE;
    GDALDatasetH result = NULL;
    GDALDatasetH dst = NULL;
    GDALDatasetH src = NULL;
    const char* dst_path = NULL;
    GDALVectorTranslateOptions* gdal_opts = NULL;
    
    // Handle input
    if (Rf_isS4(input)) {
        // GDALVectorDataset
        src = GDAL_R_UNWRAP(input);
    } else if (Rf_isString(input)) {
        // Input Path
        src = GDALOpenEx(CHAR(STRING_ELT(input, 0)), GDAL_OF_VECTOR, NULL, NULL, NULL);
    } else {
        // Error
        Rf_error("gdal_c_vector_translate: uexpected error when handling input");
        return R_NilValue;
    }

    // Handle output
    if (Rf_isString(output)) {
        // Return Output Path
        dst_path = CHAR(STRING_ELT(output, 0));
    } else if (!Rf_isNull(output)) {
        // Error, something other than NULL or a string was passed
        Rf_error("gdal_c_vector_translate: uexpected error when handling output");
        return R_NilValue;
    } else {
        GDALDriverH drv = GDALGetDriverByName("GPKG");
        if (drv == NULL) {
            Rf_error("gdal_c_vector_translate: unable to create in-memory dataset, GeoPackage driver not available");
            return R_NilValue;
        }

        dst = GDALCreate(drv, R_tmpnam2("tidygdal", "/vsimem", ".gpkg"), 0, 0, 0, GDT_Unknown, NULL);
        if (dst == NULL) {
            Rf_error("gdal_c_vector_translate: failed to create in-memory dataset");
            return R_NilValue;
        }
    }

    // Handle Options
    if (!Rf_isNull(options)) {
        R_xlen_t noptions = Rf_xlength(options);
        char* gdal_opts_str[noptions + 1];
        gdal_opts_str[noptions] = NULL;
        for (R_xlen_t i = 0; i < noptions; ++i) {
            gdal_opts_str[i] = Rf_acopy_string(CHAR(STRING_ELT(options, i)));
        }

        gdal_opts = GDALVectorTranslateOptionsNew(gdal_opts_str, NULL);
        if (gdal_opts == NULL) {
            Rf_error("unable to process arguments\n");
        }
    }

    result = GDALVectorTranslate(dst_path, dst, 1, &src, gdal_opts, &error);

    if (result == NULL) {
        Rf_error("GDALVectorTranslate returned error: %s", CPLGetLastErrorMsg());
        return R_NilValue;
    }

    if (dst_path == NULL) {
        // Return GDAL Dataset
        SEXP ptr = PROTECT(R_MakeExternalPtr(result, R_NilValue, R_NilValue));
        R_RegisterCFinalizerEx(ptr, gdal_c_release_dataset, TRUE);
        UNPROTECT(1);
        return ptr;
    }

    GDALClose(result);

    // Return file path
    return output;
}
