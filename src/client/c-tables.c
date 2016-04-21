/* File: tables.c */

/* Purpose: Angband Tables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * Only a little of the file is used for the client, the rest is commented out.
 */

#include "angband.h"



/*
 * Global array for looping through the "keypad directions"
 */
s16b ddd[9] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
 * Global arrays for converting "keypad direction" into offsets
 */
s16b ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

s16b ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
 * Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]"
 */
s16b ddx_ddd[9] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

s16b ddy_ddd[9] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };


/*
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
char hexsym[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};



/*
 * Store owners (exactly four "possible" owners per store, chosen randomly)
 * { name, purse, max greed, min greed, haggle_per, tolerance, race, unused }
 */

/* Store price maxes (purse) have now been increased by 5x from Angband 3.0.6 */
owner_type owners[MAX_STORES][MAX_OWNERS] =
{
	{
		/* General store */
		{ "Bilbo the Friendly",		5000*5,		170, 108,  5, 15, RACE_HOBBIT},
		{ "Rincewind the Chicken",	10000*5,	175, 108,  4, 12, RACE_HUMAN},
		{ "Snafu the Midget",		20000*5,	170, 107,  5, 15, RACE_GNOME},
		{ "Lyar-el the Comely",		30000*5,	165, 107,  6, 18, RACE_ELF},
	},
	{
		/* Armoury */
		{ "Kon-Dar the Ugly",		5000*5,		210, 115,  5,  7, RACE_HALF_ORC},
		{ "Darg-Low the Grim",		10000*5,	190, 111,  4,  9, RACE_HUMAN},
		{ "Decado the Handsome",	25000*5,  	200, 112,  4, 10, RACE_DUNADAN},
		{ "Mauglin the Grumpy",		30000*5,	200, 112,  4,  5, RACE_DWARF},
	},
	{
		/* Weapon Smith */
		{ "Ithyl-Mak the Beastly",	5000*5,		210, 115,  6,  6, RACE_HALF_TROLL},
		{ "Arndal Beast-Slayer",	10000*5,	185, 110,  5,  9, RACE_HALF_ELF},
		{ "Tarl Beast-Master",		25000*5,	190, 115,  5,  7, RACE_HOBBIT},
		{ "Oglign Dragon-Slayer",	30000*5,	195, 112,  4,  8, RACE_DWARF},
	},
	{
		/* Temple */
		{ "Ludwig the Humble",		15000*5,	175, 109,  6, 15, RACE_HUMAN},
		{ "Gunnar the Paladin",		20000*5,	185, 110,  5, 23, RACE_HUMAN},
		{ "Delilah the Pure",		25000*5,	180, 107,  6, 20, RACE_ELF},
		{ "Keldon the Wise",		30000*5,	185, 109,  5, 15, RACE_DWARF},
	},
	{
		/* Alchemist */
		{ "Mauser the Chemist",		10000*5,	190, 111,  5,  8, RACE_HALF_ELF},
		{ "Wizzle the Chaotic",		10000*5,	190, 110,  6,  8, RACE_HOBBIT},
		{ "Ga-nat the Greedy",		15000*5,	200, 116,  6,  9, RACE_GNOME},
		{ "Vella the Slender",		15000*5,	220, 111,  4,  9, RACE_HUMAN},
	},
	{
		/* Magic Shop */
		{ "Ariel the Sorceress",	15000*5,	200, 110,  7,  8, RACE_HALF_ELF},
		{ "Buggerby the Great",		20000*5,	215, 113,  6, 10, RACE_GNOME},
		{ "Inglorian the Mage",		25000*5,	200, 110,  7, 10, RACE_HUMAN},
		{ "Luthien Starshine",		30000*5,	175, 110,  5, 11, RACE_HIGH_ELF},
	},
	{
		/* Black Market */
		{ "Lo-Hak the Awful",		15000*5,	250, 150, 10,  5, RACE_HALF_TROLL},
		{ "Histor the Goblin",		20000*5,	250, 150, 10,  5, RACE_HALF_ORC},
		{ "Durwin the Shifty",		25000*5,	250, 150, 10,  5, RACE_HALF_ORC},
		{ "Drago the Fair",			30000*5,	250, 150, 10,  5, RACE_ELF},
	},
	{
		/* Home */
		{ "Your home",				0,      100, 100,  0, 99, 99},
		{ "Your home",				0,      100, 100,  0, 99, 99},
		{ "Your home",				0,      100, 100,  0, 99, 99},
		{ "Your home",				0,      100, 100,  0, 99, 99}
	}
};

