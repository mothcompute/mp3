# MPEG-1 Layer III
## the best audio format in fact
### (it is true)

# files
- main.c: test application for the library
- mp3.c: library code
- inc: includes
  + dat.h: miscellaneous tables and constants needed by the library
  + huffman.h: generated huffman table
  + mp3.h: library header, include this in client code
- scr: scripts unneeded to use the library, provided for reference purposes
  + huff: wrapper for huffcvt, generating huffman tables copied to the clipboard from the mp3 specification
  + scr/huffcvt.c: changes huffman string format to c array
