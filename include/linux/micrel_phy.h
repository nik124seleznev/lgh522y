/*
 * include/linux/micrel_phy.h
 *
 * Micrel PHY IDs
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MICREL_PHY_H
#define _MICREL_PHY_H

#define MICREL_PHY_ID_MASK	0x00fffff0

#define PHY_ID_KSZ8873MLL	0x000e7237
#define PHY_ID_KSZ9021		0x00221610
#define PHY_ID_KS8737		0x00221720
#define PHY_ID_KSZ8021		0x00221555
#define PHY_ID_KSZ8031		0x00221556
#define PHY_ID_KSZ8041		0x00221510
#define PHY_ID_KSZ8051		0x00221550
/*                                             */
#define PHY_ID_KSZ8001		0x0022161A
/*                         */
#define PHY_ID_KSZ8081		0x00221560
#define PHY_ID_KSZ8061		0x00221570
#define PHY_ID_KSZ9031		0x00221620

#define PHY_ID_KSZ886X		0x00221430
#define PHY_ID_KSZ8863		0x00221435

/*                                         */
#define MICREL_PHY_50MHZ_CLK	0x00000001

#endif /*               */
