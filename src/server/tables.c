/* File: tables.c */

/* Purpose: Angband Tables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
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
 * Stat Table (INT/WIS) -- Number of half-spells per level
 */
const int adj_mag_study[] =
{
	  0	/* 3 */,
	  0	/* 4 */,
	 10	/* 5 */,
	 20	/* 6 */,
	 30	/* 7 */,
	 40	/* 8 */,
	 50	/* 9 */,
	 60	/* 10 */,
	 70	/* 11 */,
	 80	/* 12 */,
	 85	/* 13 */,
	 90	/* 14 */,
	 95	/* 15 */,
	100	/* 16 */,
	105	/* 17 */,
	110	/* 18/00-18/09 */,
	115	/* 18/10-18/19 */,
	120	/* 18/20-18/29 */,
	130	/* 18/30-18/39 */,
	140	/* 18/40-18/49 */,
	150	/* 18/50-18/59 */,
	160	/* 18/60-18/69 */,
	170	/* 18/70-18/79 */,
	180	/* 18/80-18/89 */,
	190	/* 18/90-18/99 */,
	200	/* 18/100-18/109 */,
	210	/* 18/110-18/119 */,
	220	/* 18/120-18/129 */,
	230	/* 18/130-18/139 */,
	240	/* 18/140-18/149 */,
	250	/* 18/150-18/159 */,
	250	/* 18/160-18/169 */,
	250	/* 18/170-18/179 */,
	250	/* 18/180-18/189 */,
	250	/* 18/190-18/199 */,
	250	/* 18/200-18/209 */,
	250	/* 18/210-18/219 */,
	250	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- extra half-mana-points per level
 */
const int adj_mag_mana[] =
{
	  0	/* 3 */,
	 10	/* 4 */,
	 20	/* 5 */,
	 30	/* 6 */,
	 40	/* 7 */,
	 50	/* 8 */,
	 60	/* 9 */,
	 70	/* 10 */,
	 80	/* 11 */,
	 90	/* 12 */,
	100	/* 13 */,
	110	/* 14 */,
	120	/* 15 */,
	130	/* 16 */,
	140	/* 17 */,
	150	/* 18/00-18/09 */,
	160	/* 18/10-18/19 */,
	170	/* 18/20-18/29 */,
	180	/* 18/30-18/39 */,
	190	/* 18/40-18/49 */,
	200	/* 18/50-18/59 */,
	225	/* 18/60-18/69 */,
	250	/* 18/70-18/79 */,
	300	/* 18/80-18/89 */,
	350	/* 18/90-18/99 */,
	400	/* 18/100-18/109 */,
	450	/* 18/110-18/119 */,
	500	/* 18/120-18/129 */,
	550	/* 18/130-18/139 */,
	600	/* 18/140-18/149 */,
	650	/* 18/150-18/159 */,
	700	/* 18/160-18/169 */,
	750	/* 18/170-18/179 */,
	800	/* 18/180-18/189 */,
	800	/* 18/190-18/199 */,
	800	/* 18/200-18/209 */,
	800	/* 18/210-18/219 */,
	800	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Minimum failure rate (percentage)
 */
byte adj_mag_fail[] =
{
	99	/* 3 */,
	99	/* 4 */,
	99	/* 5 */,
	99	/* 6 */,
	99	/* 7 */,
	50	/* 8 */,
	30	/* 9 */,
	20	/* 10 */,
	15	/* 11 */,
	12	/* 12 */,
	11	/* 13 */,
	10	/* 14 */,
	9	/* 15 */,
	8	/* 16 */,
	7	/* 17 */,
	6	/* 18/00-18/09 */,
	6	/* 18/10-18/19 */,
	5	/* 18/20-18/29 */,
	5	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	4	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	3	/* 18/90-18/99 */,
	3	/* 18/100-18/109 */,
	2	/* 18/110-18/119 */,
	2	/* 18/120-18/129 */,
	2	/* 18/130-18/139 */,
	2	/* 18/140-18/149 */,
	1	/* 18/150-18/159 */,
	1	/* 18/160-18/169 */,
	1	/* 18/170-18/179 */,
	1	/* 18/180-18/189 */,
	1	/* 18/190-18/199 */,
	0	/* 18/200-18/209 */,
	0	/* 18/210-18/219 */,
	0	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- failure rate adjustment
 */
const int adj_mag_stat[] =
{
	-5	/* 3 */,
	-4	/* 4 */,
	-3	/* 5 */,
	-3	/* 6 */,
	-2	/* 7 */,
	-1	/* 8 */,
	 0	/* 9 */,
	 0	/* 10 */,
	 0	/* 11 */,
	 0	/* 12 */,
	 0	/* 13 */,
	 1	/* 14 */,
	 2	/* 15 */,
	 3	/* 16 */,
	 4	/* 17 */,
	 5	/* 18/00-18/09 */,
	 6	/* 18/10-18/19 */,
	 7	/* 18/20-18/29 */,
	 8	/* 18/30-18/39 */,
	 9	/* 18/40-18/49 */,
	10	/* 18/50-18/59 */,
	11	/* 18/60-18/69 */,
	12	/* 18/70-18/79 */,
	15	/* 18/80-18/89 */,
	18	/* 18/90-18/99 */,
	21	/* 18/100-18/109 */,
	24	/* 18/110-18/119 */,
	27	/* 18/120-18/129 */,
	30	/* 18/130-18/139 */,
	33	/* 18/140-18/149 */,
	36	/* 18/150-18/159 */,
	39	/* 18/160-18/169 */,
	42	/* 18/170-18/179 */,
	45	/* 18/180-18/189 */,
	48	/* 18/190-18/199 */,
	51	/* 18/200-18/209 */,
	54	/* 18/210-18/219 */,
	57	/* 18/220+ */
};


/*
 * Stat Table (CHR) -- payment percentages
 */
byte adj_chr_gold[] =
{
	130	/* 3 */,
	125	/* 4 */,
	122	/* 5 */,
	120	/* 6 */,
	118	/* 7 */,
	116	/* 8 */,
	114	/* 9 */,
	112	/* 10 */,
	110	/* 11 */,
	108	/* 12 */,
	106	/* 13 */,
	104	/* 14 */,
	103	/* 15 */,
	102	/* 16 */,
	101	/* 17 */,
	100	/* 18/00-18/09 */,
	99	/* 18/10-18/19 */,
	98	/* 18/20-18/29 */,
	97	/* 18/30-18/39 */,
	96	/* 18/40-18/49 */,
	95	/* 18/50-18/59 */,
	94	/* 18/60-18/69 */,
	93	/* 18/70-18/79 */,
	92	/* 18/80-18/89 */,
	91	/* 18/90-18/99 */,
	90	/* 18/100-18/109 */,
	89	/* 18/110-18/119 */,
	88	/* 18/120-18/129 */,
	87	/* 18/130-18/139 */,
	86	/* 18/140-18/149 */,
	85	/* 18/150-18/159 */,
	84	/* 18/160-18/169 */,
	83	/* 18/170-18/179 */,
	82	/* 18/180-18/189 */,
	81	/* 18/190-18/199 */,
	80	/* 18/200-18/209 */,
	80	/* 18/210-18/219 */,
	80	/* 18/220+ */
};


/*
 * Stat Table (INT) -- Magic devices
 */
byte adj_int_dev[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	4	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	6	/* 18/60-18/69 */,
	6	/* 18/70-18/79 */,
	7	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	17	/* 18/190-18/199 */,
	18	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (WIS) -- Saving throw
 */
byte adj_wis_sav[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	5	/* 18/80-18/89 */,
	6	/* 18/90-18/99 */,
	7	/* 18/100-18/109 */,
	8	/* 18/110-18/119 */,
	9	/* 18/120-18/129 */,
	10	/* 18/130-18/139 */,
	11	/* 18/140-18/149 */,
	12	/* 18/150-18/159 */,
	13	/* 18/160-18/169 */,
	14	/* 18/170-18/179 */,
	15	/* 18/180-18/189 */,
	16	/* 18/190-18/199 */,
	17	/* 18/200-18/209 */,
	18	/* 18/210-18/219 */,
	19	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- disarming
 */
byte adj_dex_dis[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	4	/* 18/00-18/09 */,
	4	/* 18/10-18/19 */,
	4	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	5	/* 18/60-18/69 */,
	6	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	8	/* 18/110-18/119 */,
	8	/* 18/120-18/129 */,
	8	/* 18/130-18/139 */,
	8	/* 18/140-18/149 */,
	9	/* 18/150-18/159 */,
	9	/* 18/160-18/169 */,
	9	/* 18/170-18/179 */,
	9	/* 18/180-18/189 */,
	9	/* 18/190-18/199 */,
	10	/* 18/200-18/209 */,
	10	/* 18/210-18/219 */,
	10	/* 18/220+ */
};


/*
 * Stat Table (INT) -- disarming
 */
byte adj_int_dis[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	4	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	6	/* 18/60-18/69 */,
	7	/* 18/70-18/79 */,
	8	/* 18/80-18/89 */,
	9	/* 18/90-18/99 */,
	10	/* 18/100-18/109 */,
	10	/* 18/110-18/119 */,
	11	/* 18/120-18/129 */,
	12	/* 18/130-18/139 */,
	13	/* 18/140-18/149 */,
	14	/* 18/150-18/159 */,
	15	/* 18/160-18/169 */,
	16	/* 18/170-18/179 */,
	17	/* 18/180-18/189 */,
	18	/* 18/190-18/199 */,
	19	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	19	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to ac (plus 128)
 */
byte adj_dex_ta[] =
{
	128 + -4	/* 3 */,
	128 + -3	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 1	/* 15 */,
	128 + 1	/* 16 */,
	128 + 1	/* 17 */,
	128 + 2	/* 18/00-18/09 */,
	128 + 2	/* 18/10-18/19 */,
	128 + 2	/* 18/20-18/29 */,
	128 + 2	/* 18/30-18/39 */,
	128 + 2	/* 18/40-18/49 */,
	128 + 3	/* 18/50-18/59 */,
	128 + 3	/* 18/60-18/69 */,
	128 + 3	/* 18/70-18/79 */,
	128 + 4	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to dam (plus 128)
 */
byte adj_str_td[] =
{
	128 + -2	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 2	/* 18/00-18/09 */,
	128 + 2	/* 18/10-18/19 */,
	128 + 3	/* 18/20-18/29 */,
	128 + 3	/* 18/30-18/39 */,
	128 + 3	/* 18/40-18/49 */,
	128 + 3	/* 18/50-18/59 */,
	128 + 3	/* 18/60-18/69 */,
	128 + 4	/* 18/70-18/79 */,
	128 + 5	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 10	/* 18/140-18/149 */,
	128 + 11	/* 18/150-18/159 */,
	128 + 12	/* 18/160-18/169 */,
	128 + 13	/* 18/170-18/179 */,
	128 + 14	/* 18/180-18/189 */,
	128 + 15	/* 18/190-18/199 */,
	128 + 16	/* 18/200-18/209 */,
	128 + 18	/* 18/210-18/219 */,
	128 + 20	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to hit (plus 128)
 */
byte adj_dex_th[] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + -1	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 3	/* 18/00-18/09 */,
	128 + 3	/* 18/10-18/19 */,
	128 + 3	/* 18/20-18/29 */,
	128 + 3	/* 18/30-18/39 */,
	128 + 3	/* 18/40-18/49 */,
	128 + 4	/* 18/50-18/59 */,
	128 + 4	/* 18/60-18/69 */,
	128 + 4	/* 18/70-18/79 */,
	128 + 4	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to hit (plus 128)
 */
byte adj_str_th[] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 0	/* 16 */,
	128 + 0	/* 17 */,
	128 + 1	/* 18/00-18/09 */,
	128 + 1	/* 18/10-18/19 */,
	128 + 1	/* 18/20-18/29 */,
	128 + 1	/* 18/30-18/39 */,
	128 + 1	/* 18/40-18/49 */,
	128 + 1	/* 18/50-18/59 */,
	128 + 1	/* 18/60-18/69 */,
	128 + 2	/* 18/70-18/79 */,
	128 + 3	/* 18/80-18/89 */,
	128 + 4	/* 18/90-18/99 */,
	128 + 5	/* 18/100-18/109 */,
	128 + 6	/* 18/110-18/119 */,
	128 + 7	/* 18/120-18/129 */,
	128 + 8	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- weight limit in deca-pounds
 */
byte adj_str_wgt[] =
{
	5	/* 3 */,
	6	/* 4 */,
	7	/* 5 */,
	8	/* 6 */,
	9	/* 7 */,
	10	/* 8 */,
	11	/* 9 */,
	12	/* 10 */,
	13	/* 11 */,
	14	/* 12 */,
	15	/* 13 */,
	16	/* 14 */,
	17	/* 15 */,
	18	/* 16 */,
	19	/* 17 */,
	20	/* 18/00-18/09 */,
	22	/* 18/10-18/19 */,
	24	/* 18/20-18/29 */,
	26	/* 18/30-18/39 */,
	28	/* 18/40-18/49 */,
	30	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	30	/* 18/70-18/79 */,
	30	/* 18/80-18/89 */,
	30	/* 18/90-18/99 */,
	30	/* 18/100-18/109 */,
	30	/* 18/110-18/119 */,
	30	/* 18/120-18/129 */,
	30	/* 18/130-18/139 */,
	30	/* 18/140-18/149 */,
	30	/* 18/150-18/159 */,
	30	/* 18/160-18/169 */,
	30	/* 18/170-18/179 */,
	30	/* 18/180-18/189 */,
	30	/* 18/190-18/199 */,
	30	/* 18/200-18/209 */,
	30	/* 18/210-18/219 */,
	30	/* 18/220+ */
};


/*
 * Stat Table (STR) -- weapon weight limit in pounds
 */
byte adj_str_hold[] =
{
	4	/* 3 */,
	5	/* 4 */,
	6	/* 5 */,
	7	/* 6 */,
	8	/* 7 */,
	10	/* 8 */,
	12	/* 9 */,
	14	/* 10 */,
	16	/* 11 */,
	18	/* 12 */,
	20	/* 13 */,
	22	/* 14 */,
	24	/* 15 */,
	26	/* 16 */,
	28	/* 17 */,
	30	/* 18/00-18/09 */,
	30	/* 18/10-18/19 */,
	35	/* 18/20-18/29 */,
	40	/* 18/30-18/39 */,
	45	/* 18/40-18/49 */,
	50	/* 18/50-18/59 */,
	55	/* 18/60-18/69 */,
	60	/* 18/70-18/79 */,
	65	/* 18/80-18/89 */,
	70	/* 18/90-18/99 */,
	80	/* 18/100-18/109 */,
	80	/* 18/110-18/119 */,
	80	/* 18/120-18/129 */,
	80	/* 18/130-18/139 */,
	80	/* 18/140-18/149 */,
	90	/* 18/150-18/159 */,
	90	/* 18/160-18/169 */,
	90	/* 18/170-18/179 */,
	90	/* 18/180-18/189 */,
	90	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (STR) -- digging value
 */
byte adj_str_dig[] =
{
	0	/* 3 */,
	0	/* 4 */,
	1	/* 5 */,
	2	/* 6 */,
	3	/* 7 */,
	4	/* 8 */,
	4	/* 9 */,
	5	/* 10 */,
	5	/* 11 */,
	6	/* 12 */,
	6	/* 13 */,
	7	/* 14 */,
	7	/* 15 */,
	8	/* 16 */,
	8	/* 17 */,
	9	/* 18/00-18/09 */,
	10	/* 18/10-18/19 */,
	12	/* 18/20-18/29 */,
	15	/* 18/30-18/39 */,
	20	/* 18/40-18/49 */,
	25	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	35	/* 18/70-18/79 */,
	40	/* 18/80-18/89 */,
	45	/* 18/90-18/99 */,
	50	/* 18/100-18/109 */,
	55	/* 18/110-18/119 */,
	60	/* 18/120-18/129 */,
	65	/* 18/130-18/139 */,
	70	/* 18/140-18/149 */,
	75	/* 18/150-18/159 */,
	80	/* 18/160-18/169 */,
	85	/* 18/170-18/179 */,
	90	/* 18/180-18/189 */,
	95	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (STR) -- help index into the "blow" table
 */
byte adj_str_blow[] =
{
	3	/* 3 */,
	4	/* 4 */,
	5	/* 5 */,
	6	/* 6 */,
	7	/* 7 */,
	8	/* 8 */,
	9	/* 9 */,
	10	/* 10 */,
	11	/* 11 */,
	12	/* 12 */,
	13	/* 13 */,
	14	/* 14 */,
	15	/* 15 */,
	16	/* 16 */,
	17	/* 17 */,
	20 /* 18/00-18/09 */,
	30 /* 18/10-18/19 */,
	40 /* 18/20-18/29 */,
	50 /* 18/30-18/39 */,
	60 /* 18/40-18/49 */,
	70 /* 18/50-18/59 */,
	80 /* 18/60-18/69 */,
	90 /* 18/70-18/79 */,
	100 /* 18/80-18/89 */,
	110 /* 18/90-18/99 */,
	120 /* 18/100-18/109 */,
	130 /* 18/110-18/119 */,
	140 /* 18/120-18/129 */,
	150 /* 18/130-18/139 */,
	160 /* 18/140-18/149 */,
	170 /* 18/150-18/159 */,
	180 /* 18/160-18/169 */,
	190 /* 18/170-18/179 */,
	200 /* 18/180-18/189 */,
	210 /* 18/190-18/199 */,
	220 /* 18/200-18/209 */,
	230 /* 18/210-18/219 */,
	240 /* 18/220+ */
};


/*
 * Stat Table (DEX) -- index into the "blow" table
 */
byte adj_dex_blow[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	1	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	4	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	5	/* 18/90-18/99 */,
	6	/* 18/100-18/109 */,
	7	/* 18/110-18/119 */,
	8	/* 18/120-18/129 */,
	9	/* 18/130-18/139 */,
	10	/* 18/140-18/149 */,
	11	/* 18/150-18/159 */,
	12	/* 18/160-18/169 */,
	14	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	18	/* 18/190-18/199 */,
	20	/* 18/200-18/209 */,
	20	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- chance of avoiding "theft" and "falling"
 */
byte adj_dex_safe[] =
{
	0	/* 3 */,
	1	/* 4 */,
	2	/* 5 */,
	3	/* 6 */,
	4	/* 7 */,
	5	/* 8 */,
	5	/* 9 */,
	6	/* 10 */,
	6	/* 11 */,
	7	/* 12 */,
	7	/* 13 */,
	8	/* 14 */,
	8	/* 15 */,
	9	/* 16 */,
	9	/* 17 */,
	10	/* 18/00-18/09 */,
	10	/* 18/10-18/19 */,
	15	/* 18/20-18/29 */,
	15	/* 18/30-18/39 */,
	20	/* 18/40-18/49 */,
	25	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	35	/* 18/70-18/79 */,
	40	/* 18/80-18/89 */,
	45	/* 18/90-18/99 */,
	50	/* 18/100-18/109 */,
	60	/* 18/110-18/119 */,
	70	/* 18/120-18/129 */,
	80	/* 18/130-18/139 */,
	90	/* 18/140-18/149 */,
	100	/* 18/150-18/159 */,
	100	/* 18/160-18/169 */,
	100	/* 18/170-18/179 */,
	100	/* 18/180-18/189 */,
	100	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (CON) -- base regeneration rate
 */
byte adj_con_fix[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	0	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	2	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	3	/* 18/80-18/89 */,
	3	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	4	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	6	/* 18/130-18/139 */,
	6	/* 18/140-18/149 */,
	7	/* 18/150-18/159 */,
	7	/* 18/160-18/169 */,
	8	/* 18/170-18/179 */,
	8	/* 18/180-18/189 */,
	8	/* 18/190-18/199 */,
	9	/* 18/200-18/209 */,
	9	/* 18/210-18/219 */,
	9	/* 18/220+ */
};


/*
 * Stat Table (CON) -- extra 1/100th hitpoints per level
 */
const int adj_con_mhp[] =
{
	-250	/* 3 */,
	-150	/* 4 */,
	-100	/* 5 */,
	 -75	/* 6 */,
	 -50	/* 7 */,
	 -25	/* 8 */,
	 -10	/* 9 */,
	  -5	/* 10 */,
	   0	/* 11 */,
	   5	/* 12 */,
	  10	/* 13 */,
	  25	/* 14 */,
	  50	/* 15 */,
	  75	/* 16 */,
	 100	/* 17 */,
	 150	/* 18/00-18/09 */,
	 175	/* 18/10-18/19 */,
	 200	/* 18/20-18/29 */,
	 225	/* 18/30-18/39 */,
	 250	/* 18/40-18/49 */,
	 275	/* 18/50-18/59 */,
	 300	/* 18/60-18/69 */,
	 350	/* 18/70-18/79 */,
	 400	/* 18/80-18/89 */,
	 450	/* 18/90-18/99 */,
	 500	/* 18/100-18/109 */,
	 550	/* 18/110-18/119 */,
	 600	/* 18/120-18/129 */,
	 650	/* 18/130-18/139 */,
	 700	/* 18/140-18/149 */,
	 750	/* 18/150-18/159 */,
	 800	/* 18/160-18/169 */,
	 900	/* 18/170-18/179 */,
	1000	/* 18/180-18/189 */,
	1100	/* 18/190-18/199 */,
	1250	/* 18/200-18/209 */,
	1250	/* 18/210-18/219 */,
	1250	/* 18/220+ */
};


/*
 * This table is used to help calculate the number of blows the player can
 * make in a single round of attacks (one player turn) with a normal weapon.
 *
 * This number ranges from a single blow/round for weak players to up to six
 * blows/round for powerful warriors.
 *
 * Note that certain artifacts and ego-items give "bonus" blows/round.
 *
 * First, from the player class, we extract some values:
 *
 *    Warrior --> num = 6; mul = 5; div = MAX(30, weapon_weight);
 *    Mage    --> num = 4; mul = 2; div = MAX(40, weapon_weight);
 *    Priest  --> num = 5; mul = 3; div = MAX(35, weapon_weight);
 *    Rogue   --> num = 5; mul = 3; div = MAX(30, weapon_weight);
 *    Ranger  --> num = 5; mul = 4; div = MAX(35, weapon_weight);
 *    Paladin --> num = 5; mul = 4; div = MAX(30, weapon_weight);
 *
 * To get "P", we look up the relevant "adj_str_blow[]" (see above),
 * multiply it by "mul", and then divide it by "div", rounding down.
 *
 * To get "D", we look up the relevant "adj_dex_blow[]" (see above),
 * note especially column 6 (DEX 18/101) and 11 (DEX 18/150).
 *
 * The player gets "blows_table[P][D]" blows/round, as shown below,
 * up to a maximum of "num" blows/round, plus any "bonus" blows/round.
 */
byte blows_table[12][12] =
{
	/* P/D */
	/* 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11+ */

	/* 0  */
	{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3 },

	/* 1  */
	{  1,   1,   1,   1,   2,   2,   3,   3,   3,   4,   4,   4 },

	/* 2  */
	{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5 },

	/* 3  */
	{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5 },

	/* 4  */
	{  1,   2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5 },

	/* 5  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 6  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 7  */
	{  2,   3,   3,   4,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 8  */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

	/* 9  */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

	/* 10 */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6 },

	/* 11+ */
	{  3,   3,   4,   4,   4,   4,   5,   5,   6,   6,   6,   6 },
};


#if 0

/*
 * This is the "old" table used to calculate multiple blows.
 *
 * Note that this table used a different indexing scheme to determine "P"
 */

byte old_blows_table[11][12] =
{
	/* P/D */
	/* 3,  10, /01, /50, /90,/100,/101,/110,/120,/130,/140,/150 */

	/* 0+ */
	{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3},

	/* 2+ */
	{  1,   1,   1,   1,   2,   2,   3,   3,   3,   3,   3,   4},

	/* 3+ */
	{  1,   1,   1,   2,   2,   3,   4,   4,   4,   4,   4,   5},

	/* 4+ */
	{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5},

	/* 6+ */
	{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5},

	/* 8+ */
	{  1,   2,   2,   3,   4,   4,   4,   5,   5,   5,   5,   5},

	/* 10+ */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6},

	/* 13+ */
	{  2,   3,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6},

	/* 15+ */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6},

	/* 18+ */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6},

	/* 20+ */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6}
};

#endif



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
	},
	{
		/* Player shop */
		{ "Player shop",			0,      100, 100,  0, 99, 99},
		{ "Player shop",			0,      100, 100,  0, 99, 99},
		{ "Player shop",			0,      100, 100,  0, 99, 99},
		{ "Player shop",			0,      100, 100,  0, 99, 99}
	}
};




/*
 * This table allows quick conversion from "speed" to "energy"
 * The basic function WAS ((S>=110) ? (S-110) : (100 / (120-S)))
 * Note that table access is *much* quicker than computation.
 *
 * Note that the table has been changed at high speeds.  From
 * "Slow (-40)" to "Fast (+30)" is pretty much unchanged, but
 * at speeds above "Fast (+30)", one approaches an asymptotic
 * effective limit of 50 energy per turn.  This means that it
 * is relatively easy to reach "Fast (+30)" and get about 40
 * energy per turn, but then speed becomes very "expensive",
 * and you must get all the way to "Fast (+50)" to reach the
 * point of getting 45 energy per turn.  After that point,
 * furthur increases in speed are more or less pointless,
 * except to balance out heavy inventory.
 *
 * Note that currently the fastest monster is "Fast (+30)".
 *
 * It should be possible to lower the energy threshhold from
 * 100 units to 50 units, though this may interact badly with
 * the (compiled out) small random energy boost code.  It may
 * also tend to cause more "clumping" at high speeds.
 */
byte extract_energy[200] =
{
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-50 */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-40 */     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	/* S-30 */     2,  2,  2,  2,  2,  2,  2,  3,  3,  3,
	/* S-20 */     3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
	/* S-10 */     5,  5,  5,  5,  6,  6,  7,  7,  8,  9,
	/* Norm */    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	/* F+10 */    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	/* F+20 */    30, 31, 32, 33, 34, 35, 36, 36, 37, 37,
	/* F+30 */    38, 38, 39, 39, 40, 40, 40, 41, 41, 41,
	/* F+40 */    42, 42, 42, 43, 43, 43, 44, 44, 44, 44,
	/* F+50 */    45, 45, 45, 45, 45, 46, 46, 46, 46, 46,
	/* F+60 */    47, 47, 47, 47, 47, 48, 48, 48, 48, 48,
	/* F+70 */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Fast */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
};


/*
 * This table provides for different game speeds at different
 * dungeon depths.  Shallower depths are faster, allowing for
 * easier town navigation.  Deeper depths are slow, hopefully
 * make deep combat less of a test of reflexs.
 */
byte level_speeds[128] =
{
	 75,  90,  91,  92,  93,  94,  95,  96,  97,  98, /* Town - 450' */
	 99, 100, 100, 100, 100, 100, 101, 102, 103, 104, /* 500' - 950' */
	105, 106, 107, 108, 109, 110, 111, 112, 113, 114, /* 1000' - 1450' */
	115, 116, 117, 118, 119, 120, 121, 122, 123, 124, /* 1500' - 1950' */
	125, 126, 127, 128, 129, 130, 131, 132, 133, 134, /* 2000' - 2450' */
	135, 137, 138, 139, 140, 142, 143, 144, 146, 148, /* 2500' - 2950' */
	150, 152, 154, 156, 158, 160, 162, 164, 166, 168, /* 3000' - 3450' */
	170, 172, 174, 176, 178, 180, 182, 184, 186, 188, /* 3500' - 3950' */
	190, 192, 194, 196, 198, 200, 200, 200, 200, 200, /* 4000' - 4450' */
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, /* 4500' - 4950' */
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, /* 5000' - 5450' */
	200, 200, 200, 200, 200, 200, 200, 200, 200, 200, /* 5500' - 5950' */
	200, 200, 200, 200, 200, 200, 200, 200            /* 6000' - 6350' */
};





/*
 * Base experience levels, may be adjusted up for race and/or class
 */
s32b player_exp[PY_MAX_LEVEL] =
{
	10,
	25,
	45,
	70,
	100,
	140,
	200,
	280,
	380,
	500,
	650,
	850,
	1100,
	1400,
	1800,
	2300,
	2900,
	3600,
	4400,
	5400,
	6800,
	8400,
	10200,
	12500,
	17500,
	25000,
	35000L,
	50000L,
	75000L,
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,
	450000L,
	550000L,
	700000L,
	850000L,
	1000000L,
	1250000L,
	1500000L,
	1800000L,
	2100000L,
	2400000L,
	2700000L,
	3000000L,
	3500000L,
	4000000L,
	4500000L,
	5000000L
};


/*
 * Player Sexes
 *
 *	Title,
 *	Winner
 */
const player_sex sex_info[MAX_SEXES] =
{
	{
		"Female",
		"Queen"
	},

	{
		"Male",
		"King"
	}
};


/*
 * Note that this is the same for all classes (for now).
 *
 * Such a hack....
 */
magic_type ghost_spells[PY_MAX_SPELLS] =
{
	{  1,   1, 0, 0},
	{ 10,   2, 0, 0},
	{ 15,   3, 0, 0},
	{ 20,   5, 0, 0},
	{ 25,  10, 0, 0},
	{ 35,  60, 0, 0},
	{ 45, 100, 0, 0},

	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0},
	{ 99,   0, 0, 0}
};

/*
 * Spells in each book (mage spells then priest spells)
 */
u32b spell_flags[2][9][2] =
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
    }

};


/*
 * Names of the spells (mage spells then priest spells)
 */
cptr spell_names[3][PY_MAX_SPELLS] =
{
	/*** Mage Spells ***/

	{
	  "Magic Missile",
	  "Detect Monsters",
	  "Phase Door",
	  "Light Area",
	  "Detect Treasure",
	  "Cure Light Wounds",
	  "Detect Objects",
	  "Find Hidden Traps/Doors",
	  "Stinking Cloud",

	  "Confuse Monster",
	  "Lightning Bolt",
	  "Trap/Door Destruction",
	  "Cure Poison",
	  "Sleep Monster",
	  "Teleport Self",
	  "Spear of Light",
	  "Frost Bolt",
	  "Wonder",

	  "Satisfy Hunger",
	  "Lesser Recharging",
	  "Turn Stone to Mud",
	  "Fire Bolt",
	  "Polymorph Other",
	  "Identify",
	  "Detect Invisible",
	  "Acid Bolt",
	  "Slow Monster",

	  "Frost Ball",
	  "Teleport Other",
	  "Haste Self",
	  "Mass Sleep",
	  "Fire Ball",
	  "Detect Enchantment",

	  "Resist Cold",
	  "Resist Fire",
	  "Resist Poison",
	  "Resistance",
	  "Shield",

	  "Shock Wave",
	  "Explosion",
	  "Cloudkill",
	  "Acid Ball",
	  "Ice Storm",
	  "Meteor Swarm",
	  "Rift",

	  "Door Creation",
	  "Stair Creation",
	  "Teleport Level",
	  "Word of Recall",
	  "Rune of Protection",

	  "Heroism",
	  "Berserker",
	  "Enchant Armor",
	  "Enchant Weapon",
	  "Greater Recharging",
	  "Elemental Brand",

	  "Earthquake",
	  "Bedlam",
	  "Rend Soul",
	  "Banishment",
	  "Word of Destruction",
	  "Mass Banishment",
	  "Chaos Strike",
	  "Mana Storm"
	},


	/*** Priest Spells ***/

	{
		/* Beginners Handbook (sval 0) */
		"Detect Evil",
		"Cure Light Wounds",
		"Bless",
		"Remove Fear",
		"Call Light",
		"Find Traps",
		"Detect Doors/Stairs",
		"Slow Poison",

		/* Words of Wisdom (sval 1) */
		"Scare Monster",
		"Portal",
		"Cure Serious Wounds",
		"Chant",
		"Sanctuary",
		"Satisfy Hunger",
		"Remove Curse",
		"Resist Heat and Cold",

		/* Chants and Blessings (sval 2) */
		"Neutralize Poison",
		"Orb of Draining",
		"Cure Critical Wounds",
		"Sense Invisible",
		"Protection from Evil",
		"Earthquake",
		"Sense Surroundings",
		"Cure Mortal Wounds",
		"Turn Undead",

		/* Exorcism and Dispelling (sval 3) */
		"Prayer",
		"Dispel Undead",
		"Heal",
		"Dispel Evil",
		"Glyph of Warding",
		"Holy Word",

		/* Godly Insights... (sval 5) */
		"Detect Monsters",
		"Detection",
		"Perception",
		"Probing",
		"Clairvoyance",

		/* Purifications and Healing (sval 6) */
		"Cure Serious Wounds",
		"Cure Mortal Wounds",
		"Healing",
		"Restoration",
		"Remembrance",

		/* Wrath of God (sval 8) */
		"Dispel Undead",
		"Dispel Evil",
		"Banish Evil",
		"Word of Destruction",
		"Annihilation",

		/* Holy Infusions (sval 7) */
		"Unbarring Ways",
		"Recharging",
		"Dispel Curse",
		"Enchant Weapon",
		"Enchant Armour",
		"Elemental Brand",

		/* Ethereal openings (sval 4) */
		"Blink",
		"Teleport Self",
		"Teleport Other",
		"Teleport Level",
		"Word of Recall",
		"Alter Reality",

		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)"
    },

	/*** Ghost abilities ***/
	{
		/* Standard set */
		"Blink",
		"Terrify",
		"Confuse",
		"Teleport",
		"Nether bolt",
		"Nether ball",
		"Darkness storm",

		/* XXX Unimplemented */
		
		/* Lich spell set (dead mage?) */
		"Teleport",
		"Paralyze",
		"Steal mana",
		"Cause wounds",
		"Steal experience",
		"Mind smash",
		"Mana storm",

		/* Death knight set (dead paladin?) */
		"Blind",
		"Terrify",
		"Summon zombie", /* XXX XXX */
		"Nether bolt",

		/* These may be used at some later date */
		/* Perhaps for different classes of ghosts */
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)",
		"(blank)"
	}
};



/*
 * Each chest has a certain set of traps, determined by pval
 * Each chest has a "pval" from 1 to the chest level (max 55)
 * If the "pval" is negative then the trap has been disarmed
 * The "pval" of a chest determines the quality of its treasure
 * Note that disarming a trap on a chest also removes the lock.
 */
byte chest_traps[64] =
{
	0,					/* 0 == empty */
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),			/* 5 == best small wooden */
	0,
	(CHEST_POISON),
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_POISON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),			/* 15 == best large wooden */
	0,
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_EXPLODE),			/* 25 == best small iron */
	0,
	(CHEST_POISON | CHEST_LOSE_STR),
	(CHEST_POISON | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_POISON | CHEST_SUMMON),
	(CHEST_SUMMON),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),	/* 35 == best large iron */
	0,
	(CHEST_SUMMON),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),	/* 45 == best small steel */
	0,
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_POISON | CHEST_PARALYZE),	/* 55 == best large steel */
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
};








