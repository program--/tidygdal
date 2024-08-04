
<!-- README.md is generated from README.Rmd. Please edit that file -->

# tidygdal

<!-- badges: start -->
<!-- badges: end -->

## Installation

You can install the development version of tidygdal like so:

``` r
pak::pkg_install("program--/tidygdal")
```

## Example

### Opening a vector file

``` r
path <- system.file("testdata", "nc.gpkg", package = "tidygdal")

ds <- tidygdal::gdal_vector(path)
ds
#> <GDALVectorDataset>
#> * nc.gpkg (Multi Polygon) [100 features, 14 fields, NAD27]

# Similar to `sf::st_layers`
tidygdal::gdal_vector_layers(ds)
#>   layer_name geometry_type features fields crs_name
#> 1    nc.gpkg Multi Polygon      100     14    NAD27
```

### Vector Translate (ogr2ogr)

``` r
library(tidygdal)

testdata <- system.file("testdata", package = "tidygdal")

# Convert the test GPKG to a FlatGeoBuf file
# Note: order of these steps does not matter
# Additionally, omitting `gdal_vto_set_output` returns the transformed
# data to an in-memory dataset.
gdal_vto() |>
  gdal_vto_set_input_fmt("GPKG") |>
  gdal_vto_set_output_fmt("FlatGeoBuf") |>
  gdal_vto_set_target_srs("EPSG:5070") |>
  gdal_vto_set_make_valid() |>
  gdal_vto_set_new_layer_name("nc") |>
  gdal_vto_set_input(file.path(testdata, "nc.gpkg")) |>
  gdal_vto_set_output(file.path(testdata, "nc.fgb")) |>
  gdal_vector_translate()
#> [1] "/tmp/Rtmp5JStz5/temp_libpath1e9d271c7b501d/tidygdal/testdata/nc.fgb"
```
