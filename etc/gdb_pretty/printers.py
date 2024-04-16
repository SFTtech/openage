# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Pretty printers for GDB.
"""

import re
import gdb  # type: ignore


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

        return None


pp = PrinterControl('openage')
gdb.printing.register_pretty_printer(None, pp)


def printer_typedef(type_name: str):
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


def format_fixed_point(value: int, fractional_bits: int) -> float:
    """
    Formats a fixed point value to a double.

    :param value: The fixed point value.
    :type value: int
    :param fractional_bits: The number of fractional bits.
    :type fractional_bits: int
    """
    to_double_factor = 1 / pow(2, fractional_bits)
    return float(value) * to_double_factor


@printer_regex('^openage::coord::(camhud|chunk|input|phys|scene|term|tile|viewport)(2|3)?(_delta)?')
class CoordPrinter:
    """
    Pretty printer for openage::coord types (CoordNeSe, CoordNeSeUp, CoordXY, CoordXYZ).

    TODO: Inherit from gdb.ValuePrinter when gdb 14.1 is available in all distros.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

        # Each coord type has one parent which is either
        # of CoordNeSe, CoordNeSeUp, CoordXY, CoordXYZ
        # From this parent we can get the fields
        self._parent_type = self.__val.type.fields()[0].type

    def to_string(self):
        """
        Get the coord as a string.
        """
        field_vals = []
        for child in self._parent_type.fields():
            # Include the fixed point coordinates in the summary
            val = self.__val[child.name]
            num = format_fixed_point(
                int(val['raw_value']),
                int(val.type.template_argument(1))
            )
            field_vals.append(f"{num:.5f}")

        # Example: phys3[1.00000, 2.00000, 3.00000]
        return f"{self.__val.type.tag.split('::')[-1]}[{', '.join(field_vals)}]"

    def children(self):
        """
        Get the displayed children of the coord.
        """
        for child in self._parent_type.fields():
            yield (child.name, self.__val[child.name])


@printer_typedef('openage::time::time_t')
class TimePrinter:
    """
    Pretty printer for openage::time::time_t.

    TODO: Inherit from gdb.ValuePrinter when gdb 14.1 is available in all distros.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        """
        Get the time as a string.

        Format: SS.sss (e.g. 12.345s)
        """
        seconds = format_fixed_point(
            int(self.__val['raw_value']),
            int(self.__val.type.template_argument(1))
        )

        # show as seconds with millisecond precision
        return f'{seconds:.3f}s'

    def children(self):
        """
        Get the displayed children of the time value.
        """
        yield ('raw_value', self.__val['raw_value'])


@printer_regex('^openage::util::FixedPoint<.*>')
class FixedPointPrinter:
    """
    Pretty printer for openage::util::FixedPoint.

    TODO: Inherit from gdb.ValuePrinter when gdb 14.1 is available in all distros.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        """
        Get the fixed point value as a string.

        Format: 0.12345
        """
        num = format_fixed_point(
            int(self.__val['raw_value']),
            int(self.__val.type.template_argument(1))
        )
        return f'{num:.5f}'

    def children(self):
        """
        Get the displayed children of the fixed point value.
        """
        yield ('raw_value', self.__val['raw_value'])

        # calculate the precision of the fixed point value
        # 16 * log10(2) = 16 * 0.30103 = 4.81648
        # do this manualy because it's usually optimized out by the compiler
        fractional_bits = int(self.__val.type.template_argument(1))

        precision = int(fractional_bits * 0.30103 + 1)
        yield ('approx_precision', precision)


@printer_regex('^openage::util::Vector<.*>')
class VectorPrinter:
    """
    Pretty printer for openage::util::Vector.

    TODO: Inherit from gdb.ValuePrinter when gdb 14.1 is available in all distros.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        """
        Get the vector as a string.
        """
        size = self.__val.type.template_argument(0)
        int_type = self.__val.type.template_argument(1)
        return f'openage::util::Vector<{size}, {int_type}>'

    def children(self):
        """
        Get the displayed children of the vector.
        """
        size = self.__val.type.template_argument(0)
        for i in range(size):
            yield (str(i), self.__val['_M_elems'][i])

    def child(self, index):
        """
        Get the child at the given index.
        """
        return self.__val['_M_elems'][index]

    def num_children(self):
        """
        Get the number of children of the vector.
        """
        return self.__val.type.template_argument(0)

    @staticmethod
    def display_hint():
        """
        Get the display hint for the vector.
        """
        return 'array'


@printer_regex('^openage::curve::Keyframe<.*>')
class KeyframePrinter:
    """
    Pretty printer for openage::curve::Keyframe.

    TODO: Inherit from gdb.ValuePrinter when gdb 14.1 is available in all distros.
    """

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        """
        Get the keyframe as a string.
        """
        return f'openage::curve::Keyframe<{self.__val.type.template_argument(0)}>'

    def children(self):
        """
        Get the displayed children of the keyframe.
        """
        yield ('time', self.__val['time'])
        yield ('value', self.__val['value'])

# TODO: curve types
# TODO: pathfinding types
# TODO: input event codes
# TODO: eigen types https://github.com/dmillard/eigengdb