const char *p_name = "Human\0Half-Elf\0Elf\0Hobbit\0Gnome\0Dwarf\0Half-Orc\0Half-Troll\0Dunadan\0High-Elf\0Kobold\0";

/*
 * Player Race Information:
 *    Title,
 *    {STR,INT,WIS,DEX,CON,CHR},
 *    r_dis, r_dev, r_sav, r_stl, r_srh, r_fos, r_thn, r_thb,
 *    hitdie, exp base,
 *    Age (Base, Mod),
 *    Male (Hgt, Wgt),
 *    Female (Hgt, Wgt)
 *    infra,
 *    class-choices
 */
player_race race_info[MAX_RACES] =
{
	{
		0, 0, /* "Human\0", 0-5 */
		{  0,  0,  0,  0,  0,  0 },
		0,  0,  0,  0,  0,  10,  0,  0,
		10,  100,
		14,  6,
		72,  6, 180, 25,
		66,  4, 150, 20,
		0, 
		0x3F,
		1,
		0,
		0,
		0
	},

	{
		6, 0, /* "Half-Elf\0", 6-14 */
		{ -1,  1,  0,  1, -1,  1 },
		2,  3,  3,  1, 6,  11, -1,  5,
		9,  110,
		24, 16,
		66,  6, 130, 15,
		62,  6, 100, 10,
		2,
		0x3F,
		4,
		0,
		0x00000008, /* SUST_DEX (bit 3) */
		0
	},

	{
		15, 0, /* "Elf\0", 15-18 */
		{ -1,  2,  1,  1, -2,  1 },
		5,  6,  6,  1, 8,  12, -5, 15,
		8,  120,
		75, 75,
		60,  4, 100,  6,
		54,  4, 80,  6,
		5,
		0x1F,
		0,
		0x00200004, /* SUST_DEX (bit 3) | RES_LITE (bit 22) */
		0,
		0
	},

	{
		19, 0, /* "Hobbit\0", 19-25 */
		{ -2,  2,  1,  3,  2,  1 },
		15, 18, 18, 4, 12,  15, -10, 20,
		7,  110,
		21, 12,
		36,  3, 60,  3,
		33,  3, 50,  3,
		4,
		0x0B,
		10,
		0,
		0,
		0x00000080 /* HOLD_LIFE (bit 7) */

	},

	{
		26, 0, /* "Gnome\0", 26-31 */
		{ -1,  2,  0,  2,  1, -2 },
		10, 12, 12,  3, 6,  13, -8, 12,
		8,  125,
		50, 40,
		42,  3, 90,  6,
		39,  3, 75,  3,
		4,
		0x0F,
		13,
		0,
		0,
		0x00000040 /* FREE_ACT (bit 6) */
	},

	{
		32, 0, /* "Dwarf\0", 32-37 */
		{  2, -3,  2, -2,  2, -3 },
		2,  9,  9,  -1,  7,  10, 15,  0,
		11,  120,
		35, 15,
		48,  3, 150, 10,
		46,  3, 120, 10,
		5,
		0x05,
		16,
		0,
		0x01000000, /* RES_BLIND (bit 24) */
		0
	},

	{
		38, 0, /* "Half-Orc\0", 38-46 */
		{  2, -1,  0,  0,  1, -4 },
		-3, -3, -3,  -1,  0, 7, 12, -5,
		10,  110,
		11,  4,
		66,  1, 150,  5,
		62,  1, 120,  5,
		3,
		0x0D,
		19,
		0,
		0x00800000, /* RES_DARK (bit 23) */
		0
	},

	{
		47, 0, /* "Half-Troll\0", 47-57 */
		{ 4, -4, -2, -4,  3, -6 },
		-5, -8, -8, -2,  -1, 5, 20, -10,
		12,  120,
		20, 10,
		96, 10, 250, 50,
		84,  8, 225, 40,
		3,
		0x05,
		21,
		0,
		0x00000001, /* SUS_STR (bit 0) */
		0x00000008  /* REGEN (bit 3) */
	},

	{
		58, 0, /* "Dunadan\0", 58-65 */
		{  1,  2,  2,  2,  3,  2 },
		4,  5,  5,  2, 3, 13, 15, 10,
		10,  180,
		50, 20,
		82, 5, 190, 20,
		78,  6, 180, 15,
		0,
		0x3F,
		1,
		0,
		0x00000008, /* SUS_CON (bit 3) */
		0
	},

	{
		66, 0, /* "High-Elf\0", 66-74 */
		{  1,  3, -1,  3,  1,  5 },
		4,  20, 20,  3,  3, 14, 10, 25,
		10,  200,
		100, 30,
		90, 10, 190, 20,
		82, 10, 180, 15,
		4,
		0x1F,
		7,
		0,
		0x00400000, /* RES_LITE (bit 22) */
		0x00000020  /* SEE_INVIS (bit 5) */
    },

	{
		75, 0, /* "Kobold\0", 75-81 */
		{  -1,  -1, 0,  2,  2,  -2 },
		10,  5, 0,  4,  15, 15, -5, 10,
		8,  115,
		15, 10,
		38, 4, 70, 5,
		36, 3, 60, 4,
		5,
		0x1D,
		23,
		0,
		0x00100000, /* RES_LITE (bit 20) */
		0
    }
};


