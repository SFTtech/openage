# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Generates libopenage/coord/coord_{xy, xyz, ne_se, ne_se_up}.{h, cpp}
"""

from jinja2 import Template


def generate_coord_basetypes(projectdir):
    """
    Generates the test/demo method symbol lookup file from tests_cpp.

    projectdir is a util.fslike.path.Path.
    """
    # this list contains all required member lists.
    member_lists = [
        ["x", "y"],
        ["x", "y", "z"],
        ["ne", "se"],
        ["ne", "se", "up"]
    ]

    # this list maps template file name to output file name.
    # the output filename is a jinja2 template itself.
    template_files_spec = [
        ("libopenage/coord/coord.h.template",
         "libopenage/coord/coord_{{ ''.join(members) }}.gen.h"),
        ("libopenage/coord/coord.cpp.template",
         "libopenage/coord/coord_{{ ''.join(members) }}.gen.cpp")
    ]

    templates = []
    for template_filename, output_filename in template_files_spec:
        with projectdir.joinpath(template_filename).open() as template_file:
            templates.append((
                Template(template_file.read()),
                Template(output_filename)
            ))

    for member_list in member_lists:
        def format_members(formatstring, join_with=", "):
            """
            For being called by the template engine.

            >>> format_members("{0} = {0}")
            "x = x, y = y"
            """
            return join_with.join(formatstring.format(m) for m in member_list)

        template_dict = dict(
            members=member_list,
            formatted_members=format_members,
            camelcase="".join(member.title() for member in member_list),
        )

        for template, output_filename_template in templates:
            output_filename = output_filename_template.render(template_dict)
            with projectdir.joinpath(output_filename).open("w") as output_file:
                output = template.render(template_dict)
                output_file.write(output)
                if not output.endswith('\n'):
                    output_file.write('\n')
