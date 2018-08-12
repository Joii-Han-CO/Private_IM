:: 构建Win下工程

mkdir o_win
cd o_win

::BOOST_ROOT

cmake -G "Visual Studio 15 2017" ./../src/ -DINC_Protobuf=D:\Code\lib\protobuf\src

cd ..
