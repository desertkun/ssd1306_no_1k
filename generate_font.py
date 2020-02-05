import numpy as np
import sys
from PIL import Image


img = Image.open(sys.argv[1]).convert('RGBA')
output_name = sys.argv[2] if len(sys.argv) >= 3 else "ssd1306_font.c"

col, row = img.size
pixels = img.load()

with open(output_name, "w") as f:
    f.write("#include \"ssd1306.h\"\n\n")
    f.write("static const uint8_t font_data[] = {\n")

    for y in range(int(row / 8)):
        for x in range(int(col / 8)):
            char_id = x + y * 16
            char_sym = char_id + 32
            char_value = chr(char_sym)

            f.write("  ")
            for ch_x in range(8):
                the_byte = 0
                for ch_y in range(8):
                    r, g, b, a = pixels[x*8+ch_x, y*8+ch_y]
                    if r:
                        the_byte += 1 << ch_y
                f.write("{0}, ".format(hex(the_byte)))

            f.write(" // {0} (code {1})\n".format(char_value, hex(char_sym)))
    f.write("}\n")
    f.write("""
extern const font_def_t font = {
    .characters = font_data,
    .characters_count = (32 + sizeof(font_data) / 8)
};
    """)
