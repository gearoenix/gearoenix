#!/bin/bash
clear
clear

{
	if clang++ -std=c++14 -Wall -Ofast gearoenix-ttf-to-png.cpp ../gearoenix/render/texture/rnd-txt-lodepng.cpp -o gearoenix-ttf-to-png.exe
	then
		echo "TTF to PNG compilation ended successfully."
	else
		echo "TTF to PNG failed to compile."
	fi
} &