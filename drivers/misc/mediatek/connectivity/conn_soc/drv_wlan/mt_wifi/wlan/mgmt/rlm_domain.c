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
#include "precomp.h"

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
static const UINT_16     g_u2CountryGroup0[] =
{
    COUNTRY_CODE_AO,    COUNTRY_CODE_BZ,    COUNTRY_CODE_BJ,    COUNTRY_CODE_BT,
    COUNTRY_CODE_BO,    COUNTRY_CODE_BI,    COUNTRY_CODE_CM,    COUNTRY_CODE_CF,
    COUNTRY_CODE_TD,    COUNTRY_CODE_KM,    COUNTRY_CODE_CD,    COUNTRY_CODE_CG,
    COUNTRY_CODE_CI,    COUNTRY_CODE_DJ,    COUNTRY_CODE_GQ,    COUNTRY_CODE_ER,
    COUNTRY_CODE_FJ,    COUNTRY_CODE_GA,    COUNTRY_CODE_GM,    COUNTRY_CODE_GN,
    COUNTRY_CODE_GW,    COUNTRY_CODE_RKS,   COUNTRY_CODE_KG,    COUNTRY_CODE_LY,
    COUNTRY_CODE_MG,    COUNTRY_CODE_ML,    COUNTRY_CODE_NR,    COUNTRY_CODE_NC,
    COUNTRY_CODE_ST,    COUNTRY_CODE_SC,    COUNTRY_CODE_SL,    COUNTRY_CODE_SB,
    COUNTRY_CODE_SO,    COUNTRY_CODE_SR,    COUNTRY_CODE_SZ,    COUNTRY_CODE_TJ,
    COUNTRY_CODE_TG,    COUNTRY_CODE_TO,    COUNTRY_CODE_TM,    COUNTRY_CODE_TV,
    COUNTRY_CODE_VU,    COUNTRY_CODE_IL,    COUNTRY_CODE_YE
};
static const UINT_16     g_u2CountryGroup1[] =
{
    COUNTRY_CODE_AS,    COUNTRY_CODE_AI,    COUNTRY_CODE_BM,    COUNTRY_CODE_CA,
    COUNTRY_CODE_KY,    COUNTRY_CODE_GU,    COUNTRY_CODE_FM,    COUNTRY_CODE_PR,
    COUNTRY_CODE_US,    COUNTRY_CODE_VI,

};
static const UINT_16     g_u2CountryGroup2[] = {
    COUNTRY_CODE_AR,    COUNTRY_CODE_AU,    COUNTRY_CODE_AZ,    COUNTRY_CODE_BW,
    COUNTRY_CODE_KH,    COUNTRY_CODE_CX,    COUNTRY_CODE_CO,    COUNTRY_CODE_CR,
    COUNTRY_CODE_EC,    COUNTRY_CODE_GD,    COUNTRY_CODE_GT,    COUNTRY_CODE_HK,
    COUNTRY_CODE_KI,    COUNTRY_CODE_LB,    COUNTRY_CODE_LR,    COUNTRY_CODE_MN,
    COUNTRY_CODE_AN,    COUNTRY_CODE_NZ,    COUNTRY_CODE_NI,    COUNTRY_CODE_PW,
    COUNTRY_CODE_PY,    COUNTRY_CODE_PE,    COUNTRY_CODE_PH,    COUNTRY_CODE_WS,
    COUNTRY_CODE_SG,    COUNTRY_CODE_LK,    COUNTRY_CODE_TH,    COUNTRY_CODE_TT,
    COUNTRY_CODE_UY,    COUNTRY_CODE_VN
};
static const UINT_16     g_u2CountryGroup3[] = {
    COUNTRY_CODE_AW,    COUNTRY_CODE_LA,    COUNTRY_CODE_SA,    COUNTRY_CODE_AE,
    COUNTRY_CODE_UG
};

static const UINT_16     g_u2CountryGroup4[] = {COUNTRY_CODE_MM};

