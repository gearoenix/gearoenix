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
IN_WINDOWS = 'Windows' in PLATFORM

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

def take_careof_sdl2_proj(name, version):
    dir_name = 'SDL2_' + name
    dir_path = os.path.join(SDK_PATH, dir_name)
    dir_name_ver = dir_name + '-' + version
    dir_path_ver = os.path.join(SDK_PATH, dir_name_ver)
    pack_name = dir_name_ver + '.zip'
    pack_url = 'https://www.libsdl.org/projects/SDL_' + name + '/release/' + pack_name
    pack_path = os.path.join(SDK_PATH, pack_name)
    download_lib(dir_name, pack_path, pack_url)
    if not os.path.exists(dir_path):
        shutil.move(dir_path_ver, dir_path)

take_careof_sdl2_proj('image', '2.0.5')
take_careof_sdl2_proj('mixer', '2.0.4')
take_careof_sdl2_proj('net', '2.0.1')
take_careof_sdl2_proj('ttf', '2.0.15')


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

sdl2_build_args = [
    'cmake',
    '--build',
    '.',
    '--config',
    SDL2_BUILD_TYPE
]

if 'Windows' in PLATFORM:
    SDL2_STATIC_LIB = 'SDL2-static.lib'
    SDL2_MAIN_LIB = 'SDL2main.lib'
    SDL2_STATIC = os.path.join(SDL2_BUILD_TYPE, SDL2_STATIC_LIB)
    SDL2_MAIN = os.path.join(SDL2_BUILD_TYPE, SDL2_MAIN_LIB)
else:
    SDL2_STATIC_LIB = 'libSDL2.a'
    SDL2_MAIN_LIB = 'libSDL2main.a'
    SDL2_STATIC = SDL2_STATIC_LIB
    SDL2_MAIN = SDL2_MAIN_LIB
    sdl2_build_args += ['--', '-j' + str(multiprocessing.cpu_count())]

subprocess.run(sdl2_build_args)
shutil.move(
    os.path.join(SDL2_BUILD_DIR_PATH, SDL2_STATIC),
    os.path.join(LIBS_PATH, SDL2_STATIC_LIB))
shutil.move(
    os.path.join(SDL2_BUILD_DIR_PATH, SDL2_MAIN),
    os.path.join(LIBS_PATH, SDL2_MAIN_LIB))

def make_exec_priv(name):
    subprocess.run(['chmod', '+x', str(name)])

if IN_MACOS:
    IOS_LIBS_PATH = os.path.join(LIBS_PATH, 'ios')
    if os.path.exists(IOS_LIBS_PATH):
        shutil.rmtree(IOS_LIBS_PATH)
    os.makedirs(IOS_LIBS_PATH)
    print("Going to build iOS dependancies")
    SDL2_BUILD_SCRIPTS_PATH = os.path.join(SDL2_DIR_PATH, 'build-scripts')
    IOS_BUILD_SCRIPT_NAME = 'iosbuild.sh'
    SDL2_IOS_BUILD_SCRIPTS_PATH = os.path.join(SDL2_DIR_PATH, IOS_BUILD_SCRIPT_NAME)
    os.chdir(SDL2_DIR_PATH)
    make_exec_priv('configure')
    os.chdir(SDL2_BUILD_SCRIPTS_PATH)
    make_exec_priv(IOS_BUILD_SCRIPT_NAME)
    ios_build_script_file = open(IOS_BUILD_SCRIPT_NAME, 'rt')
    ios_build_script = ios_build_script_file.read()
    ios_build_script_file.close()
    ios_build_script_file = open(IOS_BUILD_SCRIPT_NAME, 'wt')
    ios_build_script = ios_build_script.replace('BUILD_I386_IOSSIM=YES', 'BUILD_I386_IOSSIM=NO')
    ios_build_script = ios_build_script.replace('BUILD_X86_64_IOSSIM=YES', 'BUILD_X86_64_IOSSIM=NO')
    ios_build_script = ios_build_script.replace(
        '-g -O0 -pipe -fPIC -fobjc-arc"\n', 
        '-Os -pipe -fPIC -fobjc-arc -fembed-bitcode-marker -fembed-bitcode -x objective-c"\nLDFLAGS="${LDFLAGS} -flto"\n')
    ios_build_script = ios_build_script.replace('x86_64-sim/lib/libSDL2.a i386-sim/lib/libSDL2.a', '')
    ios_build_script_file.write(ios_build_script)
    ios_build_script_file.close()
    subprocess.run(['sh', IOS_BUILD_SCRIPT_NAME])
    platform_dir = os.path.join(SDL2_BUILD_SCRIPTS_PATH, 'platform')
    args = ['lipo', ]
    for d in os.listdir(platform_dir):
        if not d.endswith('-ios'):
            continue
        args.append(str(os.path.join(platform_dir, d, 'lib', SDL2_MAIN_LIB)))
    args += ['-create', '-output', str(os.path.join(IOS_LIBS_PATH, SDL2_MAIN_LIB))]
    subprocess.run(args)
    shutil.move(
        os.path.join(platform_dir, 'universal', SDL2_STATIC),
        os.path.join(IOS_LIBS_PATH, SDL2_STATIC_LIB))