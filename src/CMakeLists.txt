# 最低版本要求
cmake_minimum_required (VERSION 3.0)

# 预编译头
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(cotire)

find_package(Boost)

# Win下设置静态执行库
set(CMAKE_USER_MAKE_RULES_OVERRIDE
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)
  
# 整体工程名
set(_sln_name "pim")
project (${_sln_name})

# 命名空间
set(_namepace "im")

# 工程属性
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_compile_options("/WX")
  link_directories(${_test_name} ${Boost_LIBRARY_DIRS})
  add_compile_options("$<$<CONFIG:RELEASE>:/MT>")
  add_compile_options("$<$<CONFIG:DEBUG>:/MTd>")
else()
  add_compile_options(-std=c++11)
endif()

option(WITH_STATIC "" ON)
if (${WITH_STATIC} STREQUAL ON)
  set(_prj_lib_type STATIC)
else (${WITH_STATIC} STREQUAL ON) 
  set(_prj_lib_type STATIC)
endif (${WITH_STATIC} STREQUAL ON)

set(CMAKE_BUILD_TYPE "Debug")

# 一些全局的引用路径
set(_prj_include_dir ${Boost_INCLUDE_DIRS}
                     ${PROJECT_SOURCE_DIR}
                     ${PROJECT_SOURCE_DIR}/include/
                     ${PROJECT_SOURCE_DIR}/include/lib)

# Link时候的各种库
set(_sys_lib )
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set(_sys_lib ${_sys_lib} ${BOOST_ROOT}/lib/)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(_sys_lib ${_sys_lib} ${Boost_INCLUDE_DIR}/bin/vc14/lib)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 调整输出目录
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# 各个项目
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/msg_proto/)

# mqtt
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/mqtt/)

# server_main
add_subdirectory(${PROJECT_SOURCE_DIR}/server/server_main/)

# plugin_mosquitto
add_subdirectory(${PROJECT_SOURCE_DIR}/plugin/plugin_mosquitto/)

# sql
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/sql/)

# server_sql
add_subdirectory(${PROJECT_SOURCE_DIR}/server/server_sql/)

# config
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/config/)

# log
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/log/)

# coding
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/coding/)

# c_login
add_subdirectory(${PROJECT_SOURCE_DIR}/client/c_login/)