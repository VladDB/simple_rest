cmake_minimum_required(VERSION 3.27.0)
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

# include class for db connection
set(TDB_SRC ${COMPONENTS_DIR}/tdb.hpp
            ${COMPONENTS_DIR}/tdb.cpp)

# services dir
set(SERVICES_DIR ${CMAKE_SOURCE_DIR}/src/services)

# include user service
set(USER_SERVICE_SRC ${SERVICES_DIR}/userService.hpp
                     ${SERVICES_DIR}/userService.cpp)

# include session service
set(SESSION_SERVICE_SRC ${SERVICES_DIR}/sessionService.hpp
                     ${SERVICES_DIR}/sessionService.cpp)

# include civetweb
set(CIVETWEB_INC ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/include)
set(CIVETWEB_SRC ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/src)
include_directories(${CIVETWEB_INC})
include_directories(${CIVETWEB_SRC})
set(CIVETWEB_SRC_C ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/src/civetweb.c)

# include nlohmann json
set(JSON_DIR ${CMAKE_SOURCE_DIR}/include/json/include)
include_directories(${JSON_DIR})
set(JSON_SRC ${JSON_DIR}/nlohmann/json.hpp)

# include handlers
set(HANDLERS_DIR ${CMAKE_SOURCE_DIR}/src/handlers)

# global functions for handlers
set(GLOBAL_HANDLER_SRC ${HANDLERS_DIR}/globalsForHandlers.hpp
                      ${HANDLERS_DIR}/globalsForHandlers.cpp)  

# auth handler
set(AUTH_HANDLER_SRC ${HANDLERS_DIR}/authHandler.hpp
                 ${HANDLERS_DIR}/authHandler.cpp)

# user handler
set(USER_HANDLER_SRC ${HANDLERS_DIR}/userHandler.hpp
                     ${HANDLERS_DIR}/userHandler.cpp)

# user handler
set(HTML_HANDLER_SRC ${HANDLERS_DIR}/htmlHandler.hpp
                     ${HANDLERS_DIR}/htmlHandler.cpp)

set(HANDLERS_SRC ${GLOBAL_HANDLER_SRC}
                 ${AUTH_HANDLER_SRC}
                 ${USER_HANDLER_SRC}
                 ${HTML_HANDLER_SRC})


# Find SOCI package
find_package(SOCI REQUIRED)
include_directories(${SOCI_INCLUDE_DIRS})

add_executable(simple_rest src/main.cpp
                           ${LOGGER_SRC}
                           ${BASE64_SRC}
                           ${CIVETWEB_SRC_C}
                           ${JSON_SRC}
                           ${HANDLERS_SRC}
                           ${TDB_SRC}
                           ${USER_SERVICE_SRC}
                           ${SESSION_SERVICE_SRC})

target_link_libraries(simple_rest rpcrt4 ws2_32 soci_core_4_0 soci_postgresql_4_0)

# target_compile_options(simple_rest PRIVATE -std=c++17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
