#ifndef _CUST_BATTERY_METER_TABLE_H
#define _CUST_BATTERY_METER_TABLE_H

#include <mach/mt_typedefs.h>

//                                                             
//       
//                                                             
#define BAT_NTC_10 0
#define BAT_NTC_47 0
#define BAT_NTC_68 1

#if (BAT_NTC_10 == 1)
#define RBAT_PULL_UP_R             16900	
#define RBAT_PULL_DOWN_R		   27000	
#endif

#if (BAT_NTC_47 == 1)
#define RBAT_PULL_UP_R             61900	
#define RBAT_PULL_DOWN_R		  100000	
#endif

#if (BAT_NTC_68 == 1)   //            
#define RBAT_PULL_UP_R             62000
#define RBAT_PULL_DOWN_R		  100000	
#endif

#define RBAT_PULL_UP_VOLT          1800



//                                                             
//     
//                                                             

//                                                             
//          
//                                                             

//                                                             
//        
//                                                             
typedef struct _BATTERY_PROFILE_STRUC
{
    kal_int32 percentage;
    kal_int32 voltage;
} BATTERY_PROFILE_STRUC, *BATTERY_PROFILE_STRUC_P;

typedef struct _R_PROFILE_STRUC
{
    kal_int32 resistance; //    
    kal_int32 voltage;
} R_PROFILE_STRUC, *R_PROFILE_STRUC_P;

typedef enum
{
    T1_0C,
    T2_25C,
    T3_50C
} PROFILE_TEMPERATURE;

//                                                             
//                   
//                                                             

//                                                             
//                  
//                                                             

//                                                             
//                             
//                                                             
#if (BAT_NTC_10 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[] = {
        {-20,68237},
        {-15,53650},
        {-10,42506},
        { -5,33892},
        {  0,27219},
        {  5,22021},
        { 10,17926},
        { 15,14674},
        { 20,12081},
        { 25,10000},
        { 30,8315},
        { 35,6948},
        { 40,5834},
        { 45,4917},
        { 50,4161},
        { 55,3535},
        { 60,3014}
    };
#endif

#if (BAT_NTC_47 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[] = {
        {-20,483954},
        {-15,360850},
        {-10,271697},
        { -5,206463},
        {  0,158214},
        {  5,122259},
        { 10,95227},
        { 15,74730},
        { 20,59065},
        { 25,47000},
        { 30,37643},
        { 35,30334},
        { 40,24591},
        { 45,20048},
        { 50,16433},
        { 55,13539},
        { 60,11210}        
    };
#endif

#if (BAT_NTC_68 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[]   = {
		{-20,738931},
		{-15,547471},
		{-10,409600},
		{ -5,309299},
		{  0,235622},
		{  5,181001},
		{ 10,140153},
		{ 15,109349},
		{ 20,85934},
		{ 25,68000},
		{ 30,54165},
		{ 35,43418},
		{ 40,35014},
		{ 45,28400},
		{ 50,23164},
		{ 55,17733},
		{ 60,15656},
		{ 65,12967},
		{ 70,10791},
		{ 75,9021},
		{ 80,7574}
};
#endif

