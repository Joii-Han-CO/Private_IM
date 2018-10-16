
:: set environmental variable
set_env.bat

:: get source code
cd /d %im_lib_src%
git clone https://github.com/GerHobbelt/pthread-win32.git
git clone https://github.com/openssl/openssl.git
git clone https://github.com/madler/zlib.git
git clone https://github.com/eclipse/mosquitto.git
git clone https://github.com/google/protobuf.git
git clone https://github.com/brofield/simpleini.git
cd protobuf\third_party
git clone https://github.com/google/benchmark.git
git clone https://github.com/google/googletest.git


:: build

::pThread-Win32
cd /d %im_lib_src%/pthread-win32
:: MT
devenv.com "pthread.2015.sln" /Build Debug
devenv.com "pthread.2015.sln" /Build Release

::mosquitto
cd /d %im_lib_src%/mosquitto
git checkout v1.5.1
mkdir %im_output_name%
cd %im_output_name%
:: lib/CMakeLists.txt 替换 pthread-win32 的地址
cmake -G "Visual Studio 15 2017" ./../ -DWITH_TLS=OFF -DWITH_STATIC_LIBRARIES=ON -DWITH_PIC=ON -DPTHREAD_INCLUDE_DIR=%im_lib_src%/pthread-win32
:: 使用静态库
:: 添加 PTW32_STATIC_LIB 标识
:: MT
devenv.com "mosquitto.sln" /Build Debug
devenv.com "mosquitto.sln" /Build Release

::OpenSSL
::安装Perl
::nams
cd /d %im_lib_src%/openssl
git checkout OpenSSL_1_1_1-pre9
perl Configure VC-WIN32 no-asm
ms\do_ms.bat
nmake -f ms\nt.mak
nmake -f ms\ntdll.mak

::zlib
cd /d %im_lib_src%\zlib\contrib\vstudio\vc14
git checkout v1.2.11
::处理映像安全问题  :   /SAFESEH:NO
:: MT 
devenv.com "zlibvc.sln" /Build Debug
devenv.com "zlibvc.sln" /Build Release

::protobuf
cd /d %im_lib_src%\protobuf
git checkout v3.6.1
cd /d %im_lib_src%\protobuf\third_party\benchmark
git checkout v1.4.1
cd /d %im_lib_src%\protobuf\third_party\googletest
git checkout release-1.8.0
cd /d %im_lib_src%\protobuf
cd %im_lib_src%\protobuf
mkdir %im_output_name%
cd %im_output_name%
cmake -G "Visual Studio 15 2017" ./../cmake -Dprotobuf_BUILD_TESTS=OFF -DZLIB_LIBRARY="%im_lib_src%/zlib-1.2.11\contrib\vstudio\vc14\x86\ZlibStatRelease\zlibstat.lib"
devenv.com "protobuf.sln" /Build Debug
devenv.com "protobuf.sln" /Build Release

:: simpleini
cd /d %im_lib_src%\simpleini
git checkout 4.17


