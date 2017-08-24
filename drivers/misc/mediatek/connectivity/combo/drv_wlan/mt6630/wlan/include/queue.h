/*
                                                                              
*/

/*                 
                                                   

                                                                  
                           
*/



#ifndef _QUEUE_H
#define _QUEUE_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_typedef.h"

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/
/*                                      */
typedef struct _QUE_ENTRY_T {
	struct _QUE_ENTRY_T *prNext;
	struct _QUE_ENTRY_T *prPrev;	/*                                    */
} QUE_ENTRY_T, *P_QUE_ENTRY_T;

/*                                      */
typedef struct _QUE_T {
	P_QUE_ENTRY_T prHead;
	P_QUE_ENTRY_T prTail;
	UINT_32 u4NumElem;
} QUE_T, *P_QUE_T;


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define MAXNUM_TDLS_PEER            4

#define QUEUE_INITIALIZE(prQueue) \
	{ \
	    (prQueue)->prHead = (P_QUE_ENTRY_T)NULL; \
	    (prQueue)->prTail = (P_QUE_ENTRY_T)NULL; \
	    (prQueue)->u4NumElem = 0; \
	}

#define QUEUE_IS_EMPTY(prQueue)             (((P_QUE_T)(prQueue))->prHead == (P_QUE_ENTRY_T)NULL)

#define QUEUE_IS_NOT_EMPTY(prQueue)         ((prQueue)->u4NumElem > 0)

#define QUEUE_GET_HEAD(prQueue)             ((prQueue)->prHead)

#define QUEUE_GET_TAIL(prQueue)             ((prQueue)->prTail)

#define QUEUE_GET_NEXT_ENTRY(prQueueEntry)  ((prQueueEntry)->prNext)

#define QUEUE_INSERT_HEAD(prQueue, prQueueEntry) \
	{ \
	    ASSERT(prQueue); \
	    ASSERT(prQueueEntry); \
	    (prQueueEntry)->prNext = (prQueue)->prHead; \
	    (prQueue)->prHead = (prQueueEntry); \
	    if ((prQueue)->prTail == (P_QUE_ENTRY_T)NULL) { \
		(prQueue)->prTail = (prQueueEntry); \
	    } \
	    ((prQueue)->u4NumElem)++; \
	}

#define QUEUE_INSERT_TAIL(prQueue, prQueueEntry) \
	{ \
	    ASSERT(prQueue); \
	    ASSERT(prQueueEntry); \
	    (prQueueEntry)->prNext = (P_QUE_ENTRY_T)NULL; \
	    if ((prQueue)->prTail) { \
		((prQueue)->prTail)->prNext = (prQueueEntry); \
	    } else { \
		(prQueue)->prHead = (prQueueEntry); \
	    } \
	    (prQueue)->prTail = (prQueueEntry); \
	    ((prQueue)->u4NumElem)++; \
	}

/*                                                                                 
                                                                         
                                                                       
 */
 #define QUEUE_REMOVE_HEAD(prQueue, prQueueEntry, _P_TYPE) \
	{ \
		ASSERT(prQueue); \
		prQueueEntry = (_P_TYPE)((prQueue)->prHead); \
		if (prQueueEntry) { \
			(prQueue)->prHead = ((P_QUE_ENTRY_T)(prQueueEntry))->prNext; \
			if ((prQueue)->prHead == (P_QUE_ENTRY_T)NULL) { \
				(prQueue)->prTail = (P_QUE_ENTRY_T)NULL; \
			} \
			((P_QUE_ENTRY_T)(prQueueEntry))->prNext = (P_QUE_ENTRY_T)NULL; \
			((prQueue)->u4NumElem)--; \
		} \
	}

#define QUEUE_MOVE_ALL(prDestQueue, prSrcQueue) \
	{ \
		ASSERT(prDestQueue); \
		ASSERT(prSrcQueue); \
	    *(P_QUE_T)prDestQueue = *(P_QUE_T)prSrcQueue; \
	    QUEUE_INITIALIZE(prSrcQueue); \
	}

#define QUEUE_CONCATENATE_QUEUES(prDestQueue, prSrcQueue) \
	{ \
	    ASSERT(prDestQueue); \
	    ASSERT(prSrcQueue); \
	    if (prSrcQueue->u4NumElem > 0) { \
			if ((prDestQueue)->prTail) { \
				((prDestQueue)->prTail)->prNext = (prSrcQueue)->prHead; \
			} else { \
				(prDestQueue)->prHead = (prSrcQueue)->prHead; \
			} \
			(prDestQueue)->prTail = (prSrcQueue)->prTail; \
			((prDestQueue)->u4NumElem) += ((prSrcQueue)->u4NumElem); \
			QUEUE_INITIALIZE(prSrcQueue); \
	    } \
	}

#define QUEUE_CONCATENATE_QUEUES_HEAD(prDestQueue, prSrcQueue) \
	{ \
		ASSERT(prDestQueue); \
		ASSERT(prSrcQueue); \
		if (prSrcQueue->u4NumElem > 0) { \
			((prSrcQueue)->prTail)->prNext = (prDestQueue)->prHead; \
			(prDestQueue)->prHead = (prSrcQueue)->prHead; \
			((prDestQueue)->u4NumElem) += ((prSrcQueue)->u4NumElem); \
			if ((prDestQueue)->prTail == NULL) {                 \
				(prDestQueue)->prTail = (prSrcQueue)->prTail;      \
			}  \
			QUEUE_INITIALIZE(prSrcQueue); \
		} \
	}

/*                                                                              
                                                     
                                                                                
*/
extern UINT_8 g_arTdlsLink[MAXNUM_TDLS_PEER];

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#endif				/*          */