#ifdef CONFIG_LGE_PM_BATTERY_PROFILE
/*                                    */
#define INIT_BATTERY_PROFILE(maker) \
static int Q_MAX_POS_50 = Q_MAX_POS_50_##maker; \
static int Q_MAX_POS_25 = Q_MAX_POS_25_##maker; \
static int Q_MAX_POS_0 = Q_MAX_POS_0_##maker; \
static int Q_MAX_NEG_10 = Q_MAX_NEG_10_##maker; \
static int Q_MAX_POS_50_H_CURRENT = Q_MAX_POS_50_H_CURRENT_##maker; \
static int Q_MAX_POS_25_H_CURRENT = Q_MAX_POS_25_H_CURRENT_##maker; \
static int Q_MAX_POS_0_H_CURRENT = Q_MAX_POS_0_H_CURRENT_##maker; \
static int Q_MAX_NEG_10_H_CURRENT = Q_MAX_NEG_10_H_CURRENT_##maker; \
static int battery_profile_size = BATTERY_PROFILE_SIZE_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t0 = battery_profile_t0_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t1 = battery_profile_t1_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t2 = battery_profile_t2_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t3 = battery_profile_t3_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_temperature = battery_profile_temperature_##maker; \
static int r_profile_size = R_PROFILE_SIZE_##maker; \
static R_PROFILE_STRUC *r_profile_t0 = r_profile_t0_##maker; \
static R_PROFILE_STRUC *r_profile_t1 = r_profile_t1_##maker; \
static R_PROFILE_STRUC *r_profile_t2 = r_profile_t2_##maker; \
static R_PROFILE_STRUC *r_profile_t3 = r_profile_t3_##maker; \
static R_PROFILE_STRUC *r_profile_temperature = r_profile_temperature_##maker;

#define DECLARE_PROFILE(id, maker) \
static int Q_MAX_POS_50_##id = Q_MAX_POS_50_##maker; \
static int Q_MAX_POS_25_##id = Q_MAX_POS_25_##maker; \
static int Q_MAX_POS_0_##id = Q_MAX_POS_0_##maker; \
static int Q_MAX_NEG_10_##id = Q_MAX_NEG_10_##maker; \
static int Q_MAX_POS_50_H_CURRENT_##id = Q_MAX_POS_50_H_CURRENT_##maker; \
static int Q_MAX_POS_25_H_CURRENT_##id = Q_MAX_POS_25_H_CURRENT_##maker; \
static int Q_MAX_POS_0_H_CURRENT_##id = Q_MAX_POS_0_H_CURRENT_##maker; \
static int Q_MAX_NEG_10_H_CURRENT_##id = Q_MAX_NEG_10_H_CURRENT_##maker; \
static int battery_profile_size_##id = BATTERY_PROFILE_SIZE_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t0_##id = battery_profile_t0_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t1_##id = battery_profile_t1_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t2_##id = battery_profile_t2_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_t3_##id = battery_profile_t3_##maker; \
static BATTERY_PROFILE_STRUC *battery_profile_temperature_##id = battery_profile_temperature_##maker; \
static int r_profile_size_##id = R_PROFILE_SIZE_##maker; \
static R_PROFILE_STRUC *r_profile_t0_##id = r_profile_t0_##maker; \
static R_PROFILE_STRUC *r_profile_t1_##id = r_profile_t1_##maker; \
static R_PROFILE_STRUC *r_profile_t2_##id = r_profile_t2_##maker; \
static R_PROFILE_STRUC *r_profile_t3_##id = r_profile_t3_##maker; \
static R_PROFILE_STRUC *r_profile_temperature_##id = r_profile_temperature_##maker;

/*                         */
#define SET_BATTERY_PROFILE(id) \
{ \
	Q_MAX_POS_50 = Q_MAX_POS_50_##id; \
	Q_MAX_POS_25 = Q_MAX_POS_25_##id; \
	Q_MAX_POS_0 = Q_MAX_POS_0_##id; \
	Q_MAX_NEG_10 = Q_MAX_NEG_10_##id; \
	Q_MAX_POS_50_H_CURRENT = Q_MAX_POS_50_H_CURRENT_##id; \
	Q_MAX_POS_25_H_CURRENT = Q_MAX_POS_25_H_CURRENT_##id; \
	Q_MAX_POS_0_H_CURRENT = Q_MAX_POS_0_H_CURRENT_##id; \
	Q_MAX_NEG_10_H_CURRENT = Q_MAX_NEG_10_H_CURRENT_##id; \
	battery_profile_size = battery_profile_size_##id; \
	battery_profile_t0 = battery_profile_t0_##id; \
	battery_profile_t1 = battery_profile_t1_##id; \
	battery_profile_t2 = battery_profile_t2_##id; \
	battery_profile_t3 = battery_profile_t3_##id; \
	battery_profile_temperature = battery_profile_temperature_##id; \
	r_profile_size = r_profile_size_##id; \
	r_profile_t0 = r_profile_t0_##id; \
	r_profile_t1 = r_profile_t1_##id; \
	r_profile_t2 = r_profile_t2_##id; \
	r_profile_t3 = r_profile_t3_##id; \
	r_profile_temperature = r_profile_temperature_##id; \
}

