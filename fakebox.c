#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <math.h>

#define WIDTH 2000
#define HEIGHT 1000

#define MID (WIDTH / 2)

void fail() {

}

int main() {
	unsigned char buf[WIDTH * HEIGHT * 4] = { 0 };
	int x, y;

	for (x = 0; x < WIDTH; x++) {
		for (y = 0; y < HEIGHT; y++) {
			double xd = x - MID;
			double yd = y - MID;
			double d = sqrt(xd * xd + yd * yd);

			if (1 || d <= MID) {
				double h = (double) x / (WIDTH / 2) * 2 * M_PI;
				double c = 1;
				double l = (double) y / HEIGHT;

				// put red at the right
				h = h + (M_PI / 2 - (M_PI - 2));

				if (c > 1 - 2 * fabs(l - .5)) {
					c = 1 - 2 * fabs(l - .5);
				}

				double r1 = sin(h + M_PI - 2.0) * 0.417211 * c + l;
				double g1 = sin(h + M_PI + 1.5) * 0.158136 * c + l;
				double b1 = sin(h + M_PI      ) * 0.455928 * c + l;

				int midr = exp(log(r1 * 0.923166 + 0.0791025) * 1.25) * 255;
				int midg = exp(log(g1 * 0.923166 + 0.0791025) * 1.25) * 255;
				int midb = exp(log(b1 * 0.923166 + 0.0791025) * 1.25) * 255;

#if 0
				if (fabs(xd) < (MID - fabs(yd)) / 4) {
					midr = (midr + 255) / 2;
					midg = (midg + 255) / 2;
					midb = (midb + 255) / 2;
				} else if (fabs(yd) < (MID - fabs(xd)) / 4) {
					midr = (midr + 255) / 2;
					midg = (midg + 255) / 2;
					midb = (midb + 255) / 2;
				}
#endif

				buf[(y * WIDTH + x) * 4 + 0] = midr;
				buf[(y * WIDTH + x) * 4 + 1] = midg;
				buf[(y * WIDTH + x) * 4 + 2] = midb;
				buf[(y * WIDTH + x) * 4 + 3] = 255;
			}
		}
	}

	unsigned char *rows[HEIGHT];
	int i;
	for (i = 0 ; i < HEIGHT; i++) {
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
