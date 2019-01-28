call win\set_env.bat

cd ../..
mkdir %im_output_name%\
mkdir %im_output_name%\bin\
mkdir %im_output_name%\bin\Debug\
mkdir %im_output_name%\bin\Release\

set im_d=%im_output_name%\bin\Debug\
set im_r=%im_output_name%\bin\Release\

:: config
xcopy /r /y "tools\default_config\global.config" "%im_d%"
xcopy /r /y "tools\default_config\server_global.config" "%im_d%"
xcopy /r /y "tools\default_config\global.config" %im_r%
xcopy /r /y "tools\default_config\server_global.config" %im_r%

::mysql lib
xcopy /r /y "%mysql_dir%\lib\mysqlcppconn-7-vs14.dll" "%im_d%"
xcopy /r /y "%mysql_dir%\lib\mysqlcppconn8-1-vs14.dll" "%im_d%"
xcopy /r /y "%mysql_dir%\lib\libeay32.dll" "%im_d%"
xcopy /r /y "%mysql_dir%\lib\ssleay32.dll" "%im_d%"

xcopy /r /y "%mysql_dir%\lib\mysqlcppconn-7-vs14.dll" "%im_r%"
xcopy /r /y "%mysql_dir%\lib\mysqlcppconn8-1-vs14.dll" "%im_r%"
xcopy /r /y "%mysql_dir%\lib\libeay32.dll" "%im_r%"
xcopy /r /y "%mysql_dir%\lib\ssleay32.dll" "%im_r%"

pause
