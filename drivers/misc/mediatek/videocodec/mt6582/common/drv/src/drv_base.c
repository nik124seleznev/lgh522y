#include <linux/sched.h>
#include <linux/dma-mapping.h>
#include "val_types_private.h"
#include "val_log.h"
#include "drv_api.h"

//                                                              
//              
VAL_VCODEC_OAL_HW_CONTEXT_T  oal_hw_context[VCODEC_MULTIPLE_INSTANCE_NUM];               //                           
VAL_NON_CACHE_MEMORY_LIST_T  grNonCacheMemoryList[VCODEC_MULTIPLE_INSTANCE_NUM_x_10];    //                              

//                            
VAL_VCODEC_HW_LOCK_T grVcodecDecHWLock; //                  
VAL_VCODEC_HW_LOCK_T grVcodecEncHWLock; //                  

VAL_UINT32_T gu4LockDecHWCount; //                             
VAL_UINT32_T gu4LockEncHWCount; //                             
VAL_UINT32_T gu4DecISRCount;    //                          
VAL_UINT32_T gu4EncISRCount;    //                          


VAL_INT32_T search_HWLockSlot_ByTID(VAL_UINT32_T pa, VAL_UINT32_T curr_tid)
{
    int i;
    int j;

    for(i=0; i<VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if (oal_hw_context[i].u4VCodecThreadNum != VCODEC_THREAD_MAX_NUM)
        {
            for(j = 0; j < oal_hw_context[i].u4VCodecThreadNum; j++)
            {
                if (oal_hw_context[i].u4VCodecThreadID[j] == curr_tid)
                {
                    MFV_LOGD("[search_HWLockSlot_ByTID] Lookup curr HW Locker is ObjId %d in index%d\n", curr_tid, i);
                    return i;
                }
            }
        }
    }

    return -1;
}

VAL_INT32_T search_HWLockSlot_ByHandle(VAL_UINT32_T pa, VAL_HANDLE_T handle)
{
    VAL_INT32_T i;

    for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
        if (oal_hw_context[i].pvHandle == handle)
        {
            return i;
        }
    }

    return -1;
}

VAL_VCODEC_OAL_HW_CONTEXT_T *setCurr_HWLockSlot(VAL_UINT32_T pa, VAL_UINT32_T tid){

    int i, j;

    //                   
    for(i=0; i<VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        MFV_LOGD("Dump curr slot %d ObjId %lu \n", i, oal_hw_context[i].ObjId);
    }

    //                                                        
    for(i=0; i<VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if( oal_hw_context[i].ObjId == pa)
        {
            MFV_LOGD("Curr Already exist in %d Slot\n", i);
            return &oal_hw_context[i];
        }
    }

    //                                                        
    for(i=0; i<VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if (oal_hw_context[i].u4VCodecThreadNum != VCODEC_THREAD_MAX_NUM)
        {
            for (j = 0; j < oal_hw_context[i].u4VCodecThreadNum; j++)
            {
                if (oal_hw_context[i].u4VCodecThreadID[j] == current->pid)
                {
                    oal_hw_context[i].ObjId = pa;
                    MFV_LOGD("[setCurr_HWLockSlot] setCurr %d Slot\n", i);
                    return &oal_hw_context[i];
                }
            }
        }
    }

    MFV_LOGE("[VCodec Error][ERROR] setCurr_HWLockSlot All %d Slots unavaliable\n", VCODEC_MULTIPLE_INSTANCE_NUM);
    oal_hw_context[0].u4VCodecThreadNum = VCODEC_THREAD_MAX_NUM - 1;
    for(i = 0; i < oal_hw_context[0].u4VCodecThreadNum; i++)
    {
        oal_hw_context[0].u4VCodecThreadID[i] = current->pid;
    }
	return &oal_hw_context[0];
}


