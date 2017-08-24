/*
 * Generic HDLC support routines for Linux
 *
 * Copyright (C) 1999-2005 Krzysztof Halasa <khc@pm.waw.pl>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 */

#ifndef _UAPI__HDLC_H
#define _UAPI__HDLC_H


#define HDLC_MAX_MTU 1500	/*                     */
#if 0
#define HDLC_MAX_MRU (HDLC_MAX_MTU + 10 + 14 + 4) /*                      */
#else
#define HDLC_MAX_MRU 1600 /*                            */
#endif


#endif /*               */
