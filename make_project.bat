:: 先获取环境变量
call tools\build\win\set_env.bat

:: 构建Win下工程

mkdir o_win
cd o_win

::BOOST_ROOT
mkdir ..\src\lib\msg_proto\proto_class\
cmake -G "Visual Studio 15 2017" ./../src/ -DTHRD_LIB_PATH=%im_lib_src%

cd ..
