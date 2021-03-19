#!/bin/bash
find ./gearoenix ./examples ./tests -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format-12 -i -style=WebKit {} \;