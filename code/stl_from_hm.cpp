/*Modified from https://github.com/anoved/libtrix for use with our
 mountain heightmaps.*/

#include <stdio.h>
#include <stdlib.h>

#include "libtrix.h"
#include "make_hm.h"


typedef struct {
	int base; // boolean; output walls and bottom as well as terrain surface if true
	int ascii; // boolean; output ASCII STL instead of binary STL if true
	char *input; // path to input file; use stdin if NULL
	char *output; // path to output file; use stdout if NULL
	char *mask; // path to mask file; no mask if NULL
	int threshold; // 0-255; maximum value considered masked
	int reversed; // boolean; reverse results of mask tests if true
	int heightmask; // boolean; use heightmap as it's own mask if true
	float zscale; // scaling factor applied to raw Z values
	float baseheight; // height in STL units of base below lowest terrain (technically, offset added to scaled Z values)
} Settings;

Settings CONFIG = {
	1,    // generate base (walls and bottom)
	0,    // binary output
	NULL, // read from stdin
	"mountain.stl", // write to ourhm.stl
	NULL, // no mask
	127,  // middle of 8 bit range
	0,    // normal un-reversed mask
	0,    // no heightmasking
	1.0,  // no z scaling (use raw heightmap values)
	1.0   // minimum base thickness of one unit
};

// given four vertices and a mesh, add two triangles representing the quad with given corners
trix_result Surface(trix_mesh *mesh, const trix_vertex *v1, const trix_vertex *v2, const trix_vertex *v3, const trix_vertex *v4) {
	trix_triangle i, j;
	trix_result r;

	i.a = *v4;
	i.b = *v2;
	i.c = *v1;

	j.a = *v4;
	j.b = *v3;
	j.c = *v2;

	if ((r = trixAddTriangle(mesh, &i)) != TRIX_OK) {
		return r;
	}

	if ((r = trixAddTriangle(mesh, &j)) != TRIX_OK) {
		return r;
	}

	return TRIX_OK;
}

// returns average of all non-negative arguments.
// If any argument is negative, it is not included in the average.
// argument zp will always be nonnegative.
static float avgnonneg(float zp, float z1, float z2, float z3) {
	float sum = zp;
	float z[3] = {z1, z2, z3};
	int i, n = 1;

	for (i = 0; i < 3; i++) {
		if (z[i] >= 0) {
			sum += z[i];
			n++;
		}
	}

	return sum / (float)n;
}

static trix_result Wall(trix_mesh *mesh, const trix_vertex *a, const trix_vertex *b) {
	trix_vertex a0 = *a;
	trix_vertex b0 = *b;
	trix_triangle t1;
	trix_triangle t2;
	trix_result r;
	a0.z = 0;
	b0.z = 0;
	t1.a = *a;
	t1.b = *b;
	t1.c = b0;
	t2.a = b0;
	t2.b = a0;
	t2.c = *a;
	if ((r = trixAddTriangle(mesh, &t1)) != TRIX_OK) {
		return r;
	}
	if ((r = trixAddTriangle(mesh, &t2)) != TRIX_OK) {
		return r;
	}
	return TRIX_OK;
}


