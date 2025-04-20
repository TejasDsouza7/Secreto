#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

void hide_text(const char *image_path, const char *text_path, const char *output_path) {
    FILE *image_file = fopen(image_path, "rb");
    FILE *text_file = fopen(text_path, "rb");
    FILE *output_file = fopen(output_path, "wb");

    if (!image_file || !text_file || !output_file) {
        printf("Error opening files.\n");
        return;
    }

    fseek(text_file, 0, SEEK_END);
    long text_size = ftell(text_file);
    rewind(text_file);
    unsigned char *text_data = malloc(text_size);
    fread(text_data, 1, text_size, text_file);
    fclose(text_file);

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    png_init_io(png, image_file);
    png_read_info(png, info);

    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    png_bytep *row_pointers = malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
        row_pointers[y] = malloc(png_get_rowbytes(png, info));
    png_read_image(png, row_pointers);
    fclose(image_file);

    size_t pixel_size = png_get_rowbytes(png, info) * height;
    unsigned char *pixels = malloc(pixel_size);
    for (int y = 0; y < height; y++)
        memcpy(pixels + y * png_get_rowbytes(png, info), row_pointers[y], png_get_rowbytes(png, info));

    if (text_size + 4 > pixel_size) {
        printf("Text too large for image.\n");
        return;
    }
    pixels[0] = (text_size >> 24) & 0xFF;
    pixels[1] = (text_size >> 16) & 0xFF;
    pixels[2] = (text_size >> 8) & 0xFF;
    pixels[3] = text_size & 0xFF;

    memcpy(pixels + 4, text_data, text_size);
    free(text_data);

    for (int y = 0; y < height; y++)
        memcpy(row_pointers[y], pixels + y * png_get_rowbytes(png, info), png_get_rowbytes(png, info));
    free(pixels);

    png_structp write_png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop write_info = png_create_info_struct(write_png);
    png_init_io(write_png, output_file);
    png_set_IHDR(write_png, write_info, width, height, bit_depth, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(write_png, write_info);
    png_write_image(write_png, row_pointers);
    png_write_end(write_png, NULL);
    fclose(output_file);

    for (int y = 0; y < height; y++)
        free(row_pointers[y]);
    free(row_pointers);

    printf("Text hidden successfully!\n");
}

void extract_text(const char *image_path, const char *output_text_path) {
    FILE *image_file = fopen(image_path, "rb");
    FILE *text_file = fopen(output_text_path, "wb");

    if (!image_file || !text_file) {
        printf("Error opening files.\n");
        return;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    png_init_io(png, image_file);
    png_read_info(png, info);

    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    png_bytep *row_pointers = malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
        row_pointers[y] = malloc(png_get_rowbytes(png, info));
    png_read_image(png, row_pointers);
    fclose(image_file);

    size_t pixel_size = png_get_rowbytes(png, info) * height;
    unsigned char *pixels = malloc(pixel_size);
    for (int y = 0; y < height; y++)
        memcpy(pixels + y * png_get_rowbytes(png, info), row_pointers[y], png_get_rowbytes(png, info));

    long text_size = (pixels[0] << 24) | (pixels[1] << 16) | (pixels[2] << 8) | pixels[3];

    fwrite(pixels + 4, 1, text_size, text_file);
    fclose(text_file);

    for (int y = 0; y < height; y++)
        free(row_pointers[y]);
    free(row_pointers);
    free(pixels);

    printf("Text extracted successfully!\n");
}

int main(int argc, char *argv[]) {
    if (argc != 5 && argc != 4) {
        printf("Usage:\n");
        printf("  Hide:    %s hide <input.png> <input.txt> <output.png>\n", argv[0]);
        printf("  Extract: %s extract <input.png> <output.txt>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "hide") == 0 && argc == 5) {
        hide_text(argv[2], argv[3], argv[4]);
    } else if (strcmp(argv[1], "extract") == 0 && argc == 4) {
        extract_text(argv[2], argv[3]);
    } else {
        printf("Invalid command.\n");
    }

    return 0;
}