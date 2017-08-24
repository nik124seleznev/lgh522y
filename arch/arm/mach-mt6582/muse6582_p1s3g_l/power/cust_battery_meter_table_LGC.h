#ifndef _CUST_BATTERY_METER_TABLE_LGC_H
#define _CUST_BATTERY_METER_TABLE_LGC_H

#define Q_MAX_POS_50_LGC	2936
#define Q_MAX_POS_25_LGC	2933
#define Q_MAX_POS_0_LGC		2693
#define Q_MAX_NEG_10_LGC	2389

#define Q_MAX_POS_50_H_CURRENT_LGC	2916
#define Q_MAX_POS_25_H_CURRENT_LGC	2905
#define Q_MAX_POS_0_H_CURRENT_LGC	2414
#define Q_MAX_NEG_10_H_CURRENT_LGC	1455
/*                                  */

#define BATTERY_PROFILE_SIZE_LGC sizeof(battery_profile_t2_LGC) / sizeof(BATTERY_PROFILE_STRUC);

//        
BATTERY_PROFILE_STRUC battery_profile_t0_LGC[] =
{
		{0	,	4357},	//              
		{2	,	4327},	//               
		{4	,	4301},	//                
		{6	,	4276},	//                
		{8	,	4248},	//                
		{10	,	4216},	//                
		{13	,	4177},	//                
		{15	,	4149},	//                
		{17	,	4127},	//                
		{19	,	4109},	//                
		{21	,	4097},	//                
		{23	,	4080},	//                
		{25	,	4054},	//                
		{27	,	4018},	//                
		{29	,	3989},	//                
		{31	,	3967},	//                
		{33	,	3949},	//                
		{35	,	3933},	//                
		{38	,	3919},	//                
		{40	,	3904},	//                
		{42	,	3890},	//                
		{44	,	3878},	//                 
		{46	,	3866},	//                 
		{48	,	3856},	//                 
		{50	,	3847},	//                 
		{52	,	3837},	//                 
		{54	,	3829},	//                 
		{56	,	3822},	//                 
		{58	,	3814},	//                 
		{60	,	3807},	//                 
		{63	,	3802},	//                 
		{65	,	3798},	//                 
		{67	,	3795},	//                 
		{69	,	3791},	//                 
		{71	,	3786},	//                 
		{73	,	3780},	//                 
		{75	,	3773},	//                 
		{77	,	3765},	//                 
		{79	,	3755},	//                 
		{81	,	3745},	//                 
		{83	,	3734},	//                 
		{85	,	3724},	//                 
		{87	,	3718},	//                 
		{88	,	3715},	//                 
		{89	,	3711},	//                 
		{90	,	3709},	//                 
		{91	,	3707},	//                 
		{92	,	3705},	//                 
		{92	,	3703},	//                 
		{93	,	3702},	//                 
		{94	,	3700},	//                 
		{94	,	3698},	//                 
		{95	,	3697},	//                 
		{95	,	3695},	//                 
		{96	,	3692},	//                 
		{96	,	3689},	//                 
		{97	,	3685},	//                 
		{97	,	3682},	//                 
		{97	,	3677},	//                 
		{98	,	3671},	//                 
		{98	,	3665},	//                 
		{98	,	3659},	//                 
		{99	,	3652},	//                 
		{99	,	3645},	//                 
		{99	,	3638},	//                 
		{99	,	3630},	//                 
		{100	,	3623},	//                 
		{100	,	3617},	//                 
		{100	,	3400},	//                 
};

