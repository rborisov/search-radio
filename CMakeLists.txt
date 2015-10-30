cmake_minimum_required(VERSION 2.6)

project (search)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "2")

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Sqlite3 REQUIRED)
find_package(CURL REQUIRED)
find_package(JSON REQUIRED)

include_directories(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
    ${SQLITE3_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

link_directories(${SQLITE3_LIBRARY_DIRS}
    ${CURL_LIBRARY_DIRS} ${JSON_LIBRARY_DIRS})

include(CheckCSourceCompiles)

set(CMAKE_C_FLAGS "-std=gnu99 -Wall") 
set(CMAKE_C_FLAGS_DEBUG "-ggdb -pedantic")

get_target_property(MKDATA_EXE mkdata LOCATION)

set(SOURCES
    main.c
    json_parser.c
)

add_executable(search ${SOURCES})
target_link_libraries(search ${CMAKE_THREAD_LIBS_INIT}  
    ${SQLITE3_LIBRARIES} ${JSON_LIBRARIES}
    ${CURL_LIBRARIES})

install(TARGETS search DESTINATION bin)
