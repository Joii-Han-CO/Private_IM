:: set environmental variable
call set_env.bat

:: openssl
mkdir %im_3rd_lib%\openssl\
mkdir %im_3rd_lib%\openssl\s\
mkdir %im_3rd_lib%\openssl\d\

xcopy /r /y "%im_lib_src%\openssl-1.0.2o\out32\libeay32.lib" %im_3rd_lib%\openssl\s\
xcopy /r /y "%im_lib_src%\openssl-1.0.2o\out32\ssleay32.lib" %im_3rd_lib%\openssl\s\

xcopy /r /y "%im_lib_src%\openssl-1.0.2o\out32dll\libeay32.lib" %im_3rd_lib%\openssl\d\
xcopy /r /y "%im_lib_src%\openssl-1.0.2o\out32dll\libeay32.dll" %im_3rd_lib%\openssl\d\
xcopy /r /y "%im_lib_src%\openssl-1.0.2o\out32dll\ssleay32.lib" %im_3rd_lib%\openssl\d\
xcopy /r /y "%im_lib_src%\openssl-1.0.2o\out32dll\ssleay32.dll" %im_3rd_lib%\openssl\d\

mkdir %im_lib_src%\openssl\include
xcopy /s/y/i/f "%im_lib_src%\openssl-1.0.2o\inc32\*.h" "%im_3rd_lib%\openssl\include\"


:: pthread-win32
mkdir %im_3rd_lib%\pthread-win32\
mkdir %im_3rd_lib%\pthread-win32\debug\

xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Release\pthread_dll.dll" %im_3rd_lib%\pthread-win32\
xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Release\pthread_dll.lib" %im_3rd_lib%\pthread-win32\
xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Release\pthread_lib.lib" %im_3rd_lib%\pthread-win32\
xcopy /r /y "%im_lib_src%\pthread-win32\bin\Win32_MSVC2015.Debug\pthread_lib.lib" %im_3rd_lib%\pthread-win32\debug\


:: mosquitto
mkdir %im_3rd_lib%\mosquitto\
mkdir %im_3rd_lib%\mosquitto\debug\

xcopy /r /y "%im_lib_src%\mosquitto\o_win\src/release\mosquitto.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\mosquitto.conf" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\lib/release\mosquitto.dll" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\lib/release\mosquitto.lib" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\lib/release\mosquitto_static.lib" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\client/release\mosquitto_pub.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\client/release\mosquitto_sub.exe" %im_3rd_lib%\mosquitto\
xcopy /r /y "%im_lib_src%\mosquitto\o_win\lib/debug\mosquitto_static.lib" %im_3rd_lib%\mosquitto\debug\
xcopy /r /y "%im_lib_src%\mosquitto\lib\mosquitto.h" %im_3rd_lib%\mosquitto\include\
xcopy /r /y "%im_lib_src%\mosquitto\lib\mosquitto_plugin.h" %im_3rd_lib%\mosquitto\include\


:: protobuf
mkdir %im_3rd_lib%\protobuf\
mkdir %im_3rd_lib%\protobuf\Debug\
mkdir %im_3rd_lib%\protobuf\Release\

xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\protoc.exe" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\libprotoc.lib" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\libprotobuf-lite.lib" %im_3rd_lib%\protobuf\Release\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Release\libprotobuf.lib" %im_3rd_lib%\protobuf\Release\

xcopy /r /y "%im_lib_src%\protobuf\o_win\Debug\libprotocd.lib" %im_3rd_lib%\protobuf\Debug\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Debug\libprotobuf-lited.lib" %im_3rd_lib%\protobuf\Debug\
xcopy /r /y "%im_lib_src%\protobuf\o_win\Debug\libprotobufd.lib" %im_3rd_lib%\protobuf\Debug\

mkdir %im_lib_src%\protobuf\include
xcopy /s/y/i/f "%im_lib_src%\protobuf\src\google\*.h" "%im_3rd_lib%\protobuf\include\google"

:: simpleini
mkdir %im_lib_src%\simpleini\include
xcopy /s/y/i/f "%im_lib_src%\simpleini\SimpleIni.h" "%im_3rd_lib%\simpleini\include\"