const char *c_name = "Warrior\0Mage\0Priest\0Rogue\0Ranger\0Paladin\0";
const char *c_text = "Rookie\0Soldier\0Swordsman\0Swashbuckler\0Veteran\0Myrmidon\0Commando\0Champion\0Hero\0Lord\0Novice\0Apprentice\0Trickster\0Illusionist\0Spellbinder\0Evoker\0Conjurer\0Warlock\0Sorcerer\0Arch-Mage\0Believer\0Acolyte\0Adept\0Evangelist\0Priest\0Curate\0Canon\0Bishop\0Prophet\0Patriarch\0Vagabond\0Cutpurse\0Footpad\0Robber\0Burglar\0Filcher\0Sharper\0Rogue\0Thief\0Master Thief\0Runner\0Strider\0Scout\0Courser\0Tracker\0Guide\0Explorer\0Pathfinder\0Ranger\0Ranger Lord\0Gallant\0Keeper\0Protector\0Defender\0Warder\0Knight\0Guardian\0Chevalier\0Paladin\0Paladin Lord\0";

/*
 * Player Classes.
 *
 *   Title,
 *   {STR,INT,WIS,DEX,CON,CHR},
 *   c_dis, c_dev, c_sav, c_stl, c_srh, c_fos, c_thn, c_thb,
 *   x_dis, x_dev, x_sav, x_stl, x_srh, x_fos, x_thn, x_thb,
 *   HD, Exp
 */
