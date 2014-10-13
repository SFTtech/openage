import os
import collections

from .. import assets
from . import codegen

testspec_asset = "tests_cpp"

def generate_testregistration(cpp_src_dir):
    generated_name = "testing/testlist.gen.cpp"
    template_name = os.path.join(cpp_src_dir, "testing/testlist.cpp.template")
    testspec_filename = assets.get_asset_file(testspec_asset)

    codegen.add_manual_depend(template_name)
    codegen.add_manual_depend(testspec_filename)

    class Namespace:
        def __init__(self):
            self.namespaces = collections.defaultdict(self.__class__)
            self.functions = []

        def add_functionname(self, path):
            if len(path) == 1:
                self.functions.append(path[0])
            else:
                self.namespaces[path[0]].add_functionname(path[1:])

        def gen_prototypes(self):
            for functionname in self.functions:
                yield "void %s();\n" % functionname

            for namespacename, namespace in sorted(self.namespaces.items()):
                yield "namespace %s {\n" % namespacename
                for line in namespace.gen_prototypes():
                    yield line
                yield "} // namespace %s\n" % namespacename

        def get_functionnames(self):
            for functionname in self.functions:
                yield functionname

            for namespacename, namespace in sorted(self.namespaces.items()):
                for functionname in namespace.get_functionnames():
                    yield "::".join((namespacename, functionname))

    root_namespace = Namespace()

    with open(testspec_filename) as testspec:
        for test in testspec.read().split('\n'):
            test = test.strip()
            if not test or test.startswith('#'):
                continue

            try:
                testname, _ = test.split(' ', maxsplit=1)
            except ValueError as e:
                raise Exception("testspec malformed: %s"
                                % testspec_filename) from e

            root_namespace.add_functionname(testname.split('::'))

    test_prototypes = list(root_namespace.gen_prototypes())

    test_mappings = ["{\"%s\", ::%s}" % (functionname, functionname)
        for functionname in root_namespace.get_functionnames()]

    with open(template_name) as f:
        content = f.read()

    content = content.replace('TEST_PROTOTYPES',"".join(test_prototypes))
    content = content.replace('TEST_MAPPINGS', ",\n\t".join(test_mappings))

    return generated_name, content
