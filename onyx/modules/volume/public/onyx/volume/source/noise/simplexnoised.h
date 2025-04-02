#pragma once
/* sdnoise1234, Simplex noise with true analytic
 * derivative in 1D to 4D.
 *
 * Copyright © 2003-2012, Stefan Gustavson
 *
 * Contact: stefan.gustavson@gmail.com
 *
 * This library is public domain software, released by the author
 * into the public domain in February 2011. You may do anything
 * you like with it. You may even remove all attributions,
 * but of course I'd appreciate it if you kept my name somewhere.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

 /*
  * This is an implementation of Perlin "simplex noise" over one
  * dimension (x), two dimensions (x,y), three dimensions (x,y,z)
  * and four dimensions (x,y,z,w). The analytic derivative is
  * returned, to make it possible to do lots of fun stuff like
  * flow animations, curl noise, analytic antialiasing and such.
  *
  * Visually, this noise is exactly the same as the plain version of
  * simplex noise provided in the file "snoise1234.c". It just returns
  * all partial derivatives in addition to the scalar noise value.
  *
  */

namespace Onyx::Volume
{   
 
	namespace SimplexNoiseD
	{

        /*
         * Permutation table. This is just a random jumble of all numbers 0-255,
         * repeated twice to avoid wrapping the index at 255 for each lookup.
         */
        static constexpr unsigned char perm[512] = { 151,160,137,91,90,15,
          131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
          190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
          88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
          77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
          102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
          135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
          5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
          223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
          129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
          251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
          49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
          138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
          151,160,137,91,90,15,
          131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
          190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
          88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
          77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
          102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
          135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
          5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
          223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
          129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
          251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
          49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
          138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
        };

        /*
         * Gradient tables. These could be programmed the Ken Perlin way with
         * some clever bit-twiddling, but this is more clear, and not really slower.
         */
        static constexpr onyxF32 grad2lut[8][2] = {
          { -1.0f, -1.0f }, { 1.0f, 0.0f } , { -1.0f, 0.0f } , { 1.0f, 1.0f } ,
          { -1.0f, 1.0f } , { 0.0f, -1.0f } , { 0.0f, 1.0f } , { 1.0f, -1.0f }
        };

        inline void grad2(int hash, onyxF32 *gx, onyxF32 *gy) {
            int h = hash & 7;
            *gx = grad2lut[h][0];
            *gy = grad2lut[h][1];
            return;
        }

		/** 1D simplex noise with derivative.
		 * If the last argument is not null, the analytic derivative
		 * is also calculated.
		 */
         // change to vec 2 output
		void sdnoise1(onyxF32 x, Vector3f& output);

