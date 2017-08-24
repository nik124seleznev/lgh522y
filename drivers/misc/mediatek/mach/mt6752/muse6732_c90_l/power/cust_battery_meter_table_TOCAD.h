#ifndef _CUST_BATTERY_METER_TABLE_TOCAD_H
#define _CUST_BATTERY_METER_TABLE_TOCAD_H

#define Q_MAX_POS_50_TOCAD		2317
#define Q_MAX_POS_25_TOCAD		2275
#define Q_MAX_POS_0_TOCAD		1981
#define Q_MAX_NEG_10_TOCAD		1851

#define Q_MAX_POS_50_H_CURRENT_TOCAD	2270
#define Q_MAX_POS_25_H_CURRENT_TOCAD	2196
#define Q_MAX_POS_0_H_CURRENT_TOCAD	1435
#define Q_MAX_NEG_10_H_CURRENT_TOCAD	742

/*                                       */

#define BATTERY_PROFILE_SIZE_TOCAD sizeof(battery_profile_t2_TOCAD) / sizeof(BATTERY_PROFILE_STRUC);
//        
BATTERY_PROFILE_STRUC battery_profile_t0_TOCAD[] =
{
	{0	,	4302},	//              
	{3	,	4260},	//               
	{5	,	4225},	//                
	{8	,	4194},	//                
	{11	,	4164},	//                
	{14	,	4138},	//                
	{16	,	4111},	//                
	{19	,	4086},	//                
	{22	,	4063},	//                
	{25	,	4041},	//                
	{27	,	4019},	//                
	{30	,	3995},	//                
	{33	,	3970},	//                
	{36	,	3937},	//                
	{38	,	3889},	//                
	{41	,	3851},	//                
	{44	,	3823},	//                
	{47	,	3803},	//                
	{49	,	3786},	//                
	{52	,	3772},	//                
	{55	,	3759},	//                 
	{58	,	3746},	//                 
	{60	,	3736},	//                 
	{63	,	3725},	//                 
	{66	,	3712},	//                 
	{69	,	3699},	//                 
	{71	,	3685},	//                 
	{74	,	3668},	//                 
	{77	,	3651},	//                 
	{79	,	3631},	//                 
	{82	,	3611},	//                 
	{85	,	3588},	//                 
	{88	,	3565},	//                 
	{90	,	3542},	//                 
	{93	,	3518},	//                 
	{96	,	3482},	//                 
	{97	,	3463},	//                 
	{98	,	3444},	//                 
	{99	,	3429},	//                 
	{99	,	3416},	//                 
	{100	,	3404},	//                 
	{100	,	3399},	//                 
	{100	,	3391},	//                 
	{100	,	3382},	//                 
	{100	,	3381},	//                 
	{100	,	3373},	//                 
	{100	,	3369},	//                 
	{100	,	3365},	//                 
	{100	,	3363},	//                 
	{100	,	3361},	//                 
	{100	,	3357},	//                 
	{100	,	3357},	//                 
	{100	,	3355},	//                 
	{100	,	3355},	//                 
	{100	,	3354},	//                 
	{100	,	3352},	//                 
	{100	,	3352},	//                 
	{100	,	3350},	//                 
	{100	,	3349},	//                 
};

//      
BATTERY_PROFILE_STRUC battery_profile_t1_TOCAD[] =
{
	{0	,	4325},	//              
	{3	,	4292},	//               
	{5	,	4262},	//                
	{8	,	4236},	//                
	{10	,	4205},	//                
	{13	,	4166},	//                
	{15	,	4115},	//                
	{18	,	4074},	//                
	{20	,	4045},	//                
	{23	,	4021},	//                
	{26	,	3998},	//                
	{28	,	3976},	//                
	{31	,	3955},	//                
	{33	,	3932},	//                
	{36	,	3910},	//                
	{38	,	3887},	//                
	{41	,	3865},	//                
	{44	,	3849},	//                
	{46	,	3833},	//                
	{49	,	3818},	//                
	{51	,	3807},	//                 
	{54	,	3797},	//                 
	{56	,	3787},	//                 
	{59	,	3780},	//                 
	{61	,	3775},	//                 
	{64	,	3766},	//                 
	{67	,	3758},	//                 
	{69	,	3745},	//                 
	{72	,	3732},	//                 
	{74	,	3716},	//                 
	{77	,	3700},	//                 
	{79	,	3680},	//                 
	{82	,	3661},	//                 
	{85	,	3642},	//                 
	{87	,	3624},	//                 
	{90	,	3603},	//                 
	{92	,	3577},	//                 
	{95	,	3540},	//                 
	{97	,	3486},	//                 
	{100	,	3405},	//                 
	{100	,	3309},	//                 
	{100	,	3270},	//                 
	{100	,	3252},	//                 
	{100	,	3236},	//                 
	{100	,	3226},	//                 
	{100	,	3217},	//                 
	{100	,	3208},	//                 
	{100	,	3202},	//                 
	{100	,	3195},	//                 
	{100	,	3192},	//                 
	{100	,	3189},	//                 
	{100	,	3185},	//                 
	{100	,	3183},	//                 
	{100	,	3179},	//                 
	{100	,	3175},	//                 
	{100	,	3172},	//                 
	{100	,	3169},	//                 
	{100	,	3168},	//                 
	{100	,	3165},	//                 
};