#include <cust_battery_meter_table_LGC.h>
#include <cust_battery_meter_table_TOCAD.h>
#include <cust_battery_meter_table_BYD.h>

/*                         */
INIT_BATTERY_PROFILE(LGC);

/*                    */
DECLARE_PROFILE(DS2704_N, TOCAD);		/*          */
DECLARE_PROFILE(DS2704_L, LGC);
DECLARE_PROFILE(DS2704_C, TOCAD);
DECLARE_PROFILE(ISL6296_N, LGC);		/*          */
DECLARE_PROFILE(ISL6296_L, TOCAD);
DECLARE_PROFILE(ISL6296_C, LGC);
DECLARE_PROFILE(RA4301_VC0, BYD);
DECLARE_PROFILE(RA4301_VC1, BYD);
DECLARE_PROFILE(RA4301_VC2, BYD);		/*          */
DECLARE_PROFILE(SW3800_VC0, BYD);
DECLARE_PROFILE(SW3800_VC1, BYD);
DECLARE_PROFILE(SW3800_VC2, BYD);		/*          */
#else
//        
BATTERY_PROFILE_STRUC battery_profile_t0[] =
{
    {0  ,	4324},
    {2  ,	4301},
    {4  ,	4280},
    {6  ,	4258},
    {8  ,	4235},
    {10 ,	4206},
    {12 ,	4162},
    {14 ,	4128},
    {16 ,	4104},
    {18 ,	4084},
    {20 ,	4068},
    {22 ,	4049},
    {24 ,	4031},
    {26 ,	4012},
    {28 ,	3994},
    {30 ,	3978},
    {32 ,	3961},
    {34 ,	3944},
    {36 ,	3926},
    {38 ,	3909},
    {40 ,	3895},
    {42 ,	3882},
    {44 ,	3870},
    {46 ,	3860},
    {48 ,	3852},
    {50 ,	3843},
    {52 ,	3835},
    {54 ,	3828},
    {56 ,	3821},
    {58 ,	3816},
    {60 ,	3809},
    {62 ,	3803},
    {64 ,	3799},
    {66 ,	3792},
    {68 ,	3787},
    {70 ,	3781},
    {72 ,	3776},
    {74 ,	3769},
    {76 ,	3762},
    {78 ,	3755},
    {81 ,	3744},
    {83 ,	3733},
    {85 ,	3723},
    {87 ,	3714},
    {89 ,	3707},
    {91 ,	3704},
    {93 ,	3698},
    {94 ,	3694},
    {95 ,	3688},
    {96 ,	3683},
    {96 ,	3676},
    {97 ,	3669},
    {98 ,	3660},
    {98 ,	3654},
    {98 ,	3647},
    {98 ,	3640},
    {99 ,	3635},
    {99 ,	3628},
    {99 ,	3623},
    {99 ,	3618},
    {99 ,	3614},
    {99 ,	3610},
    {99 ,	3608},
    {100    ,	3605},
    {100    ,	3601},
    {100    ,	3598},
    {100    ,	3597},
    {100    ,	3595},
    {100    ,	3592},
    {100    ,	3590},
    {100    ,	3588},
    {100    ,	3587},
    {100    ,	3585},
    {100    ,	3582},
    {100    ,	3400}
};      
        