VAL_VCODEC_OAL_HW_CONTEXT_T *setCurr_HWLockSlot_Thread_ID(VAL_VCODEC_THREAD_ID_T a_prVcodecThreadID, VAL_UINT32_T *a_prIndex)
{
    int i;
    int j;
    int k;

    //                  
    for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if (oal_hw_context[i].u4VCodecThreadNum != VCODEC_THREAD_MAX_NUM)
        {
            for(j = 0;j < oal_hw_context[i].u4VCodecThreadNum; j++)
            {
                MFV_LOGD("[setCurr_HWLockSlot_Thread_ID] Dump curr slot %d, ThreadID[%d] = %d\n", i, j, oal_hw_context[i].u4VCodecThreadID[j]);
            }
        }
    }

    for (i = 0; i < a_prVcodecThreadID.u4VCodecThreadNum; i++)
    {
        MFV_LOGD("[setCurr_HWLockSlot_Thread_ID] VCodecThreadNum = %d, VCodecThreadID = %d\n",
            a_prVcodecThreadID.u4VCodecThreadNum,
            a_prVcodecThreadID.u4VCodecThreadID[i]
            );
    }

    //                                                       
    for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if (oal_hw_context[i].u4VCodecThreadNum != VCODEC_THREAD_MAX_NUM)
        {
            for (j = 0; j < oal_hw_context[i].u4VCodecThreadNum; j++)
            {
                for (k = 0; k < a_prVcodecThreadID.u4VCodecThreadNum; k++)
                {
                    if (oal_hw_context[i].u4VCodecThreadID[j] == a_prVcodecThreadID.u4VCodecThreadID[k])
                    {
                        MFV_LOGE("[setCurr_HWLockSlot_Thread_ID] Curr Already exist in %d Slot\n", i);
                        *a_prIndex = i;
                        return &oal_hw_context[i];
                    }
                }
            }
        }
    }

    //                                                        
    for(i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if (oal_hw_context[i].u4VCodecThreadNum == VCODEC_THREAD_MAX_NUM)
        {
            oal_hw_context[i].u4VCodecThreadNum = a_prVcodecThreadID.u4VCodecThreadNum;
            for(j = 0; j < a_prVcodecThreadID.u4VCodecThreadNum; j++)
            {
                oal_hw_context[i].u4VCodecThreadID[j] = a_prVcodecThreadID.u4VCodecThreadID[j];
                MFV_LOGD("[setCurr_HWLockSlot_Thread_ID] setCurr %d Slot, %d\n", i, oal_hw_context[i].u4VCodecThreadID[j]);
            }
            *a_prIndex = i;
            return &oal_hw_context[i];
        }
    }

    {
        MFV_LOGE("[VCodec Error][ERROR] setCurr_HWLockSlot_Thread_ID All %d Slots unavaliable\n", VCODEC_MULTIPLE_INSTANCE_NUM);
        oal_hw_context[0].u4VCodecThreadNum = a_prVcodecThreadID.u4VCodecThreadNum;
        for(i = 0; i < oal_hw_context[0].u4VCodecThreadNum; i++)
        {
            oal_hw_context[0].u4VCodecThreadID[i] = a_prVcodecThreadID.u4VCodecThreadID[i];
        }
        *a_prIndex = 0;
        return &oal_hw_context[0];
    }
}


VAL_VCODEC_OAL_HW_CONTEXT_T *freeCurr_HWLockSlot(VAL_UINT32_T pa)
{
    int i;
    int j;

    //                                                        

    for(i=0; i<VCODEC_MULTIPLE_INSTANCE_NUM; i++)
    {
        if( oal_hw_context[i].ObjId == pa)
        {
            oal_hw_context[i].ObjId = -1;
            for(j = 0; j < oal_hw_context[i].u4VCodecThreadNum; j++)
            {
                oal_hw_context[i].u4VCodecThreadID[j] = -1;
            }
            oal_hw_context[i].u4VCodecThreadNum = VCODEC_THREAD_MAX_NUM;
            oal_hw_context[i].Oal_HW_reg = (VAL_VCODEC_OAL_HW_REGISTER_T  *)0;
            MFV_LOGD("freeCurr_HWLockSlot %d Slot\n", i);
            return &oal_hw_context[i];
        }
    }

    MFV_LOGE("[VCodec Error][ERROR] freeCurr_HWLockSlot can't find pid in HWLockSlot\n");
	return 0;
}


