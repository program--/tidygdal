#' @export
gdal_config_set <- function(key, value) {
  invisible(.Call(gdal_c_set_config_option, key, value))
}

#' @export
gdal_config_get <- function() {
  opts <- .Call(gdal_c_get_config_options)

  if (length(opts) > 0) {
    opts <- strsplit(opts, "=", fixed = TRUE)
    names(opts) <- vapply(opts, "[", character(1), 1)
    opts <- vapply(opts, "[", character(1), 2)
  }

  opts
}