//       
BATTERY_PROFILE_STRUC battery_profile_t1[] =
{
    {0  ,	4312},
    {2  ,	4281},
    {4  ,	4258},
    {6  ,	4237},
    {7  ,	4217},
    {9  ,	4198},
    {11 ,	4179},
    {13 ,	4161},
    {15 ,	4142},
    {17 ,	4124},
    {19 ,	4107},
    {21 ,	4089},
    {23 ,	4074},
    {24 ,	4058},
    {26 ,	4039},
    {28 ,	4020},
    {30 ,	4001},
    {32 ,	3982},
    {34 ,	3962},
    {36 ,	3945},
    {38 ,	3927},
    {39 ,	3913},
    {41 ,	3899},
    {43 ,	3887},
    {45 ,	3877},
    {47 ,	3867},
    {49 ,	3857},
    {51 ,	3849},
    {53 ,	3840},
    {55 ,	3833},
    {56 ,	3826},
    {58 ,	3819},
    {60 ,	3813},
    {62 ,	3807},
    {64 ,	3801},
    {66 ,	3796},
    {68 ,	3791},
    {70 ,	3786},
    {71 ,	3779},
    {73 ,	3773},
    {75 ,	3766},
    {77 ,	3758},
    {79 ,	3748},
    {81 ,	3737},
    {83 ,	3724},
    {85 ,	3712},
    {86 ,	3705},
    {88 ,	3701},
    {90 ,	3698},
    {92 ,	3695},
    {94 ,	3690},
    {96 ,	3661},
    {98 ,	3582},
    {99 ,	3461},
    {100    ,	3427},
    {100    ,	3409},
    {100    ,	3400},
    {100    ,	3392},
    {100    ,	3389},
    {100    ,	3385},
    {100    ,	3382},
    {100    ,	3380},
    {100    ,	3378},
    {100    ,	3377},
    {100    ,	3376},
    {100    ,	3373},
    {100    ,	3372},
    {100    ,	3371},
    {100    ,	3368},
    {100    ,	3367},
    {100    ,	3367},
    {100    ,	3363},
    {100    ,	3363},
    {100    ,	3361},
    {100    ,	3361}
};           

//       
BATTERY_PROFILE_STRUC battery_profile_t2[] =
{
    {0  ,	4335},
    {2  ,	4309},
    {3  ,	4286},
    {5  ,	4267},
    {6  ,	4246},
    {8  ,	4227},
    {10 ,	4208},
    {11 ,	4189},
    {13 ,	4171},
    {15 ,	4152},
    {16 ,	4135},
    {18 ,	4117},
    {20 ,	4100},
    {21 ,	4083},
    {23 ,	4067},
    {24 ,	4051},
    {26 ,	4036},
    {28 ,	4020},
    {29 ,	4006},
    {31 ,	3992},
    {33 ,	3980},
    {34 ,	3967},
    {36 ,	3954},
    {37 ,	3942},
    {39 ,	3930},
    {41 ,	3917},
    {42 ,	3901},
    {44 ,	3884},
    {46 ,	3868},
    {47 ,	3855},
    {49 ,	3844},
    {50 ,	3835},
    {52 ,	3827},
    {54 ,	3820},
    {55 ,	3813},
    {57 ,	3807},
    {59 ,	3801},
    {60 ,	3796},
    {62 ,	3791},
    {64 ,	3787},
    {65 ,	3783},
    {67 ,	3779},
    {68 ,	3777},
    {70 ,	3774},
    {72 ,	3772},
    {73 ,	3769},
    {75 ,	3768},
    {77 ,	3764},
    {78 ,	3759},
    {80 ,	3750},
    {81 ,	3744},
    {83 ,	3735},
    {85 ,	3721},
    {86 ,	3709},
    {88 ,	3692},
    {90 ,	3687},
    {91 ,	3685},
    {93 ,	3681},
    {94 ,	3676},
    {96 ,	3659},
    {98 ,	3589},
    {99 ,	3469},
    {100    ,	3362},
    {100    ,	3329},
    {100    ,	3312},
    {100    ,	3302},
    {100    ,	3298},
    {100    ,	3293},
    {100    ,	3290},
    {100    ,	3287},
    {100    ,	3284},
    {100    ,	3283},
    {100    ,	3280},
    {100    ,	3279},
    {100    ,	3279}
};     

