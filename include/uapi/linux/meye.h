/*
 * Motion Eye video4linux driver for Sony Vaio PictureBook
 *
 * Copyright (C) 2001-2003 Stelian Pop <stelian@popies.net>
 *
 * Copyright (C) 2001-2002 Alcôve <www.alcove.com>
 *
 * Copyright (C) 2000 Andrew Tridgell <tridge@valinux.com>
 *
 * Earlier work by Werner Almesberger, Paul `Rusty' Russell and Paul Mackerras.
 *
 * Some parts borrowed from various video4linux drivers, especially
 * bttv-driver.c and zoran.c, see original files for credits.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _MEYE_H_
#define _MEYE_H_

/*                                                                          */
/*                                                                          */
/*                                                                          */

struct meye_params {
	unsigned char subsample;
	unsigned char quality;
	unsigned char sharpness;
	unsigned char agc;
	unsigned char picture;
	unsigned char framerate;
};

/*                               */
#define MEYEIOC_G_PARAMS	_IOR ('v', BASE_VIDIOC_PRIVATE+0, struct meye_params)
/*                             */
#define MEYEIOC_S_PARAMS	_IOW ('v', BASE_VIDIOC_PRIVATE+1, struct meye_params)
/*                                  */
#define MEYEIOC_QBUF_CAPT	_IOW ('v', BASE_VIDIOC_PRIVATE+2, int)
/*                                       */
#define MEYEIOC_SYNC		_IOWR('v', BASE_VIDIOC_PRIVATE+3, int)
/*                                   */
#define MEYEIOC_STILLCAPT	_IO  ('v', BASE_VIDIOC_PRIVATE+4)
/*                                */
#define MEYEIOC_STILLJCAPT	_IOR ('v', BASE_VIDIOC_PRIVATE+5, int)

/*                       */
#define V4L2_CID_MEYE_AGC		(V4L2_CID_USER_MEYE_BASE + 0)
#define V4L2_CID_MEYE_PICTURE		(V4L2_CID_USER_MEYE_BASE + 1)
#define V4L2_CID_MEYE_FRAMERATE		(V4L2_CID_USER_MEYE_BASE + 2)

#endif