static const UINT_16     g_u2CountryGroup5[] =
{
    COUNTRY_CODE_AL,    COUNTRY_CODE_DZ,    COUNTRY_CODE_AD,    COUNTRY_CODE_AT,
    COUNTRY_CODE_BY,    COUNTRY_CODE_BE,    COUNTRY_CODE_BA,    COUNTRY_CODE_VG,
    COUNTRY_CODE_BG,    COUNTRY_CODE_CV,    COUNTRY_CODE_HR,    COUNTRY_CODE_CY,
    COUNTRY_CODE_CZ,    COUNTRY_CODE_DK,    COUNTRY_CODE_EE,    COUNTRY_CODE_ET,
    COUNTRY_CODE_FI,    COUNTRY_CODE_FR,    COUNTRY_CODE_GF,    COUNTRY_CODE_PF,
    COUNTRY_CODE_TF,    COUNTRY_CODE_GE,    COUNTRY_CODE_DE,    COUNTRY_CODE_GH,
    COUNTRY_CODE_GR,    COUNTRY_CODE_GP,    COUNTRY_CODE_HU,    COUNTRY_CODE_IS,
    COUNTRY_CODE_IQ,    COUNTRY_CODE_IE,    COUNTRY_CODE_IT,    COUNTRY_CODE_KE,
    COUNTRY_CODE_LV,    COUNTRY_CODE_LS,    COUNTRY_CODE_LI,    COUNTRY_CODE_LT,
    COUNTRY_CODE_LU,    COUNTRY_CODE_MK,    COUNTRY_CODE_MT,    COUNTRY_CODE_MQ,
    COUNTRY_CODE_MR,    COUNTRY_CODE_MU,    COUNTRY_CODE_YT,    COUNTRY_CODE_MD,
    COUNTRY_CODE_MC,    COUNTRY_CODE_ME,    COUNTRY_CODE_MS,    COUNTRY_CODE_NL,
    COUNTRY_CODE_NO,    COUNTRY_CODE_OM,    COUNTRY_CODE_PL,    COUNTRY_CODE_PT,
    COUNTRY_CODE_RE,    COUNTRY_CODE_RO,    COUNTRY_CODE_MF,    COUNTRY_CODE_SM,
    COUNTRY_CODE_SN,    COUNTRY_CODE_RS,    COUNTRY_CODE_SK,    COUNTRY_CODE_SI,
    COUNTRY_CODE_ZA,    COUNTRY_CODE_ES,    COUNTRY_CODE_SE,    COUNTRY_CODE_CH,
    COUNTRY_CODE_TR,    COUNTRY_CODE_TC,    COUNTRY_CODE_GB,    COUNTRY_CODE_VA,
    COUNTRY_CODE_FR
};
static const UINT_16     g_u2CountryGroup6[] = {COUNTRY_CODE_JP};
static const UINT_16     g_u2CountryGroup7[] =
{
    COUNTRY_CODE_AM,    COUNTRY_CODE_IL,    COUNTRY_CODE_KW,    COUNTRY_CODE_MA,
    COUNTRY_CODE_NE,    COUNTRY_CODE_TN,    COUNTRY_CODE_MA
};
static const UINT_16     g_u2CountryGroup8[] = {COUNTRY_CODE_NP};
static const UINT_16     g_u2CountryGroup9[] = {COUNTRY_CODE_AF};
static const UINT_16     g_u2CountryGroup10[] =
{
    COUNTRY_CODE_AG,    COUNTRY_CODE_BS,    COUNTRY_CODE_BH,    COUNTRY_CODE_BB,
    COUNTRY_CODE_BN,    COUNTRY_CODE_CL,    COUNTRY_CODE_CN,    COUNTRY_CODE_EG,
    COUNTRY_CODE_SV,    COUNTRY_CODE_IN,    COUNTRY_CODE_MY,    COUNTRY_CODE_MV,
    COUNTRY_CODE_PA,    COUNTRY_CODE_VE,    COUNTRY_CODE_ZM,

};
static const UINT_16     g_u2CountryGroup11[] = {COUNTRY_CODE_JO, COUNTRY_CODE_PG};
static const UINT_16     g_u2CountryGroup12[] =
{
    COUNTRY_CODE_BF,    COUNTRY_CODE_GY,    COUNTRY_CODE_HT,    COUNTRY_CODE_HN,
    COUNTRY_CODE_JM,    COUNTRY_CODE_MO,    COUNTRY_CODE_MW,    COUNTRY_CODE_PK,
    COUNTRY_CODE_QA,    COUNTRY_CODE_RW,    COUNTRY_CODE_KN,    COUNTRY_CODE_TZ,

};
static const UINT_16     g_u2CountryGroup13[] = {COUNTRY_CODE_ID};
static const UINT_16     g_u2CountryGroup14[] = {COUNTRY_CODE_KR};
static const UINT_16     g_u2CountryGroup15[] = {COUNTRY_CODE_NG};
static const UINT_16     g_u2CountryGroup16[] =
{
    COUNTRY_CODE_BD,    COUNTRY_CODE_BR,    COUNTRY_CODE_DM,    COUNTRY_CODE_DO,
    COUNTRY_CODE_FK,    COUNTRY_CODE_KZ,    COUNTRY_CODE_MX,    COUNTRY_CODE_MZ,
    COUNTRY_CODE_NA,    COUNTRY_CODE_RU,    COUNTRY_CODE_LC,    COUNTRY_CODE_VC,
    COUNTRY_CODE_UA,    COUNTRY_CODE_UZ,    COUNTRY_CODE_ZW
};
static const UINT_16     g_u2CountryGroup17[] = {COUNTRY_CODE_MP};
static const UINT_16     g_u2CountryGroup18[] = {COUNTRY_CODE_TW};
static const UINT_16     g_u2CountryGroup19[] =
{
    COUNTRY_CODE_CK,    COUNTRY_CODE_CU,    COUNTRY_CODE_TL,    COUNTRY_CODE_FO,
    COUNTRY_CODE_GI,    COUNTRY_CODE_GB,    COUNTRY_CODE_IR,    COUNTRY_CODE_IM,
    COUNTRY_CODE_JE,    COUNTRY_CODE_KP,    COUNTRY_CODE_MH,    COUNTRY_CODE_NU,
    COUNTRY_CODE_NF,    COUNTRY_CODE_PS,    COUNTRY_CODE_PN,    COUNTRY_CODE_PM,
    COUNTRY_CODE_SS,    COUNTRY_CODE_SD,    COUNTRY_CODE_SY
};
static const UINT_16     g_u2CountryGroup20[] =
{
    COUNTRY_CODE_EU
    /*                                                                                            
                                                                
     */
};
static const UINT_16     g_u2CountryGroup21[] =
{
    COUNTRY_CODE_UDF
};

