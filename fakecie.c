#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <math.h>

#define WIDTH 1000
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

			if (d <= MID) {
				double h = atan2(1 - yd, xd) / (2 * M_PI);
				double sat = d / MID;
				double val = .5;

				double base = 0.953562 * exp(log(val) * 1.18012) + 0.0452658;

				double avail = base;
				if (1 - base < avail) {
					avail = 1 - base;
				}

				if (sat >= exp(log((avail - 0.0215801) / 0.55422) / 1.25264)) {
					sat = exp(log((avail - 0.0215801) / 0.55422) / 1.25264);
				}

				double rs, gs, bs;
				if (sat > 0) {
					rs = 0.55422 * exp(log(sat) * 1.25264) + 0.0215801;
					gs = 0.13707 * exp(log(sat) * 1.07082) + 0.00229467;
					bs = 0.419957 * exp(log(sat) * 1.01496) + 0.00139697;
				} else {
					rs = 0.0215801;
					gs = 0.00229467;
					bs = 0.00139697;
				}

				double r1 = rs * sin(h * 2 * M_PI + M_PI - 2) + base;
				double g1 = gs * sin(h * 2 * M_PI + M_PI + 1.5) + base;
				double b1 = bs * sin(h * 2 * M_PI + M_PI) + base;

				if (r1 > 1) {
					r1 = 1;
				}
				if (g1 > 1) {
					g1 = 1;
				}
				if (b1 > 1) {
					b1 = 1;
				}

				int midr = 255 * r1;
				int midg = 255 * g1;
				int midb = 255 * b1;

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

				buf[(y * HEIGHT + x) * 4 + 0] = midr;
				buf[(y * HEIGHT + x) * 4 + 1] = midg;
				buf[(y * HEIGHT + x) * 4 + 2] = midb;
				buf[(y * HEIGHT + x) * 4 + 3] = 255;
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