//       
BATTERY_PROFILE_STRUC battery_profile_t1_LGC[] =
{
		{0	,	4364},	//              
		{2	,	4312},	//               
		{4	,	4281},	//                
		{6	,	4258},	//                
		{7	,	4238},	//                
		{9	,	4217},	//                
		{11	,	4198},	//                
		{13	,	4179},	//                
		{15	,	4161},	//                
		{17	,	4142},	//                
		{19	,	4124},	//                
		{20	,	4106},	//                
		{22	,	4092},	//                
		{24	,	4082},	//                
		{26	,	4065},	//                
		{28	,	4031},	//                
		{30	,	3996},	//                
		{32	,	3974},	//                
		{33	,	3956},	//                
		{35	,	3941},	//                
		{37	,	3927},	//                
		{39	,	3914},	//                 
		{41	,	3901},	//                 
		{43	,	3887},	//                 
		{44	,	3875},	//                 
		{46	,	3864},	//                 
		{48	,	3855},	//                 
		{50	,	3846},	//                 
		{52	,	3837},	//                 
		{54	,	3829},	//                 
		{56	,	3821},	//                 
		{57	,	3814},	//                 
		{59	,	3808},	//                 
		{61	,	3801},	//                 
		{63	,	3796},	//                 
		{65	,	3791},	//                 
		{67	,	3786},	//                 
		{69	,	3784},	//                 
		{70	,	3780},	//                 
		{72	,	3776},	//                 
		{74	,	3771},	//                 
		{76	,	3765},	//                 
		{78	,	3758},	//                 
		{80	,	3747},	//                 
		{81	,	3736},	//                 
		{83	,	3725},	//                 
		{85	,	3714},	//                 
		{87	,	3706},	//                 
		{89	,	3702},	//                 
		{91	,	3698},	//                 
		{93	,	3693},	//                 
		{94	,	3679},	//                 
		{96	,	3633},	//                 
		{97	,	3586},	//                 
		{98	,	3547},	//                 
		{99	,	3512},	//                 
		{99	,	3484},	//                 
		{99	,	3459},	//                 
		{100	,	3440},	//                 
		{100	,	3425},	//                 
		{100	,	3414},	//                 
		{100	,	3405},	//                 
		{100	,	3399},	//                 
		{100	,	3393},	//                 
		{100	,	3388},	//                 
		{100	,	3385},	//                 
		{100	,	3381},	//                 
		{100	,	3381},	//                 
		{100	,	3381},	//                 
};           

//       
BATTERY_PROFILE_STRUC battery_profile_t2_LGC[] =
{
		{0	,	4386},	//              
		{2	,	4364},	//               
		{3	,	4344},	//                
		{5	,	4322},	//                
		{7	,	4302},	//                
		{8	,	4282},	//                
		{10	,	4261},	//                
		{12	,	4241},	//                
		{14	,	4222},	//                
		{15	,	4202},	//                
		{17	,	4183},	//                
		{19	,	4164},	//                
		{20	,	4146},	//                
		{22	,	4128},	//                
		{24	,	4109},	//                
		{25	,	4091},	//                
		{27	,	4078},	//                
		{29	,	4071},	//                
		{31	,	4048},	//                
		{32	,	4015},	//                
		{34	,	3991},	//                
		{36	,	3977},	//                 
		{37	,	3969},	//                 
		{39	,	3961},	//                 
		{41	,	3946},	//                 
		{42	,	3929},	//                 
		{44	,	3910},	//                 
		{46	,	3893},	//                 
		{48	,	3879},	//                 
		{49	,	3866},	//                 
		{51	,	3856},	//                 
		{53	,	3847},	//                 
		{54	,	3837},	//                 
		{56	,	3830},	//                 
		{58	,	3823},	//                 
		{59	,	3815},	//                 
		{61	,	3809},	//                 
		{63	,	3802},	//                 
		{65	,	3798},	//                 
		{66	,	3792},	//                 
		{68	,	3787},	//                 
		{70	,	3784},	//                 
		{71	,	3778},	//                 
		{73	,	3775},	//                 
		{75	,	3769},	//                 
		{76	,	3764},	//                 
		{78	,	3756},	//                 
		{80	,	3749},	//                 
		{82	,	3742},	//                 
		{83	,	3732},	//                 
		{85	,	3720},	//                 
		{87	,	3707},	//                 
		{88	,	3695},	//                 
		{90	,	3692},	//                 
		{92	,	3691},	//                 
		{93	,	3689},	//                 
		{95	,	3681},	//                 
		{97	,	3629},	//                 
		{98	,	3536},	//                 
		{100	,	3383},	//                 
		{100	,	3287},	//                 
		{100	,	3265},	//                 
		{100	,	3258},	//                 
		{100	,	3254},	//                 
		{100	,	3251},	//                 
		{100	,	3251},	//                 
		{100	,	3251},	//                 
		{100	,	3249},	//                 
		{100	,	3249},	//                 
};     

