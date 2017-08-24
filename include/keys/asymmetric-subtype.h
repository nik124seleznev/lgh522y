/* Asymmetric public-key cryptography key subtype
 *
 * See Documentation/security/asymmetric-keys.txt
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#ifndef _KEYS_ASYMMETRIC_SUBTYPE_H
#define _KEYS_ASYMMETRIC_SUBTYPE_H

#include <linux/seq_file.h>
#include <keys/asymmetric-type.h>

struct public_key_signature;

/*
                                                                      
                
 */
struct asymmetric_key_subtype {
	struct module		*owner;
	const char		*name;
	unsigned short		name_len;	/*                */

	/*                                               */
	void (*describe)(const struct key *key, struct seq_file *m);

	/*                               */
	void (*destroy)(void *payload);

	/*                                                          */
	int (*verify_signature)(const struct key *key,
				const struct public_key_signature *sig);
};

/* 
                                                                  
                             
  
                                                                           
                                          
 */
static inline
struct asymmetric_key_subtype *asymmetric_key_subtype(const struct key *key)
{
	return key->type_data.p[0];
}

#endif /*                            */
