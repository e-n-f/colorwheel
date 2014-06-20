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
				double l = (double) Y / HEIGHT;

				// saturated in middle, colorless at ends
				// double c = 1 - exp(log(fabs((double) Y / HEIGHT - .5) * 2) * 2);

				// fully saturated at black, no saturation at white
				double c = 1 - exp(log((double) Y / HEIGHT) * 3);

				double h = (double) X / WIDTH * 4 * M_PI;
				// L:   0 .. 1
				// C:   0 .. 1
				// H: -pi .. pi

				// LCH to LAB
				// http://www.brucelindbloom.com/index.html?Equations.html

				double a = c * cos(h);
				double b = c * sin(h);
				// L:   0 .. 1
				// A:  -1 .. 1
				// B:  -1 .. 1

				// LAB to XYZ
				// http://rsb.info.nih.gov/ij/plugins/download/Color_Space_Converter.java

				l *= 100;
#if 0
				// Scale so the circle all really exists at l = .5
				a *= 29;
				b *= 29;
#endif
				// Scale so everything but cyan exists at l = .5
				a *= 55;
				b *= 55;

				double y = (l + 16.0) / 116.0;
				double y3 = pow(y, 3.0);
				double x = (a / 500.0) + y;
				double x3 = pow(x, 3.0);
				double z = y - (b / 200.0);
				double z3 = pow(z, 3.0);

				if (y3 > 0.008856) {
					y = y3;
				} else {
					y = (y - (16.0 / 116.0)) / 7.787;
				}
				if (x3 > 0.008856) {
					x = x3;
				} else {
					x = (x - (16.0 / 116.0)) / 7.787;
				}
				if (z3 > 0.008856) {
					z = z3;
				} else {
					z = (z - (16.0 / 116.0)) / 7.787;
				}

#if 1 // D65
                                x *= 95.0429;
                                y *= 100.0;
                                z *= 108.8900;
#else // D50
				x *= 96.4212;
				y *= 100.0;
				z *= 82.5188;
#endif

				// X:   0 .. 100
				// Y:   0 .. 100
				// Z:   0 .. 100

				// XYZ to sRGB
				// http://rsb.info.nih.gov/ij/plugins/download/Color_Space_Converter.java

				x /= 100.0;
				y /= 100.0;
				z /= 100.0;

				double Mi[3][3] = {{ 3.2406, -1.5372, -0.4986},
				                   {-0.9689,  1.8758,  0.0415},
				                   { 0.0557, -0.2040,  1.0570}};
				double r, g;

				// [r g b] = [X Y Z][Mi]
				r = (x * Mi[0][0]) + (y * Mi[0][1]) + (z * Mi[0][2]);
				g = (x * Mi[1][0]) + (y * Mi[1][1]) + (z * Mi[1][2]);
				b = (x * Mi[2][0]) + (y * Mi[2][1]) + (z * Mi[2][2]);

				// assume sRGB
				if (r > 0.0031308) {
					r = ((1.055 * pow(r, 1.0 / 2.4)) - 0.055);
				} else {
					r = (r * 12.92);
				}
				if (g > 0.0031308) {
					g = ((1.055 * pow(g, 1.0 / 2.4)) - 0.055);
				} else {
					g = (g * 12.92);
				}
				if (b > 0.0031308) {
					b = ((1.055 * pow(b, 1.0 / 2.4)) - 0.055);
				} else {
					b = (b * 12.92);
				}

#if 0
				if (r < 0 || g < 0 || b < 0) {
					r = g = b = 0;
				}
				if (r > 1 || g > 1 || b > 1) {
					r = g = b = 1;
				}
#endif

				r = (r < 0) ? 0 : r;
				g = (g < 0) ? 0 : g;
				b = (b < 0) ? 0 : b;

				r = (r > 1) ? 1 : r;
				g = (g > 1) ? 1 : g;
				b = (b > 1) ? 1 : b;

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
