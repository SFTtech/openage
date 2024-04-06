# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Pretty printers for GDB.
"""

import gdb


class PrinterControl(gdb.printing.PrettyPrinter):
    """
    Exposes a pretty printer for a specific type name.
    """

    def __init__(self, type_name: str, printer):
        super().__init__(type_name)
        self.printer = printer

    def __call__(self, val):
        if val.type.name == self.name:
            return self.printer(val)


def printer(type_name: str):
    """
    Decorator for pretty printers.

    :param type_name: The name of the type to register the printer for.
    :type type_name: str
    """
    def _register_printer(printer):
        """
        Registers the printer with GDB.
        """
        gdb.printing.register_pretty_printer(
            None,
            PrinterControl(type_name, printer)
        )

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
        return f'{seconds:.5f}s'

    def children(self):
        yield ('raw_value', self.__val['raw_value'])
        # calculate the precision of the fixed point value
        # 16 * log10(2) = 16 * 0.30103 = 4.81648
        # do this manualy because it's usually optimized out by the compiler
        precision = int(16 * 0.30103 + 1)
        yield ('approx_precision', precision)


# def add_pretty_printer(val):
#     if str(val.type) == 'openage::time::time_t':
#         return TimePrinter(val)

#     return None


# def register_openage_printers(objfile):
#     """
#     Register the openage pretty printers with GDB.
#     """
#     gdb.pretty_printers.append(add_pretty_printer)
