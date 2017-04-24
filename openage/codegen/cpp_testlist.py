# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Generates code for C++ testing, mostly the table to look up symbols from test
names.
"""

import collections


class Namespace:
    """
    Represents a C++ namespace, which contains other namespaces and functions.

    gen_prototypes() generates the code for the namespace.
    """
    def __init__(self):
        self.namespaces = collections.defaultdict(self.__class__)
        self.functions = []

    def add_functionname(self, path):
        """
        Adds a function to the namespace.

        Path is the qualified function "path" (e.g., openage::test::foo)
        has the path ["openage", "test", "foo"].

        Descends recursively, creating subnamespaces as required.
        """
        if len(path) == 1:
            self.functions.append(path[0])
        else:
            subnamespace = self.namespaces[path[0]]
            subnamespace.add_functionname(path[1:])

    def gen_prototypes(self):
        """
        Generates the actual C++ code for this namespace,
        including all sub-namespaces and function prototypes.
        """
        for name in self.functions:
            yield "void %s();\n" % name

        for namespacename, namespace in sorted(self.namespaces.items()):
            yield "namespace %s {\n" % namespacename
            for line in namespace.gen_prototypes():
                yield line
            yield "} // %s\n\n" % namespacename

    def get_functionnames(self):
        """
        Yields all function names in this namespace,
        as well as all subnamespaces.
        """
        for name in self.functions:
            yield name

        for namespacename, namespace in sorted(self.namespaces.items()):
            for name in namespace.get_functionnames():
                yield namespacename + "::" + name


def generate_testlist(projectdir):
    """
    Generates the test/demo method symbol lookup file from tests_cpp.

    projectdir is a util.fslike.path.Path.
    """
    root_namespace = Namespace()

    from ..testing.list_processor import list_targets_cpp

    for testname, _, _, _ in list_targets_cpp():
        root_namespace.add_functionname(testname.split('::'))

    func_prototypes = list(root_namespace.gen_prototypes())

    method_mappings = [
        "{\"%s\", ::%s}" % (functionname, functionname)
        for functionname in root_namespace.get_functionnames()]

    tmpl_path = projectdir.joinpath("libopenage/testing/testlist.cpp.template")
    with tmpl_path.open() as tmpl:
        content = tmpl.read()

    content = content.replace('FUNCTION_PROTOTYPES', "".join(func_prototypes))
    content = content.replace('METHOD_MAPPINGS', ",\n\t".join(method_mappings))

    gen_path = projectdir.joinpath("libopenage/testing/testlist.gen.cpp")
    with gen_path.open("w") as gen:
        gen.write(content)
