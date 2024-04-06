# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Pretty printers for GDB.
"""

import gdb  # type: ignore
import re


class PrinterControl(gdb.printing.PrettyPrinter):
    """
    Exposes a pretty printer for a specific type.

    Printer are searched in the following order:
        1. Exact type name _with_ typedefs
        2. Regex of type name _without_ typedefs
    """

    def __init__(self, name: str):
        super().__init__(name)

        self.name_printers = {}
        self.regex_printers = {}

    def add_printer(self, type_name: str, printer):
        """
        Adds a printer for a specific type name.
        """
        self.name_printers[type_name] = printer

    def add_printer_regex(self, regex: str, printer):
        """
        Adds a printer for a specific type name.

        :param regex: The regex to match the type name.
        :type regex: str
        """
        self.regex_printers[re.compile(regex)] = printer

    def __call__(self, val: gdb.Value):
        # Check the exact type name with typedefa
        type_name = val.type.name
        if type_name in self.name_printers:
            return self.name_printers[val.type.name](val)

        # Check the type name without typedefs and regex
        type_name = val.type.unqualified().strip_typedefs().tag
        if type_name is None:
            return None

        for regex, printer in self.regex_printers.items():
            if regex.match(type_name):
                return printer(val)


pp = PrinterControl('openage')
gdb.printing.register_pretty_printer(None, pp)


def printer(type_name: str, regex: str = None):
    """
    Decorator for pretty printers.

    :param type_name: The name of the type to register the printer for.
    :type type_name: str
    """
    def _register_printer(printer):
        """
        Registers the printer with GDB.
        """
        pp.add_printer(type_name, printer)

    return _register_printer


def printer_regex(regex: str):
    """
    Decorator for pretty printers.

    :param regex: The regex to match the type name.
    :type regex: str
    """
    def _register_printer(printer):
        """
        Registers the printer with GDB.
        """
        pp.add_printer_regex(regex, printer)

    return _register_printer


@printer('openage::time::time_t')
class TimePrinter:
    """
    Pretty printer for openage::time::time_t.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        # convert the fixed point value to double
        seconds = float(self.__val['raw_value']) * float(self.__val['to_double_factor'])
        # show as seconds with millisecond precision
        return f'{seconds:.3f}s'

    def children(self):
        yield ('raw_value', self.__val['raw_value'])
        # calculate the precision of the fixed point value
        # 16 * log10(2) = 16 * 0.30103 = 4.81648
        # do this manualy because it's usually optimized out by the compiler
        precision = int(16 * 0.30103 + 1)
        yield ('approx_precision', precision)


@printer_regex('^openage::util::FixedPoint<.*>')
class FixedPointPrinter:
    """
    Pretty printer for openage::util::FixedPoint.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        # convert the fixed point value to double
        num = float(self.__val['raw_value']) * float(self.__val['to_double_factor'])
        return f'{num:.5f}'

    def children(self):
        yield ('raw_value', self.__val['raw_value'])
