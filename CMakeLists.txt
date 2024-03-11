cmake_minimum_required(VERSION 3.17.0)
project(simple_rest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDART 17)

# include spdlog
set(SPDLOG_INC ${CMAKE_SOURCE_DIR}/include/spdlog_1.13/include)
include_directories(${SPDLOG_INC})
set(LOGGER_DIR ${CMAKE_SOURCE_DIR}/src/components)
set(LOGGER_SRC ${LOGGER_DIR}/log.hpp
                ${LOGGER_DIR}/log.cpp)

# include civetweb
set(CIVETWEB_INC ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/include)
set(CIVETWEB_SRC ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/src)
include_directories(${CIVETWEB_INC})
include_directories(${CIVETWEB_SRC})
set(CIVETWEB_SRC_C ${CMAKE_SOURCE_DIR}/include/civetweb_1.16/src/civetweb.c)

add_executable(simple_rest src/main.cpp
                           ${LOGGER_SRC}
                           ${CIVETWEB_SRC_C})

target_link_libraries(simple_rest wsock32 ws2_32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
