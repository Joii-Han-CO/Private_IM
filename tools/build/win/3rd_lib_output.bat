:: set environmental variable
set_env.bat

:: pthread-win32
xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Release\pthread_dll.dll" %im_3rd_lib%\pthread-win32\
xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Release\pthread_dll.lib" %im_3rd_lib%\pthread-win32\
xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Release\pthread_lib.lib" %im_3rd_lib%\pthread-win32\


:: mosquitto
xcopy /r /y "%im_lib_src%\mosquitto\o_win\src/release\mosquitto.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\lib/release\mosquitto.dll" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\lib/release\mosquitto.lib" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\client/release\mosquitto_pub.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\client/release\mosquitto_sub.exe" %im_3rd_lib%\mosquitto\

xcopy /r /y "%im_lib_src%\mosquitto\lib\mosquitto.h" %im_3rd_lib%\mosquitto\include\


:: protobuf
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\protoc.exe" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\libprotoc.lib" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\libprotobuf-lite.lib" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\libprotobuf.lib" %im_3rd_lib%\protobuf\Release\

xcopy /r /y "%im_lib_src%\protobuf\o_win\Debug\libprotocd.lib" %im_3rd_lib%\protobuf\Debug\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Debug\libprotobuf-lited.lib" %im_3rd_lib%\protobuf\Debug\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Debug\libprotobufd.lib" %im_3rd_lib%\protobuf\Debug\

mkdir %im_lib_src%\protobuf\include
xcopy /s/y/i/f "%im_lib_src%\protobuf\src\google\*.h" "%im_3rd_lib%\protobuf\include\google"
