# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

import struct

from .util import struct_type_lookup
from ...util.strings import decode_until_null

from .data_definition import DataDefinition
from .generated_file import GeneratedFile
from .member_access import READ, READ_EXPORT, READ_UNKNOWN, NOREAD_EXPORT
from .members import IncludeMembers, ContinueReadMember, MultisubtypeMember, GroupMember, SubdataMember, DataMember
from .struct_definition import StructDefinition, vararray_match, integer_match


class Exportable:
    """
    superclass for all exportable data members

    exportable classes shall inherit from this.
    """

    def __init__(self, **args):
        # store passed arguments as members
        self.__dict__.update(args)

    def dump(self, filename):
        """
        main data dumping function, the magic happens in here.

        recursively dumps all object members as DataDefinitions.

        returns [DataDefinition, ..]
        """

        ret = list()        # returned list of data definitions
        self_data = dict()  # data of the current object

        members = self.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=True)
        for _, _, member_name, member_type in members:

            # gather data members of the currently queried object
            self_data[member_name] = getattr(self, member_name)

            if isinstance(member_type, MultisubtypeMember):
                current_member_filename = filename + "-" + member_name

                if isinstance(member_type, SubdataMember):
                    is_single_subdata  = True
                    subdata_item_iter  = self_data[member_name]

                    # filename for the file containing the single subdata type entries:
                    submember_filename = current_member_filename

                else:
                    is_single_subdata  = False

                    # TODO: bad design, move import to better place:
                    from .multisubtype_base import MultisubtypeBaseFile

                multisubtype_ref_file_data = list()  # file names for ref types
                subdata_definitions = list()         # subdata member DataDefitions
                for subtype_name, submember_class in member_type.class_lookup.items():
                    # if we are in a subdata member, this for loop will only run through once.
                    # else, do the actions for each subtype

                    if not is_single_subdata:
                        # iterate over the data for the current subtype
                        subdata_item_iter  = self_data[member_name][subtype_name]

                        # filename for the file containing one of the subtype data entries:
                        submember_filename = "%s/%s" % (filename, subtype_name)

                    submember_data = list()
                    for idx, submember_data_item in enumerate(subdata_item_iter):
                        if not isinstance(submember_data_item, Exportable):
                            raise Exception("tried to dump object not inheriting from Exportable")

                        # generate output filename for next-level files
                        nextlevel_filename = "%s/%04d" % (submember_filename, idx)

                        # recursive call, fetches DataDefinitions and the next-level data dict
                        data_sets, data = submember_data_item.dump(nextlevel_filename)

                        # store recursively generated DataDefinitions to the flat list
                        ret += data_sets

                        # append the next-level entry to the list
                        # that will contain the data for the current level DataDefinition
                        if len(data.keys()) > 0:
                            submember_data.append(data)

                    # always create a file, even with 0 entries.
                    # create DataDefinition for the next-level data pile.
                    subdata_definition = DataDefinition(
                        submember_class,
                        submember_data,
                        submember_filename,
                    )

                    if not is_single_subdata:
                        # create entry for type file index.
                        # for each subtype, create entry in the subtype data file lookup file
                        # sync this with MultisubtypeBaseFile!
                        multisubtype_ref_file_data.append({
                            MultisubtypeBaseFile.data_format[0][1]: subtype_name,
                            MultisubtypeBaseFile.data_format[1][1]: "%s%s" % (
                                subdata_definition.name_data_file, GeneratedFile.output_preferences["csv"]["file_suffix"]
                            ),
                        })

                    subdata_definitions.append(subdata_definition)

                # store filename instead of data list
                # is used to determine the file to read next.
                # -> multisubtype members: type file index
                # -> subdata members:      filename of subdata
                self_data[member_name] = current_member_filename

                # for multisubtype members, append data definition for storing references to all the subtype files
                if not is_single_subdata and len(multisubtype_ref_file_data) > 0:

                    # this is the type file index.
                    multisubtype_ref_file = DataDefinition(
                        MultisubtypeBaseFile,
                        multisubtype_ref_file_data,
                        self_data[member_name],                          # create file to contain refs to subtype files
                    )

                    subdata_definitions.append(multisubtype_ref_file)

                # store all created submembers to the flat list
                ret += subdata_definitions

        # return flat list of DataDefinitions and dict of {member_name: member_value, ...}
        return ret, self_data

    def read(self, raw, offset, cls=None, members=None):
        """
        recursively read defined binary data from raw at given offset.

        this is used to fill the python classes with data from the binary input.
        """
        if cls:
            target_class = cls
        else:
            target_class = self

        # break out of the current reading loop when members don't exist in source data file
        stop_reading_members = False

        if not members:
            members = target_class.get_data_format(allowed_modes=(True, READ_EXPORT, READ, READ_UNKNOWN), flatten_includes=False)

        for _, export, var_name, var_type in members:

            if stop_reading_members:
                if isinstance(var_type, DataMember):
                    replacement_value = var_type.get_empty_value()
                else:
                    replacement_value = 0

                setattr(self, var_name, replacement_value)
                continue

            if isinstance(var_type, GroupMember):
                if not issubclass(var_type.cls, Exportable):
                    raise Exception("class where members should be included is not exportable: %s" % var_type.cls.__name__)

                if isinstance(var_type, IncludeMembers):
                    # call the read function of the referenced class (cls),
                    # but store the data to the current object (self).
                    offset = var_type.cls.read(self, raw, offset, cls=var_type.cls)
                else:
                    # create new instance of referenced class (cls),
                    # use its read method to store data to itself,
                    # then save the result as a reference named `var_name`
                    # TODO: constructor argument passing may be required here.
                    grouped_data = var_type.cls()
                    offset = grouped_data.read(raw, offset)

                    setattr(self, var_name, grouped_data)

            elif isinstance(var_type, MultisubtypeMember):
                # subdata reference implies recursive call for reading the binary data

                # arguments passed to the next-level constructor.
                varargs = dict()

                if var_type.passed_args:
                    if isinstance(var_type.passed_args, str):
                        var_type.passed_args = set(var_type.passed_args)
                    for passed_member_name in var_type.passed_args:
                        varargs[passed_member_name] = getattr(self, passed_member_name)

                # subdata list length has to be defined beforehand as a object member OR number.
                # it's name or count is specified at the subdata member definition by length.
                list_len = var_type.get_length(self)

                # prepare result storage lists
                if isinstance(var_type, SubdataMember):
                    # single-subtype child data list
                    setattr(self, var_name, list())
                    single_type_subdata = True
                else:
                    # multi-subtype child data list
                    setattr(self, var_name, {key: [] for key in var_type.class_lookup})
                    single_type_subdata = False

                # check if entries need offset checking
                if var_type.offset_to:
                    offset_lookup = getattr(self, var_type.offset_to[0])
                else:
                    offset_lookup = None

                for i in range(list_len):

                    # if datfile offset == 0, entry has to be skipped.
                    if offset_lookup:
                        if not var_type.offset_to[1](offset_lookup[i]):
                            continue
                        # TODO: don't read sequentially, use the lookup as new offset?

                    if single_type_subdata:
                        # append single data entry to the subdata object list
                        new_data_class = var_type.class_lookup[None]
                    else:
                        # to determine the subtype class, read the binary definition
                        # this utilizes an on-the-fly definition of the data to be read.
                        offset = self.read(
                            raw, offset, cls=target_class,
                            members=(((False,) + var_type.subtype_definition),)
                        )

                        # read the variable set by the above read call to
                        # use the read data to determine the denominaton of the member type
                        subtype_name = getattr(self, var_type.subtype_definition[1])

                        # look up the type name to get the subtype class
                        new_data_class = var_type.class_lookup[subtype_name]

                    if not issubclass(new_data_class, Exportable):
                        raise Exception("dumped data is not exportable: %s" % new_data_class.__name__)

                    # create instance of submember class
                    new_data = new_data_class(**varargs)

                    # recursive call, read the subdata.
                    offset = new_data.read(raw, offset, new_data_class)

                    # append the new data to the appropriate list
                    if single_type_subdata:
                        getattr(self, var_name).append(new_data)
                    else:
                        getattr(self, var_name)[subtype_name].append(new_data)

            else:
                # reading binary data, as this member is no reference but actual content.

                data_count = 1
                is_custom_member = False

                if isinstance(var_type, str):
                    # TODO: generate and save member type on the fly
                    # instead of just reading
                    is_array = vararray_match.match(var_type)

                    if is_array:
                        struct_type = is_array.group(1)
                        data_count  = is_array.group(2)
                        if struct_type == "char":
                            struct_type = "char[]"

                        if integer_match.match(data_count):
                            # integer length
                            data_count = int(data_count)
                        else:
                            # dynamic length specified by member name
                            data_count = getattr(self, data_count)

                    else:
                        struct_type = var_type
                        data_count  = 1

                elif isinstance(var_type, DataMember):
                    # special type requires having set the raw data type
                    struct_type = var_type.raw_type
                    data_count  = var_type.get_length(self)
                    is_custom_member = True

                else:
                    raise Exception("unknown data member definition %s for member '%s'" % (var_type, var_name))

                if data_count < 0:
                    raise Exception("invalid length %d < 0 in %s for member '%s'" % (data_count, var_type, var_name))

                if struct_type not in struct_type_lookup:
                    raise Exception("%s: member %s requests unknown data type %s" % (repr(self), var_name, struct_type))

                if export == READ_UNKNOWN:
                    # for unknown variables, generate uid for the unknown memory location
                    var_name = "unknown-0x%08x" % offset

                # lookup c type to python struct scan type
                symbol = struct_type_lookup[struct_type]

                # read that stuff!!11
                struct_format = "< %d%s" % (data_count, symbol)
                result        = struct.unpack_from(struct_format, raw, offset)

                if is_custom_member:
                    if not var_type.verify_read_data(self, result):
                        raise Exception("invalid data when reading %s at offset %# 08x" % (var_name, offset))

                # TODO: move these into a read entry hook/verification method
                if symbol == "s":
                    # stringify char array
                    result = decode_until_null(result[0])
                elif data_count == 1:
                    # store first tuple element
                    result = result[0]

                    if symbol == "f":
                        import math
                        if not math.isfinite(result):
                            raise Exception("invalid float when reading %s at offset %# 08x" % (var_name, offset))

                # increase the current file position by the size we just read
                offset += struct.calcsize(struct_format)

                # run entry hook for non-primitive members
                if is_custom_member:
                    result = var_type.entry_hook(result)

                    if result == ContinueReadMember.Result.ABORT:
                        # don't go through all other members of this class!
                        stop_reading_members = True

                # store member's data value
                setattr(self, var_name, result)

        return offset

    @classmethod
    def structs(cls):
        """
        create struct definitions for this class and its subdata references.
        """

        ret = list()
        self_member_count = 0

        # acquire all struct members, including the included members
        members = cls.get_data_format(allowed_modes=(True, READ_EXPORT, NOREAD_EXPORT), flatten_includes=False)
        for _, _, _, member_type in members:
            self_member_count += 1
            if isinstance(member_type, MultisubtypeMember):
                for _, subtype_class in member_type.class_lookup.items():
                    if not issubclass(subtype_class, Exportable):
                        raise Exception("tried to export structs from non-exportable %s" % subtype_class)
                    ret += subtype_class.structs()

            elif isinstance(member_type, GroupMember):
                if not issubclass(member_type.cls, Exportable):
                    raise Exception("tried to export structs from non-exportable member included class %s" % repr(member_type.cls))
                ret += member_type.cls.structs()

            else:
                continue

        # create struct only when it has members?
        if True or self_member_count > 0:
            new_def = StructDefinition(cls)
            ret.append(new_def)

        return ret

    @classmethod
    def get_effective_type(cls):
        # TODO pylint: disable=no-member
        return cls.name_struct

    @classmethod
    def get_data_format(cls, allowed_modes=False, flatten_includes=False, is_parent=False):
        # TODO pylint: disable=no-member
        for member in cls.data_format:
            export, _, member_type = member

            definitively_return_member = False

            if isinstance(member_type, IncludeMembers):
                if flatten_includes:
                    # recursive call
                    yield from member_type.cls.get_data_format(allowed_modes, flatten_includes, is_parent=True)
                    continue

            elif isinstance(member_type, ContinueReadMember):
                definitively_return_member = True

            if allowed_modes:
                if export not in allowed_modes:
                    if not definitively_return_member:
                        continue

            member_entry = (is_parent,) + member
            yield member_entry