#define REG_DOMAIN_DEF_IDX        20  /*                   */

DOMAIN_INFO_ENTRY arSupportedRegDomains[] = {
    {
        (PUINT_16) g_u2CountryGroup0, sizeof(g_u2CountryGroup0) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_NULL,  0,  0,   0,  FALSE }, /*                    */
            { 118, BAND_NULL,  0,  0,   0,  FALSE }, /*                    */
            { 121, BAND_NULL,  0,  0,   0,  FALSE }, /*                   */
            { 125, BAND_NULL,  0,  0,   0,  FALSE }, /*                      */
            { 0,   BAND_NULL,  0,  0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup1, sizeof(g_u2CountryGroup1) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  11,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  12,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup2, sizeof(g_u2CountryGroup2) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  12,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup3, sizeof(g_u2CountryGroup3) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  12,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /*                           */
					  {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup4, sizeof(g_u2CountryGroup4) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  12,  FALSE }, /*                        */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /*                      */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup5, sizeof(g_u2CountryGroup5) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /*                        */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /*                      */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup6, sizeof(g_u2CountryGroup6) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */
            {  82, BAND_2G4, CHNL_SPAN_5,   14,   1,  FALSE }, /*                  */
            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /*                        */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /*                     */
        }
    },
    {
        (PUINT_16) g_u2CountryGroup7, sizeof(g_u2CountryGroup7) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                  */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /*                     */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup8, sizeof(g_u2CountryGroup8) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                  */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /*                          */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup9, sizeof(g_u2CountryGroup9) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_NULL,           0,   0,   0,  FALSE }, /*                   */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                  */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /*                     */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup10, sizeof(g_u2CountryGroup10) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                   */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup11, sizeof(g_u2CountryGroup11) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_NULL,           0,   0,   0,  FALSE }, /*                    */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                   */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup12, sizeof(g_u2CountryGroup12) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /*                    */
            { 118, BAND_NULL,           0,   0,   0,  FALSE }, /*                    */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                   */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup13, sizeof(g_u2CountryGroup13) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /*                    */
            { 118, BAND_NULL,           0,   0,   0,  FALSE }, /*                    */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /*                   */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup14, sizeof(g_u2CountryGroup14) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,   8,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup15, sizeof(g_u2CountryGroup15) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup16, sizeof(g_u2CountryGroup16) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup17, sizeof(g_u2CountryGroup17) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  11,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup18, sizeof(g_u2CountryGroup18) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  11,  FALSE }, /*                 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup19, sizeof(g_u2CountryGroup19) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  12,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        /*                                                */
        (PUINT_16) g_u2CountryGroup20, sizeof(g_u2CountryGroup20) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /*                 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /*                       */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /*                       */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  12,  FALSE }, /*                        */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   7,  FALSE }, /*                           */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
	{
		/*                                                                             */
		(PUINT_16) g_u2CountryGroup21, sizeof(g_u2CountryGroup21) / 2,
		{
			{  81, BAND_2G4, CHNL_SPAN_5,	 1,  	11,  FALSE }, /*                 */

			{ 115, BAND_5G, CHNL_SPAN_20,	36,  	4,  FALSE }, /*                       */
			{ 118, BAND_5G,	CHNL_SPAN_20, 	52,	 	4,  FALSE }, /*                       */
		   	{ 121, BAND_5G,	CHNL_SPAN_20,	100,	12,  FALSE }, /*                        */
		   	{ 125, BAND_5G,	CHNL_SPAN_20,	149,	7,  FALSE }, /*                           */
			{	0, BAND_NULL,		   0,	  0,    0,  FALSE }
		}
	}
};


