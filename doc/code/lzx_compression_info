some information about the format from the cabextract comments:
------

Microsoft's LZX document (in cab-sdk.exe) and their implementation
of the com.ms.util.cab Java package do not concur.

In the LZX document, there is a table showing the correlation between
window size and the number of position slots. It states that the 1MB
window = 40 slots and the 2MB window = 42 slots. In the implementation,
1MB = 42 slots, 2MB = 50 slots. The actual calculation is 'find the
first slot whose position base is equal to or more than the required
window size'. This would explain why other tables in the document refer
to 50 slots rather than 42.

The constant NUM_PRIMARY_LENGTHS used in the decompression pseudocode
is not defined in the specification.

The LZX document does not state the uncompressed block has an
uncompressed length field. Where does this length field come from, so
we can know how large the block is? The implementation has it as the 24
bits following after the 3 blocktype bits, before the alignment
padding.

The LZX document states that aligned offset blocks have their aligned
offset huffman tree AFTER the main and length trees. The implementation
suggests that the aligned offset tree is BEFORE the main and length
trees.

The LZX document decoding algorithm states that, in an aligned offset
block, if an extra_bits value is 1, 2 or 3, then that number of bits
should be read and the result added to the match offset. This is
correct for 1 and 2, but not 3, where just a huffman symbol (using the
aligned tree) should be read.

Regarding the E8 preprocessing, the LZX document states 'No translation
may be performed on the last 6 bytes of the input block'. This is
correct. However, the pseudocode provided checks for the *E8 leader*
up to the last 6 bytes. If the leader appears between -10 and -7 bytes
from the end, this would cause the next four bytes to be modified, at
least one of which would be in the last 6 bytes, which is not allowed
according to the spec.

The specification states that the huffman trees must always contain at
least one element. However, many CAB files contain blocks where the
length tree is completely empty (because there are no matches), and
this is expected to succeed.

The errors in LZX documentation appear have been corrected in the
new documentation for the LZX DELTA format.

http://msdn.microsoft.com/en-us/library/cc483133.aspx

However, this is a different format, an extension of regular LZX.
I have noticed the following differences, there may be more:

The maximum window size has increased from 2MB to 32MB. This also
increases the maximum number of position slots, etc.

The format now allows for "reference data", supplied by the caller.
If match offsets go further back than the number of bytes
decompressed so far, that is them accessing the reference data.
