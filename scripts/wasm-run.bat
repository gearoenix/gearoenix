for %%format in (*) do if not "%%format"=="run.bat" del /s /q "%%format"
for /d %%d in (*) do rmdir /q /s "%%d"
call emconfigure.bat cmake ..
call cmake --build .
call python -m SimpleHTTPServer 8080