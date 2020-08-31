#!/bin/bash
find ./gearoenix -iname *.hpp -o -iname *.cpp | xargs clang-format-10 -i -style=WebKit
find ./tests     -iname *.hpp -o -iname *.cpp | xargs clang-format-10 -i -style=WebKit
find ./demos     -iname *.hpp -o -iname *.cpp | xargs clang-format-10 -i -style=WebKit
find ./examples  -iname *.hpp -o -iname *.cpp | xargs clang-format-10 -i -style=WebKit
find ./tools     -iname *.hpp -o -iname *.cpp | xargs clang-format-10 -i -style=WebKit