//       
BATTERY_PROFILE_STRUC battery_profile_t2_TOCAD[] =
{
	{0	,	4332},	//              
	{2	,	4303},	//               
	{4	,	4276},	//                
	{7	,	4251},	//                
	{9	,	4226},	//                
	{11	,	4203},	//                
	{13	,	4179},	//                
	{16	,	4155},	//                
	{18	,	4132},	//                
	{20	,	4110},	//                
	{22	,	4088},	//                
	{25	,	4067},	//                
	{27	,	4046},	//                
	{29	,	4027},	//                
	{31	,	4008},	//                
	{33	,	3988},	//                
	{36	,	3970},	//                
	{38	,	3948},	//                
	{40	,	3922},	//                
	{42	,	3899},	//                
	{45	,	3880},	//                 
	{47	,	3863},	//                 
	{49	,	3850},	//                 
	{51	,	3839},	//                 
	{54	,	3828},	//                 
	{56	,	3817},	//                 
	{58	,	3808},	//                 
	{60	,	3800},	//                 
	{63	,	3793},	//                 
	{65	,	3786},	//                 
	{67	,	3780},	//                 
	{69	,	3775},	//                 
	{71	,	3769},	//                 
	{74	,	3761},	//                 
	{76	,	3751},	//                 
	{78	,	3741},	//                 
	{80	,	3723},	//                 
	{83	,	3708},	//                 
	{85	,	3690},	//                 
	{87	,	3684},	//                 
	{89	,	3676},	//                 
	{92	,	3664},	//                 
	{94	,	3623},	//                 
	{96	,	3562},	//                 
	{98	,	3481},	//                 
	{100	,	3381},	//                 
	{100	,	3280},	//                 
	{100	,	3148},	//                 
	{100	,	3092},	//                 
	{100	,	3054},	//                 
	{100	,	3026},	//                 
	{100	,	3005},	//                 
	{100	,	2988},	//                 
	{100	,	2976},	//                 
	{100	,	2965},	//                 
	{100	,	2955},	//                 
	{100	,	2948},	//                 
	{100	,	2942},	//                 
	{100	,	2938},	//                 
};

//       
BATTERY_PROFILE_STRUC battery_profile_t3_TOCAD[] =
{
	{0	,	4342},	//              
	{2	,	4313},	//               
	{4	,	4285},	//                
	{7	,	4260},	//                
	{9	,	4236},	//                
	{11	,	4212},	//                
	{13	,	4187},	//                
	{15	,	4165},	//                
	{18	,	4142},	//                
	{20	,	4119},	//                
	{22	,	4097},	//                
	{24	,	4074},	//                
	{26	,	4055},	//                
	{28	,	4034},	//                
	{31	,	4014},	//                
	{33	,	3995},	//                
	{35	,	3978},	//                
	{37	,	3961},	//                
	{39	,	3943},	//                
	{42	,	3920},	//                
	{44	,	3896},	//                 
	{46	,	3875},	//                 
	{48	,	3858},	//                 
	{50	,	3846},	//                 
	{53	,	3834},	//                 
	{55	,	3824},	//                 
	{57	,	3813},	//                 
	{59	,	3806},	//                 
	{61	,	3797},	//                 
	{63	,	3790},	//                 
	{66	,	3784},	//                 
	{68	,	3774},	//                 
	{70	,	3759},	//                 
	{72	,	3751},	//                 
	{74	,	3742},	//                 
	{77	,	3733},	//                 
	{79	,	3723},	//                 
	{81	,	3707},	//                 
	{83	,	3689},	//                 
	{85	,	3678},	//                 
	{88	,	3674},	//                 
	{90	,	3668},	//                 
	{92	,	3650},	//                 
	{94	,	3608},	//                 
	{96	,	3549},	//                 
	{99	,	3466},	//                 
	{100	,	3369},	//                 
	{100	,	3265},	//                 
	{100	,	3094},	//                 
	{100	,	3029},	//                 
	{100	,	2999},	//                 
	{100	,	2981},	//                 
	{100	,	2967},	//                 
	{100	,	2954},	//                 
	{100	,	2947},	//                 
	{100	,	2939},	//                 
	{100	,	2931},	//                 
	{100	,	2928},	//                 
	{100	,	2921},	//                 
};

