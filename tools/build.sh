#!/bin/bash
clear
clear
rm -rf *.exe *.txt *.bin
# {
	if clang++ -std=c++14 -Wall -Ofast \
		gearoenix-ttf-baker.cpp \
		../gearoenix/core/asset/cr-asset.cpp \
		../gearoenix/core/cache/cr-cache-cached.cpp \
		../gearoenix/render/font/rnd-fnt-2d.cpp \
		../gearoenix/render/font/rnd-fnt-font.cpp \
		../gearoenix/math/math-vector.cpp \
		../gearoenix/math/math-matrix.cpp \
		../gearoenix/system/sys-log.cpp \
		-o gearoenix-ttf-baker.exe
	then
		echo "TTF-Babker successfully compiled."
	else
		echo "TTF-Baker compilation failed."
	fi
# } &