# Enable ExternalProject CMake module
include(ExternalProject)

set(RAPIDJSON_EXTRA_PARAMS "-DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF RAPIDJSON_BUILD_TESTS=OFF")

# Add rapidjson
externalproject_add(
	rapidjson
	URL https://github.com/miloyip/rapidjson/archive/master.zip
	PREFIX ${CMAKE_BINARY_DIR}/rapidjson
	CMAKE_ARGS ${RAPIDJSON_EXTRA_PARAMS}
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
)

# Specify include dir
externalproject_get_property(rapidjson source_dir)
set(RAPIDJSON_INCLUDE_DIR ${source_dir}/include)
