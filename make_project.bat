:: �Ȼ�ȡ��������
call tools\build\win\set_env.bat

:: ����Win�¹���

mkdir o_win
cd o_win

::BOOST_ROOT
mkdir ..\src\lib\msg_proto\proto_class\
cmake -G "Visual Studio 15 2017" ./../src/ -DTHRD_LIB_PATH=%im_3rd_lib%

cd ..
