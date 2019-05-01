#!/usr/bin/python3

import os
import pyperclip

files = []

for r, d, f in os.walk('./'):
    for file in f:
        if file.endswith('.hpp') or file.endswith('.cpp'):
            files.append(
                '"${GX_ENGINE_SRC_DIR}' +
                r[2:].replace('\\', '/') +
                '/' +
                file +
                '"')

files.sort()

files_text = ''

for f in files:
    files_text += f + '\n'

pyperclip.copy(files_text)