#define REG_DOMAIN_PASSIVE_DEF_IDX	0
#define REG_DOMAIN_PASSIVE_UDF_IDX	1

static const UINT_16     g_u2CountryGroup0_Passive[] =
{
	COUNTRY_CODE_UDF
};

DOMAIN_INFO_ENTRY arSupportedRegDomains_Passive[] = {
	{
        /*                                             */
		COUNTRY_CODE_NULL,	 0,
		{
			{  81, BAND_2G4, CHNL_SPAN_5,	 1,  	0,  0 }, /*                 */

			{ 115, BAND_5G, CHNL_SPAN_20,	36,  	0,  0 }, /*                       */
			{ 118, BAND_5G,	CHNL_SPAN_20, 	52,	 	0,  0 }, /*                       */
		   	{ 121, BAND_5G,	CHNL_SPAN_20,	100,	0,  0 }, /*                        */
		   	{ 125, BAND_5G,	CHNL_SPAN_20,	149,	0,  0 }, /*                           */
			{	0, BAND_NULL,		   0,	  0,    0,  0 }
		}
	},

	{
		/*                                         */
		g_u2CountryGroup0_Passive,	 0,
		{
			{  81, BAND_2G4, CHNL_SPAN_5,	 1,  	0,  0 }, /*                 */

			{ 115, BAND_5G, CHNL_SPAN_20,	36,  	4,  0 }, /*                       */
			{ 118, BAND_5G,	CHNL_SPAN_20, 	52,	 	4,  0 }, /*                       */
		   	{ 121, BAND_5G,	CHNL_SPAN_20,	100,	12, 0 }, /*                        */
		   	{ 125, BAND_5G,	CHNL_SPAN_20,	149,	7,  0 }, /*                           */
			{	0, BAND_NULL,		   0,	  0,    0,  0 }
		}
    }

};


