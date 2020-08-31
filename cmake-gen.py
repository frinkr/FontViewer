#!/usr/bin/env python3

import platform
import os
import sys
import subprocess
import shutil

source_root = os.path.dirname(os.path.realpath(__file__))
build_root = os.path.join(source_root, "build")

if len(sys.argv) == 2 and sys.argv[1] == 'clean' and os.path.exists(build_root):
    print('removing ' + build_root + "...")
    shutil.rmtree(build_root)

if not os.path.exists(build_root):
    os.makedirs(build_root)

def gen_win():
    os.chdir(build_root)
    os.system('cmake -G "Visual Studio 16 2019" -A x64 ' + source_root)

def gen_mac():
    openssl = subprocess.check_output('brew --prefix openssl', shell=True).decode('utf-8').strip()
    boost = subprocess.check_output('brew --prefix boost', shell=True).decode('utf-8').strip()
    qt = subprocess.check_output('brew --prefix qt', shell=True).decode('utf-8').strip()
    os.environ['PKG_CONFIG_PATH'] = os.path.join(openssl, 'lib/pkgconfig')
    os.environ['LDFLAGS'] = '-L' + os.path.join(openssl, 'lib')
    os.environ['CPPFLAGS'] = '-I' + os.path.join(openssl, 'include')
    os.chdir(build_root)
    os.system('cmake -G Xcode -DCMAKE_PREFIX_PATH=' + qt + ' ' + source_root)

def gen_linux():
    os.chdir(build_root)
    os.system('cmake ' + source_root)

gs = {
    'Linux': gen_linux,
    'Windows': gen_win,
    'Darwin': gen_mac,
}

gs[platform.system()]()
