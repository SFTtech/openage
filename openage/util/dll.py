# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Windows-specific loading of compiled Python modules and DLLs.
"""

import inspect
import os
import sys

# python.dll location
DEFAULT_PYTHON_DLL_DIR = os.path.dirname(sys.executable)

# openage.dll locations (relative to this file)
DEFAULT_OPENAGE_DLL_DIRs = [
    "../../libopenage/Debug",
    "../../libopenage/Release",
    "../../libopenage/RelWithDebInfo",
    "../../libopenage/MinSizeRel",
]

# nyan.dll locations (relative to this file)
DEFAULT_NYAN_DLL_DIRS = [
    "../../../../nyan/build/nyan/Debug",
    "../../../../nyan/build/nyan/Release",
    "../../../../nyan/build/nyan/RelWithDebInfo",
    "../../../../nyan/build/nyan/MinSizeRel",
    "../../nyan-external/bin/nyan/Debug",
    "../../nyan-external/bin/nyan/Release",
    "../../nyan-external/bin/nyan/RelWithDebInfo",
    "../../nyan-external/bin/nyan/MinSizeRel",
]


class DllDirectoryManager:
    """
    Manages directories that should be added to/removed from Python's DLL search path.

    All dependent DLLs or compiled cython modules that are not in Python's default search path
    mst be added manually at runtime. Basically, this applies to all openage-specific libraries.
    """

    def __init__(self, directory_paths: list[str]):
        """
        Create a new DLL directory manager.

        :param directory_paths: Absolute paths to the directories that are added.
        """
        # Directory paths
        self.directories = directory_paths

        # Store handles for added directories
        self.handles = []

    def add_directories(self):
        """
        Add the manager's directories to Python's DLL search path.
        """
        for directory in self.directories:
            handle = os.add_dll_directory(directory)
            self.handles.append(handle)

    def remove_directories(self):
        """
        Remove the manager's directories from Python's DLL search path.
        """
        for handle in self.handles:
            handle.close()

        self.handles = []

    def __del__(self):
        """
        Ensure that DLL paths are removed when the object is deleted.
        """
        self.remove_directories()

    def __enter__(self):
        """
        Enter a context guard.
        """
        self.add_directories()

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Exit a context guard.
        """
        self.remove_directories()

    def __getstate__(self):
        """
        Change pickling behavior so that directory handles are not serialized.
        """
        content = self.__dict__
        content["handles"] = []
        return content


def default_paths() -> list[str]:
    """
    Create a list of default paths.
    """
    directory_paths = []

    # Add Python DLL search path
    directory_paths.append(DEFAULT_PYTHON_DLL_DIR)

    file_dir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda: 0)))

    # Add openage DLL search paths
    for candidate in DEFAULT_OPENAGE_DLL_DIRs:
        path = os.path.join(file_dir, candidate)
        if os.path.exists(path):
            directory_paths.append(path)

    # Add nyan DLL search paths
    for candidate in DEFAULT_NYAN_DLL_DIRS:
        path = os.path.join(file_dir, candidate)
        if os.path.exists(path):
            directory_paths.append(path)

    return directory_paths