trix_result Mesh(Matrix *hm, trix_mesh *mesh) {
	unsigned int x, y;
	float az, bz, cz, dz, ez, fz, gz, hz;
	trix_vertex vp, v1, v2, v3, v4;
	trix_result r;

	for (y = 0; y < hm->num_rows; y++) {
		for (x = 0; x < hm->num_cols; x++) {

			/*

			+---+---+---+
			|   |   |   |
			| A | B | C |
			|   |   |   |
			+---1---2---+
			|   |I /|   |
			| H | P | D |
			|   |/ J|   |
			+---4---3---+
			|   |   |   |
			| G | F | E |
			|   |   |   |
			+---+---+---+

			Current pixel position is marked at center as P.
			This pixel is output as two triangles, I and J.
			Points 1, 2, 3, and 4 are offset half a unit from P.
			Neighboring pixels are A, B, C, D, E, F, G, and H.

			Vertex 1 z is average of ABPH z
			Vertex 2 z is average of BCDP z
			Vertex 3 z is average of PDEF z
			Vertex 4 z is average of HPFG z

			Averages do not include neighbors that would lie
			outside the image, but do included masked values.

			*/

			// determine elevation of neighboring pixels in order to
			// to interpolate height of corners 1, 2, 3, and 4.
			// -1 is used to flag edge pixels to disregard.
			// (Masked neighbors are still considered.)

			if (x == 0 || y == 0) {
				az = -1;
			} else {
				az = hm->rows[x - 1][y - 1];
			}

			if (y == 0) {
				bz = -1;
			} else {
				bz = hm->rows[x][y - 1];
			}

			if (y == 0 || x + 1 == hm->num_cols) {
				cz = -1;
			} else {
				cz = hm->rows[x + 1][y - 1];
			}

			if (x + 1 == hm->num_cols) {
				dz = -1;
			} else {
				dz = hm->rows[x + 1][y];
			}

			if (x + 1 == hm->num_cols || y + 1 == hm->num_rows) {
				ez = -1;
			} else {
				ez = hm->rows[x + 1][y + 1];
			}

			if (y + 1 == hm->num_rows) {
				fz = -1;
			} else {
				fz = hm->rows[x][y + 1];
			}

			if (y + 1 == hm->num_rows || x == 0) {
				gz = -1;
			} else {
				gz = hm->rows[x - 1][y + 1];
			}

			if (x == 0) {
				hz = -1;
			} else {
				hz = hm->rows[x - 1][y];
			}

			// pixel vertex
			vp.x = (float)x;
			vp.y = (float)(hm->num_rows - y);
			vp.z = hm->rows[x][y];

			// Vertex 1
			v1.x = (float)x - 0.5;
			v1.y = ((float)hm->num_rows - ((float)y - 0.5));
			v1.z = avgnonneg(vp.z, az, bz, hz);

			// Vertex 2
			v2.x = (float)x + 0.5;
			v2.y = v1.y;
			v2.z = avgnonneg(vp.z, bz, cz, dz);

			// Vertex 3
			v3.x = v2.x;
			v3.y = ((float)hm->num_rows - ((float)y + 0.5));
			v3.z = avgnonneg(vp.z, dz, ez, fz);

			// Vertex 4
			v4.x = v1.x;
			v4.y = v3.y;
			v4.z = avgnonneg(vp.z, hz, fz, gz);

			// Upper surface
			if ((r = Surface(mesh, &v1, &v2, &v3, &v4)) != TRIX_OK) {
				return r;
			}

			// nothing left to do for this pixel unless we need to make walls
			if (!CONFIG.base) {
				continue;
			}

			// north wall (vertex 1 to 2)
			if (y == 0) {
				if ((r = Wall(mesh, &v1, &v2)) != TRIX_OK) {
					return r;
				}
			}

			// east wall (vertex 2 to 3)
			if (x + 1 == hm->num_cols) {
				if ((r = Wall(mesh, &v2, &v3)) != TRIX_OK) {
					return r;
				}
			}

			// south wall (vertex 3 to 4)
			if (y + 1 == hm->num_rows) {
				if ((r = Wall(mesh, &v3, &v4)) != TRIX_OK) {
					return r;
				}
			}

			// west wall (vertex 4 to 1)
			if (x == 0) {
				if ((r = Wall(mesh, &v4, &v1)) != TRIX_OK) {
					return r;
				}
			}

			// bottom surface - same as top, except with z = 0 and reverse winding
			v1.z = 0; v2.z = 0; v3.z = 0; v4.z = 0;
			if ((r = Surface(mesh, &v4, &v3, &v2, &v1)) != TRIX_OK) {
				return r;
			}
		}
	}

	return TRIX_OK;
}

// returns 0 on success, nonzero otherwise
int HeightmapToSTL(Matrix *hm) {
	trix_result r;
	trix_mesh *mesh;

	if ((r = trixCreate(&mesh, "hmstl")) != TRIX_OK) {
		return (int)r;
	}

	if ((r = Mesh(hm, mesh)) != TRIX_OK) {
		return (int)r;
	}

	// writes to stdout if CONFIG.output is null, otherwise writes to path it names
	if ((r = trixWrite(mesh, CONFIG.output, (CONFIG.ascii ? TRIX_STL_ASCII : TRIX_STL_BINARY))) != TRIX_OK) {
		return (int)r;
	}

	(void)trixRelease(&mesh);

	return 0;
}

int main(){
  Matrix *hm = make_mountain(37,3);
	print_matrix(hm);
  HeightmapToSTL(hm);
}
