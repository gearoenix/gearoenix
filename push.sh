#!/bin/bash
find ./gearoenix -iname *.hpp -o -iname *.cpp | xargs clang-format-8 -i -style=WebKit
find ./tests     -iname *.hpp -o -iname *.cpp | xargs clang-format-8 -i -style=WebKit
find ./examples  -iname *.hpp -o -iname *.cpp | xargs clang-format-8 -i -style=WebKit
git add --all
git commit
git push
