cmake_minimum_required (VERSION 3.19)
project("${CMAKE_PROJECT_NAME}")
include("$ENV{TOOLCHAIN_PATH}/CMakeLists.txt.in")

add_lib("lib${CMAKE_PROJECT_NAME}" "source/lib")
add_bin("shmview" "source/bin/view" libshm ${LIB_CFG} ${LIB_LOG} ${LIB_COM})
add_unittest(lib${CMAKE_PROJECT_NAME} ${LIB_COM} ${LIB_LOG})