player_class c_info[MAX_CLASS] =
{
	{
		0, /*"Warrior\0", 0-7 */
		{ 0, 7, 15, 25, 38, 46, 54, 64, 73, 80 }, /*
		"Rookie\0Soldier\0Swordsman\0Swashbuckler\0Veteran\0Myrmidon\0Commando\0Champion\0Hero\0Lord\0"
		*/
		{ 5, -2, -2, 2, 2, -1},
		25, 18, 18, 1,  14, 2, 70, 55,
		10, 7,  10, 0,  0,  0,  45, 45,
		9,  0,
		0x0182, /* BRAVERY_30 | PSEUDO_ID_HEAVY | PSEUDO_ID_IMPROV */
		6, 30, 5, /* max_attacks, min_weight, att_multiply */
		0, 0, 0, 0, /* spell_book, _stat, _first, _weight */
		9000, 40, /* sense_base, _div */
		{ 
			{ 75, 33, 1, 1 }, /* tval, sval, min, max */
			{ 23, 16, 1, 1 },
			{ 37, 4, 1, 1 },
			{ 0, 0, 0, 0 }
		}, /* Start items */
		{
			{
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},

				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0},
				{ 99,  0,  0,   0}
			}
		}
	},

	{
		8, /* "Mage\0", 8-12 */
		{ 85, 92, 103, 113, 125, 137, 144, 153, 161, 170 }, /*
		"Novice\0Apprentice\0Trickster\0Illusionist\0Spellbinder\0Evoker\0Conjurer\0Warlock\0Sorcerer\0Arch-Mage\0"
		  */
		{-5, 3, 0, 1, -2, 1},
		30, 36, 30, 2,  16, 20, 34, 20,
		7,  13, 9,  0,  0,  0,  15, 15,
		0, 30,
		0x0078, /*CUMBER_GLOVE | ZERO_FAIL | BEAM | CHOOSE_SPELLS*/
		4, 40, 2,
		90, 1, 1, 300,
		240000, 5,
		{
			{ 90, 0, 1, 1 },
			{ 23, 4, 1, 1 },
			{ 70, 11, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{
				{ 1, 1, 22, 4 },
				{ 1, 1, 23, 4 },
				{ 1, 2, 24, 4 },
				{ 1, 2, 26, 4 },
				{ 3, 3, 25, 2 },
				{ 3, 3, 25, 3 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 11, 7, 25, 6 },
				{ 15, 5, 40, 6 },
				{ 30, 10, 70, 10 },
				{ 3, 3, 27, 3 },
				{ 5, 4, 30, 4 },
				{ 5, 4, 30, 4 },
				{ 7, 5, 30, 4 },
				{ 7, 10, 50, 5 },
				{ 7, 6, 40, 6 },
				{ 15, 9, 50, 8 },
				{ 10, 7, 50, 7 },
				{ 5, 5, 30, 6 },
				{ 7, 6, 30, 5 },
				{ 9, 7, 44, 8 },
				{ 13, 9, 40, 12 },
				{ 20, 18, 60, 20 },
				{ 24, 20, 50, 20 },
				{ 5, 5, 35, 4 },
				{ 9, 7, 45, 8 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 25, 12, 65, 10 },
				{ 7, 6, 35, 5 },
				{ 17, 9, 50, 7 },
				{ 23, 12, 60, 8 },
				{ 28, 17, 65, 20 },
				{ 30, 25, 75, 15 },
				{ 11, 7, 45, 9 },
				{ 16, 10, 40, 6 },
				{ 20, 20, 50, 10 },
				{ 20, 9, 50, 8 },
				{ 25, 7, 50, 6 },
				{ 25, 15, 60, 24 },
				{ 25, 30, 80, 30 },
				{ 33, 21, 80, 35 },
				{ 38, 35, 90, 40 },
				{ 10, 5, 50, 10 },
				{ 10, 5, 50, 10 },
				{ 25, 10, 45, 20 },
				{ 28, 20, 65, 30 },
				{ 32, 24, 75, 30 },
				{ 36, 60, 70, 40 },
				{ 9, 7, 75, 10 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 30, 30, 95, 100 },
				{ 32, 60, 95, 120 },
				{ 19, 12, 55, 8 },
				{ 20, 15, 70, 20 },
				{ 26, 18, 65, 12 },
				{ 27, 22, 75, 24 },
				{ 30, 45, 95, 25 },
				{ 30, 25, 85, 34 },
				{ 35, 75, 90, 100 },
				{ 35, 30, 60, 25 },
				{ 42, 30, 95, 200 }
			}
		}
	},

	{
		13, /* "Priest\0", 13-19 */
		{ 180, 189, 197, 203, 214, 221, 228, 234, 241, 250 }, /*
		 "Believer\0Acolyte\0Adept\0Evangelist\0Priest\0Curate\0Canon\0Bishop\0Prophet\0Patriarch\0"
		 */
		{-1, -3, 3, -1, 0, 2},
		25, 30, 32, 2,  16, 8, 48, 35,
		7,  10, 12, 0,  0,  0, 20, 20,
		2, 20,
		0x0114, /* BLESS_WEAPON | ZERO_FAIL | PSEUDO_ID_IMPROV */
		4, 35, 3,
		91, 2, 1, 350,
		10000, 40,
		{
			{ 91, 0, 1, 1 },
			{ 21, 5, 1, 1 },
			{ 75, 37, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{
				{ 1, 1, 10, 4 },
				{ 1, 2, 15, 4 },
				{ 1, 2, 20, 4 },
				{ 1, 2, 25, 4 },
				{ 3, 2, 25, 1 },
				{ 3, 3, 27, 2 },
				{ 3, 3, 27, 2 },
				{ 3, 3, 28, 4 },
				{ 5, 4, 29, 3 },
				{ 5, 4, 30, 4 },
				{ 5, 4, 32, 4 },
				{ 5, 5, 34, 4 },
				{ 7, 5, 36, 3 },
				{ 7, 5, 38, 4 },
				{ 7, 6, 38, 5 },
				{ 7, 7, 38, 5 },
				{ 9, 6, 38, 4 },
				{ 9, 7, 40, 4 },
				{ 9, 7, 38, 4 },
				{ 11, 8, 42, 4 },
				{ 11, 8, 42, 4 },
				{ 11, 9, 55, 5 },
				{ 13, 10, 45, 4 },
				{ 13, 11, 45, 4 },
				{ 15, 12, 50, 5 },
				{ 15, 14, 50, 5 },
				{ 17, 14, 55, 7 },
				{ 21, 16, 60, 7 },
				{ 25, 20, 70, 12 },
				{ 33, 55, 90, 15 },
				{ 39, 32, 95, 20 },
				{ 3, 3, 50, 2 },
				{ 10, 10, 80, 20 },
				{ 20, 20, 80, 20 },
				{ 25, 10, 80, 150 },
				{ 35, 50, 80, 230 },
				{ 15, 5, 50, 25 },
				{ 17, 7, 60, 45 },
				{ 30, 50, 80, 130 },
				{ 35, 70, 90, 230 },
				{ 35, 70, 90, 250 },
				{ 15, 7, 70, 25 },
				{ 20, 10, 75, 60 },
				{ 25, 25, 80, 250 },
				{ 35, 35, 80, 115 },
				{ 45, 60, 75, 250 },
				{ 5, 6, 50, 40 },
				{ 15, 20, 80, 25 },
				{ 25, 40, 80, 160 },
				{ 35, 50, 80, 230 },
				{ 37, 60, 85, 250 },
				{ 45, 95, 85, 250 },
				{ 3, 3, 50, 6 },
				{ 10, 10, 50, 8 },
				{ 20, 20, 80, 16 },
				{ 30, 40, 75, 133 },
				{ 35, 50, 75, 11 },
				{ 40, 60, 75, 250 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 }
			}
		}
	},

	{
		20, /* "Rogue\0", 20-25 */
		{ 260, 269, 278, 286, 293, 301, 309, 317, 323, 330 }, /*
		"Vagabond\0Cutpurse\0Footpad\0Robber\0Burglar\0Filcher\0Sharper\0Rogue\0Thief\0Master Thief\0"
		*/
		{ 2, 1, -2, 3, 1, -1},
		45, 32, 28, 5, 32, 24, 60, 66,
		15, 10, 10, 0,  0,  0, 40, 30,
		6, 25,
		0x01C8, /* CUMBER_GLOVE | CHOOSE_SPELLS | PSEUDO_ID_HEAVY | PSEUDO_ID_IMPROV*/
		5, 30, 3,
		90, 1, 5, 350,
		20000, 40,
		{
			{ 90, 0, 1, 1 },
			{ 23, 8, 1, 1 },
			{ 36, 4, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{
				{ 99, 0, 0, 0 },
				{ 5, 1, 50, 1 },
				{ 7, 2, 55, 1 },
				{ 9, 3, 60, 1 },
				{ 8, 3, 50, 1 },
				{ 11, 4, 65, 1 },
				{ 10, 3, 60, 1 },
				{ 12, 4, 65, 1 },
				{ 18, 18, 60, 2 },
				{ 20, 10, 50, 4 },
				{ 27, 10, 50, 10 },
				{ 21, 12, 50, 10 },
				{ 99, 0, 0, 0 },
				{ 15, 6, 75, 1 },
				{ 19, 8, 85, 1 },
				{ 20, 10, 70, 20 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 14, 7, 60, 2 },
				{ 23, 10, 95, 1 },
				{ 24, 11, 70, 1 },
				{ 17, 7, 30, 15 },
				{ 99, 0, 0, 0 },
				{ 25, 12, 40, 25 },
				{ 21, 9, 90, 1 },
				{ 25, 12, 95, 1 },
				{ 26, 15, 50, 40 },
				{ 28, 20, 60, 60 },
				{ 32, 25, 70, 6 },
				{ 22, 9, 50, 1 },
				{ 28, 20, 70, 2 },
				{ 31, 25, 70, 3 },
				{ 25, 20, 60, 20 },
				{ 36, 40, 80, 18 },
				{ 99, 0, 0, 0 },
				{ 35, 25, 80, 50 },
				{ 99, 0, 0, 0 },
				{ 25, 20, 60, 15 },
				{ 24, 15, 80, 10 },
				{ 29, 20, 70, 20 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 16, 12, 50, 40 },
				{ 19, 12, 50, 40 },
				{ 30, 25, 75, 60 },
				{ 31, 30, 85, 80 },
				{ 34, 20, 75, 80 },
				{ 99, 0, 0, 0 },
				{ 27, 15, 99, 1 },
				{ 31, 50, 95, 90 },
				{ 33, 55, 90, 90 },
				{ 35, 40, 95, 100 },
				{ 37, 60, 80, 120 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 }
			}
		}
	},

	{
		26, /* "Ranger\0", 26-32 */
		{ 343, 350, 358, 364, 372, 380, 386, 395, 406, 413 }, /*
		"Runner\0Strider\0Scout\0Courser\0Tracker\0Guide\0Explorer\0Pathfinder\0Ranger\0Ranger Lord\0"
		*/
		{ 2, 2, 0, 1, 1, 1},
		30, 32, 28, 3,  24, 16, 56, 72,
		8,  10, 10, 0,  0,  0,  30, 45,
		4, 30,
		0x0149, /* EXTRA_SHOT | CUMBER_GLOVE | CHOOSE_SPELLS | PSEUDO_ID_IMPROV */
		5, 35, 4,
		90, 1, 3, 400,
		30000, 40,
		{
			{ 90, 0, 1, 1 },
			{ 23, 16, 1, 1 },
			{ 19, 13, 1, 1 },
			{ 17, 1, 15, 20 }
		},
		{
			{
				{ 3, 1, 30, 1 },
				{ 3, 2, 35, 2 },
				{ 3, 2, 35, 2 },
				{ 5, 3, 35, 1 },
				{ 5, 4, 45, 2 },
				{ 5, 3, 40, 1 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 23, 25, 90, 3 },
				{ 25, 15, 50, 4 },
				{ 35, 20, 70, 10 },
				{ 7, 5, 40, 3 },
				{ 9, 7, 40, 3 },
				{ 7, 6, 40, 2 },
				{ 11, 8, 40, 3 },
				{ 15, 18, 80, 10 },
				{ 15, 12, 50, 4 },
				{ 20, 16, 50, 6 },
				{ 25, 20, 60, 3 },
				{ 9, 8, 45, 3 },
				{ 13, 11, 55, 4 },
				{ 15, 13, 50, 4 },
				{ 20, 17, 20, 25 },
				{ 30, 28, 60, 16 },
				{ 32, 32, 50, 40 },
				{ 11, 9, 45, 3 },
				{ 17, 17, 55, 3 },
				{ 18, 15, 50, 40 },
				{ 23, 30, 80, 35 },
				{ 33, 25, 75, 4 },
				{ 13, 10, 45, 3 },
				{ 25, 21, 65, 3 },
				{ 31, 25, 70, 3 },
				{ 34, 27, 60, 15 },
				{ 35, 35, 75, 16 },
				{ 21, 19, 60, 3 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 22, 19, 60, 6 },
				{ 23, 20, 60, 4 },
				{ 32, 30, 70, 12 },
				{ 99, 0, 0, 0 },
				{ 37, 30, 95, 30 },
				{ 99, 0, 0, 0 },
				{ 8, 15, 50, 30 },
				{ 8, 15, 50, 30 },
				{ 26, 25, 75, 50 },
				{ 31, 40, 85, 70 },
				{ 35, 30, 75, 80 },
				{ 40, 75, 80, 80 },
				{ 29, 17, 90, 4 },
				{ 33, 45, 90, 50 },
				{ 34, 50, 90, 60 },
				{ 35, 50, 95, 115 },
				{ 31, 60, 95, 180 },
				{ 27, 21, 65, 6 },
				{ 30, 24, 70, 6 },
				{ 34, 28, 80, 9 },
				{ 35, 35, 70, 10 },
				{ 99, 0, 0, 0 },
				{ 36, 45, 80, 35 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 }
			}
		}
	},

	{
		33, /* "Paladin\0", 33-40 */
		{ 425, 433, 440, 450, 459, 466, 473, 482, 492, 500 }, /*
		"Gallant\0Keeper\0Protector\0Defender\0Warder\0Knight\0Guardian\0Chevalier\0Paladin\0Paladin Lord\0"
		*/
		{ 3, -3, 1, 0, 2, 2},
		20, 24, 25, 1,  12, 2, 68, 40,
		7,  10, 11, 0,  0,  0,  35, 30,
		6, 35,
		0x0180, /* PSEUDO_ID_HEAVY | PSEUDO_ID_IMPROV */
		5, 30, 5,
		91, 2, 1, 400,
		80000, 40,
		{
			{ 91, 0, 1, 1 },
			{ 23, 16, 1, 1 },
			{ 70, 37, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{
				{ 1, 1, 30, 4 },
				{ 2, 2, 35, 4 },
				{ 3, 3, 35, 4 },
				{ 5, 3, 35, 4 },
				{ 5, 4, 35, 4 },
				{ 7, 5, 40, 3 },
				{ 7, 5, 40, 3 },
				{ 9, 7, 40, 3 },
				{ 9, 7, 40, 3 },
				{ 9, 8, 40, 3 },
				{ 11, 9, 40, 3 },
				{ 11, 10, 45, 3 },
				{ 11, 10, 45, 3 },
				{ 13, 10, 45, 3 },
				{ 13, 11, 45, 4 },
				{ 15, 13, 45, 4 },
				{ 15, 15, 50, 4 },
				{ 17, 15, 50, 4 },
				{ 17, 15, 50, 4 },
				{ 19, 15, 50, 4 },
				{ 19, 15, 50, 4 },
				{ 21, 17, 50, 3 },
				{ 23, 17, 50, 3 },
				{ 25, 20, 50, 3 },
				{ 27, 21, 50, 3 },
				{ 29, 22, 50, 3 },
				{ 31, 24, 60, 3 },
				{ 33, 28, 60, 3 },
				{ 35, 32, 70, 4 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 5, 5, 50, 1 },
				{ 15, 15, 80, 12 },
				{ 25, 25, 80, 16 },
				{ 30, 15, 80, 135 },
				{ 99, 0, 0, 0 },
				{ 17, 15, 50, 25 },
				{ 23, 25, 60, 35 },
				{ 99, 0, 0, 0 },
				{ 45, 80, 90, 250 },
				{ 99, 0, 0, 0 },
				{ 20, 13, 70, 20 },
				{ 99, 0, 0, 0 },
				{ 30, 35, 80, 200 },
				{ 40, 40, 80, 100 },
				{ 99, 0, 0, 0 },
				{ 10, 16, 50, 20 },
				{ 25, 30, 80, 15 },
				{ 99, 0, 0, 0 },
				{ 40, 70, 80, 200 },
				{ 42, 80, 85, 250 },
				{ 47, 95, 85, 250 },
				{ 7, 7, 50, 2 },
				{ 20, 20, 50, 4 },
				{ 25, 25, 80, 12 },
				{ 35, 50, 75, 115 },
				{ 40, 60, 75, 10 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 },
				{ 99, 0, 0, 0 }
			}
		}
    },

};


/*
 * Spells in each book (mage spells then priest spells)
 */
u32b spell_flags[3][9][2] =
{
	{
		/*** Mage spell books ***/
		{ 0x000001ff, 0x00000000 },
		{ 0x0003fe00, 0x00000000 },
		{ 0x07fc0000, 0x00000000 },
		{ 0xf8000000, 0x00000001 },
		{ 0x00000000, 0x0000003e },
		{ 0x00000000, 0x00001fc0 },
		{ 0x00000000, 0x0003e000 },
		{ 0x00000000, 0x00fc0000 },
		{ 0x00000000, 0xff000000 }
	},

	{
		/*** Priest spell books ***/
		{ 0x000000ff, 0x00000000 },
		{ 0x0000ff00, 0x00000000 },
		{ 0x01ff0000, 0x00000000 },
		{ 0x7e000000, 0x00000000 },
		{ 0x00000000, 0x03f00000 },
		{ 0x80000000, 0x0000000f },
		{ 0x00000000, 0x000001f0 },
		{ 0x00000000, 0x000fc000 },
		{ 0x00000000, 0x00003e00 }
    },

};

/*
 * Hack -- the "basic" sound names (see "SOUND_xxx")
 */
cptr sound_names[SOUND_MAX] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
};


/*
 * Abbreviations of healthy stats
 */
cptr stat_names[6] =
{
	"STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "
};

/*
 * Abbreviations of damaged stats
 */
cptr stat_names_reduced[6] =
{
	"Str: ", "Int: ", "Wis: ", "Dex: ", "Con: ", "Chr: "
};


/*
 * Standard window names
 */
cptr ang_term_name[8] =
{
	"Angband",
	"Mirror",
	"Recall",
	"Choice",
	"Term-4",
	"Term-5",
	"Term-6",
	"Term-7"
};


/*
 * Certain "screens" always use the main screen, including News, Birth,
 * Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
 *
 * Later, special flags may allow sub-windows to "steal" stuff from the
 * main window, including File dump (help), File dump (artifacts, uniques),
 * Character screen, Small scale map, Previous Messages, Store screen, etc.
 *
 * The "ctrl-i" (tab) command flips the "Display inven/equip" and "Display
 * equip/inven" flags for all windows.
 *
 * The "ctrl-g" command (or pseudo-command) should perhaps grab a snapshot
 * of the main screen into any interested windows.
 */
cptr window_flag_desc[32] =
{
	"Display inven/equip",
	"Display equip/inven",
	"Display spell list",
	"Display character",
	NULL,
	NULL,
	"Display messages",
	"Display overhead view",
	"Display monster recall",
	"Display object recall",
	NULL,
	"Display snap-shot",
	NULL,
	NULL,
	"Display borg messages",
	"Display borg status",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

