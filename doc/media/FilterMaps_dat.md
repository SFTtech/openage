#Filter Maps File Format

The most annoying DAT file.

```
struct FilterMaps
{
  struct FilterMap
  {
    long HeaderLength; // number of bytes to read this filter map
    long TileSizeY; // seems to be 49, 25, or 73
    while (TileSizeY--) {
      uint8 Crap1; // read 1 byte
      int Check1 = Crap1 & 0xFF; // bitwise for reading more crap
      do
      {
        uint16 Crap2; // read 2 bytes
        int Check2 = Crap2 & 0x0F; // bitwise for reading further crap
        do
        {
          uint24 Crap3; // read 3 bytes
          // some of this crap is then loaded into the graphics rendering
          Check2--;
        } while (Check2);
        Check1--;
      } while (Check1);
    }
}[17];
```
