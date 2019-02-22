@echo off
pushd \\10.162.80.9\measuremendata\BadgeData
echo "running app"
start/B ConsoleApplication1.exe >nul
REM Assumption that the ambulation runtime every day takes less than (10) seconds
timeout 10
echo "finished c++ app"
echo "calling python script"
call C:\Users\INBT\AppData\Local\Programs\Python\Python36\python.exe \\10.162.80.9\measuremendata\BadgeData\csv_to_mysql.py %*
echo "called python script"
echo "batch script end"