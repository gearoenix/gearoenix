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
IN_MACOS = 'Darwin' in PLATFORM

print("Building in platform:", PLATFORM)
print("Platform is MacOS:", IN_MACOS)

SRC_PATH = os.path.dirname(os.path.abspath(__file__))
os.chdir(SRC_PATH)

SDK_PATH = os.path.join(SRC_PATH, 'sdk')
if not os.path.exists(SDK_PATH):
    os.makedirs(SDK_PATH)


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


def download_lib(lib_name, lib_pack_path, lib_url):
    if not os.path.isfile(lib_pack_path):
        print('Downloading', lib_name, 'from', lib_url, '...')
        urlretrieve(lib_url, lib_pack_path, report_hook)
        print('Download', lib_name, 'finished.')
        print('Extracting', lib_name, '...')
        zip = ZipFile(lib_pack_path, 'r')
        zip.extractall(SDK_PATH)
        zip.close()
    else:
        print(lib_name, 'pack has been already downloaded.')
        print('If', lib_name,
              'library is not very OK,',
              'you can delete',
              lib_pack_path,
              'and re-run this script.')


SDL2_DIR_NAME = 'SDL2'
SDL2_DIR_PATH = os.path.join(SDK_PATH, SDL2_DIR_NAME)
SDL2_VERSION = '2.0.9'
SDL2_DIR_NAME_VER = SDL2_DIR_NAME + '-' + SDL2_VERSION
SDL2_DIR_PATH_VER = os.path.join(SDK_PATH, SDL2_DIR_NAME_VER)
SDL2_BUILD_DIR_PATH = os.path.join(SDK_PATH, 'sdl2-build')
SDL2_PACK_NAME = SDL2_DIR_NAME_VER + '.zip'
SDL2_PACK_URL = 'https://www.libsdl.org/release/' + SDL2_PACK_NAME
SDL2_PACK_PATH = os.path.join(SDK_PATH, SDL2_PACK_NAME)
SDL2_BUILD_TYPE = 'MinSizeRel'
download_lib(SDL2_DIR_NAME, SDL2_PACK_PATH, SDL2_PACK_URL)
if not os.path.exists(SDL2_DIR_PATH):
    shutil.move(SDL2_DIR_PATH_VER, SDL2_DIR_PATH)

GLM_VERSION = '0.9.9.5'
GLM_DIR_NAME = 'glm-' + GLM_VERSION
GLM_DIR_PATH = os.path.join(SDK_PATH, GLM_DIR_NAME)
GLM_PACK_NAME = GLM_DIR_NAME + '.zip'
GLM_PACK_URL = 'https://github.com/g-truc/glm/releases/download/' + \
    GLM_VERSION + '/' + GLM_PACK_NAME
GLM_PACK_PATH = os.path.join(SDK_PATH, GLM_PACK_NAME)
download_lib(GLM_DIR_NAME, GLM_PACK_PATH, GLM_PACK_URL)

STB_DIR_NAME = 'stb'
STB_DIR_PATH = os.path.join(SDK_PATH, STB_DIR_NAME)
STB_PACK_NAME = STB_DIR_NAME + '.zip'
STB_PACK_URL = 'https://github.com/nothings/stb/archive/master.zip'
STB_PACK_PATH = os.path.join(SDK_PATH, STB_PACK_NAME)
download_lib(STB_DIR_NAME, STB_PACK_PATH, STB_PACK_URL)
if not os.path.exists(STB_DIR_PATH):
    shutil.move(os.path.join(SDK_PATH, 'stb-master'), STB_DIR_PATH)

BOOST_VERSION = '1_70_0'
BOOST_URL_VERSION = BOOST_VERSION.replace('_', '.')
BOOST_DIR_VERSION = '_' + BOOST_VERSION
BOOST_DIR_NAME = 'boost'
BOOST_DIR_PATH = os.path.join(SDK_PATH, BOOST_DIR_NAME)
BOOST_PACK_NAME = BOOST_DIR_NAME + BOOST_DIR_VERSION + '.zip'
BOOST_PACK_URL = 'https://dl.bintray.com/boostorg/release/1.70.0/source/' + BOOST_PACK_NAME
BOOST_PACK_PATH = os.path.join(SDK_PATH, BOOST_PACK_NAME)
download_lib(BOOST_DIR_NAME, BOOST_PACK_PATH, BOOST_PACK_URL)
if not os.path.exists(BOOST_DIR_PATH):
    shutil.move(BOOST_DIR_PATH + BOOST_DIR_VERSION, BOOST_DIR_PATH)