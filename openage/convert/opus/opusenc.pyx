# Copyright 2018-2018 the openage authors. See copying.md for legal info.

import time
from libc.string cimport memcpy, memset
from cpython.mem cimport PyMem_Malloc, PyMem_Free

from ...log import dbg, spam

from .bytearray cimport (PyByteArray_AS_STRING, PyByteArray_GET_SIZE,
                         PyByteArray_Resize)
cimport ogg, opus

def encode(inputdata):
    '''
    Converts the wav file in the bytes object 'inputdata' to an opusfile
    and returns it as bytes object. If allocations fail, raises a MemoryError.
    If something else fails, a number or an error message is returned.
    '''
    inopt = read_wav(inputdata)
    if not isinstance(inopt, dict):
        return inopt

    dbg("Wavefile: Total length: %s\tHeader length: %s\tTrailer length: %s",\
        len(inputdata), inopt['header_len'], inopt['trailer_len'])
    if inopt['trailer_len']:
        inputdata = inputdata[inopt['header_len']:-inopt['trailer_len']]
    else:
        inputdata = inputdata[inopt['header_len']:]

    outdata = bytearray(1024*50)
    cdef size_t outsize = 0  # valid bytes inside 'outdata'

    cdef ogg.ogg_packet op
    cdef ogg.ogg_page og
    cdef ogg.ogg_stream_state os
    cdef opus.OpusEncoder *oe
    cdef int err
    cdef int channels = inopt['channels']
    cdef int wframe_len = inopt['wframe_len']
    cdef opus.opus_int32 rate = inopt['input_rate']
    cdef opus.opus_int32 coding_rate = 48000
    cdef opus.opus_int32 frame_sz = 960
    cdef opus.opus_int32 lookahead

    if rate > 24000:
        coding_rate = 48000
    elif rate > 16000:
        coding_rate = 24000
    elif rate > 12000:
        coding_rate = 16000
    elif rate > 8000:
        coding_rate = 12000
    else:
        coding_rate = 8000

    frame_sz = <opus.opus_int32> (frame_sz / (48000 / coding_rate))

    if coding_rate != rate:
        dbg("Resampling necessary: input rate: %s\tcoding rate: %s",
            rate, coding_rate)
        try:
            inputdata = upsample(inputdata, len(inputdata), inopt, coding_rate)
        except MemoryError:
            raise MemoryError("Upsampling failed.") from None
        dbg("Resampled data length: %s", len(inputdata))

    # initialize structs
    if ogg.ogg_stream_init(&os, int(time.time() * 100) % (1<<31)):
        return "Could not initialize ogg_stream."

    oe = opus.opus_encoder_create(coding_rate, channels, opus.OPUS_APPLICATION_AUDIO, &err)
    if err != opus.OPUS_OK:
        ogg.ogg_stream_clear(&os)
        if err == opus.OPUS_ALLOC_FAIL:
            raise MemoryError("Could not allocate opus encoder.")
        return "Could not allocate opus encoder."

    err = opus.opus_encoder_ctl(oe, opus.OPUS_GET_LOOKAHEAD(&lookahead))
    if err != opus.OPUS_OK:
        free_buffers(&os, oe, NULL, NULL)
        return "Getting encoder lookahead failed: {}".format(err)

    inopt['pre_skip'] = lookahead / (48000 / coding_rate)

    try:
        if write_opus_header(&os, &op, inopt):
            free_buffers(&os, oe, NULL, NULL)
            return "Could not write opus header."

        while ogg.ogg_stream_flush(&os, &og):
            outsize = write_ogg_page(outsize, outdata, &og)
    except MemoryError:
        free_buffers(&os, oe, NULL, NULL)
        raise MemoryError("Could not create opus header.") from None

    try:
        if write_opus_comment(&os, &op):
            free_buffers(&os, oe, NULL, NULL)
            return "Could not write opus comment."

        while ogg.ogg_stream_flush(&os, &og):
            outsize = write_ogg_page(outsize, outdata, &og)
    except MemoryError:
        free_buffers(&os, oe, NULL, NULL)
        raise MemoryError("Could not write opus comment.") from None

    # allocate buffer for samples.
    cdef size_t padbuf_sz = wframe_len * frame_sz
    cdef size_t outbuf_sz = wframe_len * frame_sz  #TODO better size
    cdef unsigned char *inbuf = NULL
    cdef unsigned char *padbuf = <unsigned char *> PyMem_Malloc(padbuf_sz)
    op.packet = <unsigned char *> PyMem_Malloc(outbuf_sz)
    if padbuf == NULL or op.packet == NULL:
        free_buffers(&os, oe, padbuf, op.packet)
        raise MemoryError("Could not allocate buffers.")
    op.granulepos = <ogg.ogg_int64_t> inopt['pre_skip']

    # Main encoding loop (one frame per iteration)
    cdef opus.opus_int32 enc_bytes = 0  # encoded bytes in this iteration
    cdef size_t ttl_samples = 0         # read samples
    cdef size_t in_sz = len(inputdata)
    cdef size_t in_pos = 0              # offset of next sample in inputdata
    cdef size_t nb_samples              # no. of samples available this iteration
    dbg("Starting encoding loop.")
    while not op.e_o_s:
        # fill buffer. Read directly from input_data, if possible.
        nb_samples = min((in_sz - in_pos) // wframe_len, frame_sz)
        inbuf = (<unsigned char *> inputdata) + ttl_samples * wframe_len
        if nb_samples < frame_sz:
            op.e_o_s = 1
            memcpy(padbuf, inbuf, in_sz - in_pos)
            memset(padbuf + in_sz - in_pos, 0, padbuf_sz - (in_sz - in_pos))
            inbuf = padbuf

        in_pos += nb_samples * wframe_len
        ttl_samples += nb_samples


        # convert.
        enc_bytes = opus.opus_encode(oe, <opus.opus_int16 *> inbuf, frame_sz, op.packet, outbuf_sz)
        # check how much was converted/ loop or EOF ?
        if enc_bytes < 0:
            free_buffers(&os, oe, padbuf, op.packet)
            return "Encoding error in opus_encode(): {}".format(enc_bytes)

        # append converted opuspacket.
        op.bytes = enc_bytes
        op.granulepos += nb_samples * (48000 / coding_rate)
        if op.e_o_s:
            pass  # TODO. set granulepos ? for resampling decoders.
        op.packetno += 1

        err = ogg.ogg_stream_packetin(&os, &op)
        if err:
            free_buffers(&os, oe, padbuf, op.packet)
            return "ogg_stream_packetin() failed."

        try:
            # Try to write page or force, if end of stream is reached.
            while ogg.ogg_stream_pageout(&os, &og)\
                    or (op.e_o_s and ogg.ogg_stream_flush(&os, &og)):
                outsize = write_ogg_page(outsize, outdata, &og)
        except MemoryError:
                free_buffers(&os, oe, padbuf, op.packet)
                raise MemoryError("Could not write opus data.") from None


    free_buffers(&os, oe, padbuf, op.packet)

    return outdata[:outsize]


cdef int write_opus_header(ogg.ogg_stream_state *os, ogg.ogg_packet *op, inopt):
    '''
    Write the Opus header.
    Returns non-zero on failure.
    '''
    buf = bytearray(19)
    buf[0:8] = b'OpusHead'
    buf[8] = 1                # Opus encapsulating version.
    buf[9] = inopt['channels']
    buf[10:12] = inopt['pre_skip'].to_bytes(2, 'little')
    buf[12:16] = inopt['input_rate'].to_bytes(4, 'little')
    buf[16:18] = b'\x00\x00'  # Output gain
    buf[18] = 0               # Channel mapping family


    op.packet = buf
    op.bytes = <long> 19
    op.b_o_s = <long> 1
    op.e_o_s = <long> 0
    op.granulepos = <ogg.ogg_int64_t> 0
    op.packetno = <ogg.ogg_int64_t> 0

    return ogg.ogg_stream_packetin(os, op)


cdef int write_opus_comment(ogg.ogg_stream_state *os, ogg.ogg_packet *op):
    '''
    Write the Opus comment header.
    Returns non-zero on failure.
    '''
    vendor = b'openage asset converter'
    buf = b''.join((b'OpusTags', len(vendor).to_bytes(4, 'little'), vendor,
                    b'\x00\x00\x00\x00'))  # number of comments following

    op.packet = buf
    op.bytes = <long> len(buf)
    op.b_o_s = <long> 0
    op.e_o_s = <long> 0
    op.granulepos = <ogg.ogg_int64_t> 0
    op.packetno = <ogg.ogg_int64_t> 1

    return ogg.ogg_stream_packetin(os, op)


def read_wav(wav):
    '''
    Reads the .wav file 'wav' and returns a dictionary with settings
    or a string, stating an error.
    '''
    le = 'little'
    inopt = {}
    cdef pos = 0
    chk_len = 0

    if not wav or wav[0:4] != b'RIFF' or wav[8:12] != b'WAVE':
        return "Not a RIFF-WAVE file."

    riff_len = int.from_bytes(wav[4:8], le)
    # There seems to be idv3-tags or something at the end, if len(wav) - 8 > riff_len.
    if len(wav) - 8 < riff_len:
        err = "Stated file-length {} in RIFF-header too high. " +\
                "It should be at most {}. Is the file maybe truncated?"
        return err.format(riff_len, len(wav) - 8)

    pos = 12
    # Search for 'fmt ' chunk.
    while wav[pos:pos+4] != b'fmt ':
        chk_len = int.from_bytes(wav[pos+4:pos+8], le)
        pos += 8 + chk_len + (chk_len & 1)  # Pad to even byte offset.
        if pos >= 8 + riff_len:
            return "Could not find 'fmt '-chunk in Wave-file."

    chk_len = int.from_bytes(wav[pos+4:pos+8], le)
    if chk_len < 16:
        return "Invalid WAVE-file: 'fmt '-chunk has invalid size."
    pos += 8

    if wav[pos:pos+2] != b'\x01\x00':
        return "Samples not in PCM-format."

    inopt['channels'] = int.from_bytes(wav[pos+2:pos+4], le)
    if inopt['channels'] not in (1, 2):
        return "WAVE-file has {} channels. Need 1 or 2."\
                .format(inopt['channels'])

    inopt['input_rate'] = int.from_bytes(wav[pos+4:pos+8], le)
    if inopt['input_rate'] > 48000:
        return "Inputrate {} too high for opus."\
                .format(inopt['input_rate'])

    bytes_per_second = int.from_bytes(wav[pos+8:pos+12], le)
    # wframe_len is the space needed to save one sample (padded to whole bytes)
    # for each channel.
    wframe_len = int.from_bytes(wav[pos+12:pos+14], le)
    bits_per_sample = int.from_bytes(wav[pos+14:pos+16], le)
    if bits_per_sample not in range(1,17):
        return "Samplesize is {}, but should be between 1 and 16 bits inclusive."\
                .format(bits_per_sample)

    assert(wframe_len * inopt['input_rate'] == bytes_per_second)
    assert((bits_per_sample + 7) // 8 * inopt['channels'] == wframe_len)

    inopt['bits_per_sample'] = bits_per_sample
    inopt['wframe_len'] = wframe_len

    pos += chk_len + (chk_len & 1)
    # Search for 'data' chunk.
    while wav[pos:pos+4] != b'data':
        chk_len = int.from_bytes(wav[pos+4:pos+8], le)
        pos += 8 + chk_len + (chk_len & 1)  # Pad to even byte offset.
        if pos >= 8 + riff_len:
            return "Could not find 'data'-chunk in Wave-file."

    chk_len = int.from_bytes(wav[pos+4:pos+8], le)
    pos += 8
    if 8 + riff_len - pos < chk_len:
        err = "Stated data-length {} in WAVE-header too high. " +\
                "It should be at most {}. Is the file maybe truncated?"
        return err.format(chk_len, 8 + riff_len - pos)

    inopt['header_len'] = pos
    inopt['trailer_len'] = len(wav) - pos - chk_len
    return inopt


cdef int write_ogg_page(size_t offset, bytearray outdata, ogg.ogg_page *og) except? 0:
    '''
    Write 'og' at 'offset' into 'outdata'.
    If succesful, return new offset. Otherwise, raise a MemoryError.
    '''
    if PyByteArray_GET_SIZE(outdata) < offset + og.header_len + og.body_len:
        # TODO increase more than necessary
        PyByteArray_Resize(outdata, offset + og.header_len + og.body_len)

    cdef char *out = PyByteArray_AS_STRING(outdata) + offset
    memcpy(out, og.header, og.header_len)
    memcpy(out + og.header_len, og.body, og.body_len)
    return offset + og.header_len + og.body_len


cdef void free_buffers(ogg.ogg_stream_state *os, opus.OpusEncoder *oe,
                       unsigned char *pymem1, unsigned char *pymem2):
    ogg.ogg_stream_clear(os)
    if oe != NULL:
        opus.opus_encoder_destroy(oe)
    if pymem1 != NULL:
        PyMem_Free(pymem1)
    if pymem2 != NULL:
        PyMem_Free(pymem2)
    return


cdef upsample(const char *inp, size_t inp_len, dict opts, int target_rate):
    '''
    Upsamples the PCM-data in 'indata' to 'target_rate' using linear
    interpolation. 'opts' is a dict containing info on 'indata'.
    Assumptions: * 1 ≤ opts['bit_depth'] ≤ 16
                 * opts['input_rate'] < target_rate
                 * opts['channels'] ∈ {1, 2}
    TODO: * a more eloquent version?
          * a faster version?
          * a better interpolation?
    '''
    cdef:
        int wframe_len = opts['wframe_len']
        int ch, channel = opts['channels']
        int bit_depth = opts['bits_per_sample']
        opus.opus_int16 mask = ~((1 << (16 - bit_depth)) - 1)
        float alpha

        size_t iin = 0, iout = 0
        size_t ismpls, osmpls  # number of input and output samples
        short a,b
        int num, den, divisor

        size_t outsize
        char *out

    ismpls = inp_len // wframe_len
    if ismpls < 2:
        # Nothing to do.
        ret = bytearray(inp_len)
        memcpy(PyByteArray_AS_STRING(ret), inp, inp_len)
        return ret

    divisor = gcd(target_rate, opts['input_rate'])
    num = <int> (target_rate / divisor)
    den = <int> (opts['input_rate'] / divisor)

    # We're using linear interpolation. That means, we need for every output
    # sample one input samples that comes before and one after the output
    # sample. ismpls - 1 is the index of the last input sample. By multiplying
    # with num / den we get the index of the last output sample. In general,
    # this is not a whole number. Therefore we take only the integer part.
    # Because indices are zero-based, add one for the number of output samples.
    osmpls = ((ismpls - 1) * num / den).__trunc__() + 1
    outsize = osmpls * channel * 2  # we convert to int16

    ret = bytearray(outsize)
    out = PyByteArray_AS_STRING(ret)

    # iout is the next sample we want to calculate. We need one sample earlier
    # and one later in time. Those are at indices iin and iin + 1. We need to
    # check in every iteration, if iout is now after iin + 1 and in that case
    # increase iin by one. Because we are _up_-sampling, the time between two
    # output samples is shorter than between two input samples, and therefore
    # adding one to iin suffices to keep the invariant
    #           iin * num/den ≤ iout ≤ (iin + 1) * num/den .
    #
    # The following two blocks are very similar. The differences are the casts
    # in the assignments of a and b as well as the shift in the last line
    # When bit_depth is smaller than 9, only one byte is used per sample.
    # Therefore inp must be casted to (char *). But we convert to a stream
    # with 16 bits ber sample. To keep the audio volume the same, we set the
    # higher order byte to the calculated value.
    if bit_depth > 8:
        for iout in range(0, osmpls):
            if (iin + 1) * num < iout * den:
                iin += 1

            alpha = (<float> iout * den) / num - iin
            for ch in range(0, channel):
                a = (<short *> inp)[iin * channel + ch]
                b = (<short *> inp)[(iin + 1) * channel + ch]
                (<opus.opus_int16 *> out)[iout * channel + ch] = \
                        (<opus.opus_int16> (a + alpha * (b - a))) & mask
    else:
        for iout in range(0, osmpls):
            if (iin + 1) * num < iout * den:
                iin += 1

            alpha = (<float> iout * den) / num - iin
            for ch in range(0, channel):
                a = (<char *> inp)[iin * channel + ch]
                b = (<char *> inp)[(iin + 1) * channel + ch]
                (<opus.opus_int16 *> out)[iout * channel + ch] = \
                        (<opus.opus_int16> (a + alpha * (b - a))) << 8 & mask

    return ret


cdef int gcd(int a, int b):
    ''' Greatest common divisor. '''
    cdef int m = max(a, b)
    cdef int n = min(a, b)
    cdef int r = -1
    while r != 0:
        r = m % n
        m, n = n, r
    return m
