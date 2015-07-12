# Copyright 2015-2015 the openage authors. See copying.md for legal info.

from libc.string cimport memcpy
from libcpp cimport bool

from cpython.ref cimport PyObject
from cpython.bytes cimport PyBytes_FromStringAndSize

from libopenage.util.compress.lzxd cimport (
    LZXDecompressor as c_LZXDecompressor,
    LZX_FRAME_SIZE
)

from libopenage.pyinterface.functional cimport Func2

from openage.cppinterface.typedefs cimport voidptr


cdef class LZXDecompressor:
    """
    Decompresses an LZX-compressed stream.

    Its constructor takes a callback method that shall provide the compressed
    data stream.

    decompress_next_frame() decodes the next frame and returns the
    decompressed data as a bytes object.

    Constructor arguments:

    @param py_read_callback
        The read callback method. On EOF, b"" should be returned.
        Otherwise, at least one and at most the number of requested bytes
        should be returned.
    @param window_bits
        The LZX window size; for MSCAB archives, this is indicated in the
        CAB file header.
    @param reset_interval
        The LZX stream reset interval; for MSCAB archives, this is 0.
    """

    """ The C++ object. """
    cdef c_LZXDecompressor *thisptr

    # "public" makes those fields available from python (__getattr__).

    """ The python read callback function. """
    cdef public object py_read_callback

    """
    If a C function throws an exception, the c_LZXDecompressor object
    becomes invalid (using it again is undefined behavior).
    """
    cdef public bool invalid

    @staticmethod
    cdef size_t read_callback(void *context, unsigned char *buf, size_t size) except * with gil:
        """
        Read callback function that's passed to c_LZXDecompressor.

        context is a pointer to this LZXDecompressor object; the callback
        uses it to determine the py_read_callback function.
        """
        cdef object lzxdecompressor_obj = <object> <PyObject *> context
        cdef bytes result

        result = lzxdecompressor_obj.py_read_callback(size)

        cdef size_t resultsize
        cdef unsigned char *resultbuf
        resultsize, resultbuf = len(result), <unsigned char *> result

        if resultsize > size:
            raise Exception(
                "read callback returned more data then requested.")

        if resultbuf == NULL:
            raise Exception(
                "internal error in read callback")

        memcpy(buf, resultbuf, resultsize)
        return resultsize

    def __cinit__(self, py_read_callback,
                  unsigned int window_bits=21,
                  unsigned int reset_interval=0):

        """ Invokes the C++ constructor. """

        self.invalid = False
        self.py_read_callback = py_read_callback

        cdef Func2[size_t, unsigned char *, size_t] read_callback
        read_callback.bind1[voidptr](self.read_callback, <void *> <PyObject *> self)

        with nogil:
            self.thisptr = new c_LZXDecompressor(
                read_callback, window_bits, reset_interval)

    def __dealloc__(self):
        with nogil:
            del self.thisptr

    def decompress_next_frame(self):
        """
        Decodes the next frame of the stream

        @returns
            b"" on EOF,
            a bytes object between 1 and LZX_FRAME_SIZE bytes in size
            otherwise.
        """
        if self.invalid:
            raise Exception("LZXDecompressor has been invalidated by a "
                            "previous error.")

        # alloc the 32-KiB frame buffer
        cdef bytes result = PyBytes_FromStringAndSize(NULL, LZX_FRAME_SIZE)
        cdef unsigned char *result_buf = result

        # call the C method to fill the frame buffer
        cdef unsigned int frame_size
        try:
            with nogil:
                frame_size = self.thisptr.decompress_next_frame(result_buf)
        except:
            # set self.invalid to make sure we'll never call the decompressor
            # again (as that would be undefined behavior now).
            self.invalid = True
            raise

        # performance optimization:
        # all but the last frame have this size.
        if frame_size == LZX_FRAME_SIZE:
            return result

        # the last frame will have some non-zero size.
        # EOF is indicated by a zero return value (so we'll return b"").
        return result[:frame_size]
