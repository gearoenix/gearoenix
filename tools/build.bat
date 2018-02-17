title Building tools
echo Start build tools
cl.exe /EHsc /Ox gearoenix-ttf-to-png.cpp ../gearoenix/render/texture/rnd-txt-lodepng.cpp
del *.obj
pause