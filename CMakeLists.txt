cmake_minimum_required(VERSION 3.16)

project(GivIO VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

find_package(PkgConfig)
pkg_check_modules(FMT REQUIRED fmt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(GivIO
    GivIO.cpp
)

configure_file(
    ${PROJECT_NAME}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    @ONLY
  )
  
set_target_properties(GivIO PROPERTIES PUBLIC_HEADER "GivIO.h")

install(TARGETS GivIO)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

add_executable(test-givio
  test-givio.cpp)

target_link_libraries(test-givio
  GivIO
  fmt
)
