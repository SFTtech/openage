# Copyright 2014-2018 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C

"""
dataformat-specific utilities.
there would be no use in providing them globally.
"""

# type lookup for C -> python struct
struct_type_lookup = {
    "char":               "b",
    "unsigned char":      "B",
    "int8_t":             "b",
    "uint8_t":            "B",
    "short":              "h",
    "unsigned short":     "H",
    "int16_t":            "h",
    "uint16_t":           "H",
    "int":                "i",
    "unsigned int":       "I",
    "int32_t":            "i",
    "uint32_t":           "I",
    "long":               "l",
    "unsigned long":      "L",
    "long long":          "q",
    "unsigned long long": "Q",
    "int64_t":            "q",
    "uint64_t":           "Q",
    "float":              "f",
    "double":             "d",
    "char[]":             "s",
}


def encode_value(val):
    """
    encodes val to a (possibly escaped) string,
    for use in a csv column of type valtype (string)
    """

    val = str(val)
    val = val.replace("\\", "\\\\")
    val = val.replace(",", "\\,")
    val = val.replace("\n", "\\n")

    return val


def gather_data(obj, members):
    """
    queries the given object for the given member variables
    and returns that as a dict.

    key: member name
    value: obj's member value
    """
    ret = dict()

    for attr, _ in members:
        ret[attr] = getattr(obj, attr)

    return ret


def determine_header(for_type):
    """
    returns the includable headers for using the given C type.
    """

    # these headers are needed for the type
    ret = set()

    from .header_snippet import HeaderSnippet

    # yes, i know. but we can hardly do better.
    # pylint: disable=too-many-locals

    cstddefh = HeaderSnippet("cstddef", is_global=True)
    cstdinth = HeaderSnippet("cstdint", is_global=True)
    cstdioh = HeaderSnippet("cstdio", is_global=True)
    cstringh = HeaderSnippet("cstring", is_global=True)
    stringh = HeaderSnippet("string", is_global=True)
    vectorh = HeaderSnippet("vector", is_global=True)
    error_error_h = HeaderSnippet("error/error.h", is_global=False)
    log_h = HeaderSnippet("log.h", is_global=False)
    util_csv_h = HeaderSnippet("util/csv.h", is_global=False)
    util_path_h = HeaderSnippet("util/path.h", is_global=False)
    util_strings_h = HeaderSnippet("util/strings.h", is_global=False)

    # lookup for type->{header}
    type_map = {
        "int8_t":          {cstdinth},
        "uint8_t":         {cstdinth},
        "int16_t":         {cstdinth},
        "uint16_t":        {cstdinth},
        "int32_t":         {cstdinth},
        "uint32_t":        {cstdinth},
        "int64_t":         {cstdinth},
        "uint64_t":        {cstdinth},
        "std::string":     {stringh},
        "std::vector":     {vectorh},
        "strcmp":          {cstringh},
        "strncpy":         {cstringh},
        "strtok_custom":   {util_strings_h},
        "sscanf":          {cstdioh},
        "size_t":          {cstddefh},
        "float":           set(),
        "int":             set(),
        "csv_collection":  {util_csv_h},
        "read_csv_file":   {util_csv_h},
        "csv_subdata":     {util_csv_h},
        "engine_error":    {error_error_h},
        "engine_log":      {log_h},
        "util::Path":      {util_path_h},
    }

    if for_type in type_map:
        ret |= type_map[for_type]
    else:
        raise Exception("could not determine header for {}".format(for_type))

    return ret


def determine_headers(for_types):
    if not any(isinstance(for_types, type) for type in (list, tuple, set)):
        for_types = {for_types}

    ret = set()
    for t in for_types:
        ret |= determine_header(t)

    return ret


def commentify_lines(commentstr, text):
    return "".join(commentstr + line + "\n" for line in text.split("\n"))
