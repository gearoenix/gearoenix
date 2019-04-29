#!/usr/bin/python3

import sys
import os
import subprocess
import shutil
import multiprocessing
from urllib.request import urlretrieve
from zipfile import ZipFile

SRC_PATH = os.path.dirname(os.path.abspath(__file__))

SDK_PATH = SRC_PATH + "/sdk/"

if not os.path.exists(SDK_PATH):
    os.makedirs(SDK_PATH)

SDL2_DIR_NAME = 'SDL2-2.0.9'
SDL2_DIR_PATH = SDK_PATH + SDL2_DIR_NAME
SDL2_BUILD_DIR_PATH = SDK_PATH + 'sdl2-build'
SDL2_PACK_NAME = SDL2_DIR_NAME + '.zip'
SDL2_PACK_URL = 'https://www.libsdl.org/release/' + SDL2_PACK_NAME
SDL2_PACK_PATH = SDK_PATH + SDL2_PACK_NAME


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
subprocess.run([
    'cmake',
    '-B' + SDL2_BUILD_DIR_PATH,
    '-H' + SDL2_DIR_PATH,
    '-DCMAKE_BUILD_TYPE=MinSizeRel'
])
subprocess.run([
    'make',
    '-j' + str(multiprocessing.cpu_count())
])
