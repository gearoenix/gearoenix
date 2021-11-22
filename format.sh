#!/bin/bash
find ./gearoenix ./examples ./tests -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format-13 -i -style=WebKit {} \;
