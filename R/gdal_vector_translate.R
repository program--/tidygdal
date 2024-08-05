#' @export
gdal_vector_translate <- function(options, quiet = !interactive()) {
  if (!inherits(options, "GDALVectorTranslateOptions")) {
    stop("`options` must be a GDALVectorTranslateOptions object, not ", paste0(class(options), collapse = "/"))
  }

  if (!quiet) {
    options <- c(options, "-progress")
  }

  result <- .Call(
    gdal_c_vector_translate,
    attr(options, "input", TRUE),
    attr(options, "output", TRUE),
    as.character(options),
    quiet
  )

  if (inherits(result, "externalptr")) {
    result <- methods::new("GDALVectorDataset", pointer = result)
  }

  result
}

#' @export
gdal_vto <- function() structure(character(0), class = "GDALVectorTranslateOptions")

#' @export
format.GDALVectorTranslateOptions <- function(x, ...) {
  input <- attr(x, "input", TRUE)
  if (is.null(input)) {
    input <- "<UNSET> (call `gdal_vto_set_input(...)`)"
  }

  if (methods::is(input, "GDALVectorDataset")) {
    input <- "GDALVectorDataset"
  }

  output <- attr(x, "output", TRUE)
  if (is.null(output)) {
    output <- "<GDALVectorDataset>"
  }

  paste0(
    "<GDALVectorTranslateOptions>\n",
    "Input: ", input, "\n",
    "Output: ", output, "\n",
    "Options: `", paste(x, collapse = " "), "`"
  )
}

#' @export
print.GDALVectorTranslateOptions <- function(x, ...) {
  cat(format(x))
}

#' @export
c.GDALVectorTranslateOptions <- function(vto, ...) {
  res <- c(unclass(vto), ...)
  class(res) <- "GDALVectorTranslateOptions"

  input_attr <- attr(vto, "input", TRUE)
  if (!is.null(input_attr)) {
    attr(res, "input") <- input_attr
  }

  output_attr <- attr(vto, "output", TRUE)
  if (!is.null(output_attr)) {
    attr(res, "output") <- output_attr
  }

  res
}

#' @export
gdal_vto_set_input <- \(vto, input) {
  attr(vto, "input") <- input
  vto
}

#' @export
gdal_vto_set_output <- \(vto, output) {
  attr(vto, "output") <- output
  vto
}

#' @export
gdal_vto_set_input_fmt <- \(vto, format) c(vto, "-if", format)

#' @export
gdal_vto_set_output_fmt <- \(vto, format) c(vto, "-of", format)

#' @export
gdal_vto_set_select <- \(vto, ...) c(vto, "-select", paste0("'", paste(..., sep = ","), "'"))

#' @export
gdal_vto_set_layer_creation_opt <- \(vto, option, value) c(vto, "-lco", paste(option, value, sep = "="))

#' @export
gdal_vto_set_dataset_creation_opt <- \(vto, option, value) c(vto, "-dsco", paste(option, value, sep = "="))

#' @export
gdal_vto_set_file_behavior <- \(vto, behavior = c("overwrite", "append", "upsert")) c(vto, paste0("-", match.arg(behavior)))

#' @export
gdal_vto_set_update <- \(vto) c(vto, "-update")

#' @export
gdal_vto_set_sql <- \(vto, sql) c(vto, "-sql", paste0("'", sql, "'"))

#' @export
gdal_vto_set_skipfailures <- \(vto) c(vto, "-skipfailures")

#' @export
gdal_vto_set_new_layer_name <- \(vto, name) c(vto, "-nln", name)

#' @export
gdal_vto_set_promote_to_multi <- \(vto) c(vto, "-nlt", "PROMOTE_TO_MULTI")

#' @export
gdal_vto_set_convert_to_linear <- \(vto) c(vto, "-nlt", "CONVERT_TO_LINEAR")

#' @export
gdal_vto_set_convert_to_curve <- \(vto) c(vto, "-nlt", "CONVERT_TO_CURVE")

#' @export
gdal_vto_set_coord_dimension <- \(vto, dim = c("XY", "XYZ", "XYM", "XYZM")) c(vto, "-dim", match.arg(dim))

#' @export
gdal_vto_set_source_open_option <- \(vto, option, value) c(vto, "-oo", paste(option, value, sep = "="))

#' @export
gdal_vto_set_target_open_option <- \(vto, option, value) c(vto, "-doo", paste(option, value, sep = "="))

#' @export
gdal_vto_set_make_valid <- \(vto) c(vto, "-makevalid")

#' @export
gdal_vto_set_skip_invalid <- \(vto) c(vto, "-skipinvalid")

#' @export
gdal_vto_set_explode_collections <- \(vto) c(vto, "-explodecollections")

#' @export
gdal_vto_set_attribute_filter <- \(vto, query) c(vto, "-where", paste0("'", query, "'"))

#' @export
gdal_vto_set_spatial_filter <- function(vto, xmin, ymin, xmax, ymax, srs_override = NULL, geom_field = NULL) {
  vto <- c(vto, "-spat", xmin, ymin, xmax, ymax)

  if (!is.null(srs_override)) {
    vto <- c(vto, "-spat_srs", srs_override)
  }

  if (!is.null(geom_field)) {
    vto <- c(vto, "-geomfield", geom_field)
  }

  vto
}

#' @export
gdal_vto_set_new_layer_type <- function(
  vto,
  type = c("NONE", "GEOMETRY", "POINT", "LINESTRING", "POLYGON", "GEOMETRYCOLLECTION",
           "MULTIPOINT", "MULTIPOLYGON", "MULTILINESTRING", "CIRCULARSTRING",
           "COMPOUNDCURVE", "CURVEPOLYGON", "MULTICURVE", "MULTISURFACE")
) c(vto, paste("-nlt", match.arg(type)))

#' @export
gdal_vto_set_target_srs <- function(vto, srs, epoch = NULL) {
  vto <- c(vto, "-t_srs", srs)

  if (!is.null(epoch)) {
    vto <- c(vto, "-t_coord_epoch", epoch)
  }

  vto
}

#' @export
gdal_vto_set_source_srs <- function(vto, srs, epoch = NULL) {
  vto <- c(vto, "-s_srs", srs)

  if (!is.null(epoch)) {
    vto <- c(vto, "-s_coord_epoch", epoch)
  }

  vto
}
