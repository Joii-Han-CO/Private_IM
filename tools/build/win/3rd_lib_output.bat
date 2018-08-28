:: set environmental variable
set_env.bat

:: pthread-win32
xcopy /r /y "F:\Dev\3rd_lib\pthread-win32\bin\Win32_MSVC2015.Release\pthread_dll.dll" %im_3rd_lib%\pthread-win32\
xcopy /r /y "F:\Dev\3rd_lib\pthread-win32\bin\Win32_MSVC2015.Release\pthread_dll.lib" %im_3rd_lib%\pthread-win32\
xcopy /r /y "F:\Dev\3rd_lib\pthread-win32\bin\Win32_MSVC2015.Release\pthread_lib.lib" %im_3rd_lib%\pthread-win32\


:: mosquitto
xcopy /r /y "F:\Dev\3rd_lib\mosquitto\o_win\src/release\mosquitto.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "F:\Dev\3rd_lib\mosquitto\o_win\lib/release\mosquitto.dll" %im_3rd_lib%\mosquitto\
xcopy /r /y "F:\Dev\3rd_lib\mosquitto\o_win\lib/release\mosquitto.lib" %im_3rd_lib%\mosquitto\
xcopy /r /y "F:\Dev\3rd_lib\mosquitto\o_win\client/release\mosquitto_pub.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "F:\Dev\3rd_lib\mosquitto\o_win\client/release\mosquitto_sub.exe" %im_3rd_lib%\mosquitto\

xcopy /r /y "F:\Dev\3rd_lib\mosquitto\lib\mosquitto.h" %im_3rd_lib%\mosquitto\include\


:: protobuf
xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Release\protoc.exe" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Release\libprotoc.lib" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Release\libprotobuf-lite.lib" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Release\libprotobuf.lib" %im_3rd_lib%\protobuf\Release\

xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Debug\libprotocd.lib" %im_3rd_lib%\protobuf\Debug\
xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Debug\libprotobuf-lited.lib" %im_3rd_lib%\protobuf\Debug\
xcopy /r /y "F:\Dev\3rd_lib\protobuf\o_win\Debug\libprotobufd.lib" %im_3rd_lib%\protobuf\Debug\

mkdir F:\Dev\3rd_lib\protobuf\include
xcopy /s/y/i/f "F:\Dev\3rd_lib\protobuf\src\google\*.h" "%im_3rd_lib%\protobuf\include\google"
