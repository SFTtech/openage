#STemplet.dat File Format

Used for elevation tiles.

```
struct Templets
{
  struct Templet
  {
    long HeaderLength; // number of bytes to read this Templet
    long TileSizeX; // seems to be always 97
    long TileSizeY; // mainly 49, but 25 for shorter tiles, and 73 for longer tiles.
    long Unknown1; // seems to be a length used internally
    long Unknown2;
    long TilesTotal; // used for allocating rhombus tile count in bytes
    long OffsetStart1; // Colors1 start from header
    long OffsetStart2; // Colors2 start from header
    RGBA Colors1[TileSizeY]; // seems to be the color mapping used with ICM
    RGBA Colors2[TileSizeY];
  }
}[17]; // always seems to be 17
```