		/** 2D simplex noise with derivatives.
		 * If the last two arguments are not null, the analytic derivative
		 * (the 2D gradient of the scalar noise field) is also calculated.
		 */
        // change to vec 2 input
#define FASTFLOOR(x) ( ((int)(x)<=(x)) ? ((int)x) : (((int)x)-1) )
                /* Skewing factors for 2D simplex grid:
         * F2 = 0.5*(sqrt(3.0)-1.0)
         * G2 = (3.0-Math.sqrt(3.0))/6.0
         */
#define F2 .366025403f
#define G2 .211324865f
		inline void sdnoise2(const Vector3f& pos, Vector3f& output, bool calculateGradient)
        {
            onyxF32 n0, n1, n2; /* Noise contributions from the three simplex corners */
            onyxF32 gx0, gy0, gx1, gy1, gx2, gy2; /* Gradients at simplex corners */
            onyxF32 t0, t1, t2, x1, x2, y1, y2;
            onyxF32 t20, t40, t21, t41, t22, t42;

            /* Skew the input space to determine which simplex cell we're in */
            onyxF32 s = (pos[0] + pos[1]) * F2; /* Hairy factor for 2D */
            //onyxF32 xs = pos[0] + s;
            //onyxF32 ys = pos[1] + s;
            int ii, i = FASTFLOOR((pos[0] + s));
            int jj, j = FASTFLOOR((pos[1] + s));

            onyxF32 t = (i + j) * G2;
            onyxF32 X0 = i - t; /* Unskew the cell origin back to (x,y) space */
            onyxF32 Y0 = j - t;
            onyxF32 x0 = pos[0] - X0; /* The x,y distances from the cell origin */
            onyxF32 y0 = pos[1] - Y0;

            /* For the 2D case, the simplex shape is an equilateral triangle.
             * Determine which simplex we are in. */
            int i1, j1; /* Offsets for second (middle) corner of simplex in (i,j) coords */
            if (x0 > y0) { i1 = 1; j1 = 0; } /* lower triangle, XY order: (0,0)->(1,0)->(1,1) */
            else { i1 = 0; j1 = 1; }      /* upper triangle, YX order: (0,0)->(0,1)->(1,1) */

            /* A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
             * a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
             * c = (3-sqrt(3))/6   */
            x1 = x0 - i1 + G2; /* Offsets for middle corner in (x,y) unskewed coords */
            y1 = y0 - j1 + G2;
            x2 = x0 - 1.0f + 2.0f * G2; /* Offsets for last corner in (x,y) unskewed coords */
            y2 = y0 - 1.0f + 2.0f * G2;

            /* Wrap the integer indices at 256, to avoid indexing perm[] out of bounds */
            ii = i & 0xff;
            jj = j & 0xff;

            /* Calculate the contribution from the three corners */
            t0 = 0.5f - x0 * x0 - y0 * y0;
            if (t0 < 0.0f)
            {
                t40 = t20 = t0 = n0 = gx0 = gy0 = 0.0f; /* No influence */
            }
            else {
                grad2(perm[ii + perm[jj]], &gx0, &gy0);
                t20 = t0 * t0;
                t40 = t20 * t20;
                n0 = t40 * (gx0 * x0 + gy0 * y0);
            }

            t1 = 0.5f - x1 * x1 - y1 * y1;
            if (t1 < 0.0f) t21 = t41 = t1 = n1 = gx1 = gy1 = 0.0f; /* No influence */
            else {
                grad2(perm[ii + i1 + perm[jj + j1]], &gx1, &gy1);
                t21 = t1 * t1;
                t41 = t21 * t21;
                n1 = t41 * (gx1 * x1 + gy1 * y1);
            }

            t2 = 0.5f - x2 * x2 - y2 * y2;
            if (t2 < 0.0f) t42 = t22 = t2 = n2 = gx2 = gy2 = 0.0f; /* No influence */
            else {
                grad2(perm[ii + 1 + perm[jj + 1]], &gx2, &gy2);
                t22 = t2 * t2;
                t42 = t22 * t22;
                n2 = t42 * (gx2 * x2 + gy2 * y2);
            }

            /* Add contributions from each corner to get the final noise value.
             * The result is scaled to return values in the interval [-1,1]. */
            output[0] = 40.0f * (n0 + n1 + n2);

            if (calculateGradient)
            {
                /*  A straight, unoptimised calculation would be like:
                    *    *dnoise_dx = -8.0f * t20 * t0 * x0 * ( gx0 * x0 + gy0 * y0 ) + t40 * gx0;
                    *    *dnoise_dy = -8.0f * t20 * t0 * y0 * ( gx0 * x0 + gy0 * y0 ) + t40 * gy0;
                    *    *dnoise_dx += -8.0f * t21 * t1 * x1 * ( gx1 * x1 + gy1 * y1 ) + t41 * gx1;
                    *    *dnoise_dy += -8.0f * t21 * t1 * y1 * ( gx1 * x1 + gy1 * y1 ) + t41 * gy1;
                    *    *dnoise_dx += -8.0f * t22 * t2 * x2 * ( gx2 * x2 + gy2 * y2 ) + t42 * gx2;
                    *    *dnoise_dy += -8.0f * t22 * t2 * y2 * ( gx2 * x2 + gy2 * y2 ) + t42 * gy2;
                    */
                onyxF32 temp = t20 * t0 * (gx0* x0 + gy0 * y0);
                output[1] = temp * x0;
                output[2] = temp * y0;
                temp = t21 * t1 * (gx1 * x1 + gy1 * y1);
                output[1] += temp * x1;
                output[2] += temp * y1;
                temp = t22 * t2 * (gx2* x2 + gy2 * y2);
                output[1] += temp * x2;
                output[2] += temp * y2;
                output[1] *= -8.0f;
                output[2] *= -8.0f;
                output[1] += t40 * gx0 + t41 * gx1 + t42 * gx2;
                output[2] += t40 * gy0 + t41 * gy1 + t42 * gy2;
                output[1] *= 40.0f; /* Scale derivative to match the noise scaling */
                output[2] *= 40.0f;
            }
        }

		/** 3D simplex noise with derivatives.
		 * If the last tthree arguments are not null, the analytic derivative
		 * (the 3D gradient of the scalar noise field) is also calculated.
		 */
		void sdnoise3(const Vector3f& pos,
			Vector4f& output);

		/** 4D simplex noise with derivatives.
		 * If the last four arguments are not null, the analytic derivative
		 * (the 4D gradient of the scalar noise field) is also calculated.
		 */
		onyxF32 sdnoise4(onyxF32 x, onyxF32 y, onyxF32 z, onyxF32 w,
			onyxF32 *dnoise_dx, onyxF32 *dnoise_dy,
			onyxF32 *dnoise_dz, onyxF32 *dnoise_dw);
	}
}