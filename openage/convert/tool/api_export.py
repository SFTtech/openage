# Copyright 2023-2024 the openage authors. See copying.md for legal info.

"""
Export tool for dumping the nyan API of the engine from the converter.
"""

from openage.convert.entity_object.conversion.modpack import Modpack
from openage.convert.entity_object.export.formats.nyan_file import NyanFile
from openage.convert.processor.export.data_exporter import DataExporter
from openage.convert.processor.export.generate_manifest_hashes import generate_hashes
from openage.convert.service.read.nyan_api_loader import load_api
from openage.nyan.import_tree import ImportTree
from openage.util.fslike.directory import Directory
from openage.util.fslike.union import Union, UnionPath
from openage.util.fslike.wrapper import DirectoryCreator

from ...log import info


def init_subparser(cli):
    """ Initializes the parser for convert-specific args. """

    cli.set_defaults(entrypoint=main)

    cli.add_argument("dir", help="modpack output directory")


def main(args, error):
    """
    CLI entry point for API export.
    """
    del error  # unused

    path = Union().root
    path.mount(Directory(args.dir).root)

    export_api(path)


def export_api(exportdir: UnionPath) -> None:
    """
    Export the nyan API of the engine to the target directory.

    :param exportdir: The target directory for the modpack folder.
    :type exportdir: Directory
    """
    modpack = create_modpack()

    info("Dumping info file...")

    targetdir = DirectoryCreator(exportdir).root
    outdir = targetdir / "engine"

    # Modpack info file
    DataExporter.export([modpack.info], outdir)

    info("Dumping data files...")

    # Data files
    DataExporter.export(modpack.get_data_files(), outdir)

    # Manifest file
    generate_hashes(modpack, outdir)
    DataExporter.export([modpack.manifest], outdir)


def create_modpack() -> Modpack:
    """
    Create the nyan API as a modpack.

    :return: The modpack containing the nyan API.
    :rtype: Modpack
    """
    modpack = Modpack("engine")

    mod_def = modpack.get_info()

    mod_def.set_info("engine", modpack_version="0.5.0", versionstr="0.5.0", repo="openage")

    mod_def.add_include("**")

    create_nyan_files(modpack)

    return modpack


def create_nyan_files(modpack: Modpack) -> None:
    """
    Create the nyan files from the API objects.

    :param modpack: The modpack to add the nyan files to.
    :type modpack: Modpack
    """
    api_objects = load_api()
    created_nyan_files: dict[str, NyanFile] = {}

    for fqon, obj in api_objects.items():
        fqon_parts = fqon.split(".")
        obj_location = "/".join(fqon_parts[1:-2]) + "/"
        obj_filename = fqon_parts[-2] + ".nyan"
        nyan_file_path = f"{modpack.name}/{obj_location}{obj_filename}"

        if nyan_file_path in created_nyan_files:
            nyan_file = created_nyan_files[nyan_file_path]

        else:
            nyan_file = NyanFile(obj_location, obj_filename, modpack.name)
            created_nyan_files.update({nyan_file.get_relative_file_path(): nyan_file})
            modpack.add_data_export(nyan_file)

        nyan_file.add_nyan_object(obj)

    import_tree = ImportTree()

    for nyan_file in created_nyan_files.values():
        import_tree.expand_from_file(nyan_file)

    for nyan_file in created_nyan_files.values():
        nyan_file.set_import_tree(import_tree)

    set_static_aliases(import_tree)


def set_static_aliases(import_tree: ImportTree) -> None:
    """
    Create the import tree for the nyan files.

    :param import_tree: The import tree to add the aliases to.
    :type import_tree: ImportTree
    """
    import_tree.add_alias(("engine", "root"), "root")