//                                                                                     
BATTERY_PROFILE_STRUC battery_profile_temperature_TOCAD[] =
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
};

#define R_PROFILE_SIZE_TOCAD sizeof(r_profile_t2_TOCAD) / sizeof(R_PROFILE_STRUC);
//                                                             
//                              
//                                                             
//        
R_PROFILE_STRUC r_profile_t0_TOCAD[] =
{
	{570	,	4302},		//              
	{635	,	4260},		//               
	{640	,	4225},		//                
	{655	,	4194},		//                
	{665	,	4164},		//                
	{658	,	4138},		//                
	{660	,	4111},		//                
	{683	,	4086},		//                
	{680	,	4063},		//                
	{688	,	4041},		//                
	{725	,	4019},		//                
	{755	,	3995},		//                
	{785	,	3970},		//                
	{880	,	3937},		//                
	{1090	,	3889},		//                
	{1200	,	3851},		//                
	{1240	,	3823},		//                
	{1280	,	3803},		//                
	{1265	,	3786},		//                
	{1260	,	3772},		//                
	{1293	,	3759},		//                 
	{1278	,	3746},		//                 
	{1320	,	3736},		//                 
	{1315	,	3725},		//                 
	{1328	,	3712},		//                 
	{1340	,	3699},		//                 
	{1353	,	3685},		//                 
	{1378	,	3668},		//                 
	{1368	,	3651},		//                 
	{1410	,	3631},		//                 
	{1418	,	3611},		//                 
	{1438	,	3588},		//                 
	{1498	,	3565},		//                 
	{1518	,	3542},		//                 
	{1610	,	3518},		//                 
	{1683	,	3482},		//                 
	{1663	,	3463},		//                 
	{1610	,	3444},		//                 
	{1573	,	3429},		//                 
	{1543	,	3416},		//                 
	{1513	,	3404},		//                 
	{1500	,	3399},		//                 
	{1480	,	3391},		//                 
	{1458	,	3382},		//                 
	{1460	,	3381},		//                 
	{1435	,	3373},		//                 
	{1425	,	3369},		//                 
	{1415	,	3365},		//                 
	{1410	,	3363},		//                 
	{1415	,	3361},		//                 
	{1400	,	3357},		//                 
	{1403	,	3357},		//                 
	{1393	,	3355},		//                 
	{1388	,	3355},		//                 
	{1388	,	3354},		//                 
	{1390	,	3352},		//                 
	{1383	,	3352},		//                 
	{1378	,	3350},		//                 
	{1380	,	3349},		//                 
};

//      
R_PROFILE_STRUC r_profile_t1_TOCAD[] =
{
	{363	,	4325},		//              
	{363	,	4292},		//               
	{373	,	4262},		//                
	{383	,	4236},		//                
	{395	,	4205},		//                
	{438	,	4166},		//                
	{535	,	4115},		//                
	{648	,	4074},		//                
	{710	,	4045},		//                
	{723	,	4021},		//                
	{733	,	3998},		//                
	{735	,	3976},		//                
	{753	,	3955},		//                
	{768	,	3932},		//                
	{768	,	3910},		//                
	{750	,	3887},		//                
	{738	,	3865},		//                
	{750	,	3849},		//                
	{760	,	3833},		//                
	{750	,	3818},		//                
	{758	,	3807},		//                 
	{775	,	3797},		//                 
	{768	,	3787},		//                 
	{783	,	3780},		//                 
	{793	,	3775},		//                 
	{795	,	3766},		//                 
	{820	,	3758},		//                 
	{810	,	3745},		//                 
	{815	,	3732},		//                 
	{828	,	3716},		//                 
	{818	,	3700},		//                 
	{828	,	3680},		//                 
	{825	,	3661},		//                 
	{828	,	3642},		//                 
	{855	,	3624},		//                 
	{860	,	3603},		//                 
	{905	,	3577},		//                 
	{920	,	3540},		//                 
	{958	,	3486},		//                 
	{1010	,	3405},		//                 
	{1163	,	3309},		//                 
	{1175	,	3270},		//                 
	{1133	,	3252},		//                 
	{1095	,	3236},		//                 
	{1068	,	3226},		//                 
	{1048	,	3217},		//                 
	{1025	,	3208},		//                 
	{1008	,	3202},		//                 
	{995	,	3195},		//                 
	{983	,	3192},		//                 
	{973	,	3189},		//                 
	{968	,	3185},		//                 
	{960	,	3183},		//                 
	{955	,	3179},		//                 
	{943	,	3175},		//                 
	{933	,	3172},		//                 
	{923	,	3169},		//                 
	{923	,	3168},		//                 
	{918	,	3165},		//                 
};