VAL_INT32_T Add_NonCacheMemoryList(VAL_UINT32_T a_u4KVA, VAL_UINT32_T a_u4KPA, VAL_UINT32_T a_u4Size, VAL_UINT32_T a_u4VCodecThreadNum, VAL_UINT32_T* a_pu4VCodecThreadID)
{
    VAL_INT32_T i4I = 0;
    VAL_INT32_T i4J = 0;

    MFV_LOGD("Add_NonCacheMemoryList +, KVA = 0x%x, KPA = 0x%x, Size = 0x%x\n", a_u4KVA, a_u4KPA, a_u4Size);

     for(i4I = 0; i4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; i4I++)
    {
        if ((grNonCacheMemoryList[i4I].u4KVA == 0xffffffff) && (grNonCacheMemoryList[i4I].u4KPA == 0xffffffff))
        {
            MFV_LOGD("ADD Add_NonCacheMemoryList index = %d, VCodecThreadNum = %d, curr_tid = %d\n",
                i4I, a_u4VCodecThreadNum, current->pid);

            grNonCacheMemoryList[i4I].u4VCodecThreadNum = a_u4VCodecThreadNum;
            for (i4J = 0; i4J < grNonCacheMemoryList[i4I].u4VCodecThreadNum; i4J++)
            {
                grNonCacheMemoryList[i4I].u4VCodecThreadID[i4J] = *(a_pu4VCodecThreadID + i4J);
                MFV_LOGD("[Add_NonCacheMemoryList] VCodecThreadNum = %d, VCodecThreadID = %d\n",
                    grNonCacheMemoryList[i4I].u4VCodecThreadNum, grNonCacheMemoryList[i4I].u4VCodecThreadID[i4J]);
            }

            grNonCacheMemoryList[i4I].u4KVA = a_u4KVA;
            grNonCacheMemoryList[i4I].u4KPA = a_u4KPA;
            grNonCacheMemoryList[i4I].u4Size = a_u4Size;
            break;
        }
    }

    if (i4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10)
    {
        MFV_LOGE("[ERROR] CAN'T ADD Add_NonCacheMemoryList, List is FULL!!\n");
    }

    MFV_LOGD("Add_NonCacheMemoryList -\n");
    return ((VCODEC_MULTIPLE_INSTANCE_NUM_x_10 == i4I) ? -1 : i4I);
}

VAL_INT32_T Free_NonCacheMemoryList(VAL_UINT32_T a_u4KVA, VAL_UINT32_T a_u4KPA)
{
    VAL_UINT32_T i4I = 0;
    VAL_UINT32_T i4J = 0;

    MFV_LOGD("Free_NonCacheMemoryList +, KVA = 0x%x, KPA = 0x%x\n", a_u4KVA, a_u4KPA);

    for(i4I = 0; i4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; i4I++)
    {
        if ((grNonCacheMemoryList[i4I].u4KVA == a_u4KVA) && (grNonCacheMemoryList[i4I].u4KPA == a_u4KPA))
        {
            MFV_LOGD("Free Free_NonCacheMemoryList index = %d\n", i4I);
            grNonCacheMemoryList[i4I].u4VCodecThreadNum = VCODEC_THREAD_MAX_NUM;
            for (i4J = 0; i4J <VCODEC_THREAD_MAX_NUM; i4J++)
            {
                grNonCacheMemoryList[i4I].u4VCodecThreadID[i4J] = 0xffffffff;
            }

            grNonCacheMemoryList[i4I].u4KVA = 0xffffffff;
            grNonCacheMemoryList[i4I].u4KPA = 0xffffffff;
            grNonCacheMemoryList[i4I].u4Size = 0xffffffff;
            break;
        }
    }

    if (i4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10)
    {
        MFV_LOGE("[ERROR] CAN'T Free Free_NonCacheMemoryList, Address is not found!! KVA = 0x%08x, KPA = 0x%08x\n", a_u4KVA, a_u4KPA);
        MFV_LOGE("[%d] Dump NonCacheMemoryList: \n", current->pid);
        for(i4J = 0; i4J < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; i4J++)
        {
            if ((grNonCacheMemoryList[i4J].u4KVA != 0xffffffff) || (grNonCacheMemoryList[i4J].u4KPA != 0xffffffff))
            {
                MFV_LOGE("[%d] KVA = 0x%08x, KPA = 0x%08x, size = %u\n", i4J,
                    grNonCacheMemoryList[i4J].u4KVA, grNonCacheMemoryList[i4J].u4KPA, grNonCacheMemoryList[i4J].u4Size);
            }
        }
    }

    MFV_LOGD("Free_NonCacheMemoryList -\n");
    return ((VCODEC_MULTIPLE_INSTANCE_NUM_x_10 == i4I) ? -1 : i4I);
}


