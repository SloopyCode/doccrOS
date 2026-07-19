from PIL import Image
import sys
import os

def convert_png_to_bmp(input_path, output_path=None):
    if not os.path.isfile(input_path):
        print(f"File not found: {input_path}")
        return

    if output_path is None:
        output_path = os.path.splitext(input_path)[0] + ".bmp"

    img = Image.open(input_path).convert("RGBA")

    img.save(output_path, format="BMP")

    print(f"Converted: {input_path} -> {output_path}")
    print("Transparency (alpha channel) has been preserved.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python png_to_bmp.py image.png [output.bmp]")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2] if len(sys.argv) > 2 else None
        convert_png_to_bmp(input_file, output_file)