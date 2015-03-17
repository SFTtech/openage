# Copyright 2015-2015 the openage authors. See copying.md for legal info.

from .pack_config import PackConfig

from openage.util import VirtualFile


def packcfg_read():
    """
    test reading a virtual root config file
    """

    pcfg = PackConfig()

    f = VirtualFile(name="virtual_input_test.cfg", binary=False)
    content = (
        "[" + PackConfig.root_cfg + "]\n"
        "name = {name}\n"
        "version = {version}\n"
        "author = {author}\n"
        "pack_type = {pack_type}\n"
        "config_type = {config_type}\n"
        "config = {config}\n"
    )

    test = {
        "name": "Valid_Name_1337",
        "version": "v42-r8",
        "pack_type": "mod",
        "author": "test bot 9001",
        "config_type": "cfg",
        "config": "nonexistant.cfg",
    }

    f.set_data(content.format(**test))
    pcfg.read(f)

    if not pcfg.get_attrs() == test:
        raise Exception("parsed config does not equal the input")


def packcfg_write():
    """
    test writing the root config to a virtual file
    """

    pcfg = PackConfig()
    test = {
        "name": "roflPack2000",
        "version": "v1337-r42",
        "pack_type": "game",
        "author": "i did nothing wrong",
        "config_type": "cfg",
        "config": "roflfile.cfg",
    }
    pcfg.set_attrs(**test)

    o = VirtualFile(name="virtual_output_test.cfg", binary=False)
    pcfg.write(o)

    lines = o.data().split("\n")

    result = dict()
    # ignore first and last line
    for line in lines[1:-2]:
        k, v = tuple(line.split(" = "))
        result[k] = v

    if test.keys() != result.keys():
        raise Exception("unexpected key set found.")

    for k, v in result.items():
        if test[k] != v:
            raise Exception("%s didn't have expected value" % k)
