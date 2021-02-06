import os
import subprocess

current_dir=os.path.dirname(os.path.realpath(__file__))

gtest_parallel=os.path.join(current_dir, "gtest-parallel", "gtest_parallel.py")
executable = os.path.join(current_dir, "out", "Release", "Allocators.exe")
subprocess.check_call([gtest_parallel, executable], shell=True)