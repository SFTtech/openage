# Copyright 2021-2021 the openage authors. See copying.md for legal info.


"""
Exports data formats from a modpack to files.
"""


class DataExporter:
    """
    Writes the contents of a created modpack into a targetdir.
    """

    @staticmethod
    def export(data_files, exportdir, args):
        """
        Exports data files.
        """
        for data_file in data_files:
            output_dir = exportdir.joinpath(data_file.targetdir)
            output_content = data_file.dump()

            # generate human-readable file
            with output_dir[data_file.filename].open('wb') as outfile:
                outfile.write(output_content.encode('utf-8'))
