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

// https://en.wikipedia.org/wiki/CIE_1931_color_space
function XYZtoxyY(X, Y, Z) {
	X /= 100;
	Y /= 100;
	Z /= 100;

	var x = X / (X + Y + Z);
	var y = Y / (X + Y + Z);

	return [ x, y, Y ];
}

// https://en.wikipedia.org/wiki/CIE_1931_color_space
function xyYtoXYZ(x, y, Y) {
	var X = Y / y * x;
	var Z = Y / y * (1 - x - y);

	return [ X * 100, Y * 100, Z * 100 ];
}

function hex(v) {
	var h = Math.floor(v).toString(16);
	if (h.length < 2) {
		h = '0' + h;
	}
	if (h.length > 2) {
		return '00';
	}
	return h;
}

function RGBtohex(R, G, B) {
	return '#' + hex(R) + hex(G) + hex(B);
}


// http://www-cvrl.ucsd.edu/cmfs.htm
// CIE 1931 standard observer
var NMtoXYZ = {
	360: [ 0.0001299, 0.000003917, 0.0006061 ],
	365: [ 0.0002321, 0.000006965, 0.001086 ],
	370: [ 0.0004149, 0.00001239, 0.001946 ],
	375: [ 0.0007416, 0.00002202, 0.003486 ],
	380: [ 0.001368, 0.000039, 0.006450001 ],
	385: [ 0.002236, 0.000064, 0.01054999 ],
	390: [ 0.004243, 0.00012, 0.02005001 ],
	395: [ 0.00765, 0.000217, 0.03621 ],
	400: [ 0.01431, 0.000396, 0.06785001 ],
	405: [ 0.02319, 0.00064, 0.1102 ],
	410: [ 0.04351, 0.00121, 0.2074 ],
	415: [ 0.07763, 0.00218, 0.3713 ],
	420: [ 0.13438, 0.004, 0.6456 ],
	425: [ 0.21477, 0.0073, 1.0390501 ],
	430: [ 0.2839, 0.0116, 1.3856 ],
	435: [ 0.3285, 0.01684, 1.62296 ],
	440: [ 0.34828, 0.023, 1.74706 ],
	445: [ 0.34806, 0.0298, 1.7826 ],
	450: [ 0.3362, 0.038, 1.77211 ],
	455: [ 0.3187, 0.048, 1.7441 ],
	460: [ 0.2908, 0.06, 1.6692 ],
	465: [ 0.2511, 0.0739, 1.5281 ],
	470: [ 0.19536, 0.09098, 1.28764 ],
	475: [ 0.1421, 0.1126, 1.0419 ],
	480: [ 0.09564, 0.13902, 0.8129501 ],
	485: [ 0.05795001, 0.1693, 0.6162 ],
	490: [ 0.03201, 0.20802, 0.46518 ],
	495: [ 0.0147, 0.2586, 0.3533 ],
	500: [ 0.0049, 0.323, 0.272 ],
	505: [ 0.0024, 0.4073, 0.2123 ],
	510: [ 0.0093, 0.503, 0.1582 ],
	515: [ 0.0291, 0.6082, 0.1117 ],
	520: [ 0.06327, 0.71, 0.07824999 ],
	525: [ 0.1096, 0.7932, 0.05725001 ],
	530: [ 0.1655, 0.862, 0.04216 ],
	535: [ 0.2257499, 0.9148501, 0.02984 ],
	540: [ 0.2904, 0.954, 0.0203 ],
	545: [ 0.3597, 0.9803, 0.0134 ],
	550: [ 0.4334499, 0.9949501, 0.008749999 ],
	555: [ 0.5120501, 1, 0.005749999 ],
	560: [ 0.5945, 0.995, 0.0039 ],
	565: [ 0.6784, 0.9786, 0.002749999 ],
	570: [ 0.7621, 0.952, 0.0021 ],
	575: [ 0.8425, 0.9154, 0.0018 ],
	580: [ 0.9163, 0.87, 0.001650001 ],
	585: [ 0.9786, 0.8163, 0.0014 ],
	590: [ 1.0263, 0.757, 0.0011 ],
	595: [ 1.0567, 0.6949, 0.001 ],
	600: [ 1.0622, 0.631, 0.0008 ],
	605: [ 1.0456, 0.5668, 0.0006 ],
	610: [ 1.0026, 0.503, 0.00034 ],
	615: [ 0.9384, 0.4412, 0.00024 ],
	620: [ 0.8544499, 0.381, 0.00019 ],
	625: [ 0.7514, 0.321, 0.0001 ],
	630: [ 0.6424, 0.265, 0.00005 ],
	635: [ 0.5419, 0.217, 0.00003 ],
	640: [ 0.4479, 0.175, 0.00002 ],
	645: [ 0.3608, 0.1382, 0.00001 ]
};

/*

var r = Math.floor(Math.random() * 256);
var g = Math.floor(Math.random() * 256);
var b = Math.floor(Math.random() * 256);

console.log([r, g, b]);
var xyz = RGBtoXYZ(r, g, b);
console.log(xyz);
var xyY = XYZtoxyY(xyz[0], xyz[1], xyz[2]);
console.log(xyY);
xyz = xyYtoXYZ(xyY[0], xyY[1], xyY[2]);
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

*/

for (var i = 360; i < 650; i += 5) {
	var xyz = NMtoXYZ[i];
	lab = XYZtoLAB(xyz[0] * 100, xyz[1] * 100, xyz[2] * 100);
	lch = LABtoLCH(lab[0], lab[1], lab[2]);
	lab = LCHtoLAB(50, 29, lch[2]);
	xyz = LABtoXYZ(lab[0], lab[1], lab[2]);
	rgb = XYZtoRGB(xyz[0], xyz[1], xyz[2]);
	console.log(i + " " + lch + " " + RGBtohex(rgb[0], rgb[1], rgb[2]));
}
