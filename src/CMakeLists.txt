# 最低版本要求
cmake_minimum_required (VERSION 3.0)

# 预编译头
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

find_package(Boost)

# 整体工程名
set(_sln_name "pim")
project (${_sln_name})

# 命名空间
set(_namepace "im")

# 工程属性
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_compile_options("/MTd")
  add_compile_options("/WX")
else()
  add_compile_options(-std=c++11)
endif()

# 输出动态库，还是静态库
#set(_prj_lib_type SHARED)
set(_prj_lib_type STATIC)

set(CMAKE_BUILD_TYPE "Debug")

# Link时候的各种库
set(_sys_lib )
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set(_sys_lib ${_sys_lib} ${BOOST_ROOT}/lib/)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(_sys_lib ${_sys_lib} ${Boost_INCLUDE_DIR}/bin/vc14/lib)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 各个项目
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/msg_proto/)

# msg_mqtt_client
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/msg_mqtt_client/)