# 最低版本要求
cmake_minimum_required (VERSION 3.0)

################################################################
# Lib

# 工程名
set(_lib_name "server_sql")
project(${_lib_name})

set(_lib_folder "server")

# C\C++ 包含源码路径
set(_include_dirs )

# C\C++ 包含库路径
set(_lib_dirs )

# /src/include
set(_incldue ${PROJECT_SOURCE_DIR}/../include/)
set(_incldue_src ${PROJECT_SOURCE_DIR}/../../lib/include/)

set(_stdafx ${PROJECT_SOURCE_DIR}/stdafx)

# 源码包含目录
include_directories(${_lib_name}
                    ${_incldue}
                    ${_incldue_src}
                    ${PROJECT_SOURCE_DIR}
                    ${_include_dirs})

# 创建目录
set(_inc_file
  ${_stdafx}.h
  ${_incldue}/${_namepace}_${_lib_name}.h)
source_group("include" FILES ${_inc_file})

set(_cpp_file
  ${_stdafx}.cpp
  ${PROJECT_SOURCE_DIR}/${_namepace}_${_lib_name}.cpp)
source_group("source" FILES ${_cpp_file})

add_library(${_lib_name} ${_prj_lib_type}
            ${_inc_file} ${_cpp_file})
target_link_libraries(${_lib_name} LINK_PUBLIC ${_lib_dirs})

# 设置环境变量
set(_compile_def IM_LIB)
if (${_prj_lib_type} STREQUAL "STATIC")
  set(_compile_def ${_compile_def} IM_STATIC)
endif()
target_compile_definitions(${_lib_name}
                           PRIVATE
                           ${_compile_def})
                           
# 设置工程目录
set_target_properties(${_lib_name} PROPERTIES FOLDER
                      ${_lib_folder}/${_lib_name})

# 预编译头
set_target_properties(${_lib_name} PROPERTIES
                      COTIRE_CXX_PREFIX_HEADER_INIT ${_stdafx}.h)
################################################################


################################################################
# Test

set(_test_name ${_lib_name}_test)
project(${_test_name})

set(_stdafx ${PROJECT_SOURCE_DIR}/test/stdafx)

# 源码包含目录
include_directories(${_test_name} ${_incldue})

# 创建目录
set(_inc_file
  ${_stdafx}.h
  ${PROJECT_SOURCE_DIR}/test/test_${_lib_name}.h)
source_group("include" FILES ${_inc_file})

set(_cpp_file
  ${_stdafx}.cpp
  ${PROJECT_SOURCE_DIR}/test/test_${_lib_name}.cpp)
source_group("source" FILES ${_cpp_file})

# add src files
add_executable(${_test_name}
               ${_inc_file} ${_cpp_file})

# 设置环境变量
if (${_prj_lib_type} STREQUAL "STATIC")
  set(_compile_def ${_compile_def} IM_STATIC)
endif()
target_compile_definitions(${_test_name}
                           PRIVATE
                           ${_compile_def})
                           
set_target_properties(${_test_name} PROPERTIES FOLDER
                      ${_lib_folder}/${_lib_name})

# 预编译头
set_target_properties(${_test_name} PROPERTIES
                      COTIRE_CXX_PREFIX_HEADER_INIT ${_stdafx}.h)
#set_target_properties(${_test_name} PROPERTIES
#                      COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)
################################################################


################################################################

# 使用 unicode 编码
Add_Definitions(-D_UNICODE)

# 项目依赖
target_link_libraries(${_lib_name} sql_base)
target_link_libraries(${_test_name} ${_lib_name})

cotire(${_lib_name} ${_test_name})
