setClass("GDALVectorDataset", contains = "GDALDataset")

#' @export
gdal_vector_layers <- function(ds) as.data.frame(.Call(gdal_c_vector_layers_info, ds@pointer))

#' @export
gdal_vector <- function(path) {
  if (length(path) != 1 && !is.character(path)) {
    stop("`path` must be of type [class=character,len=1], not [class=", paste(class(path), sep = "/"), ", len=", length(path), "]")
  }

  methods::new("GDALVectorDataset", pointer = .Call(gdal_c_open_dataset, path, 0x04))
  
}

#' @importMethodsFrom methods show
#' @export
setMethod("show", "GDALDataset", function(object) {
  info <- gdal_vector_layers(object)

  cat(
    "<GDALVectorDataset>\n",
    paste0(
      "* ",
      info$layer_name,
      " (", info$geometry_type, ") [",
      info$features,
      " ",
      ifelse(info$features == 1, "feature", "features"),
      ", ",
      info$fields,
      " ",
      ifelse(info$fields == 1, "field", "fields"),
      ", ",
      info$crs_name,
      "]",
      collapse = "\n"
    ),
    sep = ""
  )
})
