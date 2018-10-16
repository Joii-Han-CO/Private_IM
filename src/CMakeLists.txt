# 最低版本要求
cmake_minimum_required (VERSION 3.0)

# 预编译头
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

find_package(Boost)

# Win下设置静态执行库
set(CMAKE_USER_MAKE_RULES_OVERRIDE
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/cxx_flag_overrides.cmake)
  
# 整体工程名
set(_sln_name "pim")
project (${_sln_name})

# 命名空间
set(_namepace "im")

# 工程属性
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_compile_options("/WX")
  link_directories(${_test_name} ${Boost_LIBRARY_DIRS})
  set(CMAKE_CXX_FLAGS_DEBUG_INIT          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")
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

# server_main
add_subdirectory(${PROJECT_SOURCE_DIR}/server/server_main/)

# net_communication
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/net_communication/)

# plugin_mosquitto
add_subdirectory(${PROJECT_SOURCE_DIR}/plugin/plugin_mosquitto/)

# sql_base
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/sql_base/)

# server_sql
add_subdirectory(${PROJECT_SOURCE_DIR}/server/server_sql/)

# ssl_base
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/ssl_base/)

# login
add_subdirectory(${PROJECT_SOURCE_DIR}/client/login/)

# config
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/config/)