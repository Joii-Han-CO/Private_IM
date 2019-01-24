echo off

:: 先获取环境变量
call tools\build\win\set_env.bat

:: 构建Win下工程

mkdir o_win
cd o_win

::BOOST_ROOT
mkdir ..\src\lib\msg_proto\proto_class\
cmake -G "Visual Studio 15 2017" ./../src/ -DTHRD_LIB_PATH=%im_3rd_lib% -DBoost_LIBRARY_DIRS=%im_boost_lib_dir% -Dmysql_dir="%mysql_dir%"

set cmake_ref_val=%errorlevel%
cd ..

if %cmake_ref_val%==0 goto end

:cmake_error
echo ----CMake Failed!----
pause

:end
