#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import os
import subprocess
import sys

from pathlib import Path

import setuptools
import setuptools.command.build_ext


def get_project_version() -> str:
    version : str = "0.0.0"
    python_dir = Path(__file__).parent.absolute()
    version_file = python_dir.parent / "version.txt"
    print(f"Reading C++ version from {version_file}")
    with open(version_file, "r") as f:
        version = f.readline().strip()
    print(f"C++ library version={version}")
    return version


class CMakeExtension(setuptools.Extension):
    def __init__(self, name: str, sourcedir=""):
        super().__init__(name, sources=[])
        self.sourcedir = Path(sourcedir).parent
        self.sourcedir = self.sourcedir.absolute()


class CMakeBuild(setuptools.command.build_ext.build_ext):
    def build_extension(self, ext: CMakeExtension):
        extdir = Path(self.get_ext_fullpath(ext.name)).parent.absolute()
        build_type = "Release"

        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            "-DBUILD_PYGRABCUT=ON",
            "-DBUILD_GRABCUT_APPS=OFF",
            "-DBUILD_SHARED_LIBS=OFF",
        ]
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",

            #f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
        ]
        build_args = []
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            if hasattr(self, "parallel") and self.parallel:
                build_args += [f"-j{self.parallel}"]

        if not Path(self.build_temp).exists():
            Path(self.build_temp).mkdir(parents=True, exist_ok=True)

        subprocess.check_call(["cmake", ext.sourcedir.parent] + cmake_args, cwd=self.build_temp)
        subprocess.check_call(["cmake", "--build", ".", "-j"] + build_args, cwd=self.build_temp)


setuptools.setup(
    name="pygrabcut",
    version=get_project_version(),
    author="Piotr Gródek",
    author_email="ipc0nfg+gh@gmail.com",
    description="Grabcut algorithm implemented in C++ usable from python",
    license="Other/Proprietary License",  # figure out later license
    url="https://github.com/rAum/grabcut",
    ext_modules=[CMakeExtension("_pygrabcut")],
    cmdclass={"build_ext": CMakeBuild},
    #packages=["pygrabcut"],
    #package_dir={"": "src"},
    package_data={"": ["*.so"]},
    python_requires=">=3.6",
    zip_safe=False,
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3 :: Only",
        "License :: Other/Proprietary License",
        "Development Status :: 1 - Planning",
        "Operating System :: OS Independent",
        "Topic :: Scientific/Engineering :: Image Processing",
        "Private :: Do Not Upload",  # to prevent accident upload
    ],
)
