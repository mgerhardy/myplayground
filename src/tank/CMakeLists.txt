cmake_minimum_required(VERSION 2.8)
get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

set(FIPS_EXCEPTIONS ON)

include_directories(src)
include_directories(src/modules)

fips_setup()
fips_project(tank)
fips_add_subdirectory(src)
fips_finish()
