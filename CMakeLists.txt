CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(towerdefense)

INCLUDE(FindPkgConfig)
INCLUDE(ExternalProject)

#execute_process(COMMAND  xeno-config --prefix OUTPUT_VARIABLE xeno-prefix)
#execute_process(COMMAND  xeno-config --cc OUTPUT_VARIABLE xeno-cc)
#execute_process(COMMAND  xeno-config --skin=native --cflags OUTPUT_VARIABLE xeno-cflags)
#execute_process(COMMAND  xeno-config --skin=native --ldflags OUTPUT_VARIABLE xeno-ldflags)
#MESSAGE(STATUS "Xeno Prefix: " ${xeno-prefix})
#MESSAGE(STATUS "Xeno Prefix: " ${xeno-cc})
#MESSAGE(STATUS "Xeno Prefix: " ${xeno-cflags})
#MESSAGE(STATUS "Xeno Prefix: " ${xeno-ldflags})

SET(CMAKE_C_COMPILER "gcc-5")
SET(CMAKE_CXX_COMPILER "g++-5")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
#SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(cereal_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cereal)
EXTERNALPROJECT_ADD(
        cereal
        PREFIX ${cereal_PREFIX}
        URL "https://github.com/USCiLab/cereal/archive/v1.2.1.tar.gz"
        URL_MD5 "64476ed74c19068ee543b53ad3992261"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND mkdir -p ${cereal_PREFIX}/include/ && cp -r ${cereal_PREFIX}/src/cereal/include/cereal ${cereal_PREFIX}/include/
)
INCLUDE_DIRECTORIES(${cereal_PREFIX}/include)

ADD_SUBDIRECTORY(src)