//       
BATTERY_PROFILE_STRUC battery_profile_t3_LGC[] =
{
		{0	,	4390},	//              
		{2	,	4370},	//               
		{3	,	4350},	//                
		{5	,	4331},	//                
		{7	,	4310},	//                
		{9	,	4290},	//                
		{10	,	4270},	//                
		{12	,	4250},	//                
		{14	,	4230},	//                
		{15	,	4210},	//                
		{17	,	4191},	//                
		{19	,	4172},	//                
		{20	,	4152},	//                
		{22	,	4134},	//                
		{24	,	4115},	//                
		{26	,	4097},	//                
		{27	,	4080},	//                
		{29	,	4062},	//                
		{31	,	4046},	//                
		{32	,	4030},	//                
		{34	,	4012},	//                
		{36	,	3998},	//                 
		{37	,	3984},	//                 
		{39	,	3969},	//                 
		{41	,	3955},	//                 
		{43	,	3939},	//                 
		{44	,	3921},	//                 
		{46	,	3901},	//                 
		{48	,	3885},	//                 
		{49	,	3871},	//                 
		{51	,	3860},	//                 
		{53	,	3850},	//                 
		{54	,	3841},	//                 
		{56	,	3833},	//                 
		{58	,	3824},	//                 
		{60	,	3817},	//                 
		{61	,	3810},	//                 
		{63	,	3804},	//                 
		{65	,	3797},	//                 
		{66	,	3792},	//                 
		{68	,	3786},	//                 
		{70	,	3781},	//                 
		{71	,	3773},	//                 
		{73	,	3761},	//                 
		{75	,	3753},	//                 
		{77	,	3745},	//                 
		{78	,	3738},	//                 
		{80	,	3730},	//                 
		{82	,	3723},	//                 
		{83	,	3715},	//                 
		{85	,	3703},	//                 
		{87	,	3689},	//                 
		{88	,	3678},	//                 
		{90	,	3676},	//                 
		{92	,	3674},	//                 
		{94	,	3673},	//                 
		{95	,	3662},	//                 
		{97	,	3609},	//                 
		{99	,	3520},	//                 
		{100	,	3372},	//                 
		{100	,	3263},	//                 
		{100	,	3250},	//                 
		{100	,	3245},	//                 
		{100	,	3243},	//                 
		{100	,	3242},	//                 
		{100	,	3241},	//                 
		{100	,	3241},	//                 
		{100	,	3240},	//                 
		{100	,	3240},	//                 
};           

//                                                                                     
BATTERY_PROFILE_STRUC battery_profile_temperature_LGC[] =
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
};    

#define R_PROFILE_SIZE_LGC sizeof(r_profile_t2_LGC) / sizeof(R_PROFILE_STRUC);
//                                                             
//                              
//                                                             
//        
R_PROFILE_STRUC r_profile_t0_LGC[] =
{
		{795	,	4357},		//              
		{795	,	4327},		//               
		{810	,	4301},		//                
		{820	,	4276},		//                
		{830	,	4248},		//                
		{850	,	4216},		//                
		{893	,	4177},		//                
		{955	,	4149},		//                
		{968	,	4127},		//                
		{980	,	4109},		//                
		{1003	,	4097},		//                
		{1020	,	4080},		//                
		{1013	,	4054},		//                
		{983	,	4018},		//                
		{973	,	3989},		//                
		{975	,	3967},		//                
		{975	,	3949},		//                
		{978	,	3933},		//                
		{980	,	3919},		//                
		{978	,	3904},		//                
		{975	,	3890},		//                
		{975	,	3878},		//                 
		{973	,	3866},		//                 
		{978	,	3856},		//                 
		{985	,	3847},		//                 
		{988	,	3837},		//                 
		{993	,	3829},		//                 
		{1003	,	3822},		//                 
		{1008	,	3814},		//                 
		{1013	,	3807},		//                 
		{1023	,	3802},		//                 
		{1038	,	3798},		//                 
		{1058	,	3795},		//                 
		{1078	,	3791},		//                 
		{1098	,	3786},		//                 
		{1118	,	3780},		//                 
		{1143	,	3773},		//                 
		{1170	,	3765},		//                 
		{1195	,	3755},		//                 
		{1228	,	3745},		//                 
		{1258	,	3734},		//                 
		{1295	,	3724},		//                 
		{1295	,	3718},		//                 
		{1288	,	3715},		//                 
		{1278	,	3711},		//                 
		{1275	,	3709},		//                 
		{1270	,	3707},		//                 
		{1265	,	3705},		//                 
		{1258	,	3703},		//                 
		{1255	,	3702},		//                 
		{1253	,	3700},		//                 
		{1248	,	3698},		//                 
		{1248	,	3697},		//                 
		{1238	,	3695},		//                 
		{1233	,	3692},		//                 
		{1225	,	3689},		//                 
		{1213	,	3685},		//                 
		{1208	,	3682},		//                 
		{1195	,	3677},		//                 
		{1180	,	3671},		//                 
		{1165	,	3665},		//                 
		{1153	,	3659},		//                 
		{1130	,	3652},		//                 
		{1115	,	3645},		//                 
		{1095	,	3638},		//                 
		{1080	,	3630},		//                 
		{1063	,	3623},		//                 
		{1048	,	3617},		//                 
		{505	,	3400},		//                 
};      