//       
R_PROFILE_STRUC r_profile_t2_TOCAD[] =
{
	{178	,	4332},		//              
	{178	,	4303},		//               
	{178	,	4276},		//                
	{178	,	4251},		//                
	{180	,	4226},		//                
	{183	,	4203},		//                
	{190	,	4179},		//                
	{190	,	4155},		//                
	{190	,	4132},		//                
	{198	,	4110},		//                
	{205	,	4088},		//                
	{208	,	4067},		//                
	{213	,	4046},		//                
	{218	,	4027},		//                
	{223	,	4008},		//                
	{228	,	3988},		//                
	{230	,	3970},		//                
	{225	,	3948},		//                
	{210	,	3922},		//                
	{190	,	3899},		//                
	{185	,	3880},		//                 
	{178	,	3863},		//                 
	{183	,	3850},		//                 
	{183	,	3839},		//                 
	{185	,	3828},		//                 
	{188	,	3817},		//                 
	{190	,	3808},		//                 
	{198	,	3800},		//                 
	{200	,	3793},		//                 
	{203	,	3786},		//                 
	{208	,	3780},		//                 
	{210	,	3775},		//                 
	{208	,	3769},		//                 
	{208	,	3761},		//                 
	{208	,	3751},		//                 
	{208	,	3741},		//                 
	{205	,	3723},		//                 
	{218	,	3708},		//                 
	{213	,	3690},		//                 
	{218	,	3684},		//                 
	{235	,	3676},		//                 
	{260	,	3664},		//                 
	{290	,	3623},		//                 
	{343	,	3562},		//                 
	{375	,	3481},		//                 
	{433	,	3381},		//                 
	{565	,	3280},		//                 
	{820	,	3148},		//                 
	{738	,	3092},		//                 
	{643	,	3054},		//                 
	{580	,	3026},		//                 
	{520	,	3005},		//                 
	{478	,	2988},		//                 
	{448	,	2976},		//                 
	{433	,	2965},		//                 
	{390	,	2955},		//                 
	{395	,	2948},		//                 
	{380	,	2942},		//                 
	{353	,	2938},		//                 
};

//       
R_PROFILE_STRUC r_profile_t3_TOCAD[] =
{
	{103	,	4342},		//              
	{103	,	4313},		//               
	{103	,	4285},		//                
	{105	,	4260},		//                
	{103	,	4236},		//                
	{108	,	4212},		//                
	{105	,	4187},		//                
	{110	,	4165},		//                
	{108	,	4142},		//                
	{113	,	4119},		//                
	{110	,	4097},		//                
	{110	,	4074},		//                
	{120	,	4055},		//                
	{120	,	4034},		//                
	{118	,	4014},		//                
	{120	,	3995},		//                
	{130	,	3978},		//                
	{140	,	3961},		//                
	{148	,	3943},		//                
	{143	,	3920},		//                
	{125	,	3896},		//                 
	{113	,	3875},		//                 
	{108	,	3858},		//                 
	{108	,	3846},		//                 
	{110	,	3834},		//                 
	{110	,	3824},		//                 
	{108	,	3813},		//                 
	{118	,	3806},		//                 
	{115	,	3797},		//                 
	{120	,	3790},		//                 
	{128	,	3784},		//                 
	{125	,	3774},		//                 
	{110	,	3759},		//                 
	{113	,	3751},		//                 
	{110	,	3742},		//                 
	{113	,	3733},		//                 
	{113	,	3723},		//                 
	{115	,	3707},		//                 
	{118	,	3689},		//                 
	{118	,	3678},		//                 
	{118	,	3674},		//                 
	{123	,	3668},		//                 
	{140	,	3650},		//                 
	{188	,	3608},		//                 
	{208	,	3549},		//                 
	{220	,	3466},		//                 
	{248	,	3369},		//                 
	{288	,	3265},		//                 
	{450	,	3094},		//                 
	{573	,	3029},		//                 
	{505	,	2999},		//                 
	{463	,	2981},		//                 
	{425	,	2967},		//                 
	{408	,	2954},		//                 
	{380	,	2947},		//                 
	{348	,	2939},		//                 
	{333	,	2931},		//                 
	{338	,	2928},		//                 
	{323	,	2921},		//                 
};

//                                                                                     
R_PROFILE_STRUC r_profile_temperature_TOCAD[] =
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
};
#endif /*                                   */