#if 0
COUNTRY_CH_SET_T arCountryChSets[] = {
    /*                                                                                           
                                     */
    {CH_SET_2G4_1_11,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_165},
    /*                                                 */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_165},
    /*                                           */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_NA},
    /*            */
    {CH_SET_2G4_1_14,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_NA},
    /*                             */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_NA,
     CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_165},
    /*                  */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_165},
    /*                               */
    {CH_SET_2G4_1_11,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_161},
    /*               */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_161},
    /*                  */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_NA,
     CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_161},
    /*               */
    {CH_SET_2G4_1_11,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_116_132_140,    CH_SET_UNII_UPPER_149_165},
    /*                                               */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_165},
    /*                         */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_NA},
    /*                        */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_NA,
     CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_NA},
    /*                     */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_128,    CH_SET_UNII_UPPER_149_165},
    /*                */
    {CH_SET_2G4_1_11,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_165},
    /*                          */
    {CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
     CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_173}
};
#endif


/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

UINT32
rlmDomainSupOperatingClassIeFill(
	UINT_8				*pBuf
	)
{
	/*
                                                                              
 */
	UINT32 u4IeLen;
	UINT8 aucClass[12] = { 0x01, 0x02, 0x03, 0x05, 0x16, 0x17, 0x19, 0x1b,
		0x1c, 0x1e, 0x20, 0x21 };


	/*
                                                                           
                                                                         

                                                                              
                                                                             
                                                        
 */
	SUP_OPERATING_CLASS_IE(pBuf)->ucId = ELEM_ID_SUP_OPERATING_CLASS;
	SUP_OPERATING_CLASS_IE(pBuf)->ucLength = 1+sizeof(aucClass);
	SUP_OPERATING_CLASS_IE(pBuf)->ucCur = 0x0c; /*      */
	kalMemCopy(SUP_OPERATING_CLASS_IE(pBuf)->ucSup, aucClass, sizeof(aucClass));
	u4IeLen = (SUP_OPERATING_CLASS_IE(pBuf)->ucLength + 2);
	pBuf += u4IeLen;

	COUNTRY_IE(pBuf)->ucId = ELEM_ID_COUNTRY_INFO;
	COUNTRY_IE(pBuf)->ucLength = 6;
	COUNTRY_IE(pBuf)->aucCountryStr[0] = 0x55;
	COUNTRY_IE(pBuf)->aucCountryStr[1] = 0x53;
	COUNTRY_IE(pBuf)->aucCountryStr[2] = 0x20;
	COUNTRY_IE(pBuf)->arCountryStr[0].ucFirstChnlNum = 1;
	COUNTRY_IE(pBuf)->arCountryStr[0].ucNumOfChnl = 11;
	COUNTRY_IE(pBuf)->arCountryStr[0].cMaxTxPwrLv = 0x1e;
	u4IeLen += (COUNTRY_IE(pBuf)->ucLength + 2);

	return u4IeLen;
}