//      
R_PROFILE_STRUC r_profile_t1_LGC[] =
{
		{408	,	4364},		//              
		{408	,	4312},		//               
		{443	,	4281},		//                
		{448	,	4258},		//                
		{453	,	4238},		//                
		{453	,	4217},		//                
		{455	,	4198},		//                
		{458	,	4179},		//                
		{465	,	4161},		//                
		{465	,	4142},		//                
		{468	,	4124},		//                
		{470	,	4106},		//                
		{483	,	4092},		//                
		{513	,	4082},		//                
		{523	,	4065},		//                
		{498	,	4031},		//                
		{478	,	3996},		//                
		{475	,	3974},		//                
		{470	,	3956},		//                
		{468	,	3941},		//                
		{463	,	3927},		//                
		{460	,	3914},		//                 
		{453	,	3901},		//                 
		{443	,	3887},		//                 
		{440	,	3875},		//                 
		{443	,	3864},		//                 
		{448	,	3855},		//                 
		{453	,	3846},		//                 
		{458	,	3837},		//                 
		{460	,	3829},		//                 
		{465	,	3821},		//                 
		{470	,	3814},		//                 
		{475	,	3808},		//                 
		{478	,	3801},		//                 
		{485	,	3796},		//                 
		{488	,	3791},		//                 
		{493	,	3786},		//                 
		{503	,	3784},		//                 
		{513	,	3780},		//                 
		{525	,	3776},		//                 
		{538	,	3771},		//                 
		{553	,	3765},		//                 
		{573	,	3758},		//                 
		{585	,	3747},		//                 
		{605	,	3736},		//                 
		{628	,	3725},		//                 
		{653	,	3714},		//                 
		{680	,	3706},		//                 
		{725	,	3702},		//                 
		{785	,	3698},		//                 
		{858	,	3693},		//                 
		{950	,	3679},		//                 
		{1028	,	3633},		//                 
		{970	,	3586},		//                 
		{873	,	3547},		//                 
		{780	,	3512},		//                 
		{713	,	3484},		//                 
		{653	,	3459},		//                 
		{603	,	3440},		//                 
		{565	,	3425},		//                 
		{538	,	3414},		//                 
		{515	,	3405},		//                 
		{500	,	3399},		//                 
		{485	,	3393},		//                 
		{478	,	3388},		//                 
		{468	,	3385},		//                 
		{460	,	3381},		//                 
		{460	,	3381},		//                 
		{460	,	3381},		//                 
};     

