#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <math.h>

#define WIDTH 2000
#define HEIGHT 200

#define MID (WIDTH / 2)

void fail() {

}

int main(int argc, char **argv) {
	double bright = .5;
	if (argc > 1) {
		bright = atof(argv[1]);
	}

	unsigned char buf[WIDTH * HEIGHT * 4] = { 0 };
	int X, Y;

	for (X = 0; X < WIDTH; X++) {
		for (Y = 0; Y < HEIGHT; Y++) {
			double xd = X - MID;
			double yd = Y - MID;
			double d = sqrt(xd * xd + yd * yd);

			if (1) {
				double l = .5;

				// saturated in middle, colorless at ends
				// double c = 1 - exp(log(fabs((double) Y / HEIGHT - .5) * 2) * 2);

				// fully saturated at black, no saturation at white
				double c = 1;

				double h = X * 4 * M_PI / WIDTH;

				h *= 6 / (2 * M_PI);
				if (h > 6) {
					h -= 6;
				}
				double x = 1 - fabs((h - (floor(h / 2) * 2)) - 1);

				double r, g, b;
				if (h < 1) {
					r = c;
					g = x;
					b = 0;
				} else if (h < 2) {
					r = x;
					g = c;
					b = 0;
				} else if (h < 3) {
					r = 0;
					g = c;
					b = x;
				} else if (h < 4) {
					r = 0;
					g = x;
					b = c;
				} else if (h < 5) {
					r = x;
					g = 0;
					b = c;
				} else {
					r = c;
					g = 0;
					b = x;
				}


				buf[(Y * WIDTH + X) * 4 + 0] = r * 255;
				buf[(Y * WIDTH + X) * 4 + 1] = g * 255;
				buf[(Y * WIDTH + X) * 4 + 2] = b * 255;
				buf[(Y * WIDTH + X) * 4 + 3] = 255;
			}
		}
	}

	unsigned char *rows[HEIGHT];
	int i;
	for (i = 0; i < HEIGHT; i++) {
		rows[i] = buf + i * (4 * WIDTH);
	}

	png_structp png_ptr;
	png_infop info_ptr;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, fail, fail, fail);
	if (png_ptr == NULL) {
		fprintf(stderr, "PNG failure (write struct)\n");
		exit(EXIT_FAILURE);
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		fprintf(stderr, "PNG failure (info struct)\n");
		exit(EXIT_FAILURE);
	}

	png_set_IHDR(png_ptr, info_ptr, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_rows(png_ptr, info_ptr, rows);
	png_init_io(png_ptr, stdout);
	png_write_png(png_ptr, info_ptr, 0, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}
