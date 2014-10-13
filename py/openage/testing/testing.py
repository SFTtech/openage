from .. import assets

testspec_asset = "tests_python"

def read_testspec():
    tests = {}

    testspec_filename = assets.get_asset_file(testspec_asset)
    with open(testspec_filename) as testspec:
        for test in testspec.read().split('\n'):
            test = test.strip()
            if not test or test.startswith('#'):
                continue

            try:
                testname, description = test.split(':', maxsplit=1)
            except ValueError as e:
                raise Exception("testspec malformed: %s"
                                % testspec_filename) from e

            tests[testname] = description


def run_test(functionname):
    module, name = functionname.rsplit('.', maxsplit=1)
    namespace = {}
    exec('from %s import %s' % (module, name), namespace)
    function = eval(name, namespace)

    result = function()
    if not result:
        raise Exception("test %s returned %s" % (testname, result))