/*
 * Class titles for the player.
 *
 * The player gets a new title every five levels, so each class
 * needs only ten titles total.
 */
cptr player_title[MAX_CLASS][PY_MAX_LEVEL/5] =
{
	/* Warrior */
	{
		"Rookie",
		"Soldier",
		"Mercenary",
		"Veteran",
		"Swordsman",
		"Champion",
		"Hero",
		"Baron",
		"Duke",
		"Lord",
	},

	/* Mage */
	{
		"Novice",
		"Apprentice",
		"Trickster",
		"Illusionist",
		"Spellbinder",
		"Evoker",
		"Conjurer",
		"Warlock",
		"Sorcerer",
		"Mage Lord",
	},

	/* Priest */
	{
		"Believer",
		"Acolyte",
		"Adept",
		"Curate",
		"Canon",
		"Lama",
		"Patriarch",
		"Priest",
		"High Priest",
		"Priest Lord",
	},

	/* Rogues */
	{
		"Vagabond",
		"Cutpurse",
		"Robber",
		"Burglar",
		"Filcher",
		"Sharper",
		"Low Thief",
		"High Thief",
		"Master Thief",
		"Assassin",
	},

	/* Rangers */
	{
		"Runner",
		"Strider",
		"Scout",
		"Courser",
		"Tracker",
		"Guide",
		"Pathfinder",
		"Low Ranger",
		"High Ranger",
		"Ranger Lord",
	},

	/* Paladins */
	{
		"Gallant",
		"Keeper",
		"Protector",
		"Defender",
		"Warder",
		"Knight",
		"Guardian",
		"Low Paladin",
		"High Paladin",
		"Paladin Lord",
    }

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
 * Full stat names
 */
cptr stat_names_full[A_MAX] =
{
	"strength",
	"intelligence",
	"wisdom",
	"dexterity",
	"constitution",
	"charisma"
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

