// http://rsb.info.nih.gov/ij/plugins/download/Color_Space_Converter.java

/**
* sRGB to XYZ conversion matrix
*/
var M = [ [0.4124, 0.3576,  0.1805],
	  [0.2126, 0.7152,  0.0722],
	  [0.0193, 0.1192,  0.9505] ];

/**
* XYZ to sRGB conversion matrix
*/
var Mi = [ [3.2406, -1.5372, -0.4986],
	   [-0.9689,  1.8758,  0.0415],
	   [ 0.0557, -0.2040,  1.0570] ];


var whitePoint = [ 95.0429, 100.0, 108.8900 ]; /* D65 */

function RGBtoXYZ(R, G, B) {
	// convert 0..255 into 0..1
	var r = R / 255.0;
	var g = G / 255.0;
	var b = B / 255.0;

	// assume sRGB
	if (r <= 0.04045) {
		r = r / 12.92;
	} else {
		r = Math.pow(((r + 0.055) / 1.055), 2.4);
	}
	if (g <= 0.04045) {
		g = g / 12.92;
	} else {
		g = Math.pow(((g + 0.055) / 1.055), 2.4);
	}
	if (b <= 0.04045) {
		b = b / 12.92;
	} else {
		b = Math.pow(((b + 0.055) / 1.055), 2.4);
	}

	r *= 100.0;
	g *= 100.0;
	b *= 100.0;

	// [X Y Z] = [r g b][M]
	return [
		(r * M[0][0]) + (g * M[0][1]) + (b * M[0][2]),
		(r * M[1][0]) + (g * M[1][1]) + (b * M[1][2]),
		(r * M[2][0]) + (g * M[2][1]) + (b * M[2][2])
	];
}

function XYZtoLAB(X, Y, Z) {
	var x = X / whitePoint[0];
	var y = Y / whitePoint[1];
	var z = Z / whitePoint[2];

	if (x > 0.008856) {
		x = Math.pow(x, 1.0 / 3.0);
	} else {
		x = (7.787 * x) + (16.0 / 116.0);
	}
	if (y > 0.008856) {
		y = Math.pow(y, 1.0 / 3.0);
	} else {
		y = (7.787 * y) + (16.0 / 116.0);
	}
	if (z > 0.008856) {
		z = Math.pow(z, 1.0 / 3.0);
	} else {
		z = (7.787 * z) + (16.0 / 116.0);
	}

	return [ (116.0 * y) - 16.0, 500.0 * (x - y), 200.0 * (y - z) ];
}

function LABtoLCH(L, A, B) {
	var C = Math.sqrt(A * A + B * B);
	var H = Math.atan2(B, A);

	return [ L, C, H ];
}

function LCHtoLAB(L, C, H) {
	var A = C * Math.cos(H);
	var B = C * Math.sin(H);

	return [ L, A, B ];
}

function LABtoXYZ(L, a, b) {
	var y = (L + 16.0) / 116.0;
	var y3 = Math.pow(y, 3.0);
	var x = (a / 500.0) + y;
	var x3 = Math.pow(x, 3.0);
	var z = y - (b / 200.0);
	var z3 = Math.pow(z, 3.0);

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

	return [ x * whitePoint[0], y * whitePoint[1], z * whitePoint[2] ];
}

function XYZtoRGB(X, Y, Z) {
	var x = X / 100.0;
	var y = Y / 100.0;
	var z = Z / 100.0;

	// [r g b] = [X Y Z][Mi]
	var r = (x * Mi[0][0]) + (y * Mi[0][1]) + (z * Mi[0][2]);
	var g = (x * Mi[1][0]) + (y * Mi[1][1]) + (z * Mi[1][2]);
	var b = (x * Mi[2][0]) + (y * Mi[2][1]) + (z * Mi[2][2]);

	// assume sRGB
	if (r > 0.0031308) {
		r = ((1.055 * Math.pow(r, 1.0 / 2.4)) - 0.055);
	} else {
		r = (r * 12.92);
	}
	if (g > 0.0031308) {
		g = ((1.055 * Math.pow(g, 1.0 / 2.4)) - 0.055);
	} else {
		g = (g * 12.92);
	}
	if (b > 0.0031308) {
		b = ((1.055 * Math.pow(b, 1.0 / 2.4)) - 0.055);
	} else {
		b = (b * 12.92);
	}

	r = (r < 0) ? 0 : r;
	g = (g < 0) ? 0 : g;
	b = (b < 0) ? 0 : b;

	return [r * 255, g * 255, b * 255];
}


var r = Math.floor(Math.random() * 256);
var g = Math.floor(Math.random() * 256);
var b = Math.floor(Math.random() * 256);

console.log([r, g, b]);
var xyz = RGBtoXYZ(r, g, b);
console.log(xyz);
var lab = XYZtoLAB(xyz[0], xyz[1], xyz[2]);
console.log(lab);
var lch = LABtoLCH(lab[0], lab[1], lab[2]);
console.log(lch);
lab = LCHtoLAB(lch[0], lch[1], lch[2]);
console.log(lab);
xyz = LABtoXYZ(lab[0], lab[1], lab[2]);
var rgb = XYZtoRGB(xyz[0], xyz[1], xyz[2]);
console.log(rgb);
