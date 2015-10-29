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

int main(int argc, char **argv) {
	double lightness = -50;
	if (argc > 1) {
		lightness = atof(argv[1]);
	}

	unsigned char buf[WIDTH * HEIGHT * 4] = { 0 };
	int X, Y;

	for (X = 0; X < WIDTH; X++) {
		for (Y = 0; Y < HEIGHT; Y++) {
			double xd = X - MID;
			double yd = Y - MID;
			double d = sqrt(xd * xd + yd * yd);

			if (1 || d <= MID) {
				double l = (double) X /  WIDTH;
				double c = (double) Y / WIDTH;
				double h = -2;
				h = (double) X / WIDTH * 2 * M_PI - M_PI;

				if (lightness > -50) { l = lightness / 100.0; }
				// L:   0 .. 1
				// C:   0 .. 1
				// H: -pi .. pi



				{
					double a               = 0.157232;
					double u               = -3.09771;
					double o               = 1.35964;
					double a1              = 0.0294973;
					double u1              = 0.360255;
					double o1              = 0.657014;

					// apparent lightness of chroma 5 compared to chroma 0

					double m               = 0.175277;
					double b               = 0.332425;

					double lbase = 0;
					int hh;
					for (hh = -1; hh <= 1; hh++) {
						lbase += m * (a * exp(- (hh * h - u) * (hh * h - u) / (2 * o * o)) / (o * sqrt(2 * M_PI)) + a1 * exp(- (hh * h - u1) * (hh * h - u1) / (2 * o1 * o1)) / (o1 * sqrt(2 * M_PI))) + b;
					}

					if (c < 0.05) {
						l *= (lbase - 1) * c / 0.05 + 1;
					} else {
						l *= lbase;

						// incremental multiplier for each doubling of chroma

						b               = 0.324736;
						m = 1;

						double inc = log(c / 0.05) / log(2);

						double linc = 0;
						int hh;
						for (hh = -1; hh <= 1; hh++) {
							linc += m * (a * exp(- (hh * h - u) * (hh * h - u) / (2 * o * o)) / (o * sqrt(2 * M_PI)) + a1 * exp(- (hh * h - u1) * (hh * h - u1) / (2 * o1 * o1)) / (o1 * sqrt(2 * M_PI))) + b;
						}

						l *= exp(log(linc) * inc);
					}
				}



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
				a *= 100;
				b *= 100;
#if 0
				// Scale so the circle all really exists at l = .5
				a *= 29;
				b *= 29;
				// Scale so everything but cyan exists at l = .5
				a *= 55;
				b *= 55;
#endif

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

double K6100[3] = { 95.17497923187392, 100, 102.45906488893944 }; /* 6100K */

#if 0
				x *= 95.0429;
				y *= 100.0;
				z *= 108.8900;
#endif
				x *= K6100[0];
				y *= K6100[1];
				z *= K6100[2];

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

				if (r < 0 || g < 0 || b < 0 || r > 1 || g > 1 || b > 1) {
					r = g = b = 0;
				}

				r = (r < 0) ? 0 : r;
				g = (g < 0) ? 0 : g;
				b = (b < 0) ? 0 : b;

				r = (r > 1) ? 1 : r;
				g = (g > 1) ? 1 : g;
				b = (b > 1) ? 1 : b;

				buf[(Y * HEIGHT + X) * 4 + 0] = r * 255;
				buf[(Y * HEIGHT + X) * 4 + 1] = g * 255;
				buf[(Y * HEIGHT + X) * 4 + 2] = b * 255;
				buf[(Y * HEIGHT + X) * 4 + 3] = 255;
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
