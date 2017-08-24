/*
                                                                                          
*/

/*                  
                              

                       
*/



/*
                 
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                
                                                
  
                   
                                                                                
                                                                                               
                                                                                                       
                                                                               
  
                   
                                                                                
                              
*/

/*                                                                             
                                                     
                                                                               
*/
#if !defined(MCR_EHTCR)
#define MCR_EHTCR                           0x0054
#endif

/*                                                                              
                                                      
                                                                                
*/
#include "gl_os.h"
#include "colibri.h"
#include "wlan_lib.h"

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                              
                                                                                
*/

/*                                                                              
                                               
                                                                                
*/

/*                                                                              
                                         
                                                                                
*/

/*                                                                              
                                                        
                                                                                
*/
static BOOL
kalDevRegRead_impl(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, OUT PUINT_32 pu4Value);

static BOOL
kalDevRegWrite_impl(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, IN UINT_32 u4Value);


/*                                                                              
                                            
                                                                                
*/

/*                                                                            */
/* 
                                                                          
 
                                                                 
                                                
                                                                                  
 
              
               
*/
/*                                                                            */
BOOL kalDevRegRead(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, OUT PUINT_32 pu4Value)
{
	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prGlueInfo);
	ASSERT(pu4Value);

	/*                     */
	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	/*              */
	kalDevRegRead_impl(prGlueInfo, u4Register, pu4Value);

	/*                      */
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	return TRUE;
}


/*                                                                            */
/* 
                                                                         
 
                                                                 
                                                
                                                                           
 
              
               
*/
/*                                                                            */
BOOL kalDevRegWrite(P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, IN UINT_32 u4Value)
{
	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prGlueInfo);

	/*                     */
	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	/*              */
	kalDevRegWrite_impl(prGlueInfo, u4Register, u4Value);

	/*                      */
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	return TRUE;
}


/*                                                                            */
/* 
                                                                            
 
                                                                     
                                                    
                                                              
                                                               
                                                                         
 
              
               
*/
/*                                                                            */
BOOL
kalDevPortRead(IN P_GLUE_INFO_T prGlueInfo,
	       IN UINT_16 u2Port,
	       IN UINT_16 u2Len, OUT PUINT_8 pucBuf, IN UINT_16 u2ValidOutBufSize)
{
	UINT_32 i;
	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prGlueInfo);

	/*                     */
	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	/*                                                             */
	if (u2Len > 4)
		kalDevRegWrite_impl(prGlueInfo, MCR_EHTCR, ALIGN_4(u2Len));

	/*                  */
#if EHPI16
	writew(u2Port, prGlueInfo->rHifInfo.mcr_addr_base);
#elif EHPI8
	writew((u2Port & 0xFF), prGlueInfo->rHifInfo.mcr_addr_base);
	writew(((u2Port & 0xFF00) >> 8), prGlueInfo->rHifInfo.mcr_addr_base);
#endif

	/*               */
	for (i = 0; i < ALIGN_4(u2Len); i += 4) {
#if EHPI16
		*((PUINT_16) & (pucBuf[i])) =
		    (UINT_16) (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFFFF);
		*((PUINT_16) & (pucBuf[i + 2])) =
		    (UINT_16) (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFFFF);
#elif EHPI8
		*((PUINT_8) & (pucBuf[i])) =
		    (UINT_8) (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF);
		*((PUINT_8) & (pucBuf[i + 1])) =
		    (UINT_8) (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF);
		*((PUINT_8) & (pucBuf[i + 2])) =
		    (UINT_8) (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF);
		*((PUINT_8) & (pucBuf[i + 3])) =
		    (UINT_8) (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF);
#endif
	}

	/*                                     */
	if (u2Len > 4)
		kalDevRegWrite_impl(prGlueInfo, MCR_EHTCR, 4);

	/*                      */
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	return TRUE;
}


/*                                                                            */
/* 
                                                                           
 
                                                                     
                                                    
                                                               
                                                                
                                                                         
 
              
               
*/
/*                                                                            */
BOOL
kalDevPortWrite(P_GLUE_INFO_T prGlueInfo,
		IN UINT_16 u2Port, IN UINT_16 u2Len, IN PUINT_8 pucBuf, IN UINT_16 u2ValidInBufSize)
{
	UINT_32 i;
	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prGlueInfo);

	/*                     */
	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	/*                                                             */
	if (u2Len > 4)
		kalDevRegWrite_impl(prGlueInfo, MCR_EHTCR, ALIGN_4(u2Len));

	/*                  */
#if EHPI16
	writew(u2Port, prGlueInfo->rHifInfo.mcr_addr_base);
