//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Param_1920x1080_D65_1000lux.h
// @brief: Static params for ISP with 1000lux D65 light source.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Params.h"

BLC_PARAM BLCPARAM_1920x1080_D65_1000Lux = {
	10,
	16
};

LSC_PARAM LSCPARM_1920x1080_D65_1000Lux = {
	//bGain
	{{ 2.934418,   2.6059866,  2.220798,   1.94021428, 1.74472368, 1.600148,   1.50334871, 1.44453084, 1.42655516, 1.45087564, 1.51691735, 1.620021,   1.77389872, 1.984156,   2.26868367, 2.685018,   3.086324   },
	 { 2.780221,   2.42652345, 2.055793,   1.81020319, 1.61290979, 1.47168612, 1.37039435, 1.312223,   1.292875,   1.31676388, 1.381954,   1.48782563, 1.63721418, 1.83661532, 2.09502649, 2.48011971, 2.89426875 },
	 { 2.66630936, 2.266075,   1.92535543, 1.69699764, 1.50311673, 1.35843492, 1.26004076, 1.20359969, 1.1837939,  1.20910418, 1.27213573, 1.37395608, 1.51697993, 1.71529222, 1.95610869, 2.29821563, 2.73281074 },
	 { 2.56463766, 2.158125,   1.849616,   1.61822569, 1.42308247, 1.27978182, 1.18043447, 1.12182248, 1.10311854, 1.12834954, 1.19434536, 1.29403162, 1.43295217, 1.6283437,  1.86529028, 2.177293,   2.59969044 },
	 { 2.49308729, 2.09290552, 1.79948926, 1.56733191, 1.36901522, 1.2242583,  1.12401092, 1.06499517, 1.047911,   1.07009184, 1.13591647, 1.23624086, 1.37226307, 1.568607,   1.802117,   2.0997088,  2.51218    },
	 { 2.44614935, 2.05794668, 1.77400577, 1.53863382, 1.34037089, 1.19284844, 1.09375918, 1.03644383, 1.01813066, 1.04016006, 1.100972,   1.20247412, 1.33787072, 1.534938,   1.77078509, 2.0591023,  2.45250845 },
	 { 2.45007682, 2.05073738, 1.7705425,  1.53213227, 1.33331537, 1.18785942, 1.08808076, 1.0271945,  1.00769913, 1.03327465, 1.09398258, 1.19292164, 1.33314526, 1.52600241, 1.76134288, 2.0468235,  2.434896   },
	 { 2.46845961, 2.07147646, 1.78483438, 1.5487783,  1.35236824, 1.206258,   1.10778213, 1.04479361, 1.02893651, 1.050206,   1.11128664, 1.21239,    1.35215569, 1.54298,    1.78045,    2.06631827, 2.46595645 },
	 { 2.53892612, 2.12474346, 1.82424378, 1.59075677, 1.39276385, 1.24721909, 1.14948976, 1.08533442, 1.06639385, 1.08696747, 1.152703,   1.25387907, 1.39364779, 1.58492756, 1.8212918,  2.1178968,  2.52721024 },
	 { 2.633789,   2.21714544, 1.88529706, 1.64690328, 1.45256436, 1.30626082, 1.208156,   1.1472615,  1.12522137, 1.14996147, 1.21406388, 1.31402481, 1.45510221, 1.6442908,  1.88103974, 2.19772482, 2.63523436 },
	 { 2.75180864, 2.34643865, 1.97836983, 1.73083889, 1.53518808, 1.38939548, 1.289249,   1.22797668, 1.2085588,  1.230665,   1.29428446, 1.39603722, 1.54160321, 1.731839,   1.9690479,  2.323122,   2.762924   },
	 { 2.88964343, 2.5124588,  2.11312318, 1.8474673,  1.64406931, 1.49958229, 1.39923692, 1.33945262, 1.3191148,  1.3403734,  1.40577114, 1.5079881,  1.65210688, 1.84798753, 2.10569763, 2.50678158, 2.92251229 },
	 { 3.00785327, 2.7100594,  2.293494,   1.98375225, 1.78258061, 1.63429224, 1.53504336, 1.476219,   1.45520091, 1.47230053, 1.53835821, 1.63606775, 1.78734374, 1.98851943, 2.287967,   2.707317,   3.12480426 }},

	 //gbGain
	{{ 3.20395136, 2.84868073, 2.42183638, 2.12311363, 1.91179454, 1.756917,   1.64773118, 1.58069026, 1.55754256, 1.5834856,  1.645207,   1.74964738, 1.90877008, 2.1196,     2.41837454, 2.8678813,  3.28876615 },
	 { 3.04708457, 2.663059,   2.24919081, 1.97792578, 1.76208353, 1.60036659, 1.48812246, 1.417937,   1.39421225, 1.41655445, 1.48318923, 1.59559619, 1.75348139, 1.96583772, 2.233202,   2.655679,   3.10571742 },
	 { 2.92302465, 2.48888254, 2.1116817,  1.85055757, 1.63272786, 1.46424925, 1.34551013, 1.27543545, 1.24784422, 1.27263653, 1.34007871, 1.45550752, 1.61366773, 1.82921481, 2.08797812, 2.461877,   2.94127584 },
	 { 2.8214345,  2.3663547,  2.02269816, 1.75983441, 1.53516734, 1.361737,   1.2384063,  1.16158283, 1.13543427, 1.15850842, 1.231841,   1.34799671, 1.50860322, 1.7240684,  1.98546493, 2.32491326, 2.80031347 },
	 { 2.73184371, 2.289873,   1.96527112, 1.69687462, 1.46764481, 1.29081559, 1.161118,   1.08390057, 1.05756307, 1.07895672, 1.15257657, 1.27148235, 1.43292487, 1.64994228, 1.915042,   2.240633,   2.69434977 },
	 { 2.69206333, 2.253345,   1.93331969, 1.66203713, 1.43015265, 1.24909043, 1.12103391, 1.04398239, 1.015522,   1.03709757, 1.10727417, 1.22759187, 1.39046443, 1.61131787, 1.87466943, 2.18963933, 2.630392   },
	 { 2.67528725, 2.245041,   1.92658973, 1.65386665, 1.42180026, 1.2418189,  1.11271071, 1.03221583, 1.00493169, 1.0286479,  1.09747708, 1.216828,   1.38011265, 1.59923065, 1.86305082, 2.17897844, 2.61477852 },
	 { 2.70543,    2.26843548, 1.94370532, 1.6720295,  1.44359267, 1.26507568, 1.13638735, 1.05558419, 1.02966225, 1.05121982, 1.12055671, 1.24107945, 1.40166306, 1.61662066, 1.880561,   2.19829535, 2.64114118 },
	 { 2.77835655, 2.32503271, 1.98681116, 1.71911311, 1.49061728, 1.31596816, 1.1931448,  1.11112535, 1.0833174,  1.1038332,  1.17803967, 1.29450274, 1.45461929, 1.66681635, 1.927938,   2.25338554, 2.71144366 },
	 { 2.87977815, 2.42023182, 2.06020546, 1.78849733, 1.567631,   1.39687121, 1.27794135, 1.2006619,  1.17235661, 1.19521058, 1.26618552, 1.37799537, 1.53730309, 1.74319911, 1.99989152, 2.34575939, 2.824801   },
	 { 3.00670934, 2.570281,   2.167367,   1.89439619, 1.67329276, 1.51058328, 1.39316261, 1.32073271, 1.29580414, 1.31593478, 1.381983,   1.49326217, 1.64755106, 1.85299551, 2.108913,   2.49322033, 2.96424222 },
	 { 3.14779449, 2.760479,   2.327393,   2.03506637, 1.81627846, 1.65615547, 1.54346013, 1.47480285, 1.45027065, 1.46974921, 1.53530085, 1.64169729, 1.79380322, 1.995513,   2.268367,   2.69766188, 3.13380671 },
	 { 3.27151227, 2.97488356, 2.53580165, 2.202857,   1.99046743, 1.83107555, 1.71881962, 1.65477848, 1.630454,   1.65059066, 1.71336091, 1.81254041, 1.96194947, 2.165229,   2.478046,   2.92654252, 3.32641459 }},

	 //grGain
	{{ 3.25572634, 2.89292026, 2.4517138,  2.14141822, 1.91608775, 1.75056577, 1.6297735,  1.55698335, 1.53054082, 1.55629158, 1.62307167, 1.74096286, 1.90679991, 2.12313032, 2.435579,   2.887383,   3.319794   },
	 { 3.1139915,  2.71121383, 2.28292322, 2.001553,   1.773555,   1.60334086, 1.47839546, 1.40285826, 1.37624037, 1.39933944, 1.46882379, 1.58867931, 1.75515807, 1.97776151, 2.256206,   2.685162,   3.150439   },
	 { 2.99251127, 2.539546,   2.15274119, 1.88075721, 1.65062714, 1.47220039, 1.3437233,  1.26727808, 1.2381109,  1.26250851, 1.33401537, 1.4550693,  1.62251461, 1.84627759, 2.113808,   2.500503,   2.99223828 },
	 { 2.88620067, 2.42083025, 2.06689,    1.792972,   1.557844,   1.37428069, 1.24328971, 1.15899968, 1.13094807, 1.155757,   1.23183417, 1.35374939, 1.52196252, 1.74707985, 2.01720285, 2.36469722, 2.85595417 },
	 { 2.805475,   2.34426785, 2.01149154, 1.73086941, 1.49328375, 1.30466914, 1.168742,   1.08562386, 1.05711555, 1.07877731, 1.155649,   1.27911854, 1.45085931, 1.67629755, 1.95046341, 2.28564167, 2.75199223 },
	 { 2.75480866, 2.30931067, 1.97921109, 1.70017242, 1.456584,   1.266083,   1.12949848, 1.047226,   1.016633,   1.03824031, 1.11095607, 1.23864925, 1.40823913, 1.63847983, 1.9110508,  2.23481369, 2.68167949 },
	 { 2.74190378, 2.29949617, 1.97337341, 1.68797028, 1.44639432, 1.257866,   1.11907864, 1.035803,   1.0060153,  1.02904892, 1.10084367, 1.2262038,  1.39805651, 1.6266675,  1.89996767, 2.22219658, 2.66637921 },
	 { 2.76845646, 2.31891775, 1.984939,   1.70612419, 1.46595716, 1.27839935, 1.14174712, 1.05593383, 1.02896273, 1.050837,   1.12268,    1.24922979, 1.41899669, 1.64322937, 1.912983,   2.23781657, 2.69614768 },
	 { 2.8345952,  2.37168837, 2.02467871, 1.74893022, 1.50960279, 1.325559,   1.19417679, 1.1083734,  1.07775366, 1.09952343, 1.1761843,  1.2994734,  1.46681154, 1.68724191, 1.95761359, 2.28763771, 2.762187   },
	 { 2.93678975, 2.462873,   2.09207153, 1.81134331, 1.58088207, 1.3999697,  1.27424335, 1.191744,   1.16132,    1.18497109, 1.25811017, 1.37668681, 1.54437339, 1.75989628, 2.024353,   2.37870121, 2.86460948 },
	 { 3.05432677, 2.60200286, 2.19123387, 1.91062164, 1.68002653, 1.50665259, 1.38154352, 1.303675,   1.27637732, 1.29852414, 1.36698258, 1.4857558,  1.64721251, 1.8632412,  2.12321568, 2.51894569, 3.000998   },
	 { 3.18675113, 2.789452,   2.345823,   2.04380536, 1.81311774, 1.64452136, 1.52401233, 1.44757223, 1.42193472, 1.443355,   1.51186883, 1.62641156, 1.78541982, 1.99573874, 2.27726436, 2.7194345,  3.164886   },
	 { 3.30671382, 2.99426556, 2.54958248, 2.203216,   1.97926247, 1.80847907, 1.69107234, 1.61833107, 1.59140134, 1.61120772, 1.68296742, 1.78529263, 1.94878912, 2.151207,   2.4813168,  2.93416166, 3.34963751 }},

	 //rGain
	 {{ 3.26140237, 2.90829229, 2.44395447, 2.14117551, 1.92580986, 1.76098931, 1.64909816, 1.57704759, 1.554534,   1.58389091, 1.65474868, 1.77599037, 1.95483339, 2.17210364, 2.49666047, 2.99027562, 3.42638421 },
	  { 3.10735345, 2.697388,   2.26802278, 1.99615026, 1.77538717, 1.60802543, 1.48951221, 1.41702485, 1.39288378, 1.420929,   1.49541867, 1.61999846, 1.79255354, 2.01902175, 2.304034,   2.75090647, 3.242971   },
	  { 2.97376251, 2.51484275, 2.12860441, 1.86797869, 1.64624727, 1.47517741, 1.35393524, 1.28068864, 1.25363386, 1.28241861, 1.35708928, 1.47977376, 1.64895618, 1.8743093,  2.14584637, 2.539748,   3.059384   },
	  { 2.858949,   2.38620639, 2.041443,   1.77220476, 1.54693,    1.37411761, 1.25212264, 1.171063,   1.14400363, 1.17096663, 1.25206625, 1.3736707,  1.54114032, 1.765352,   2.03399444, 2.38793254, 2.90239644 },
	  { 2.7713716,  2.310951,   1.97953582, 1.70900917, 1.47973192, 1.30391,    1.17499089, 1.09408915, 1.06599355, 1.08896232, 1.17121,    1.29502475, 1.46141851, 1.687468,   1.960504,   2.29183125, 2.79347658 },
	  { 2.71774817, 2.27686,    1.94907153, 1.6727308,  1.44130123, 1.26184094, 1.13418913, 1.05143952, 1.0188297,  1.04448736, 1.12113214, 1.24961007, 1.41669822, 1.64373708, 1.916427,   2.24438834, 2.72118878 },
	  { 2.72102714, 2.26686883, 1.9444381,  1.66600811, 1.43206453, 1.25416493, 1.12143385, 1.03733242, 1.00448644, 1.03098881, 1.10780609, 1.23636043, 1.40724814, 1.6315949,  1.90451217, 2.22804785, 2.701279   },
	  { 2.73730564, 2.29291,    1.96152222, 1.68367219, 1.45214558, 1.27432334, 1.142153,   1.0562,     1.02521479, 1.05012667, 1.12978125, 1.25924039, 1.426747,   1.65111053, 1.92359591, 2.247887,   2.73132014 },
	  { 2.81994367, 2.34834957, 2.004114,   1.73133981, 1.49727178, 1.32004464, 1.19515443, 1.10757565, 1.07641768, 1.10019207, 1.18332469, 1.308249,   1.47925,    1.69980335, 1.96897411, 2.306974,   2.80438614 },
	  { 2.92639732, 2.44216776, 2.07634854, 1.799391,   1.56975734, 1.39434123, 1.27109933, 1.190723,   1.15844178, 1.18562472, 1.26415074, 1.38814867, 1.5586611,  1.77739751, 2.04431272, 2.4038713,  2.91905713 },
	  { 3.055297,   2.585944,   2.17785,    1.90352452, 1.673042,   1.500913,   1.37676442, 1.29876518, 1.27201283, 1.29613686, 1.3717885,  1.49768281, 1.66706812, 1.887339,   2.15350842, 2.56109357, 3.0679822  },
	  { 3.192101,   2.78453779, 2.33802271, 2.03929257, 1.8125155,  1.64407277, 1.52214587, 1.44450521, 1.41926432, 1.44269049, 1.520358,   1.6435467,  1.81265891, 2.03104234, 2.32003713, 2.78072262, 3.248023   },
	  { 3.322562,   3.01884437, 2.54817438, 2.208989,   1.98511779, 1.81881928, 1.70027828, 1.62241781, 1.5952847,  1.62019539, 1.69814765, 1.817103,   1.98954976, 2.2055006,  2.55155873, 3.02624321, 3.44730616 }},
};

