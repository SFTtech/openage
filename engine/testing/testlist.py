# Copyright 2015-2023 the openage authors. See copying.md for legal info.

""" Lists of all possible tests; enter your tests here. """


def doctest_modules():
    """
    Yields the names of all Python modules that shall be tested during doctest.
    """

    yield "engine.util.math"
    yield "engine.util.strings"
    yield "engine.util.system"


def tests_py():
    """
    Yields tuples of (name, description, condition_function)
    for all Python test methods.

    If no description is required, just the name may be yielded.
    """

    yield ("engine.testing.doctest.test",
           "doctest on all modules from DOCTEST_MODULES")
    yield "engine.assets.test"
    yield ("engine.cabextract.test.test", "test CAB archive extraction",
           lambda env: env["has_assets"])
    yield "engine.convert.service.init.changelog.test"
    yield "engine.cppinterface.exctranslate_tests.cpp_to_py"
    yield ("engine.cppinterface.exctranslate_tests.cpp_to_py_bounce",
           "translates the exception back and forth a few times")
    yield ("engine.testing.misc_cpp.enum",
           "tests the interface for C++'s util::Enum class")
    yield ("engine.util.fslike.test.test",
           "test the filesystem abstraction subsystem")
    yield "engine.util.threading.test_concurrent_chain"


def demos_py():
    """
    Yields tuples of (name, description) for all Python demo methods.
    """

    yield ("engine.cppinterface.exctranslate_tests.cpp_to_py_demo",
           "translates a C++ exception and its causes to python")
    yield ("engine.log.tests.demo",
           "demonstrates the translation of Python log messages")
    yield ("engine.convert.service.export.opus.demo.convert",
           "encodes an opus file from a wave file")
    yield ("engine.event.demo.curvepong",
           "play pong on steroids through future prediction")
    yield ("engine.renderer.tests.renderer_demo",
           "showcases the new renderer")
    yield ("engine.main.tests.engine_demo",
           "showcases the engine features")


def benchmark_py():
    """
    Yields tuples of (name, description) for python benchmark
    methods.
    """

    # TODO Add a real benchmark here, and remove this one
    yield ("engine.testing.benchmark.benchmark_test_function",
           "Benchmark yourself")


def tests_cpp():
    """
    Yields tuples of (name, description, condition_function)
    for all C++ test methods.

    If no description is required, just the name may be yielded.
    """

    yield "openage::coord::tests::coord"
    yield "openage::datastructure::tests::concurrent_queue"
    yield "openage::datastructure::tests::constexpr_map"
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
    yield "openage::util::tests::fixed_point"
    yield "openage::util::tests::init"
    yield "openage::util::tests::matrix"
    yield "openage::util::tests::quaternion"
    yield "openage::util::tests::vector"
    yield "openage::util::tests::siphash"
    yield "openage::util::tests::array_conversion"
    yield "openage::input::legacy::tests::parse_event_string", "keybinds parsing"
    yield "openage::curve::tests::container"
    yield "openage::curve::tests::curve_types"
    yield "openage::event::tests::eventtrigger"


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
    yield ("openage::gamestate::tests::activity_demo",
           "showcases the activity system in the gamestate")
    yield ("openage::input::tests::action_demo",
           "showcases the low-level input system")
    yield ("openage::log::tests::demo",
           "showcases the logging system")
    yield ("openage::pyinterface::tests::err_py_to_cpp_demo",
           "translates a Python exception to C++")
    yield ("openage::pyinterface::tests::pyobject_demo",
           "a tiny interactive interpreter using PyObjectRef")


def benchmark_cpp():
    """
    Yields tuples of (name, description) for C++ benchmark
    methods.
    """

    # TODO Add a real benchmark here!
    yield ("openage::test::benchmark", "Test the benchmark")
