# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Implements the MSCAB checksum algorithm.

The algorithm is used for calculating checksums of data blocks, and was
designed by a true genius:

Given e.g. 11 bytes of data, ABCDEFGHIJK, it creates three four-byte integers

M  L
S  S
B  B

DCBA    <- little endian
HGFE    <- little endian
 IJK    <- big endian

and XORs them.

For the original doc, see

https://msdn.microsoft.com/en-us/library/bb417343.aspx#chksum
"""

cdef unsigned int as_little_endian(unsigned char *data) nogil:
    """
    Given a character pointer, decodes the next four bytes as a little-endian
    value.
    """
    return (
        ((<unsigned int> data[0]) <<  0) |
        ((<unsigned int> data[1]) <<  8) |
        ((<unsigned int> data[2]) << 16) |
        ((<unsigned int> data[3]) << 24))


cdef unsigned int as_big_endian(unsigned char *data, unsigned int bytecount) nogil:
    """
    Given a character pointer, decodes the next bytecount bytes as a
    big-endian value.

    Won't work properly for bytecount > 4.
    """
    cdef unsigned int result = 0

    for i in range(bytecount):
        result <<= 8
        result |= <unsigned int> data[i]

    return result


def mscab_csum(bytes data):
    """
    Implements the checksum algorithm that is described in the module doc.

    The for loop gets optimized to C-level performance - wheeee!
    """
    cdef unsigned int result = 0

    cdef size_t bufsize
    cdef unsigned char *buf
    bufsize, buf = len(data), data
    if buf == NULL:
        raise Exception("invalid data for checksum")

    cdef unsigned int *data_ptr = <unsigned int *> buf

    cdef unsigned int count = bufsize // 4
    cdef unsigned int remainder = bufsize % 4

    with nogil:
        for i in range(count):
            result ^= data_ptr[i]

    # we have so far ignored endianess issues.
    # on a non-little endian system, the interpretation is wrong.
    # thus, interpret it as binary data and decode it as little endian.
    result = (
        as_little_endian(<unsigned char *> &result)
        ^
        as_big_endian(<unsigned char *> &data_ptr[count], remainder)
    )

    return result
