# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import collections

from .. import assets
from . import codegen

testspec_asset = "tests_cpp"


def generate_testregistration(cpp_src_dir):
    generated_name = "testing/testlist.gen.cpp"
    template_name = os.path.join(cpp_src_dir, "testing/testlist.cpp.template")
    testspec_filename = assets.get_file(testspec_asset)

    codegen.add_manual_depend(template_name)
    codegen.add_manual_depend(testspec_filename)

    class Namespace:
        def __init__(self):
            self.namespaces = collections.defaultdict(self.__class__)
            self.functions = []

        def add_functionname(self, path, functiontype):
            if len(path) == 1:
                self.functions.append((path[0], functiontype))
            else:
                subnamespace = self.namespaces[path[0]]
                subnamespace.add_functionname(path[1:], functiontype)

        def gen_prototypes(self):
            for functionname, functiontype in self.functions:
                if functiontype == 'test':
                    yield "void %s();\n" % functionname
                elif functiontype == 'demo':
                    yield "void %s(int argc, char **argv);\n" % functionname
                else:
                    raise Exception("unknown function type: %s" % functiontype)

            for namespacename, namespace in sorted(self.namespaces.items()):
                yield "namespace %s {\n" % namespacename
                for line in namespace.gen_prototypes():
                    yield line
                yield "} // namespace %s\n" % namespacename

        def get_functionnames(self, wantedtype):
            for functionname, functiontype in self.functions:
                if functiontype == wantedtype:
                    yield functionname

            for namespacename, namespace in sorted(self.namespaces.items()):
                for functionname in namespace.get_functionnames(wantedtype):
                    yield "::".join((namespacename, functionname))

    root_namespace = Namespace()

    with open(testspec_filename) as testspec:
        for test in testspec.read().split('\n'):
            test = test.strip()
            if not test or test.startswith('#'):
                continue

            try:
                testname, testtype, _ = test.split(' ', maxsplit=2)
            except ValueError as e:
                raise Exception("testspec malformed: %s"
                                % testspec_filename) from e

            root_namespace.add_functionname(testname.split('::'), testtype)

    func_prototypes = list(root_namespace.gen_prototypes())

    test_mappings = [
        "{\"%s\", ::%s}" % (functionname, functionname)
        for functionname in root_namespace.get_functionnames('test')]

    demo_mappings = [
        "{\"%s\", ::%s}" % (functionname, functionname)
        for functionname in root_namespace.get_functionnames('demo')]

    with open(template_name) as f:
        content = f.read()

    content = content.replace('FUNCTION_PROTOTYPES', "".join(func_prototypes))
    content = content.replace('TEST_MAPPINGS', ",\n\t".join(test_mappings))
    content = content.replace('DEMO_MAPPINGS', ",\n\t".join(demo_mappings))

    return generated_name, content
