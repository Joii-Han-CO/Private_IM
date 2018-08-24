echo off
set name=msg_mqtt_client
set folder=lib
set s_path=E:\code\Private_IM

python .\python\add.py -p %s_path% -n %name% -f %folder%
pause