/*                                                                            */
/* 
        
 
                
 
              
*/
/*                                                                            */
P_DOMAIN_INFO_ENTRY
rlmDomainGetDomainInfo (
    P_ADAPTER_T     prAdapter
    )
{
#define REG_DOMAIN_GROUP_NUM \
        (sizeof(arSupportedRegDomains) / sizeof(DOMAIN_INFO_ENTRY))

    UINT_16                 i, j;
    P_DOMAIN_INFO_ENTRY     prDomainInfo;
    P_REG_INFO_T            prRegInfo;
    UINT_16                 u2TargetCountryCode;

    ASSERT(prAdapter);

    prRegInfo = &prAdapter->prGlueInfo->rRegInfo;

    DBGLOG(RLM, INFO, ("Domain: map=%d, idx=%d, code=0x%04x\n",
        prRegInfo->eRegChannelListMap, prRegInfo->ucRegChannelListIndex,
        prAdapter->rWifiVar.rConnSettings.u2CountryCode));

	/*                                                         */
	
	/*                 */
    if (prRegInfo->eRegChannelListMap == REG_CH_MAP_TBL_IDX &&
        prRegInfo->ucRegChannelListIndex < REG_DOMAIN_GROUP_NUM) {
        prDomainInfo = &arSupportedRegDomains[prRegInfo->ucRegChannelListIndex];
        goto L_set_domain_info;
    }	/*                        */
    else if (prRegInfo->eRegChannelListMap == REG_CH_MAP_CUSTOMIZED) {
        prDomainInfo = &prRegInfo->rDomainInfo;
        goto L_set_domain_info;
    }


	/*                         */
    u2TargetCountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;
	
    for (i = 0; i < REG_DOMAIN_GROUP_NUM; i++) {
        prDomainInfo = &arSupportedRegDomains[i];

        ASSERT((prDomainInfo->u4CountryNum && prDomainInfo->pu2CountryGroup) ||
               prDomainInfo->u4CountryNum == 0);

        for (j = 0; j < prDomainInfo->u4CountryNum; j++) {
            if (prDomainInfo->pu2CountryGroup[j] == u2TargetCountryCode) {
                break;
            }
        }
        if (j < prDomainInfo->u4CountryNum) {
            break; /*       */
        }
    }

    DATA_STRUC_INSPECTING_ASSERT(REG_DOMAIN_DEF_IDX < REG_DOMAIN_GROUP_NUM);


	/*                           */
	if (i >= REG_DOMAIN_GROUP_NUM){
		if (prAdapter->prDomainInfo) /*                            */
			return prAdapter->prDomainInfo;
		else	/*                             */
			prDomainInfo = &arSupportedRegDomains[REG_DOMAIN_DEF_IDX];	
	}
	

L_set_domain_info:

    prAdapter->prDomainInfo = prDomainInfo;
    return prDomainInfo;
}

/*                                                                            */
/* 
        
 
                                                                         
                                                                           
 
              
*/
/*                                                                            */
VOID
rlmDomainGetChnlList (
    P_ADAPTER_T             prAdapter,
    ENUM_BAND_T             eSpecificBand,
    UINT_8                  ucMaxChannelNum,
    PUINT_8                 pucNumOfChannel,
    P_RF_CHANNEL_INFO_T     paucChannelList
    )
{
    UINT_8                  i, j, ucNum;
    P_DOMAIN_SUBBAND_INFO   prSubband;
    P_DOMAIN_INFO_ENTRY     prDomainInfo;

    ASSERT(prAdapter);
    ASSERT(paucChannelList);
    ASSERT(pucNumOfChannel);

    /*                                                              */
    prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
    ASSERT(prDomainInfo);

    ucNum = 0;
    for (i = 0; i < MAX_SUBBAND_NUM; i++) {
        prSubband = &prDomainInfo->rSubBand[i];

        if (prSubband->ucBand == BAND_NULL || prSubband->ucBand >= BAND_NUM ||
            (prSubband->ucBand == BAND_5G && !prAdapter->fgEnable5GBand)) {
            continue;
        }

        if (eSpecificBand == BAND_NULL || prSubband->ucBand == eSpecificBand) {
            for (j = 0; j < prSubband->ucNumChannels; j++) {
                if (ucNum >= ucMaxChannelNum) {
                    break;
                }
                paucChannelList[ucNum].eBand = prSubband->ucBand;
                paucChannelList[ucNum].ucChannelNum =
                    prSubband->ucFirstChannelNum + j * prSubband->ucChannelSpan;
                ucNum++;
            }
        }
    }

    *pucNumOfChannel = ucNum;
}

