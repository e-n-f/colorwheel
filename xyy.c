#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <math.h>

#define WIDTH 1024
#define HEIGHT 1024

#define MID (WIDTH / 2)

void fail() {

}


double g(x) {
	double a               = 1.20847  ;
	double u               = -1.67808 ;
	double o               = 1.04243  ;
	double a1              = 1.73992  ;
	double u1              = 1.29054  ;
	double o1              = 0.954128 ;

	return a / (o * sqrt(2 * M_PI)) * exp(-(pow(x - u, 2)) / (2 * o * o)) + a1 / (o1 * sqrt(2 * M_PI)) * exp(-(pow(x - u1, 2)) / (2 * o1 * o1));
}

double h(x) {
	return g(x) + g(x - 2 * M_PI) + g (x + 2 * M_PI);
}


// http://rsb.info.nih.gov/ij/plugins/download/Color_Space_Converter.java

/**
* sRGB to XYZ conversion matrix
*/
double M[3][3] = { {0.4124, 0.3576,  0.1805},
          {0.2126, 0.7152,  0.0722},
          {0.0193, 0.1192,  0.9505} };

/**
* XYZ to sRGB conversion matrix
*/
double Mi[3][3] = { {3.2406, -1.5372, -0.4986},
           {-0.9689,  1.8758,  0.0415},
           { 0.0557, -0.2040,  1.0570} };

double D65[3] = { 95.0429, 100.0, 108.8900 }; /* D65 */
double K6100[3] = { 95.17497923187392, 100, 102.45906488893944 }; /* 6100K */

double fpow(double base, double e) {
	return exp(log(base) * e);
}

void RGBtoXYZ(int R, int G, int B, double *x, double *y, double *z) {
	// convert 0..255 into 0..1
	double r = R / 255.0;
	double g = G / 255.0;
	double b = B / 255.0;

	// assume sRGB
	if (r <= 0.04045) {
		r = r / 12.92;
	} else {
		r = fpow(((r + 0.055) / 1.055), 2.4);
	}
	if (g <= 0.04045) {
		g = g / 12.92;
	} else {
		g = fpow(((g + 0.055) / 1.055), 2.4);
	}
	if (b <= 0.04045) {
		b = b / 12.92;
	} else {
		b = fpow(((b + 0.055) / 1.055), 2.4);
	}

	r *= 100.0;
	g *= 100.0;
	b *= 100.0;

	// [X Y Z] = [r g b][M]
	*x = (r * M[0][0]) + (g * M[0][1]) + (b * M[0][2]);
	*y = (r * M[1][0]) + (g * M[1][1]) + (b * M[1][2]);
	*z = (r * M[2][0]) + (g * M[2][1]) + (b * M[2][2]);
}

void XYZtoLAB(double X, double Y, double Z, double *l, double *a, double *b, double *whitePoint) {
	double x = X / whitePoint[0];
	double y = Y / whitePoint[1];
	double z = Z / whitePoint[2];

	if (x > 0.008856) {
		x = fpow(x, 1.0 / 3.0);
	} else {
		x = (7.787 * x) + (16.0 / 116.0);
	}
	if (y > 0.008856) {
		y = fpow(y, 1.0 / 3.0);
	} else {
		y = (7.787 * y) + (16.0 / 116.0);
	}
	if (z > 0.008856) {
		z = fpow(z, 1.0 / 3.0);
	} else {
		z = (7.787 * z) + (16.0 / 116.0);
	}

	*l = (116.0 * y) - 16.0;
	*a = 500.0 * (x - y);
	*b = 200.0 * (y - z);
}

void LABtoLCH(double L, double A, double B, double *l, double *c, double *h) {
	double C = sqrt(A * A + B * B);
	double H = atan2(B, A);

	*l = L;
	*c = C;
	*h = H;
}

void LCHtoLAB(double L, double C, double H, double *l, double *a, double *b) {
	double A = C * cos(H);
	double B = C * sin(H);

	*l = L;
	*a = A;
	*b = B;
}