//       
BATTERY_PROFILE_STRUC battery_profile_t3[] =
{
    {0  ,	4342},
    {2  ,	4316},
    {3  ,	4294},
    {5  ,	4273},
    {6  ,	4254},
    {8  ,	4233},
    {10 ,	4213},
    {11 ,	4194},
    {13 ,	4176},
    {15 ,	4157},
    {16 ,	4139},
    {18 ,	4121},
    {19 ,	4103},
    {21 ,	4087},
    {23 ,	4070},
    {24 ,	4054},
    {26 ,	4038},
    {28 ,	4023},
    {29 ,	4007},
    {31 ,	3994},
    {32 ,	3980},
    {34 ,	3967},
    {36 ,	3954},
    {37 ,	3942},
    {39 ,	3930},
    {41 ,	3919},
    {42 ,	3907},
    {44 ,	3895},
    {45 ,	3880},
    {47 ,	3862},
    {49 ,	3848},
    {50 ,	3837},
    {52 ,	3828},
    {54 ,	3820},
    {55 ,	3813},
    {57 ,	3806},
    {58 ,	3800},
    {60 ,	3795},
    {62 ,	3789},
    {63 ,	3784},
    {65 ,	3781},
    {67 ,	3776},
    {68 ,	3773},
    {70 ,	3770},
    {71 ,	3766},
    {73 ,	3759},
    {75 ,	3749},
    {76 ,	3745},
    {78 ,	3739},
    {80 ,	3733},
    {81 ,	3726},
    {83 ,	3719},
    {84 ,	3709},
    {86 ,	3695},
    {88 ,	3680},
    {89 ,	3669},
    {91 ,	3667},
    {93 ,	3663},
    {94 ,	3659},
    {96 ,	3648},
    {97 ,	3598},
    {100    ,	3500},
    {100    ,	3330},
    {100    ,	3270},
    {100    ,	3260},
    {100    ,	3256},
    {100    ,	3254},
    {100    ,	3253},
    {100    ,	3252},
    {100    ,	3251},
    {100    ,	3251},
    {100    ,	3251},
    {100    ,	3250},
    {100    ,	3248},
    {100    ,	3248}
};           

//                                                                                     
BATTERY_PROFILE_STRUC battery_profile_temperature[] =
{
  {0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }
};    

//                                                             
//                              
//                                                             
//        
R_PROFILE_STRUC r_profile_t0[] =
{
    {275    ,	4324},
    {535    ,	4301},
    {560    ,	4280},
    {575    ,	4258},
    {593    ,	4235},
    {613    ,	4206},
    {673    ,	4162},
    {780    ,	4128},
    {835    ,	4104},
    {860    ,	4084},
    {878    ,	4068},
    {885    ,	4049},
    {903    ,	4031},
    {913    ,	4012},
    {923    ,	3994},
    {928    ,	3978},
    {925    ,	3961},
    {923    ,	3944},
    {918    ,	3926},
    {908    ,	3909},
    {910    ,	3895},
    {913    ,	3882},
    {918    ,	3870},
    {918    ,	3860},
    {930    ,	3852},
    {938    ,	3843},
    {945    ,	3835},
    {953    ,	3828},
    {963    ,	3821},
    {973    ,	3816},
    {980    ,	3809},
    {988    ,	3803},
    {1003   ,	3799},
    {1005   ,	3792},
    {1015   ,	3787},
    {1023   ,	3781},
    {1033   ,	3776},
    {1040   ,	3769},
    {1050   ,	3762},
    {1065   ,	3755},
    {1073   ,	3744},
    {1078   ,	3733},
    {1090   ,	3723},
    {1103   ,	3714},
    {1123   ,	3707},
    {1160   ,	3704},
    {1213   ,	3698},
    {1235   ,	3694},
    {1220   ,	3688},
    {1210   ,	3683},
    {1190   ,	3676},
    {1175   ,	3669},
    {1155   ,	3660},
    {1138   ,	3654},
    {1118   ,	3647},
    {1100   ,	3640},
    {1088   ,	3635},
    {1075   ,	3628},
    {1060   ,	3623},
    {1048   ,	3618},
    {1040   ,	3614},
    {1028   ,	3610},
    {1023   ,	3608},
    {1018   ,	3605},
    {1005   ,	3601},
    {995    ,	3598},
    {995    ,	3597},
    {988    ,	3595},
    {990    ,	3592},
    {975    ,	3590},
    {980    ,	3588},
    {973    ,	3587},
    {975    ,	3585},
    {965    ,	3582},
    {508    ,	3400}
};      

