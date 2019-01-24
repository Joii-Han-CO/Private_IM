echo off
:: first and set environment variable
call build\win\set_env.bat

set name=s_sql
set folder=server
set s_path=E:\code\Private_IM

python .\python\add.py -p %s_path% -n %name% -f %folder%
pause
