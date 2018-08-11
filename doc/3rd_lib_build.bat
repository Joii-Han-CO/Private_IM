set im_3rd_lib=D:\Code\Private_IM\3rd_lib
set im_lib_src=D:\Code\lib
set im_output_name=o_win

cd /d %im_lib_src%
git clone https://github.com/eclipse/paho.mqtt.c.git
git clone https://github.com/eclipse/paho.mqtt.cpp.git
git clone https://github.com/google/protobuf.git
cd protobuf\third_party
git clone https://github.com/google/benchmark.git
git clone git clone https://github.com/google/googletest.git

::OpenSSL
cd /d %im_lib_src%/openssl-1.0.2o
perl Configure VC-WIN32 no-asm
ms\do_ms.bat
nmake -f ms\nt.mak
nmake -f ms\ntdll.mak


::Mqtt C
cd /d %im_lib_src%/paho.mqtt.c
git checkout v1.2.1
mkdir %im_output_name%
cd %im_output_name%
cmake -G "Visual Studio 15 2017" ./../ -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_STATIC=TRUE -DPAHO_ENABLE_TESTING=FALSE -DOPENSSL_INCLUDE_DIR="%im_lib_src%/openssl-1.0.2o/inc32/" -DOPENSSL_LIB="%im_lib_src%/openssl-1.0.2o/out32.dbg/libeay32.lib" -DOPENSSLCRYPTO_LIB="%im_lib_src%/openssl-1.0.2o/out32.dbg/ssleay32.lib"
devenv.com "Eclipse Paho C.sln" /Build Debug
devenv.com "Eclipse Paho C.sln" /Build Release


::Mqtt CPP
cd /d %im_lib_src%/paho.mqtt.cpp
git checkout v1.0.0
mkdir %im_output_name%
cd %im_output_name%
cmake -G "Visual Studio 15 2017" ./../ -DPAHO_WITH_SSL=TRUE -DPAHO_MQTT_C_PATH=%im_lib_src%/paho.mqtt.c
devenv.com "paho-mqtt-cpp.sln" /Build Debug
devenv.com "paho-mqtt-cpp.sln" /Build Release


::zlib
cd /d %im_lib_src%\zlib-1.2.11\contrib\vstudio\vc14
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
mkdir %im_output_name%
cd %im_lib_src%\protobuf
mkdir %im_output_name%
cd %im_output_name%
cmake -G "Visual Studio 15 2017" ./../cmake -Dprotobuf_BUILD_TESTS=OFF -DZLIB_LIBRARY="%im_lib_src%/zlib-1.2.11\contrib\vstudio\vc14\x86\ZlibStatRelease\zlibstat.lib"
devenv.com "protobuf.sln" /Build Debug
devenv.com "protobuf.sln" /Build Release





