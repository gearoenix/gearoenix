#!/bin/bash
find ./gearoenix ./examples -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format-10 -i -style=WebKit {} \;