void Force_Free_NonCacheMemoryList(VAL_UINT32_T a_u4Tid)
{
    VAL_UINT32_T u4I = 0;
    VAL_UINT32_T u4K = 0;

    MFV_LOGD("Force_Free_NonCacheMemoryList +, curr_id = %d", a_u4Tid);

    for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {

        if (grNonCacheMemoryList[u4I].u4KPA != 0xffffffff)
        {
            MFV_LOGE("[WARNING] Force_Free_NonCacheMemoryList index = %d, tid = %d, KVA = 0x%x, KPA = 0x%x, Size = %d\n",
                u4I, a_u4Tid, grNonCacheMemoryList[u4I].u4KVA, grNonCacheMemoryList[u4I].u4KPA, grNonCacheMemoryList[u4I].u4Size);

            dma_free_coherent(0, grNonCacheMemoryList[u4I].u4Size, (void *)grNonCacheMemoryList[u4I].u4KVA, (dma_addr_t)grNonCacheMemoryList[u4I].u4KPA);

            grNonCacheMemoryList[u4I].u4VCodecThreadNum = VCODEC_THREAD_MAX_NUM;
            for (u4K = 0; u4K <VCODEC_THREAD_MAX_NUM; u4K++)
            {
                grNonCacheMemoryList[u4I].u4VCodecThreadID[u4K] = 0xffffffff;
            }
            grNonCacheMemoryList[u4I].u4KVA = 0xffffffff;
            grNonCacheMemoryList[u4I].u4KPA = 0xffffffff;
            grNonCacheMemoryList[u4I].u4Size = 0xffffffff;
        }
    }

    MFV_LOGD("Force_Free_NonCacheMemoryList -, curr_id = %d", a_u4Tid);
}

VAL_UINT32_T Search_NonCacheMemoryList_By_KPA(VAL_UINT32_T a_u4KPA)
{
    VAL_UINT32_T u4I = 0;
    VAL_UINT32_T u4VA_Offset = 0;

    u4VA_Offset = a_u4KPA & 0x00000fff;

    MFV_LOGD("Search_NonCacheMemoryList_By_KPA +, KPA = 0x%x, u4VA_Offset = 0x%x\n", a_u4KPA, u4VA_Offset);

    for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {
        if (grNonCacheMemoryList[u4I].u4KPA == (a_u4KPA - u4VA_Offset)) {
            MFV_LOGD("Find Search_NonCacheMemoryList_By_KPA index = %d\n", u4I);
            break;
        }
    }

    if (u4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10) {
        MFV_LOGE("[ERROR] CAN'T Find Search_NonCacheMemoryList_By_KPA, Address is not find!!\n");
        return (grNonCacheMemoryList[0].u4KVA + u4VA_Offset);
    }

    MFV_LOGD("Search_NonCacheMemoryList_By_KPA, u4VA = 0x%x -\n", (grNonCacheMemoryList[u4I].u4KVA + u4VA_Offset));

    return (grNonCacheMemoryList[u4I].u4KVA + u4VA_Offset);
}
