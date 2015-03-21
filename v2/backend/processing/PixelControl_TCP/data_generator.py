import jarray
import math

def prepare_data(dimension, gamma, brightness):
    gammatable = [int((math.pow(i / 255.0, gamma) * 255.0 + 0.5) * brightness) for i in range(0, 256)]
    data = jarray.zeros(dimension * 24, "b")
    return data, gammatable

def image2data(data, long_line, gammatable):
    offset = 0
    pixel_nb = 0
    for x in range(0, height):
        pixel_line = pixels[pixel_nb:pixel_nb+width]
        if long_line and pixel_nb/width%2 == 1:
            pixel_line = reversed(pixel_line)
        for px in pixel_line:
            pixel = [colorWiring(px, gammatable) for i in range(0, 8)]
            imgmask = 0x800000
            while imgmask != 0:
                b = 0
                for i in range(0, 8):
                    if ((pixel[i] & imgmask) != 0):
                        b |= (1 << i)
                if b > 127:
                    # Convert to signed bytes (expected by jarray)
                    b -= 2**8
                    data[offset] = b
                else:
                    data[offset] = b
                offset += 1
                imgmask >>= 1
            pixel_nb +=1
    return data

def colorWiring(c, gammatable):
    red = (c & 0xFF0000) >> 16
    green = (c & 0x00FF00) >> 8
    blue = (c & 0x0000FF)
    red = gammatable[red] >> 8
    green = gammatable[green] >> 8
    blue = gammatable[blue] >> 8
    return (green << 16) | (red << 8) | (blue)