//       
R_PROFILE_STRUC r_profile_t2_LGC[] =
{
		{145	,	4386},		//              
		{145	,	4364},		//               
		{145	,	4344},		//                
		{143	,	4322},		//                
		{143	,	4302},		//                
		{148	,	4282},		//                
		{148	,	4261},		//                
		{153	,	4241},		//                
		{153	,	4222},		//                
		{153	,	4202},		//                
		{158	,	4183},		//                
		{160	,	4164},		//                
		{165	,	4146},		//                
		{168	,	4128},		//                
		{168	,	4109},		//                
		{170	,	4091},		//                
		{175	,	4078},		//                
		{190	,	4071},		//                
		{185	,	4048},		//                
		{183	,	4015},		//                
		{185	,	3991},		//                
		{190	,	3977},		//                 
		{198	,	3969},		//                 
		{200	,	3961},		//                 
		{193	,	3946},		//                 
		{185	,	3929},		//                 
		{170	,	3910},		//                 
		{158	,	3893},		//                 
		{155	,	3879},		//                 
		{148	,	3866},		//                 
		{148	,	3856},		//                 
		{150	,	3847},		//                 
		{148	,	3837},		//                 
		{153	,	3830},		//                 
		{153	,	3823},		//                 
		{153	,	3815},		//                 
		{155	,	3809},		//                 
		{155	,	3802},		//                 
		{163	,	3798},		//                 
		{160	,	3792},		//                 
		{163	,	3787},		//                 
		{168	,	3784},		//                 
		{165	,	3778},		//                 
		{165	,	3775},		//                 
		{160	,	3769},		//                 
		{160	,	3764},		//                 
		{153	,	3756},		//                 
		{158	,	3749},		//                 
		{158	,	3742},		//                 
		{160	,	3732},		//                 
		{160	,	3720},		//                 
		{163	,	3707},		//                 
		{155	,	3695},		//                 
		{158	,	3692},		//                 
		{173	,	3691},		//                 
		{188	,	3689},		//                 
		{200	,	3681},		//                 
		{193	,	3629},		//                 
		{208	,	3536},		//                 
		{250	,	3383},		//                 
		{220	,	3287},		//                 
		{168	,	3265},		//                 
		{148	,	3258},		//                 
		{140	,	3254},		//                 
		{133	,	3251},		//                 
		{130	,	3251},		//                 
		{130	,	3251},		//                 
		{128	,	3249},		//                 
		{128	,	3249},		//                 
}; 

//       
R_PROFILE_STRUC r_profile_t3_LGC[] =
{
		{113	,	4390},		//              
		{113	,	4370},		//               
		{110	,	4350},		//                
		{113	,	4331},		//                
		{110	,	4310},		//                
		{113	,	4290},		//                
		{110	,	4270},		//                
		{113	,	4250},		//                
		{115	,	4230},		//                
		{115	,	4210},		//                
		{118	,	4191},		//                
		{120	,	4172},		//                
		{118	,	4152},		//                
		{120	,	4134},		//                
		{118	,	4115},		//                
		{123	,	4097},		//                
		{125	,	4080},		//                
		{125	,	4062},		//                
		{130	,	4046},		//                
		{133	,	4030},		//                
		{135	,	4012},		//                
		{138	,	3998},		//                 
		{140	,	3984},		//                 
		{143	,	3969},		//                 
		{148	,	3955},		//                 
		{148	,	3939},		//                 
		{140	,	3921},		//                 
		{128	,	3901},		//                 
		{120	,	3885},		//                 
		{115	,	3871},		//                 
		{115	,	3860},		//                 
		{115	,	3850},		//                 
		{118	,	3841},		//                 
		{120	,	3833},		//                 
		{118	,	3824},		//                 
		{120	,	3817},		//                 
		{120	,	3810},		//                 
		{125	,	3804},		//                 
		{125	,	3797},		//                 
		{130	,	3792},		//                 
		{130	,	3786},		//                 
		{133	,	3781},		//                 
		{130	,	3773},		//                 
		{118	,	3761},		//                 
		{120	,	3753},		//                 
		{120	,	3745},		//                 
		{120	,	3738},		//                 
		{120	,	3730},		//                 
		{118	,	3723},		//                 
		{120	,	3715},		//                 
		{120	,	3703},		//                 
		{118	,	3689},		//                 
		{115	,	3678},		//                 
		{118	,	3676},		//                 
		{123	,	3674},		//                 
		{130	,	3673},		//                 
		{133	,	3662},		//                 
		{128	,	3609},		//                 
		{140	,	3520},		//                 
		{158	,	3372},		//                 
		{158	,	3263},		//                 
		{125	,	3250},		//                 
		{115	,	3245},		//                 
		{110	,	3243},		//                 
		{108	,	3242},		//                 
		{108	,	3241},		//                 
		{108	,	3241},		//                 
		{105	,	3240},		//                 
		{105	,	3240},		//                 
}; 

//                                                                                     
R_PROFILE_STRUC r_profile_temperature_LGC[] =
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
};    

#endif	//                                       