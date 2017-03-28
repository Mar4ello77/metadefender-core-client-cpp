cmake_minimum_required(VERSION 2.8)
project("Metadefender REST lib test")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../CMakeModules)

include(rapidjson)
include_directories(${RAPIDJSON_INCLUDE_DIR})

include(gtest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ../include ${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY_DIR})

add_executable(test-mdrestclient
	def_resp_object_creator.cpp
	json_builder.cpp
	mock_http_client.cpp
	tst_create_session.cpp
	tst_destroy_session.cpp
	tst_exceptions.cpp
	tst_fetch_sanitized_file_by_id.cpp
	tst_fetch_scan_result_by_hash.cpp
	tst_fetch_scan_result_by_id.cpp
	tst_get_engine_info.cpp
	tst_license_info.cpp
	tst_scan_file.cpp
	tst_scan_file_by_path.cpp
	tst_version_info.cpp
	util.cpp
)

add_dependencies(test-mdrestclient googletest rapidjson)

target_link_libraries(test-mdrestclient	
	${GTEST_LIBRARY}
	${GTEST_MAIN_LIBRARY}
)

if (UNIX)
	SET(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
endif (UNIX)
