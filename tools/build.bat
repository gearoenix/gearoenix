cls
cls
title Building tools
echo Start build tools
cl.exe /EHsc /Ox ^
		gearoenix-ttf-baker.cpp ^
		../gearoenix/core/asset/cr-asset.cpp ^
		../gearoenix/core/cache/cr-cache-cached.cpp ^
		../gearoenix/render/font/rnd-fnt-2d.cpp ^
		../gearoenix/render/font/rnd-fnt-font.cpp ^
		../gearoenix/math/math-vector.cpp ^
		../gearoenix/math/math-matrix.cpp ^
		../gearoenix/system/sys-log.cpp
del *.obj
pause