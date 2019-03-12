#!/usr/bin/env python

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
    pass
    
def gen_mac():
    openssl = subprocess.check_output(['brew', '--prefix', 'openssl']).decode('utf-8').strip()
    qt = subprocess.check_output(['brew', '--prefix', 'qt']).decode('utf-8').strip()
    os.environ['PKG_CONFIG_PATH'] = os.path.join(openssl, 'lib/pkgconfig')
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