void LABtoXYZ(double L, double a, double b, double *xo, double *yo, double *zo, double *whitePoint) {
	double y = (L + 16.0) / 116.0;
	double y3 = fpow(y, 3.0);
	double x = (a / 500.0) + y;
	double x3 = fpow(x, 3.0);
	double z = y - (b / 200.0);
	double z3 = fpow(z, 3.0);

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

	*xo = x * whitePoint[0];
	*yo = y * whitePoint[1];
	*zo = z * whitePoint[2];
}

int XYZtoRGB(double X, double Y, double Z, int *R, int *G, int *B) {
	double x = X / 100.0;
	double y = Y / 100.0;
	double z = Z / 100.0;

	// [r g b] = [X Y Z][Mi]
	double r = (x * Mi[0][0]) + (y * Mi[0][1]) + (z * Mi[0][2]);
	double g = (x * Mi[1][0]) + (y * Mi[1][1]) + (z * Mi[1][2]);
	double b = (x * Mi[2][0]) + (y * Mi[2][1]) + (z * Mi[2][2]);

	// assume sRGB
	if (r > 0.0031308) {
		r = ((1.055 * fpow(r, 1.0 / 2.4)) - 0.055);
	} else {
		r = (r * 12.92);
	}
	if (g > 0.0031308) {
		g = ((1.055 * fpow(g, 1.0 / 2.4)) - 0.055);
	} else {
		g = (g * 12.92);
	}
	if (b > 0.0031308) {
		b = ((1.055 * fpow(b, 1.0 / 2.4)) - 0.055);
	} else {
		b = (b * 12.92);
	}

	*R = r * 255;
	*G = g * 255;
	*B = b * 255;

	return 1;
}

void sRGBtoRGB(int ir, int ig, int ib, double *R, double *G, double *B) {
	double r = ir / 255.0;
	double g = ig / 255.0;
	double b = ib / 255.0;

	if (r <= 0.04045) {
		r = r / 12.92;
	} else {
		r = fpow(((r + 0.055) / 1.055), 2.4);
	}
	if (g <= 0.04045) {
		g = g / 12.92;
	} else {
		g = fpow(((g + 0.055) / 1.055), 2.4);
	}
	if (b <= 0.04045) {
		b = b / 12.92;
	} else {
		b = fpow(((b + 0.055) / 1.055), 2.4);
	}

	*R = r;
	*G = g;
	*B = b;
}

void RGBtosRGB(double r, double g, double b, int *ir, int *ig, int *ib) {
	if (r > 0.0031308) {
		r = ((1.055 * fpow(r, 1.0 / 2.4)) - 0.055);
	} else {
		r = (r * 12.92);
	}
	if (g > 0.0031308) {
		g = ((1.055 * fpow(g, 1.0 / 2.4)) - 0.055);
	} else {
		g = (g * 12.92);
	}
	if (b > 0.0031308) {
		b = ((1.055 * fpow(b, 1.0 / 2.4)) - 0.055);
	} else {
		b = (b * 12.92);
	}

	*ir = r * 255;
	*ig = g * 255;
	*ib = b * 255;
}

void RGBtoLMS(double r, double g, double b, double *l, double *m, double *s) {
	// 2 deg
	*l = 1.021457 * r - 0.021553 * g + 0.000095 * b;
	*m = 1.573718 * r - 0.576264 * g + 0.002546 * b;
	*s = 0.033326 * r - 0.039022 * g + 1.005695 * b;

	// 10 deg
	*l = 1.017175 * r - 0.017223 * g + 0.000048 * b;
	*m = 1.409345 * r - 0.410486 * g + 0.001141 * b;
	*s = 0.057309 * r - 0.051593 * g + 0.994284 * b;

	// stockman-stiles 1999
	*l = 2.846201 * r + 11.092490 * g + b;
	*m = 0.168926 * r +  8.265895 * g + b;
	*s = 0.000000 * r +  0.010600 * g + b;
}

