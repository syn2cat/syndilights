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
    data = jarray.zeros(dimension * 24, "b")
    return data

def image2data(data, long_line):
    offset = 0
    pixel_nb = 0
    for x in range(0, height):
        pixel_line = pixels[pixel_nb:pixel_nb+width]
        if long_line and pixel_nb/width%2 == 1:
            pixel_line = reversed(pixel_line)
        for px in pixel_line:
            py_bytes = _encode_pixel(px)
            for b in py_bytes:
                if b > 127:
                    # Convert to signed bytes (expected by jarray)
                    b -= 256
                    data[offset] = b
                else:
                    data[offset] = b
                offset += 1
            pixel_nb +=1
    return data