//      
R_PROFILE_STRUC r_profile_t1[] =
{
    {525    ,	4312},
    {370    ,	4281},
    {378    ,	4258},
    {383    ,	4237},
    {390    ,	4217},
    {393    ,	4198},
    {400    ,	4179},
    {410    ,	4161},
    {413    ,	4142},
    {420    ,	4124},
    {428    ,	4107},
    {433    ,	4089},
    {445    ,	4074},
    {450    ,	4058},
    {450    ,	4039},
    {450    ,	4020},
    {448    ,	4001},
    {443    ,	3982},
    {428    ,	3962},
    {420    ,	3945},
    {408    ,	3927},
    {408    ,	3913},
    {403    ,	3899},
    {403    ,	3887},
    {405    ,	3877},
    {410    ,	3867},
    {408    ,	3857},
    {413    ,	3849},
    {415    ,	3840},
    {420    ,	3833},
    {423    ,	3826},
    {428    ,	3819},
    {433    ,	3813},
    {438    ,	3807},
    {440    ,	3801},
    {445    ,	3796},
    {453    ,	3791},
    {458    ,	3786},
    {458    ,	3779},
    {463    ,	3773},
    {465    ,	3766},
    {465    ,	3758},
    {465    ,	3748},
    {470    ,	3737},
    {473    ,	3724},
    {470    ,	3712},
    {470    ,	3705},
    {480    ,	3701},
    {490    ,	3698},
    {515    ,	3695},
    {565    ,	3690},
    {608    ,	3661},
    {620    ,	3582},
    {653    ,	3461},
    {573    ,	3427},
    {528    ,	3409},
    {500    ,	3400},
    {485    ,	3392},
    {475    ,	3389},
    {470    ,	3385},
    {458    ,	3382},
    {453    ,	3380},
    {453    ,	3378},
    {448    ,	3377},
    {453    ,	3376},
    {453    ,	3373},
    {448    ,	3372},
    {450    ,	3371},
    {453    ,	3368},
    {448    ,	3367},
    {450    ,	3367},
    {448    ,	3363},
    {450    ,	3363},
    {450    ,	3361},
    {413    ,	3361}
};     

