#!/bin/bash
find ./gearoenix ./examples ./tests -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format-11 -i -style=WebKit {} \;