#elif EHPI8
	writew((u2Port & 0xFF), prGlueInfo->rHifInfo.mcr_addr_base);
	writew(((u2Port & 0xFF00) >> 8), prGlueInfo->rHifInfo.mcr_addr_base);
#endif

	/*               */
	for (i = 0; i < ALIGN_4(u2Len); i += 4) {
#if EHPI16
		writew((UINT_32) (*((PUINT_16) & (pucBuf[i]))), prGlueInfo->rHifInfo.mcr_data_base);
		writew((UINT_32) (*((PUINT_16) & (pucBuf[i + 2]))),
		       prGlueInfo->rHifInfo.mcr_data_base);
#elif EHPI8
		writew((UINT_32) (*((PUINT_8) & (pucBuf[i]))), prGlueInfo->rHifInfo.mcr_data_base);
		writew((UINT_32) (*((PUINT_8) & (pucBuf[i + 1]))),
		       prGlueInfo->rHifInfo.mcr_data_base);
		writew((UINT_32) (*((PUINT_8) & (pucBuf[i + 2]))),
		       prGlueInfo->rHifInfo.mcr_data_base);
		writew((UINT_32) (*((PUINT_8) & (pucBuf[i + 3]))),
		       prGlueInfo->rHifInfo.mcr_data_base);
#endif
	}

	/*                                     */
	if (u2Len > 4)
		kalDevRegWrite_impl(prGlueInfo, MCR_EHTCR, 4);

	/*                      */
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	return TRUE;
}


/*                                                                            */
/* 
                                                                        
 
                                                                     
                                               
                                                                 
                                                                         
 
                                         
                                      
*/
/*                                                                            */
BOOL kalDevWriteWithSdioCmd52(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Addr, IN UINT_8 ucData)
{
	UINT_32 u4RegValue;
	BOOLEAN bRet;
	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prGlueInfo);

	/*                     */
	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	/*                                                                                                   */
	if (kalDevRegRead_impl(prGlueInfo, u4Addr, &u4RegValue) == TRUE) {
		u4RegValue &= 0x00;
		u4RegValue |= ucData;

		bRet = kalDevRegWrite_impl(prGlueInfo, u4Addr, u4RegValue);
	} else {
		bRet = FALSE;
	}

	/*                      */
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_EHPI_BUS);

	return bRet;
}


/*                                                                            */
/* 
                                                                         
                                                                    
 
                                                                 
                                                
                                                                                  
 
              
               
*/
/*                                                                            */
static BOOL
kalDevRegRead_impl(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, OUT PUINT_32 pu4Value)
{
	ASSERT(prGlueInfo);

	/*                  */
#if EHPI16
	writew(u4Register, prGlueInfo->rHifInfo.mcr_addr_base);
#elif EHPI8
	writew((u4Register & 0xFF), prGlueInfo->rHifInfo.mcr_addr_base);
	writew(((u4Register & 0xFF00) >> 8), prGlueInfo->rHifInfo.mcr_addr_base);
#endif

	/*               */
#if EHPI16
	*pu4Value = (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFFFF);
	*pu4Value |= ((readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFFFF) << 16);
#elif EHPI8
	*pu4Value = (readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF);
	*pu4Value |= ((readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF) << 8);
	*pu4Value |= ((readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF) << 16);
	*pu4Value |= ((readw(prGlueInfo->rHifInfo.mcr_data_base) & 0xFF) << 24);
#endif

	return TRUE;
}


/*                                                                            */
/* 
                                                                         
                                                                    
 
                                                                 
                                                
                                                                           
 
              
               
*/
/*                                                                            */
static BOOL
kalDevRegWrite_impl(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, IN UINT_32 u4Value)
{
	ASSERT(prGlueInfo);

	/*                  */
#if EHPI16
	writew(u4Register, prGlueInfo->rHifInfo.mcr_addr_base);
#elif EHPI8
	writew((u4Register & 0xFF), prGlueInfo->rHifInfo.mcr_addr_base);
	writew(((u4Register & 0xFF00) >> 8), prGlueInfo->rHifInfo.mcr_addr_base);
#endif

	/*               */
#if EHPI16
	writew(u4Value, prGlueInfo->rHifInfo.mcr_data_base);
	writew((u4Value & 0xFFFF0000) >> 16, prGlueInfo->rHifInfo.mcr_data_base);
#elif EHPI8
	writew((u4Value & 0x000000FF), prGlueInfo->rHifInfo.mcr_data_base);
	writew((u4Value & 0x0000FF00) >> 8, prGlueInfo->rHifInfo.mcr_data_base);
	writew((u4Value & 0x00FF0000) >> 16, prGlueInfo->rHifInfo.mcr_data_base);
	writew((u4Value & 0xFF000000) >> 24, prGlueInfo->rHifInfo.mcr_data_base);
#endif

	return TRUE;
}
