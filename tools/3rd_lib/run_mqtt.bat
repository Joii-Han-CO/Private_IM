echo off
call ..\build\win\set_env.bat

cd /d "%im_3rd_lib%\mosquitto\lib\release_mt\"

mosquitto.exe -c mosquitto.conf -v

pause