GCC_PARAM GCCPARAM_1920x1080_D65_1000Lux{
	0.3
};

WB_PARM WBPARAM_1920x1080_D65_1000Lux{
	true,
	{ 1.994, 1.0, 2.372 },
	{ 1.378, 1.0, 1.493 }
};

CC_PARAM CCPARAM_1920x1080_D65_1000Lux{
	{{ 1.819,	-0.248,		0.213  },
	 { -1.069,	1.322,		-1.078 },
	 { 0.250,	-0.074,		1.865  }}
};

GAMMA_PARAM GAMMAPARAM_1920x1080_D65_1000Lux{
	{	0, 4, 9, 13, 18, 21, 24, 27,
		30, 33, 36, 39, 42, 45, 48, 51,
		54, 57, 60, 63, 66, 69, 72, 75,
		78, 81, 85, 88, 92, 95, 99, 102,
		106, 109, 112, 115, 118, 121, 125, 129,
		133, 136, 140, 143, 147, 150, 153, 156,
		159, 162, 165, 168, 171, 174, 177, 180,
		183, 186, 189, 192, 195, 198, 201, 204,
		207, 209, 212, 214, 217, 219, 222, 224,
		227, 230, 233, 236, 239, 241, 244, 246,
		249, 251, 254, 256, 259, 261, 264, 266,
		269, 271, 273, 275, 277, 279, 282, 284,
		287, 289, 292, 294, 297, 299, 301, 303,
		305, 307, 309, 311, 313, 315, 317, 319,
		321, 323, 325, 327, 329, 331, 333, 335,
		337, 339, 341, 343, 345, 346, 348, 349,
		351, 352, 354, 355, 357, 359, 361, 363,
		365, 367, 369, 371, 373, 375, 377, 379,
		381, 382, 384, 386, 388, 389, 391, 392,
		394, 395, 397, 398, 400, 401, 403, 404,
		406, 408, 410, 412, 414, 415, 417, 418,
		420, 421, 423, 424, 426, 428, 430, 432,
		434, 435, 437, 438, 440, 441, 442, 443,
		444, 445, 447, 448, 450, 452, 454, 456,
		458, 459, 461, 462, 464, 465, 466, 467,
		468, 469, 471, 472, 474, 475, 477, 478,
		480, 481, 483, 484, 486, 487, 489, 490,
		492, 493, 494, 495, 496, 497, 499, 500,
		502, 503, 505, 506, 508, 509, 510, 511,
		512, 513, 515, 516, 518, 519, 521, 522,
		524, 525, 526, 527, 528, 529, 531, 532,
		534, 535, 536, 537, 538, 539, 540, 541,
		542, 543, 545, 546, 548, 549, 550, 551,
		552, 553, 555, 556, 558, 559, 561, 562,
		564, 565, 566, 567, 568, 569, 570, 571,
		572, 573, 574, 575, 576, 577, 578, 579,
		580, 581, 582, 583, 584, 585, 586, 587,
		588, 589, 590, 591, 592, 593, 595, 596,
		598, 599, 600, 601, 602, 603, 604, 605,
		606, 607, 608, 609, 610, 611, 612, 613,
		614, 615, 616, 617, 618, 619, 620, 621,
		622, 623, 624, 625, 626, 627, 628, 629,
		630, 630, 631, 631, 632, 633, 634, 635,
		636, 637, 638, 639, 640, 641, 642, 643,
		645, 646, 647, 648, 649, 650, 651, 652,
		653, 654, 655, 656, 657, 658, 659, 660,
		661, 662, 663, 664, 665, 666, 667, 668,
		669, 669, 670, 671, 672, 672, 673, 674,
		675, 676, 677, 678, 679, 679, 680, 680,
		681, 682, 683, 684, 685, 685, 686, 687,
		688, 688, 689, 690, 691, 692, 693, 694,
		695, 696, 697, 698, 699, 699, 700, 700,
		701, 701, 702, 703, 704, 704, 705, 706,
		707, 708, 709, 710, 711, 711, 712, 712,
		713, 713, 714, 715, 716, 716, 717, 718,
		719, 720, 721, 722, 723, 723, 724, 724,
		725, 725, 726, 727, 728, 728, 729, 730,
		731, 732, 733, 734, 735, 735, 736, 736,
		737, 738, 739, 740, 741, 741, 742, 743,
		744, 744, 745, 746, 747, 747, 748, 748,
		749, 749, 750, 751, 752, 752, 753, 754,
		755, 755, 756, 756, 757, 757, 758, 759,
		760, 760, 761, 762, 763, 763, 764, 764,
		765, 765, 766, 767, 768, 768, 769, 770,
		771, 771, 772, 772, 773, 773, 774, 775,
		776, 776, 777, 778, 779, 779, 780, 780,
		781, 781, 782, 783, 784, 784, 785, 786,
		787, 787, 788, 788, 789, 789, 790, 791,
		792, 792, 793, 794, 795, 795, 796, 796,
		797, 797, 798, 799, 800, 800, 801, 802,
		803, 803, 804, 804, 805, 805, 806, 807,
		808, 808, 809, 810, 811, 811, 812, 812,
		813, 813, 814, 815, 816, 816, 817, 818,
		819, 819, 820, 820, 821, 821, 822, 822,
		823, 823, 824, 824, 825, 825, 826, 826,
		827, 827, 828, 828, 829, 829, 830, 830,
		831, 831, 832, 832, 833, 833, 834, 835,
		836, 836, 837, 838, 839, 839, 840, 840,
		841, 841, 842, 843, 844, 844, 845, 845,
		846, 846, 847, 847, 848, 848, 849, 850,
		851, 851, 852, 852, 853, 853, 854, 854,
		855, 855, 856, 856, 857, 857, 858, 858,
		859, 859, 860, 860, 861, 861, 862, 863,
		864, 864, 865, 865, 866, 866, 867, 867,
		868, 868, 869, 869, 870, 870, 871, 871,
		872, 872, 873, 873, 874, 874, 875, 875,
		876, 876, 877, 877, 878, 878, 879, 879,
		880, 880, 881, 881, 882, 882, 883, 883,
		884, 884, 885, 885, 886, 886, 887, 887,
		888, 888, 889, 889, 890, 890, 891, 891,
		892, 892, 893, 893, 894, 894, 895, 895,
		896, 896, 897, 897, 898, 898, 899, 899,
		900, 900, 901, 902, 903, 903, 904, 904,
		905, 905, 906, 906, 907, 907, 908, 908,
		909, 909, 910, 910, 911, 911, 912, 912,
		913, 913, 914, 914, 915, 915, 915, 915,
		916, 916, 917, 917, 918, 918, 919, 919,
		920, 920, 921, 921, 922, 922, 922, 922,
		923, 923, 924, 924, 925, 925, 926, 926,
		927, 927, 928, 928, 929, 929, 930, 930,
		931, 931, 931, 931, 932, 932, 933, 933,
		934, 934, 934, 934, 935, 935, 936, 936,
		937, 937, 938, 938, 939, 939, 940, 940,
		941, 941, 942, 942, 943, 943, 944, 944,
		945, 945, 946, 946, 947, 947, 947, 947,
		948, 948, 949, 949, 950, 950, 951, 951,
		952, 952, 953, 953, 954, 954, 954, 954,
		955, 955, 956, 956, 957, 957, 958, 958,
		959, 959, 960, 960, 961, 961, 962, 962,
		963, 963, 963, 963, 964, 964, 965, 965,
		966, 966, 966, 966, 967, 967, 968, 968,
		969, 969, 970, 970, 971, 971, 971, 971,
		972, 972, 973, 973, 974, 974, 974, 974,
		975, 975, 975, 975, 976, 976, 977, 977,
		978, 978, 979, 979, 980, 980, 981, 981,
		982, 982, 982, 982, 983, 983, 984, 984,
		985, 985, 986, 986, 987, 987, 987, 987,
		988, 988, 989, 989, 990, 990, 991, 991,
		992, 992, 993, 993, 994, 994, 994, 994,
		995, 995, 996, 996, 997, 997, 998, 998,
		999, 999, 1000, 1000, 1001, 1001, 1002, 1002,
		1003, 1003,	1003, 1003, 1004, 1004, 1004, 1004,
		1005, 1005,	1006, 1006, 1007, 1007, 1007, 1007,
		1008, 1008,	1009, 1009, 1010, 1010, 1010, 1010,
		1011, 1011,	1012, 1012, 1013, 1013, 1014, 1014,
		1015, 1015,	1015, 1015, 1016, 1016, 1016, 1016,
		1017, 1017, 1018, 1018, 1019, 1019, 1019, 1019,
		1020, 1020, 1021, 1021, 1022, 1022, 1022, 1023 }
};

WNR_PARAM WNRPARAM_1920x1080_D65_1000Lux{
	20,
	20,
	50
};

EE_PARAM EEPARAM_1920x1080_D65_1000Lux{
	1.0,
	5,
	4,
};
