/* mpihelp-div.c  -  MPI helper functions
 *	Copyright (C) 1994, 1996 Free Software Foundation, Inc.
 *	Copyright (C) 1998, 1999 Free Software Foundation, Inc.
 *
 * This file is part of GnuPG.
 *
 * GnuPG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GnuPG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * Note: This code is heavily based on the GNU MP Library.
 *	 Actually it's the same code with only minor changes in the
 *	 way the data is stored; this is to support the abstraction
 *	 of an optional secure memory allocation which may be used
 *	 to avoid revealing of sensitive data due to paging etc.
 *	 The GNU MP Library itself is published under the LGPL;
 *	 however I decided to publish this code under the plain GPL.
 */

#include "mpi-internal.h"
#include "longlong.h"

#ifndef UMUL_TIME
#define UMUL_TIME 1
#endif
#ifndef UDIV_TIME
#define UDIV_TIME UMUL_TIME
#endif

/*                                                  
                                                         
                                                         
                                                                       
                        
                                                                           
  
                 
                     
                                                          
                                                                   
                                                            
                                                                           
                       
                                                       
 */

mpi_limb_t
mpihelp_divrem(mpi_ptr_t qp, mpi_size_t qextra_limbs,
	       mpi_ptr_t np, mpi_size_t nsize, mpi_ptr_t dp, mpi_size_t dsize)
{
	mpi_limb_t most_significant_q_limb = 0;

	switch (dsize) {
	case 0:
		/*                                                                 
                                                                 */
		/*
                                                         
                                                              
   */
		return 1 / dsize;

	case 1:
		{
			mpi_size_t i;
			mpi_limb_t n1;
			mpi_limb_t d;

			d = dp[0];
			n1 = np[nsize - 1];

			if (n1 >= d) {
				n1 -= d;
				most_significant_q_limb = 1;
			}

			qp += qextra_limbs;
			for (i = nsize - 2; i >= 0; i--)
				udiv_qrnnd(qp[i], n1, n1, np[i], d);
			qp -= qextra_limbs;

			for (i = qextra_limbs - 1; i >= 0; i--)
				udiv_qrnnd(qp[i], n1, n1, 0, d);

			np[0] = n1;
		}
		break;

	case 2:
		{
			mpi_size_t i;
			mpi_limb_t n1, n0, n2;
			mpi_limb_t d1, d0;

			np += nsize - 2;
			d1 = dp[1];
			d0 = dp[0];
			n1 = np[1];
			n0 = np[0];

			if (n1 >= d1 && (n1 > d1 || n0 >= d0)) {
				sub_ddmmss(n1, n0, n1, n0, d1, d0);
				most_significant_q_limb = 1;
			}

			for (i = qextra_limbs + nsize - 2 - 1; i >= 0; i--) {
				mpi_limb_t q;
				mpi_limb_t r;

				if (i >= qextra_limbs)
					np--;
				else
					np[0] = 0;

				if (n1 == d1) {
					/*                                                       
                                                            
                        */
					q = ~(mpi_limb_t) 0;

					r = n0 + d1;
					if (r < d1) {	/*                        */
						add_ssaaaa(n1, n0, r - d0,
							   np[0], 0, d0);
						qp[i] = q;
						continue;
					}
					n1 = d0 - (d0 != 0 ? 1 : 0);
					n0 = -d0;
				} else {
					udiv_qrnnd(q, r, n1, n0, d1);
					umul_ppmm(n1, n0, d0, q);
				}

				n2 = np[0];
q_test:
				if (n1 > r || (n1 == r && n0 > n2)) {
					/*                                 */
					q--;
					sub_ddmmss(n1, n0, n1, n0, 0, d0);
					r += d1;
					if (r >= d1)	/*                              */
						goto q_test;
				}

				qp[i] = q;
				sub_ddmmss(n1, n0, r, n2, n1, n0);
			}
			np[1] = n1;
			np[0] = n0;
		}
		break;

	default:
		{
			mpi_size_t i;
			mpi_limb_t dX, d1, n0;

			np += nsize - dsize;
			dX = dp[dsize - 1];
			d1 = dp[dsize - 2];
			n0 = np[dsize - 1];

			if (n0 >= dX) {
				if (n0 > dX
				    || mpihelp_cmp(np, dp, dsize - 1) >= 0) {
					mpihelp_sub_n(np, np, dp, dsize);
					n0 = np[dsize - 1];
					most_significant_q_limb = 1;
				}
			}

			for (i = qextra_limbs + nsize - dsize - 1; i >= 0; i--) {
				mpi_limb_t q;
				mpi_limb_t n1, n2;
				mpi_limb_t cy_limb;

				if (i >= qextra_limbs) {
					np--;
					n2 = np[dsize];
				} else {
					n2 = np[dsize - 1];
					MPN_COPY_DECR(np + 1, np, dsize - 1);
					np[0] = 0;
				}

				if (n0 == dX) {
					/*                                                        
                                          */
					q = ~(mpi_limb_t) 0;
				} else {
					mpi_limb_t r;

					udiv_qrnnd(q, r, n0, np[dsize - 1], dX);
					umul_ppmm(n1, n0, d1, q);

					while (n1 > r
					       || (n1 == r
						   && n0 > np[dsize - 2])) {
						q--;
						r += dX;
						if (r < dX)	/*                                    */
							break;
						n1 -= n0 < d1;
						n0 -= d1;
					}
				}

				/*                                                             
                                                                
                                                       */
				cy_limb = mpihelp_submul_1(np, dp, dsize, q);

				if (n2 != cy_limb) {
					mpihelp_add_n(np, np, dp, dsize);
					q--;
				}

				qp[i] = q;
				n0 = np[dsize - 1];
			}
		}
	}

	return most_significant_q_limb;
}
