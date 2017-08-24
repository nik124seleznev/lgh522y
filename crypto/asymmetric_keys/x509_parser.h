/* X.509 certificate parser internal definitions
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#include <crypto/public_key.h>

struct x509_certificate {
	struct x509_certificate *next;
	struct public_key *pub;			/*                    */
	char		*issuer;		/*                            */
	char		*subject;		/*                             */
	char		*fingerprint;		/*                        */
	char		*authority;		/*                                  */
	struct tm	valid_from;
	struct tm	valid_to;
	enum pkey_algo	pkey_algo : 8;		/*                      */
	enum pkey_algo	sig_pkey_algo : 8;	/*                                */
	enum pkey_hash_algo sig_hash_algo : 8;	/*                          */
	const void	*tbs;			/*             */
	size_t		tbs_size;		/*                     */
	const void	*sig;			/*                */
	size_t		sig_size;		/*                  */
};

/*
                     
 */
extern void x509_free_certificate(struct x509_certificate *cert);
extern struct x509_certificate *x509_cert_parse(const void *data, size_t datalen);
