/*                                  
                                     
 */
/*
 * MIPS floating point support
 * Copyright (C) 1994-2000 Algorithmics Ltd.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 */


#include "ieee754dp.h"

ieee754dp ieee754dp_flong(s64 x)
{
	u64 xm;
	int xe;
	int xs;

	CLEARCX;

	if (x == 0)
		return ieee754dp_zero(0);
	if (x == 1 || x == -1)
		return ieee754dp_one(x < 0);
	if (x == 10 || x == -10)
		return ieee754dp_ten(x < 0);

	xs = (x < 0);
	if (xs) {
		if (x == (1ULL << 63))
			xm = (1ULL << 63);	/*                                 */
		else
			xm = -x;
	} else {
		xm = x;
	}

	/*           */
	xe = DP_MBITS + 3;
	if (xm >> (DP_MBITS + 1 + 3)) {
		/*                         */
		while (xm >> (DP_MBITS + 1 + 3)) {
			XDPSRSX1();
		}
	} else {
		/*                                            */
		while ((xm >> (DP_MBITS + 3)) == 0) {
			xm <<= 1;
			xe--;
		}
	}
	DPNORMRET1(xs, xe, xm, "dp_flong", x);
}

ieee754dp ieee754dp_fulong(u64 u)
{
	if ((s64) u < 0)
		return ieee754dp_add(ieee754dp_1e63(),
				     ieee754dp_flong(u & ~(1ULL << 63)));
	return ieee754dp_flong(u);
}