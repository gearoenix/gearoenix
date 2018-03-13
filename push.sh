#!/bin/bash
find ./gearoenix -iname *.hpp -o -iname *.cpp | xargs clang-format-6.0 -i -style=WebKit
find ./tools -iname *.hpp -o -iname *.cpp | xargs clang-format-6.0 -i -style=WebKit
git add --all
git commit
git push origin master
git push origin development
