for %%f in (*) do if not "%%f"=="run.bat" del /s /q "%%f"
for /d %%d in (*) do rmdir /q /s "%%d"
call emconfigure.bat cmake ..
call cmake --build .
call python -m SimpleHTTPServer 8080