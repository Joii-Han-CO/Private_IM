# 最低版本要求
cmake_minimum_required (VERSION 3.0)

################################################################
# Lib

# 工程名
set(_lib_name "server_main")
project(${_lib_name})

set(_lib_folder "server")

# C\C++ 包含源码路径
set(_include_dirs )

# C\C++ 包含库路径
set(_lib_dirs )

# /src/include
set(_incldue ${PROJECT_SOURCE_DIR}/../../include/server/)

set(_pch ${PROJECT_SOURCE_DIR}/pch)

# 源码包含目录
include_directories(${_lib_name}
                    ${_prj_include_dir}
                    ${PROJECT_SOURCE_DIR}
                    ${_include_dirs})

# 创建目录
set(_inc_file
  ${_pch}.h
  ${PROJECT_SOURCE_DIR}/${_lib_name}.h)
source_group("include" FILES ${_inc_file})

set(_cpp_file
  ${_pch}.cpp
  ${PROJECT_SOURCE_DIR}/${_lib_name}.cpp)
source_group("source" FILES ${_cpp_file})

set(_framework_file
  ${PROJECT_SOURCE_DIR}/frame/server_frame_work.h
  ${PROJECT_SOURCE_DIR}/frame/server_frame_work.cpp
  ${PROJECT_SOURCE_DIR}/frame/publish_channel.h
  ${PROJECT_SOURCE_DIR}/frame/publish_channel.cpp)
source_group("frame" FILES ${_framework_file})
set(_cpp_file ${_cpp_file} ${_framework_file})

set(_user_file
  ${PROJECT_SOURCE_DIR}/users/server_user.h
  ${PROJECT_SOURCE_DIR}/users/server_user.cpp
  ${PROJECT_SOURCE_DIR}/users/server_user_manager.h
  ${PROJECT_SOURCE_DIR}/users/server_user_manager.cpp
  ${PROJECT_SOURCE_DIR}/users/pri_channel.h
  ${PROJECT_SOURCE_DIR}/users/pri_channel.cpp)
source_group("users" FILES ${_user_file})
set(_cpp_file ${_cpp_file} ${_user_file})

add_executable(${_lib_name}
               ${_inc_file} ${_cpp_file})
               
set(_compile_def PRJ_NAME="${_lib_name}")
target_compile_definitions(${_lib_name}
                           PRIVATE
                           ${_compile_def})
# 设置工程目录
set_target_properties(${_lib_name} PROPERTIES FOLDER
                      ${_lib_folder})

# 预编译头
set_target_properties(${_lib_name} PROPERTIES
                      COTIRE_CXX_PREFIX_HEADER_INIT ${_pch}.h)
################################################################


################################################################

# 使用 unicode 编码
Add_Definitions(-D_UNICODE)

cotire(${_lib_name})

# 相关依赖
target_link_libraries(${_lib_name} mqtt config msg msg_proto s_sql)
