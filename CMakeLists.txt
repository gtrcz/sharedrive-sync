cmake_minimum_required(VERSION 2.8.9)
project	(FileSync)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -std=c++17")


include_directories(
    ${PROJECT_SOURCE_DIR}/inc/
    ${PROJECT_SOURCE_DIR}/lib/json/
    ${PROJECT_SOURCE_DIR}/lib/curl/

)


aux_source_directory(${PROJECT_SOURCE_DIR}/src/ SRCS)

message(STATUS "SRCS:${SRCS}")

#find
find_library(LIB_DEPENDS_PATH_JSONCPP jsoncpp ${PROJECT_SOURCE_DIR}/lib/)
if(NOT LIB_DEPENDS_PATH_JSONCPP)
message(FATAL_ERROR "libjsoncpp not found")
endif(NOT LIB_DEPENDS_PATH_JSONCPP)

find_library(LIB_DEPENDS_PATH_CURL curl ${PROJECT_SOURCE_DIR}/lib/)
if(NOT LIB_DEPENDS_PATH_CURL)
message(FATAL_ERROR "libcurl not found")
endif(NOT LIB_DEPENDS_PATH_CURL)


add_executable(
    FileSync
    ${SRCS}
)


target_link_libraries(
    FileSync 
    ${LIB_DEPENDS_PATH_JSONCPP} 
    ${LIB_DEPENDS_PATH_CURL} 
    
)
