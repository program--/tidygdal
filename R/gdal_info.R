
#' Runtime GDAL Version
#' @returns Version and release date of linked GDAL library.
#' @export
gdal_version <- function() .Call(gdal_c_version)

#' Runtime GDAL build flags
#' @returns named `list` of flags set when building linked GDAL library.
#' @export
gdal_build_info <- function() {
  flags <- .Call(gdal_c_build_info)

  kv <- strsplit(
    strsplit(flags, "\n", fixed = TRUE)[[1]],
    "=",
    fixed = TRUE
  )

  names(kv) <- lapply(kv, "[", 1)
  lapply(kv, "[", 2)
}
