echo off
set name=c_login
set folder=client
set s_path=E:\code\Private_IM

python .\python\add.py -p %s_path% -n %name% -f %folder%
pause
