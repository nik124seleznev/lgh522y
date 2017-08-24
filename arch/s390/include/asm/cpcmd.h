/*
 *  S390 version
 *    Copyright IBM Corp. 1999
 *    Author(s): Martin Schwidefsky (schwidefsky@de.ibm.com),
 *               Christian Borntraeger (cborntra@de.ibm.com),
 */

#ifndef _ASM_S390_CPCMD_H
#define _ASM_S390_CPCMD_H

/*
                                  
                                                                             
 */
extern int __cpcmd(const char *cmd, char *response, int rlen, int *response_code);

/*
                                                           
  
                                                           
                                                      
                                                                     
                                                              
                                                        
                                                    
                                                                             
                                                           
                                                                  
 */
extern int cpcmd(const char *cmd, char *response, int rlen, int *response_code);

#endif /*                   */
