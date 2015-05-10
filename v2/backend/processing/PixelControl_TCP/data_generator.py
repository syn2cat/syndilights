import jarray
import math
import itertools

_byte_lookup = {}
brightness_max = 0.5

# Bit values which represent the zero and one bit pulses.
_ZERO = bytearray([0b00000000])
_ONE = bytearray([0b11111111])

def _build_byte_lookup():
    """Return lookup table to map from every byte value 0-255 and the associated raw SPI data."""
    lookup = {}
    for i in range(256):
        value = bytearray()
        for j in range(7, -1, -1):
            if ((i >> j) & 1) == 0:
                value += _ZERO
            else:
                value += _ONE
        lookup[i] = value
    return lookup

def color_to_rgb(c):
    """Convert a 24 bit color to RGB triplets."""
    return ((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF)

def _encode_color_grb(c):
    """Encode an RGB tuple into NeoPixel GRB 24 byte SPI bit stream."""
    return _byte_lookup[int(c[1] * brightness)] + _byte_lookup[int(c[0] * brightness)] + _byte_lookup[int(c[2] * brightness)]

def _encode_pixel(c):
    """Encode an RGB tuple into NeoPixel GRB 24 byte SPI bit stream."""
    rgb = color_to_rgb(c)
    encoded = _encode_color_grb(rgb)
    return encoded

def prepare_data(dimension, b):
    global brightness
    global _byte_lookup
    if b > brightness_max:
        brightness = brightness_max
    else:
        brightness = b
    _byte_lookup = _build_byte_lookup()
    data = jarray.zeros(dimension * 24 + 1, "b")
    return data

'''
Possibilities:
    Start: Top left / Top Right / Bottom Left / Bottom Right
    Direction: Up / Down / Right / Left
'''


def make_line(type, nb, long_line):
    '''
    If moving up or down: 0 <= nb < height
    If moving right or left: 0 <= nb < width
    '''
    indexes = []
    if type == 0:
        # top left -> down / OK
        pxstart = nb
        for h in range(height):
            indexes.append(pxstart + width * h)
    elif type == 1:
        # top left -> right / OK
        pxstart = nb * width
        for w in range(width):
            indexes.append(pxstart + w)
    elif type == 2:
        # bottom left -> up / OK
        pxstart = width * (height - 1) + nb
        for h in range(height):
            indexes.append(pxstart - width * h)
    elif type == 3:
        # bottom left -> right / OK
        pxstart = width * (height - 1) - nb * width
        for w in range(width):
            indexes.append(pxstart + w)
    elif type == 4:
        # top right -> down / OK
        pxstart = width - 1 - nb
        for h in range(height):
            indexes.append(pxstart + width * h)
    elif type == 5:
        # top right -> left / OK
        pxstart = width - 1 + nb * width
        for w in range(width):
            indexes.append(pxstart - w)
    elif type == 6:
        # bottom right -> up / OK
        pxstart = width * height - 1 - nb * height
        for h in range(height):
            indexes.append(pxstart - width * h)
    elif type == 7:
        # bottom right -> left / OK
        pxstart = width * height - 1 - nb * width
        for w in range(width):
            indexes.append(pxstart - w)
    else:
        raise Exception("Invalid Type")

    if long_line and nb % 2 == 1:
        return reversed([pixels[px] for px in indexes])
    return [pixels[px] for px in indexes]


def image2data(data, type, long_line):
    offset = 0
    loadPixels()
    inline_image = []
    if type in [0, 2, 4, 6]:
        # Organized up or down
        for x in range(0, width):
            inline_image += make_line(type, x, long_line)
    elif type in [1, 3, 5, 7]:
        # Organized left or right
        for x in range(0, height):
            inline_image += make_line(type, x, long_line)
    for img_px in inline_image:
        py_bytes = _encode_pixel(img_px)
        for b in py_bytes:
            if b > 127:
                # Convert to signed bytes (expected by jarray)
                b -= 256
                data[offset] = b
            else:
                data[offset] = b
            offset += 1
    # New line
    data[-1] = 10
    return data
