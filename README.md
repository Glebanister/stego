
## Crop and rotate BMP images, insert stenography into images.


Crop and rotate:

`./hw_01 crop-rotate ‹in-bmp› ‹out-bmp› ‹x› ‹y› ‹w› ‹h›`

Parameters:

`‹in-bmp›` - name of the input file\
`<out-bmp>` - name of the output file\
`<x>, <y>` - coordinates of the upper left corner of the area to be cut and rotated.\
`<w>, <h>` - width and height of the area before turning

Insert stenography:


`./hw_01 insert ‹in-bmp› ‹out-bmp› ‹key-txt› ‹msg-txt›`


Extract stenography:


`./hw_01 extract ‹in-bmp› ‹key-txt› ‹msg-txt›`


Parameters:


`<in-bmp>` - name of the input file \
`<out-bmp>` - name of the output file \
`<key-txt>` - text file with key \
`<msg-txt>` - text file with message

Encoding method.\
The original message consists only of capital Latin letters, a space, a period, and a comma. Each character is converted to a number from 0 to 28, respectively (a total of 29 different values), and the number is converted into five bits, written from the lowest to the highest. In total, a message of N characters is encoded using 7N Bits.

To transmit a message, in addition to the carrier image, a key is required - a text file that describes in which pixels the message bits are encoded. In this file on separate lines are written:

  - The x and y coordinates (0 <= x < W, 0 <= y < H) of the pixel to which the corresponding bit should be saved.
  - The letter R / G / B denotes the color channel in the low bit of which you want to write a message bit.If the key writes more bits than the message needs, the last lines are ignored.