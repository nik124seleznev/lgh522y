/*
 * V4L2 OF binding parsing library
 *
 * Copyright (C) 2012 - 2013 Samsung Electronics Co., Ltd.
 * Author: Sylwester Nawrocki <s.nawrocki@samsung.com>
 *
 * Copyright (C) 2012 Renesas Electronics Corp.
 * Author: Guennadi Liakhovetski <g.liakhovetski@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 */
#ifndef _V4L2_OF_H
#define _V4L2_OF_H

#include <linux/list.h>
#include <linux/types.h>
#include <linux/errno.h>

#include <media/v4l2-mediabus.h>

struct device_node;

/* 
                                                               
                                        
                                                      
                                                     
                                        
 */
struct v4l2_of_bus_mipi_csi2 {
	unsigned int flags;
	unsigned char data_lanes[4];
	unsigned char clock_lane;
	unsigned short num_data_lanes;
};

/* 
                                                                 
                                        
                                
                                  
 */
struct v4l2_of_bus_parallel {
	unsigned int flags;
	unsigned char bus_width;
	unsigned char data_shift;
};

/* 
                                                        
                                                                               
                                                           
                                                       
                                           
                      
                                         
                                      
 */
struct v4l2_of_endpoint {
	unsigned int port;
	unsigned int id;
	const struct device_node *local_node;
	const __be32 *remote;
	enum v4l2_mbus_type bus_type;
	union {
		struct v4l2_of_bus_parallel parallel;
		struct v4l2_of_bus_mipi_csi2 mipi_csi2;
	} bus;
	struct list_head head;
};

#ifdef CONFIG_OF
void v4l2_of_parse_endpoint(const struct device_node *node,
				struct v4l2_of_endpoint *link);
struct device_node *v4l2_of_get_next_endpoint(const struct device_node *parent,
					struct device_node *previous);
struct device_node *v4l2_of_get_remote_port_parent(
					const struct device_node *node);
struct device_node *v4l2_of_get_remote_port(const struct device_node *node);
#else /*           */

static inline int v4l2_of_parse_endpoint(const struct device_node *node,
					struct v4l2_of_endpoint *link)
{
	return -ENOSYS;
}

static inline struct device_node *v4l2_of_get_next_endpoint(
					const struct device_node *parent,
					struct device_node *previous)
{
	return NULL;
}

static inline struct device_node *v4l2_of_get_remote_port_parent(
					const struct device_node *node)
{
	return NULL;
}

static inline struct device_node *v4l2_of_get_remote_port(
					const struct device_node *node)
{
	return NULL;
}

#endif /*           */

#endif /*            */