/*                                                                            */
/* 
        
 
            
 
                
*/
/*                                                                            */
VOID
rlmDomainSendCmd (
    P_ADAPTER_T     prAdapter,
    BOOLEAN         fgIsOid
    )
{
    P_DOMAIN_INFO_ENTRY     prDomainInfo;
    P_CMD_SET_DOMAIN_INFO_T prCmd;
    WLAN_STATUS             rStatus;
    P_DOMAIN_SUBBAND_INFO   prSubBand;
    UINT_8                  i;	

	char 					strBuf[64] = "";
	char 					tmpStr[16] = "";

    prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
    ASSERT(prDomainInfo);

    prCmd = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_SET_DOMAIN_INFO_T));
    ASSERT(prCmd);

    /*                         */
    if (!prCmd) {
        DBGLOG(RLM, ERROR, ("Domain: no buf to send cmd\n"));
        return;
    }
    kalMemZero(prCmd, sizeof(CMD_SET_DOMAIN_INFO_T));

    prCmd->u2CountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;
    prCmd->u2IsSetPassiveScan = 0;
    prCmd->uc2G4Bandwidth = prAdapter->rWifiVar.rConnSettings.uc2G4BandwidthMode;
    prCmd->uc5GBandwidth = prAdapter->rWifiVar.rConnSettings.uc5GBandwidthMode;
	prCmd->aucReserved[0] = 0;
	prCmd->aucReserved[1] = 0;	

    for (i = 0; i < MAX_SUBBAND_NUM; i++) {
        prSubBand = &prDomainInfo->rSubBand[i];

        prCmd->rSubBand[i].ucRegClass = prSubBand->ucRegClass;
        prCmd->rSubBand[i].ucBand = prSubBand->ucBand;

        if (prSubBand->ucBand != BAND_NULL && prSubBand->ucBand < BAND_NUM) {
            prCmd->rSubBand[i].ucChannelSpan = prSubBand->ucChannelSpan;
            prCmd->rSubBand[i].ucFirstChannelNum = prSubBand->ucFirstChannelNum;
            prCmd->rSubBand[i].ucNumChannels = prSubBand->ucNumChannels;
        }

		if ((prSubBand->ucNumChannels > 0) && (i != MAX_SUBBAND_NUM - 1)){
			if (i != MAX_SUBBAND_NUM - 2)
				sprintf(tmpStr, "%d ~ %d, ", prSubBand->ucFirstChannelNum, prSubBand->ucFirstChannelNum + (prSubBand->ucNumChannels - 1)*prSubBand->ucChannelSpan);
			else
				sprintf(tmpStr, "%d ~ %d", prSubBand->ucFirstChannelNum, prSubBand->ucFirstChannelNum + (prSubBand->ucNumChannels - 1)*prSubBand->ucChannelSpan);

			kalStrCat(strBuf, tmpStr);
		}
    }

	DBGLOG(RLM, INFO, ("SetQueryCmd Support CH: %s\n", strBuf));

    /*                            */
    rStatus = wlanSendSetQueryCmd (
                prAdapter,                  /*           */
                CMD_ID_SET_DOMAIN_INFO,     /*       */
                TRUE,                       /*            */
                FALSE,                      /*            */
                fgIsOid,                    /*         */
                NULL,                       /*                 */
                NULL,                       /*                     */
                sizeof(CMD_SET_DOMAIN_INFO_T),    /*                   */
                (PUINT_8) prCmd,            /*               */
                NULL,                       /*                  */
                0                           /*                     */
                );

    ASSERT(rStatus == WLAN_STATUS_PENDING);

    cnmMemFree(prAdapter, prCmd);

	rlmDomainPassiveScanSendCmd(prAdapter, fgIsOid);
}

