/*
 * nfsd-specific authentication stuff.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#ifndef LINUX_NFSD_AUTH_H
#define LINUX_NFSD_AUTH_H

/*
                                                                
              
 */
int nfsd_setuser(struct svc_rqst *, struct svc_export *);

#endif /*                   */
