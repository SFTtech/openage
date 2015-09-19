# Copyright 2015-2016 the openage authors. See copying.md for legal info.

""" Lists of all possible tests; enter your tests here. """


def doctest_modules():
    """
    Yields the names of all Python modules that shall be tested during doctest.
    """

    yield "openage.util.math"
    yield "openage.util.strings"
    yield "openage.util.system"


def tests_py():
    """
    Yields tuples of (name, description) for all Python test methods.

    If no description is required, just the name may be yielded.
    """

    yield ("openage.testing.doctest.test",
           "doctest on all modules from DOCTEST_MODULES")
    yield "openage.assets.test"
    yield "openage.cabextract.test.test"
    yield "openage.convert.changelog.test"
    yield "openage.cppinterface.exctranslate_tests.cpp_to_py"
    yield ("openage.cppinterface.exctranslate_tests.cpp_to_py_bounce",
           "translates the exception back and forth a few times")
    yield ("openage.testing.misc_cpp.enum",
           "tests the interface for C++'s util::Enum class")
    yield "openage.util.threading.test_concurrent_chain"


def demos_py():
    """
    Yields tuples of (name, description) for all Python demo methods.
    """

    yield ("openage.cppinterface.exctranslate_tests.cpp_to_py_demo",
           "translates a C++ exception and its causes to python")
    yield ("openage.log.tests.demo",
           "demonstrates the translation of Python log messages")


def tests_cpp():
    """
    Yields tuples of (name, description) for all C++ test methods.

    If no description is required, just the name may be yielded.
    """

    yield "openage::coord::tests::coord"
    yield "openage::datastructure::tests::constexpr_map"
    yield "openage::datastructure::tests::doubly_linked_list"
    yield "openage::datastructure::tests::pairing_heap"
    yield "openage::job::tests::test_job_manager"
    yield "openage::path::tests::path_node", "pathfinding"
    yield "openage::pyinterface::tests::pyobject"
    yield "openage::pyinterface::tests::err_py_to_cpp"
    yield "openage::renderer::tests::font"
    yield "openage::renderer::tests::font_manager"
    yield "openage::rng::tests::run"
    yield "openage::util::tests::constinit_vector"
    yield "openage::util::tests::enum_"
    yield "openage::util::tests::init"
    yield "openage::util::tests::matrix"
    yield "openage::util::tests::vector"
    yield "openage::input::tests::parse_event_string", "keybinds parsing"


def demos_cpp():
    """
    Yields tuples of (name, description) for all C++ demo methods.
    """

    yield ("openage::console::tests::render",
           "prints a few test lines to a buffer, and renders it to stdout")
    yield ("openage::console::tests::interactive",
           "showcases console as an interactive terminal on your current tty")
    yield ("openage::error::demo",
           "showcases the openage exceptions, including backtraces")
    yield ("openage::log::tests::demo",
           "showcases the logging system")
    yield ("openage::pyinterface::tests::err_py_to_cpp_demo",
           "translates a Python exception to C++")
    yield ("openage::pyinterface::tests::pyobject_demo",
           "a tiny interactive interpreter using PyObjectRef")