void LMStoRGB(double l, double m, double s, double *r, double *g, double *b) {
	// 2 deg
	*r =  0.941669 * l + 0.058331 * m + 0.000000 * s;
	*g =  0.481193 * l + 0.517322 * m + 0.001485 * s;
	*b =  0.034807 * l + 0.053464 * m + 0.911730 * s;

	// 10 deg
	*r =  0.942032 * l + 0.057968 * m + 0.000000 * s;
	*g =  0.485478 * l + 0.513799 * m + 0.000823 * s;
	*b =  0.050188 * l + 0.077054 * m + 0.872758 * s;

	// wolfram alpha inversion of stockman-stiles 1999
	*r =  0.381762     * l - 0.512476   * m + 0.130714 * s;
	*g = -0.00781889   * l + 0.131621   * m - 0.123809 * s;
	*b =  0.0000828061 * l - 0.00139518 * m + 1.00131 * s;

	if (*r < 0 || *g < 0 || *b < 0 || *r > 1 || *g > 1 || *b > 1) {
		*r = *g = *b = 1;
	}

	if (*r < 0) { *r = 0; }
	if (*g < 0) { *g = 0; }
	if (*b < 0) { *b = 0; }
	if (*r > 1) { *r = 1; }
	if (*g > 1) { *g = 1; }
	if (*b > 1) { *b = 1; }
}

// https://en.wikipedia.org/wiki/CIE_1931_color_space
void XYZtoxyY(double X, double Y, double Z, double *outx, double *outy, double *outY) {
  X /= 100;
  Y /= 100;
  Z /= 100;

  *outx = X / (X + Y + Z);
  *outy = Y / (X + Y + Z);
  *outY = Y;
}

// https://en.wikipedia.org/wiki/CIE_1931_color_space
void xyYtoXYZ(double x, double y, double Y, double *outX, double *outY, double *outZ) {
  double X = Y / y * x;
  double Z = Y / y * (1 - x - y);

  *outX = X * 100;
  *outY = Y * 100;
  *outZ = Z * 100;
}

// InstituttRapportUiO2008-XYZrepresentastionsOfStockmanSharpeFachConeFundamentals-AlychnaeDaylightAdaptation-Rapport.pdf
void LMStoXYZ(double l, double m, double s, double *x, double *y, double *z) {
	*x = (1.916925 * l - 1.337568 * m + 0.452183 * s) * 100;
	*y = (0.646565 * l + 0.397901 * m + 0        * s) * 100;
	*z = (0        * l + 0        * m + 2.135669 * s) * 100;
}

// Wolfram Alpha inversion
void XYZtoLMS(double X, double Y, double Z, double *l, double *m, double *s) {
	X /= 100;
	Y /= 100;
	Z /= 100;

	*l =  0.24475  * X + 0.821819 * Y - 0.0517625 * Z;
	*m = -0.397258 * X + 1.17778  * Y + 0.0841109 * Z;
	*s =  0        * X + 0        * Y + 0.468237  * Z;
}

double lightness_adjust(double l, double c, double h) {
	l /= 100;
	c /= 100;

	double a               = 0.157232;
	double u               = -3.09771;
	double o               = 1.35964;
	double a1              = 0.0294973;
	double u1              = 0.360255;
	double o1              = 0.657014;

	// apparent lightness of chroma 5 compared to chroma 0

	double lbase = 0.00309038 * sin(h + -0.856537) + 1.00238;

#if 0
	double m               = 0.175277;
	double b               = 0.332425;

	double lbase = 0;
	int hh;
	for (hh = -1; hh <= 1; hh++) {
		lbase += m * (a * exp(- (hh * h - u) * (hh * h - u) / (2 * o * o)) / (o * sqrt(2 * M_PI)) + a1 * exp(- (hh * h - u1) * (hh * h - u1) / (2 * o1 * o1)) / (o1 * sqrt(2 * M_PI))) + b;
	}
#endif

	if (c < 0.05) {
		l *= (lbase - 1) * c / 0.05 + 1;
	} else {
		l *= lbase;

		// incremental multiplier for each doubling of chroma

		double b               = 0.324736;
		double m = 1;

		double inc = log(c / 0.05) / log(2);

		double linc = 0;
		int hh;
		for (hh = -1; hh <= 1; hh++) {
			linc += m * (a * exp(- (hh * h - u) * (hh * h - u) / (2 * o * o)) / (o * sqrt(2 * M_PI)) + a1 * exp(- (hh * h - u1) * (hh * h - u1) / (2 * o1 * o1)) / (o1 * sqrt(2 * M_PI))) + b;
		}

		l *= exp(log(linc) * inc);
	}

	l *= 100;
	return l;
}

