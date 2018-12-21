call set_env.bat

cd %im_lib_src%
git clone https://github.com/GerHobbelt/pthread-win32.git
cd pthread-win32

cd %im_lib_src%
git clone https://github.com/eclipse/mosquitto.git
cd mosquitto
git checkout v1.5.5
