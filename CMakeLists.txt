cmake_minimum_required(VERSION 3.17.0)
project(simple_rest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDART 17)

# components directory
set(COMPONENTS_DIR ${CMAKE_SOURCE_DIR}/src/components)

# include spdlog
set(SPDLOG_INC ${CMAKE_SOURCE_DIR}/include/spdlog_1.13/include)
include_directories(${SPDLOG_INC})
set(LOGGER_SRC ${COMPONENTS_DIR}/log.hpp
                ${COMPONENTS_DIR}/log.cpp)

# include base64
set(BASE64_SRC ${COMPONENTS_DIR}/base64.h
               ${COMPONENTS_DIR}/base64.cpp)

# repository dir
set(REPOSITORY_DIR ${CMAKE_SOURCE_DIR}/src/repository)

#include class for db connection
set(TDB_SRC ${REPOSITORY_DIR}/tdb.hpp
            ${REPOSITORY_DIR}/tdb.cpp)

# include civetweb
set(CIVETWEB_INC ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/include)
set(CIVETWEB_SRC ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/src)
include_directories(${CIVETWEB_INC})
include_directories(${CIVETWEB_SRC})
set(CIVETWEB_SRC_C ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/src/civetweb.c)

#include nlohmann json
set(JSON_DIR ${CMAKE_SOURCE_DIR}/include/json/include)
include_directories(${JSON_DIR})
set(JSON_SRC ${JSON_DIR}/nlohmann/json.hpp)

#include handlers
set(HANDLERS_DIR ${CMAKE_SOURCE_DIR}/src/handlers)
set(HANDLERS_SRC ${HANDLERS_DIR}/authHandler.hpp
                 ${HANDLERS_DIR}/authHandler.cpp)


# Find SOCI package
find_package(SOCI REQUIRED)
include_directories(${SOCI_INCLUDE_DIRS})

add_executable(simple_rest src/main.cpp
                           ${LOGGER_SRC}
                           ${BASE64_SRC}
                           ${CIVETWEB_SRC_C}
                           ${JSON_SRC}
                           ${HANDLERS_SRC}
                           ${TDB_SRC})

target_link_libraries(simple_rest rpcrt4 ws2_32 soci_core_4_0 soci_postgresql_4_0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
