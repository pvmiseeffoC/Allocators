import subprocess
import sys
import os

vs_version = "Visual Studio 16 2019"
if len(sys.argv) == 2:
    if sys.argv[1] == "vs2017":
        vs_version = "Visual Studio 15 2017"
    elif sys.argv[1] == "vs2015":
        vs_version = "Visual Studio 14 2015"

if not os.path.exists("out"):
    os.mkdir("out")

cmakeCmd = ["cmake", '-G', vs_version, ".."]
try:
    retCode = subprocess.check_call(cmakeCmd, stderr=subprocess.STDOUT, cwd="out", shell=True)
except subprocess.CalledProcessError as os:
    print(os.stdout)