//       
R_PROFILE_STRUC r_profile_t2[] =
{
    {233    ,	4335},
    {173    ,	4309},
    {170    ,	4286},
    {173    ,	4267},
    {173    ,	4246},
    {175    ,	4227},
    {178    ,	4208},
    {178    ,	4189},
    {180    ,	4171},
    {180    ,	4152},
    {185    ,	4135},
    {185    ,	4117},
    {190    ,	4100},
    {193    ,	4083},
    {198    ,	4067},
    {200    ,	4051},
    {205    ,	4036},
    {210    ,	4020},
    {218    ,	4006},
    {220    ,	3992},
    {230    ,	3980},
    {233    ,	3967},
    {235    ,	3954},
    {238    ,	3942},
    {240    ,	3930},
    {238    ,	3917},
    {225    ,	3901},
    {210    ,	3884},
    {195    ,	3868},
    {188    ,	3855},
    {180    ,	3844},
    {178    ,	3835},
    {178    ,	3827},
    {180    ,	3820},
    {180    ,	3813},
    {180    ,	3807},
    {180    ,	3801},
    {180    ,	3796},
    {183    ,	3791},
    {183    ,	3787},
    {185    ,	3783},
    {185    ,	3779},
    {190    ,	3777},
    {190    ,	3774},
    {190    ,	3772},
    {188    ,	3769},
    {195    ,	3768},
    {193    ,	3764},
    {190    ,	3759},
    {183    ,	3750},
    {188    ,	3744},
    {185    ,	3735},
    {185    ,	3721},
    {188    ,	3709},
    {185    ,	3692},
    {183    ,	3687},
    {188    ,	3685},
    {193    ,	3681},
    {208    ,	3676},
    {228    ,	3659},
    {250    ,	3589},
    {328    ,	3469},
    {405    ,	3362},
    {328    ,	3329},
    {285    ,	3312},
    {263    ,	3302},
    {250    ,	3298},
    {240    ,	3293},
    {228    ,	3290},
    {220    ,	3287},
    {220    ,	3284},
    {210    ,	3283},
    {205    ,	3280},
    {210    ,	3279},
    {203    ,	3279}
}; 

//       
R_PROFILE_STRUC r_profile_t3[] =
{
    {105    ,	4342},
    {135    ,	4316},
    {138    ,	4294},
    {138    ,	4273},
    {143    ,	4254},
    {140    ,	4233},
    {140    ,	4213},
    {138    ,	4194},
    {143    ,	4176},
    {143    ,	4157},
    {143    ,	4139},
    {145    ,	4121},
    {145    ,	4103},
    {150    ,	4087},
    {148    ,	4070},
    {150    ,	4054},
    {150    ,	4038},
    {155    ,	4023},
    {153    ,	4007},
    {158    ,	3994},
    {158    ,	3980},
    {163    ,	3967},
    {163    ,	3954},
    {170    ,	3942},
    {175    ,	3930},
    {180    ,	3919},
    {185    ,	3907},
    {188    ,	3895},
    {180    ,	3880},
    {163    ,	3862},
    {153    ,	3848},
    {148    ,	3837},
    {148    ,	3828},
    {145    ,	3820},
    {148    ,	3813},
    {145    ,	3806},
    {148    ,	3800},
    {150    ,	3795},
    {148    ,	3789},
    {148    ,	3784},
    {153    ,	3781},
    {153    ,	3776},
    {155    ,	3773},
    {160    ,	3770},
    {160    ,	3766},
    {153    ,	3759},
    {145    ,	3749},
    {148    ,	3745},
    {145    ,	3739},
    {145    ,	3733},
    {148    ,	3726},
    {143    ,	3719},
    {145    ,	3709},
    {145    ,	3695},
    {145    ,	3680},
    {143    ,	3669},
    {145    ,	3667},
    {148    ,	3663},
    {158    ,	3659},
    {160    ,	3648},
    {168    ,	3598},
    {178    ,	3500},
    {205    ,	3330},
    {178    ,	3270},
    {153    ,	3260},
    {140    ,	3256},
    {140    ,	3254},
    {140    ,	3253},
    {138    ,	3252},
    {130    ,	3251},
    {138    ,	3251},
    {140    ,	3251},
    {138    ,	3250},
    {135    ,	3248},
    {135    ,	3248}       
}; 

//                                                                                     
R_PROFILE_STRUC r_profile_temperature[] =
{
  {0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }
};
#endif /*                               */

//                                                             
//                   
//                                                             
int fgauge_get_saddles(void);
BATTERY_PROFILE_STRUC_P fgauge_get_profile(kal_uint32 temperature);

int fgauge_get_saddles_r_table(void);
R_PROFILE_STRUC_P fgauge_get_profile_r_table(kal_uint32 temperature);

#endif	//                                   
