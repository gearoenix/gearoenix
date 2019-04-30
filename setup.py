#!/usr/bin/python3

import sys
import os
import subprocess
import shutil
import multiprocessing
import platform
from urllib.request import urlretrieve
from zipfile import ZipFile

PLATFORM = platform.system()
print("Building in platform:", PLATFORM)

SRC_PATH = os.path.dirname(os.path.abspath(__file__))
os.chdir(SRC_PATH)

print("Updaating the git sub modules ...")
subprocess.run([
    'git', 
    'submodule', 
    'update',
    '--init',
    '--recursive', 
    '--remote'
])

SDK_PATH = os.path.join(SRC_PATH, 'sdk')
if not os.path.exists(SDK_PATH):
    os.makedirs(SDK_PATH)

SDL2_DIR_NAME = 'SDL2-2.0.9'
SDL2_DIR_PATH = os.path.join(SDK_PATH, SDL2_DIR_NAME)
SDL2_BUILD_DIR_PATH = os.path.join(SDK_PATH, 'sdl2-build')
SDL2_PACK_NAME = SDL2_DIR_NAME + '.zip'
SDL2_PACK_URL = 'https://www.libsdl.org/release/' + SDL2_PACK_NAME
SDL2_PACK_PATH = os.path.join(SDK_PATH, SDL2_PACK_NAME)
SDL2_BUILD_TYPE = 'MinSizeRel'


def report_hook(blocknum, blocksize, totalsize):
    bytesread = blocknum * blocksize
    if totalsize > 0:
        percent = bytesread * 1e2 / totalsize
        s = "\r%5.1f%% (%*d / %d bytes)" % (
            percent,
            len(str(totalsize)),
            bytesread,
            totalsize)
        sys.stderr.write(s)
        if bytesread >= totalsize:
            sys.stderr.write("\n")
    else:
        sys.stderr.write("read %d\n" % (bytesread,))


if not os.path.isfile(SDL2_PACK_PATH):
    print("Downloading SDL2 from", SDL2_PACK_URL, "...")
    urlretrieve(SDL2_PACK_URL, SDL2_PACK_PATH, report_hook)
    print("Download SDL2 finished.")
    print("Extracting SDL2 ...")
    zip = ZipFile(SDL2_PACK_PATH, 'r')
    zip.extractall(SDK_PATH)
    zip.close()
else:
    print("SDL2 pack has been already downloaded.")
    print("If SDL2 library is not very OK you can delete it and re-run this script.")

if os.path.exists(SDL2_BUILD_DIR_PATH):
    shutil.rmtree(SDL2_BUILD_DIR_PATH)
os.makedirs(SDL2_BUILD_DIR_PATH)
os.chdir(SDL2_BUILD_DIR_PATH)

LIBS_PATH = os.path.join(SDK_PATH, 'libs')
if os.path.exists(LIBS_PATH):
    shutil.rmtree(LIBS_PATH)
os.makedirs(LIBS_PATH)

subprocess.run([
    'cmake',
    '-B' + SDL2_BUILD_DIR_PATH,
    '-H' + SDL2_DIR_PATH,
    '-DCMAKE_BUILD_TYPE=' + SDL2_BUILD_TYPE,
])

if 'Windows' in PLATFORM:
    print('Building with Visual Studio')
    SDL2_STATIC = 'SDL2-static'
    SDL2_MAIN = 'SDL2main'
    SDL2_STATIC_LIB = SDL2_STATIC + '.lib'
    SDL2_MAIN_LIB = SDL2_MAIN + '.lib'
    subprocess.run([
        'msbuild',
        SDL2_STATIC + '.vcxproj',
        '/p:configuration=' + SDL2_BUILD_TYPE,
        '/p:platform=x64'
    ])
    subprocess.run([
        'msbuild',
        SDL2_MAIN + '.vcxproj',
        '/p:configuration=' + SDL2_BUILD_TYPE,
        '/p:platform=x64'
    ])
    SDL2_BUILD_LIB_DIR_PATH = os.path.join(
        SDL2_BUILD_DIR_PATH, SDL2_BUILD_TYPE)
    shutil.move(
        os.path.join(SDL2_BUILD_LIB_DIR_PATH, SDL2_STATIC_LIB),
        os.path.join(LIBS_PATH, SDL2_STATIC_LIB))
    shutil.move(
        os.path.join(SDL2_BUILD_LIB_DIR_PATH, SDL2_MAIN_LIB),
        os.path.join(LIBS_PATH, SDL2_MAIN_LIB))

elif 'Linux' in PLATFORM:
    subprocess.run([
        'make',
        '-j' + str(multiprocessing.cpu_count())
    ])
