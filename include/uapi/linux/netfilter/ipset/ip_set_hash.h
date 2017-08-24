#ifndef _UAPI__IP_SET_HASH_H
#define _UAPI__IP_SET_HASH_H

/*                                */
enum {
	/*              */
	IPSET_ERR_HASH_FULL = IPSET_ERR_TYPE_SPECIFIC,
	/*                     */
	IPSET_ERR_HASH_ELEM,
	/*                  */
	IPSET_ERR_INVALID_PROTO,
	/*                                        */
	IPSET_ERR_MISSING_PROTO,
	/*                     */
	IPSET_ERR_HASH_RANGE_UNSUPPORTED,
	/*               */
	IPSET_ERR_HASH_RANGE,
};


#endif /*                      */
