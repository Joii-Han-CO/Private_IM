
source tools/build/mac/set_env.sh

cd o_mac
cmake -G "Xcode" ./../src/ -DTHRD_LIB_PATH=$im_3rd_lib -DBoost_LIBRARY_DIRS=$im_boost_lib_dir -Dmysql_dir="$mysql_dir" -Dmos_lib_dir="$im_mosquitto_lib" -Dprotobuf_dir="$im_protobuf_src"

cd ..