int main(int argc, char **argv) {
	double bright = .075;

	if (argc > 1) {
		bright = atof(argv[1]);

		double X, Y, Z;
		LABtoXYZ(bright, 0, 0, &X, &Y, &Z, K6100);
		bright = Y / 100;
	}

	double brown[3];
	xyYtoXYZ(.4345, .4140, 1, &brown[0], &brown[1], &brown[2]);

	unsigned char buf[WIDTH * HEIGHT * 4] = { 0 };
	int X, Y;

	double stripes[1000];
	int nstripes = 0;

	double here = -M_PI;
	while (here < M_PI && nstripes < 1000) {
		double there = here + h(here);
		stripes[nstripes++] = here - h(here) / 2; // (here + there) / 2;

		//printf("%f %f %f\n", here, there, h(here));
		here = there;
	}

	double min = INT_MAX;
	double max = INT_MIN;

	for (X = 0; X < WIDTH; X++) {
		for (Y = 0; Y < HEIGHT; Y++) {
			double x = (double) X / WIDTH;
			double y = 1 - ((double) Y / WIDTH);

			double cX, cY, cZ;
			xyYtoXYZ(x, y, bright, &cX, &cY, &cZ);

			int r, g, b;
			XYZtoRGB(cX, cY, cZ, &r, &g, &b);

#undef LIGHTNESS_ADJUST

#ifdef LIGHTNESS_ADJUST
			double L, C, H;
			double A, B;
			XYZtoLAB(cX, cY, cZ, &L, &A, &B, K6100);
			LABtoLCH(L, A, B, &L, &C, &H);
			double L1 = lightness_adjust(L, C, H);

			// printf("%f %f %f\n", L1, L, L1 / L);

			if (L1 / L < min) {
				min = L1 / L;
			}
			if (L1 / L > max) {
				max = L1 / L;
			}

#if 0
			r = 255 - ((L1 / L - 1) / .3) * 255;
			g = 255 - ((L1 / L - 1) / .3) * 255;
			b = 255 - ((L1 / L - 1) / .3) * 255;
#endif

			if (r >= 0 && g >= 0 && b >= 0 && r <= 255 && g <= 255 && b <= 255) {
				LABtoXYZ(L1, A, B, &cX, &cY, &cZ, K6100);
				XYZtoRGB(cX, cY, cZ, &r, &g, &b);
			}

#endif

			buf[(Y * HEIGHT + X) * 4 + 0] = r;
			buf[(Y * HEIGHT + X) * 4 + 1] = g;
			buf[(Y * HEIGHT + X) * 4 + 2] = b;
			buf[(Y * HEIGHT + X) * 4 + 3] = 255 * (r >= 0 && g >= 0 && b >= 0 && r <= 255 && g <= 255 && b <= 255);

			if (r < 0 || g < 0 || b < 0 || r > 255 || g > 255 | b > 255) {
				buf[(Y * HEIGHT + X) * 4 + 0] = 50;
				buf[(Y * HEIGHT + X) * 4 + 1] = 50;
				buf[(Y * HEIGHT + X) * 4 + 2] = 50;
				buf[(Y * HEIGHT + X) * 4 + 3] = 255;
			}

		}
	}

	fprintf(stderr, "lightness adjust %f %f\n", min, max);

	{
		int x, y;
#if 0
		int x = 0.37863113935913634 * WIDTH;
		int y = 0.2967842309084085 * HEIGHT;

		y = HEIGHT - 1 - y;
		buf[(y * HEIGHT + x) * 4 + 0] = 255;
		buf[(y * HEIGHT + x) * 4 + 1] = 255;
		buf[(y * HEIGHT + x) * 4 + 2] = 255;
		buf[(y * HEIGHT + x) * 4 + 3] = 255;

		x = 0.25950077422571144 * WIDTH;
		y = 0.37690276825054037 * HEIGHT;

		y = HEIGHT - 1 - y;
		buf[(y * HEIGHT + x) * 4 + 0] = 255;
		buf[(y * HEIGHT + x) * 4 + 1] = 255;
		buf[(y * HEIGHT + x) * 4 + 2] = 255;
		buf[(y * HEIGHT + x) * 4 + 3] = 255;
#endif

#if 0
		// sRGB D65 white

		x = 0.3127 * WIDTH;
		y = 0.3290 * HEIGHT;

		y = HEIGHT - 1 - y;
		buf[(y * HEIGHT + x) * 4 + 0] = 255;
		buf[(y * HEIGHT + x) * 4 + 1] = 255;
		buf[(y * HEIGHT + x) * 4 + 2] = 255;
		buf[(y * HEIGHT + x) * 4 + 3] = 255;
#endif

#if 0
		// This is close to 6100K white,
		// .31977181747811490829,.33598307040241928137
		// and lies directly between green and pink neutral points

		x =  .31977181747811490829 * WIDTH;
		y = .33598307040241928137 * HEIGHT;

#endif

#if 0
		x = .31906595679242389000 * WIDTH;
		y = .33684349957947443500 * HEIGHT;

		y = HEIGHT - 1 - y;
		buf[(y * HEIGHT + x) * 4 + 0] = 255;
		buf[(y * HEIGHT + x) * 4 + 1] = 255;
		buf[(y * HEIGHT + x) * 4 + 2] = 255;
		buf[(y * HEIGHT + x) * 4 + 3] = 255;
#endif
	}

	int orat = INT_MAX;
	for (X = 0; X < WIDTH; X++) {
		for (Y = 0; Y < HEIGHT; Y++) {
			double x = (double) X / WIDTH;
			double y = 1 - ((double) Y / WIDTH);

			double cX, cY, cZ;
			xyYtoXYZ(x, y, bright, &cX, &cY, &cZ);

			double l, m, s;
			XYZtoLMS(cX, cY, cZ, &l, &m, &s);

			int rat = floor(log(l / s) * 5);

			if (rat != orat) {
				buf[(Y * HEIGHT + X) * 4 + 0] = 100;
				buf[(Y * HEIGHT + X) * 4 + 1] = 100;
				buf[(Y * HEIGHT + X) * 4 + 2] = 100;
				buf[(Y * HEIGHT + X) * 4 + 3] = 255;
				orat = rat;
			}
		}
	}

	char s[2000];

	FILE *f = fopen("cvrl/cc2012xyz2_fine_5dp.csv", "r");
	while (fgets(s, 2000, f)) {
		double nm, cx, cy, cY;
		if (sscanf(s, "%lf,%lf,%lf,%lf", &nm, &cx, &cy, &cY) == 4) {
			int x = cx * WIDTH;
			int y = cy * HEIGHT;

			y = HEIGHT - 1 - y;

			buf[(y * HEIGHT + x) * 4 + 0] = 255;
			buf[(y * HEIGHT + x) * 4 + 1] = 255;
			buf[(y * HEIGHT + x) * 4 + 2] = 255;
			buf[(y * HEIGHT + x) * 4 + 3] = 255;
		}
	}
	fclose(f);

#if 0
	f = fopen("chroma-74-40/chroma-from-gray-log", "r");
	while (fgets(s, 2000, f)) {
		double h, c, l, delta;
		if (sscanf(s, "%lf %lf %lf %lf", &h, &c, &l, &delta) == 4) {
			double i;
			for (i = 0; i < 1; i++) {
				// Use the lightness of the triangle,
				// not the lightness of the test,
				// because the chroma step is the
				// same in each lightness, which isn't
				// the same in xy space.
				double cX, cY, cZ;
				xyYtoXYZ(.3, .3, bright, &cX, &cY, &cZ);
				double cL, cA, cB;
				XYZtoLAB(cX, cY, cZ, &cL, &cA, &cB, K6100);

				double a, b;
				LCHtoLAB(cL, (i + 1) * delta, h, &l, &a, &b);
				double X, Y, Z;
				LABtoXYZ(l, a, b, &X, &Y, &Z, K6100);
				double cx, cy;
				XYZtoxyY(X, Y, Z, &cx, &cy, &Y);

				int x = cx * WIDTH;
				int y = cy * HEIGHT;

				y = HEIGHT - 1 - y;

				if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
					buf[(y * HEIGHT + x) * 4 + 0] = 255;
					buf[(y * HEIGHT + x) * 4 + 1] = 255;
					buf[(y * HEIGHT + x) * 4 + 2] = 255;
					buf[(y * HEIGHT + x) * 4 + 3] = 255;
				}
			}
		}
	}
	fclose(f);

	f = fopen("chroma-74-40/chroma-from-warm-log", "r");
	while (fgets(s, 2000, f)) {
		double h, c, l, delta;
		if (sscanf(s, "%lf %lf %lf %lf", &h, &c, &l, &delta) == 4) {
			double i;
			for (i = 0; i < 1; i++) {
				// Use the lightness of the triangle,
				// not the lightness of the test,
				// because the chroma step is the
				// same in each lightness, which isn't
				// the same in xy space.
				double cX, cY, cZ;
				xyYtoXYZ(.3, .3, bright, &cX, &cY, &cZ);
				double cL, cA, cB;
				XYZtoLAB(cX, cY, cZ, &cL, &cA, &cB, brown);

				double a, b;
				LCHtoLAB(cL, (i + 1) * delta, h, &l, &a, &b);
				double X, Y, Z;
				LABtoXYZ(l, a, b, &X, &Y, &Z, brown);
				double cx, cy;
				XYZtoxyY(X, Y, Z, &cx, &cy, &Y);

				int x = cx * WIDTH;
				int y = cy * HEIGHT;

				y = HEIGHT - 1 - y;

				int xo, yo;
				for (xo = 0; xo < 2; xo++) {
					for (yo = 0; yo < 2; yo++) {

						if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
							buf[((y + yo) * HEIGHT + x + xo) * 4 + 0] = 255;
							buf[((y + yo) * HEIGHT + x + xo) * 4 + 1] = 255;
							buf[((y + yo) * HEIGHT + x + xo) * 4 + 2] = 255;
							buf[((y + yo) * HEIGHT + x + xo) * 4 + 3] = 255;
						}
					}
				}
			}
		}
	}
	fclose(f);