VOID rlmDomainPassiveScanSendCmd(    
	P_ADAPTER_T     prAdapter, 
    BOOLEAN         fgIsOid	
)
{
    P_DOMAIN_INFO_ENTRY     prDomainInfo;
	P_CMD_SET_DOMAIN_INFO_T prCmd;
    WLAN_STATUS             rStatus;
    P_DOMAIN_SUBBAND_INFO   prSubBand;
    UINT_8                  i;	

	char 					strBuf[64] = "";
	char 					tmpStr[16] = "";

	prCmd = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_SET_DOMAIN_INFO_T));
   	ASSERT(prCmd);


	/*                         */
	if (!prCmd) {
	   DBGLOG(RLM, ERROR, ("Domain: no buf to send cmd\n"));
	   return;
	}
	kalMemZero(prCmd, sizeof(CMD_SET_DOMAIN_INFO_T));

	prCmd->u2CountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;
	prCmd->u2IsSetPassiveScan = 1;
	prCmd->uc2G4Bandwidth = prAdapter->rWifiVar.rConnSettings.uc2G4BandwidthMode;
	prCmd->uc5GBandwidth = prAdapter->rWifiVar.rConnSettings.uc5GBandwidthMode;
	prCmd->aucReserved[0] = 0;
	prCmd->aucReserved[1] = 0;


	DBGLOG(RLM, INFO, ("CountryCode = %x\n", prAdapter->rWifiVar.rConnSettings.u2CountryCode));

	if (prAdapter->rWifiVar.rConnSettings.u2CountryCode == COUNTRY_CODE_UDF){
		prDomainInfo = &arSupportedRegDomains_Passive[REG_DOMAIN_PASSIVE_UDF_IDX];
	}
	else{
		prDomainInfo = &arSupportedRegDomains_Passive[REG_DOMAIN_PASSIVE_DEF_IDX];
	}

	for (i = 0; i < MAX_SUBBAND_NUM; i++) {
	   prSubBand = &prDomainInfo->rSubBand[i];

	   prCmd->rSubBand[i].ucRegClass = prSubBand->ucRegClass;
	   prCmd->rSubBand[i].ucBand = prSubBand->ucBand;

	   if (prSubBand->ucBand != BAND_NULL && prSubBand->ucBand < BAND_NUM) {
		   prCmd->rSubBand[i].ucChannelSpan = prSubBand->ucChannelSpan;
		   prCmd->rSubBand[i].ucFirstChannelNum = prSubBand->ucFirstChannelNum;
		   prCmd->rSubBand[i].ucNumChannels = prSubBand->ucNumChannels;
	   }

		if ((prSubBand->ucNumChannels > 0) && (i != MAX_SUBBAND_NUM - 1)){
			if (i != MAX_SUBBAND_NUM - 2)
				sprintf(tmpStr, "%d ~ %d, ", prSubBand->ucFirstChannelNum, prSubBand->ucFirstChannelNum + (prSubBand->ucNumChannels - 1)*prSubBand->ucChannelSpan);
			else
				sprintf(tmpStr, "%d ~ %d", prSubBand->ucFirstChannelNum, prSubBand->ucFirstChannelNum + (prSubBand->ucNumChannels - 1)*prSubBand->ucChannelSpan);

			kalStrCat(strBuf, tmpStr);
		}
	}


	DBGLOG(RLM, INFO, ("SetQueryCmd Passive CH: %s\n", strBuf));

	rStatus = wlanSendSetQueryCmd (
				prAdapter,					/*           */
				CMD_ID_SET_DOMAIN_INFO, 	/*       */
				TRUE,						/*            */
				FALSE,						/*            */
				fgIsOid,					/*         */
				NULL,						/*                 */
				NULL,						/*                     */
				sizeof(CMD_SET_DOMAIN_INFO_T),	  /*                   */
				(PUINT_8) prCmd,			/*               */
				NULL,						/*                  */
				0							/*                     */
				);

	ASSERT(rStatus == WLAN_STATUS_PENDING);

	cnmMemFree(prAdapter, prCmd);			
}

/*                                                                            */
/* 
        
 
                
 
                             
                                                             
*/
/*                                                                            */
BOOLEAN
rlmDomainIsLegalChannel (
    P_ADAPTER_T     prAdapter,
    ENUM_BAND_T     eBand,
    UINT_8          ucChannel
    )
{
    UINT_8                  i, j;
    P_DOMAIN_SUBBAND_INFO   prSubband;
    P_DOMAIN_INFO_ENTRY     prDomainInfo;

    prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
    ASSERT(prDomainInfo);

    for (i = 0; i < MAX_SUBBAND_NUM; i++) {
        prSubband = &prDomainInfo->rSubBand[i];

        if (prSubband->ucBand == BAND_5G && !prAdapter->fgEnable5GBand) {
            continue;
        }

        if (prSubband->ucBand == eBand) {
            for (j = 0; j < prSubband->ucNumChannels; j++) {
                if ((prSubband->ucFirstChannelNum + j*prSubband->ucChannelSpan)
                    == ucChannel) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