#endif

#if 0
	{
		double a;
		for (a = -40; a <= 40; a += .1) {
			int dir;
			for (dir = -1; dir <= 1; dir += 2) {
#define BESTFIT
#ifdef BESTFIT
				double xu = a - -0.555;
				double b = dir * 335.012 * exp(- xu * xu / (2 * 15.4883 * 15.4883)) / (15.4883 * sqrt(2 * M_PI)) - 0.32;

				double h = atan2(b, a);
				double c = sqrt(a * a + b * b);

				h -= .090;
#else

				double b = dir * 335.582 * exp(- a * a / (2 * 15.5723 * 15.5723)) / (15.5723 * sqrt(2 * M_PI));

				double h = atan2(b, a);
				double c = sqrt(a * a + b * b);

				h -= .075;
#endif

				double cX, cY, cZ;
				xyYtoXYZ(.3, .3, bright, &cX, &cY, &cZ);
				double cL, cA, cB;
				XYZtoLAB(cX, cY, cZ, &cL, &cA, &cB, K6100);

				int i;
				for (i = 1; i < 2; i++) {
					double l, a, b;

					LCHtoLAB(cL, c * i, h, &l, &a, &b);
					double X, Y, Z;
					LABtoXYZ(l, a, b, &X, &Y, &Z, K6100);
					double cx, cy;
					XYZtoxyY(X, Y, Z, &cx, &cy, &Y);

					int x = cx * WIDTH;
					int y = cy * HEIGHT;

					y = HEIGHT - 1 - y;

					if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
						buf[(y * HEIGHT + x) * 4 + 0] = 255;
						buf[(y * HEIGHT + x) * 4 + 1] = 255;
						buf[(y * HEIGHT + x) * 4 + 2] = 255;
						buf[(y * HEIGHT + x) * 4 + 3] = 255;
					}
				}
			}
		}
	}
#endif

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
