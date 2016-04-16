/* File: store.c */

/* Purpose: Store commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define SERVER

#include "angband.h"

#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] =
{
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
};

/*
 * Successful haggle.
 */
static void say_comment_1(int Ind)
{
	msg_print(Ind, comment_1[rand_int(MAX_COMMENT_1)]);
}

/*
 * We store the current "store number" here so everyone can access it
 */
static int store_num = 7;

/*
 * Buying and selling adjustments for race combinations.
 * Entry[owner][player] gives the basic "cost inflation".
 */
static byte rgold_adj[MAX_RACES][MAX_RACES] =
{
    /*Hum, HfE, Elf, Hal, Gno, Dwa, HfO, HfT, Dun, HiE */

	/* Human */
    { 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 120 },

	/* Half-Elf */
    { 110, 100, 100, 105, 110, 120, 125, 130, 110, 100, 115 },

	/* Elf */
    { 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 115 },

	/* Halfling */
    { 115, 110, 105,  95, 105, 110, 115, 130, 115, 105, 115 },

	/* Gnome */
    { 115, 115, 110, 105,  95, 110, 115, 130, 115, 110, 125 },

	/* Dwarf */
    { 115, 120, 120, 110, 110,  95, 125, 135, 115, 120, 115 },

	/* Half-Orc */
    { 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 110 },

	/* Half-Troll */
    { 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110 },

	/* Dunedain  */
    { 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 120 },

	/* High_Elf */
    { 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 125 },

	/* Kobold */
    { 110, 115, 120, 110, 105, 110, 110, 120, 110, 125,  90 }
};




/*
 * Determine the price of an item (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * item, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
static s32b price_item(int Ind, object_type *o_ptr, int greed, bool flip)
{
	player_type *p_ptr = Players[Ind];
	owner_type *ot_ptr = &owners[p_ptr->store_num][store[p_ptr->store_num].owner];
	int     factor;
	int     adjust;
	s32b    price;


	/* Get the value of one of the items */
	price = object_value(Ind, o_ptr);
	
	/* Use sellers asking price as base price in player owned shops */
	if (p_ptr->store_num == 8)
	{
		price = price*3;
		if (o_ptr->askprice > price)
		{
			price = o_ptr->askprice;
		}
		return ((price <= 0)? 0L: price);
	}

	/* Worthless items */
	if (price <= 0) return (0L);


	/* Compute the racial factor */
	factor = rgold_adj[ot_ptr->owner_race][p_ptr->prace];

	/* Add in the charisma factor */
	factor += adj_chr_gold[p_ptr->stat_ind[A_CHR]];


	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (p_ptr->store_num == 6) price = price / 3;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* The black market is expensive */
		if (p_ptr->store_num == 6) price = price * 3;
	}

	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Special "mass production" computation
 */
static int mass_roll(int num, int max)
{
	int i, t = 0;
	for (i = 0; i < num; i++) t += rand_int(max);
	return (t);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
static void mass_produce(object_type *o_ptr)
{
	int size = 1;
	int discount = 0;

	s32b cost = object_value(0, o_ptr);


	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			if (cost <= 5L) size += mass_roll(3, 5);
			if (cost <= 20L) size += mass_roll(3, 5);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if (cost <= 60L) size += mass_roll(3, 5);
			if (cost <= 240L) size += mass_roll(1, 5);
			break;
		}

		case TV_MAGIC_BOOK:
		case TV_PRAYER_BOOK:
		{
			if (cost <= 50L) size += mass_roll(2, 3);
			if (cost <= 500L) size += mass_roll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SHIELD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		case TV_BOW:
		{
			if (o_ptr->name2) break;
			if (cost <= 10L) size += mass_roll(3, 5);
			if (cost <= 100L) size += mass_roll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (cost <= 5L) size += mass_roll(5, 5);
			if (cost <= 50L) size += mass_roll(5, 5);
			if (cost <= 500L) size += mass_roll(5, 5);
			break;
		}
	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (rand_int(50) == 0)
	{
		discount = 25;
	}
	else if (rand_int(300) == 0)
	{
		discount = 50;
	}
	else if (rand_int(600) == 0)
	{
		discount = 75;
	}
	else if (rand_int(1000) == 0)
	{
		discount = 90;
	}


	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);
}

/*
 * Determine if a store item can "absorb" another item
 *
 * See "object_similar()" for the same function for the "player"
 */
static bool store_object_similar(object_type *o_ptr, object_type *j_ptr)
{
	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);

	/* Different charges (etc) cannot be stacked */
	if (o_ptr->pval != j_ptr->pval) return (0);

	/* Require many identical values */
	if (o_ptr->to_h  !=  j_ptr->to_h) return (0);
	if (o_ptr->to_d  !=  j_ptr->to_d) return (0);
	if (o_ptr->to_a  !=  j_ptr->to_a) return (0);

	/* Require identical "artifact" names */
	if (o_ptr->name1 != j_ptr->name1) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2) return (0);

    /* Require identical "random artifact" names */
    if (o_ptr->name3 != j_ptr->name3) return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	/* Hack -- Never stack recharging items */
	if (o_ptr->timeout || j_ptr->timeout) return (0);

	/* Require many identical values */
	if (o_ptr->ac    !=  j_ptr->ac)   return (0);
	if (o_ptr->dd    !=  j_ptr->dd)   return (0);
	if (o_ptr->ds    !=  j_ptr->ds)   return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);

	/* Require matching discounts */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store item to absorb another item
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;
}


/*
 * Check to see if the shop will be carrying too many objects	-RAK-
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.  Before, one could "nuke" potions this way.
 */
static bool store_check_num(int st, object_type *o_ptr)
{
	int        i;
	object_type *j_ptr;
	store_type *st_ptr = &store[st];

	/* Free space is always usable */
	if (st_ptr->stock_num < st_ptr->stock_size) return TRUE;

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* But there was no room at the inn... */
	return (FALSE);
}




/*
 * Determine if the current store will purchase the given item
 *
 * Note that a shop-keeper must refuse to buy "worthless" items
 */
static bool store_will_buy(int Ind, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];
        u32b            f1,f2,f3;

	/* Switch on the store */
	switch (p_ptr->store_num)
	{
		/* General Store */
		case 0:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_FOOD:
				case TV_LITE:
				case TV_FLASK:
				case TV_SPIKE:
				case TV_SHOT:
				case TV_ARROW:
				case TV_BOLT:
				case TV_DIGGING:
				case TV_CLOAK:
					break;
				default:
					return (FALSE);
			}
			break;
		}

		/* Armoury */
		case 1:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_BOOTS:
				case TV_GLOVES:
				case TV_CROWN:
				case TV_HELM:
				case TV_SHIELD:
				case TV_CLOAK:
				case TV_SOFT_ARMOR:
				case TV_HARD_ARMOR:
				case TV_DRAG_ARMOR:
					break;
				default:
					return (FALSE);
			}
			break;
		}

		/* Weapon Shop */
		case 2:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SHOT:
				case TV_BOLT:
				case TV_ARROW:
				case TV_BOW:
				case TV_DIGGING:
				case TV_HAFTED:
				case TV_POLEARM:
				case TV_SWORD:
					break;
				default:
					return (FALSE);
			}
			break;
		}

		/* Temple */
		case 3:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_PRAYER_BOOK:
				case TV_SCROLL:
				case TV_POTION:
				case TV_HAFTED:
					break;
				case TV_POLEARM:
                                case TV_SWORD:
					/* if Known, aware, and Blessed, yes. */
					if (object_aware_p(Ind, o_ptr)) {
						if (object_known_p(Ind, o_ptr)){
                            object_flags(o_ptr, &f1, &f2, &f3);
							if (f3 & TR3_BLESSED) break;
						}
					}
					return FALSE;
				default:
				return (FALSE);
			}
			break;
		}

		/* Alchemist */
		case 4:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SCROLL:
				case TV_POTION:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Magic Shop */
		case 5:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_MAGIC_BOOK:
				case TV_AMULET:
				case TV_RING:
				case TV_STAFF:
				case TV_WAND:
				case TV_ROD:
				case TV_SCROLL:
				case TV_POTION:
				break;
				default:
				return (FALSE);
			}
			break;
		}
	}

	/* XXX XXX XXX Ignore "worthless" items */
	if (object_value(Ind, o_ptr) <= 0) return (FALSE);

	/* Assume okay */
	return (TRUE);
}

/*
 * Add the item "o_ptr" to a real stores inventory.
 *
 * If the item is "worthless", it is thrown away (except in the home).
 *
 * If the item cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.  Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an item to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(int st, object_type *o_ptr)
{
	int		i, slot;
	s32b	value, j_value;
	object_type	*j_ptr;
	store_type *st_ptr = &store[st];


	/* Evaluate the object */
	value = object_value(0, o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	if (value <= 0) return (-1);
	
	/* Artifacts "disappear" when sold */
	if (artifact_p(o_ptr))
	{
		/* Mark the artifact so it can be found again */
		a_info[o_ptr->name1].cur_num = 0;
		return (-1);
	}

	/* Erase the inscription for normal shops */
	o_ptr->note = 0;

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* In the "back room" we only combine items from the same seller */
			if (st == 8)
			{
				if (!strcmp(quark_str(j_ptr->note),quark_str(o_ptr->note)))
				{
					/* Hack -- extra items disappear */
					store_object_absorb(j_ptr, o_ptr);
			
					/* All done */
					return (slot);
				}
			}
			else
			{
				/* Hack -- extra items disappear */
				store_object_absorb(j_ptr, o_ptr);
			
				/* All done */
				return (slot);
			}
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/* Evaluate that slot */
		j_value = object_value(0, j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i-1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.  This can result in zero items.
 */
static void store_item_increase(int st, int item, int num)
{
	int         cnt;
	object_type *o_ptr;
	store_type *st_ptr = &store[st];

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255) cnt = 255;
	else if (cnt < 0) cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int st, int item)
{
	int         j;
	object_type *o_ptr;
	store_type *st_ptr = &store[st];

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx) return;

	/* Must have no items */
	if (o_ptr->number) return;

	/* One less item */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	invwipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any item that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(object_type *o_ptr)
{
	int		i, j;

	/* Ego items are never crap */
	if (o_ptr->name2) return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check the other "normal" stores */
	for (i = 0; i < 6; i++)
	{
		/* Check every item in the store */
		for (j = 0; j < store[i].stock_num; j++)
		{
			object_type *j_ptr = &store[i].stock[j];

			/* Duplicate item "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx) return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */

static void store_delete(int st)
{
	store_type *st_ptr = &store[st];
	int what, num;

	/* Pick a random slot */
	what = rand_int(st_ptr->stock_num);

	/* Determine how many items are here */
	num = st_ptr->stock[what].number;

	/* Hack -- sometimes, only destroy half the items */
	if (rand_int(100) < 50) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if (rand_int(100) < 50) num = 1;

	/* Hack -- preserve artifacts */
	if (artifact_p(&st_ptr->stock[what]))
	{
		/* Preserve this one */
		a_info[st_ptr->stock[what].name1].cur_num = 0;
	}

	/* Actually destroy (part of) the item */
	store_item_increase(st, what, -num);
	store_item_optimize(st, what);
}

/*
 * Creates a random item and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 * Currently, "normal" stores use a pre-built array.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given item?
 */
static void store_create(int st)
{
	store_type *st_ptr = &store[st];
	int			i, tries, level;
	object_type		tmp_obj;
	object_type		*o_ptr = &tmp_obj;

	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= st_ptr->stock_size) return;

	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		/* Black Market */
		if (store_num == 6)
		{
			/* Pick a level for object/magic */
			level = 30 + rand_int(25);
			//level = 25;

			/* Random item (usually of given level) */
			i = get_obj_num(level);

			/* Handle failure */
			if (!i) continue;
		}
		
		/* Normal Store */
		else
		{
			/* Hack -- Pick an item to sell */
			i = st_ptr->table[rand_int(st_ptr->table_num)];

			/* Hack -- fake level for apply_magic() */
			level = rand_range(1, STORE_OBJ_LEVEL);
		}

		/* Create a new object of the chosen kind */
		invcopy(o_ptr, i);

		/* Apply some "low-level" magic (no artifacts) */
		/* Boost BM to bypass the "good" roll */
		if (store_num == 6)
			apply_magic(0, o_ptr, level, FALSE, TRUE, FALSE);
		else
			apply_magic(0, o_ptr, level, FALSE, FALSE, FALSE);

		/* Hack -- Charge lite's */
		if (o_ptr->tval == TV_LITE)
		{
			if (o_ptr->sval == SV_LITE_TORCH) o_ptr->pval = FUEL_TORCH / 2;
			if (o_ptr->sval == SV_LITE_LANTERN) o_ptr->pval = FUEL_LAMP / 2;
		}

		/* The item is "known" */
		object_known(o_ptr);

		/* Mega-Hack -- no chests in stores */
		if (o_ptr->tval == TV_CHEST) continue;

		/* Prune the black market */
		if (store_num == 6)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(o_ptr)) continue;

			/* Hack -- No "cheap" items */
			if (object_value(0, o_ptr) < 40) continue;
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(0, o_ptr) <= 0) continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(o_ptr);

		/* Attempt to carry the (known) item */
		(void)store_carry(st, o_ptr);

		/* Definitely done */
		break;
	}
}

/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(s32b minprice)
{
	/* Hack -- We never haggle anyway --KLJ-- */
	return (TRUE);

}

/*
 * Re-displays a single store entry
 *
 * Actually re-sends a single store entry --KLJ--
 */
static void display_entry(int Ind, int pos)
{
	player_type *p_ptr = Players[Ind];
	store_type *st_ptr = &store[p_ptr->store_num];
	owner_type *ot_ptr = &owners[p_ptr->store_num][st_ptr->owner];

	object_type		*o_ptr;
	s32b		x;

	char		o_name[80];
	byte		attr;
	int		wgt;

	int maxwid = 75;

	/* Get the item */
	o_ptr = &st_ptr->stock[pos];

	/* Must leave room for the "price" */
	maxwid = 65;

	/* Describe the object (fully) */
	object_desc_store(Ind, o_name, o_ptr, TRUE, 3);
	o_name[maxwid] = '\0';

	attr = tval_to_attr[o_ptr->tval];

	/* Only show the weight of an individual item */
	wgt = o_ptr->weight;

	/* Extract the "minimum" price */
	x = price_item(Ind, o_ptr, ot_ptr->min_inflate, FALSE);

	/* Send the info */
	Send_store(Ind, pos, attr, wgt, o_ptr->number, x, o_name);
}

/* 
 * Send a player owned store entry
 */
static void display_entry_live(int Ind, int pos, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];
	store_type *st_ptr = &store[p_ptr->store_num];
	owner_type *ot_ptr = &owners[p_ptr->store_num][st_ptr->owner];
	u32b		x;
	char		o_name[80];
	byte		attr;
	int			wgt;
	int 		maxwid = 75;

	/* Must leave room for the "price" */
	maxwid = 65;

	/* Describe the object (fully) */
	object_desc_store(Ind, o_name, o_ptr, TRUE, 3);
	o_name[maxwid] = '\0';

	attr = tval_to_attr[o_ptr->tval];

	/* Only show the weight of an individual item */
	wgt = o_ptr->weight;

	/* Extract the item price */
	if (house_owned_by(Ind, p_ptr->player_store_num))
	{
		/* Viewing our own shop - the price we will get */
		x = price_item(Ind, o_ptr, ot_ptr->min_inflate, FALSE);
		x = (x * 90) / 100;
	}
	else
	{
		/* Viewing someone else's shop - the price we will pay */
		x = price_item(Ind, o_ptr, ot_ptr->min_inflate, FALSE);
	}


	/* Send the info */
	Send_store(Ind, pos, attr, wgt, o_ptr->number, x, o_name);
}

/*
 * Displays a store's inventory			-RAK-
 * All prices are listed as "per individual object".  -BEN-
 *
 * The inventory is "sent" not "displayed". -KLJ-
 * Returns the number of items listed
 */
static int display_inventory(int Ind)
{
	player_type 	*p_ptr = Players[Ind];
	store_type 		*st_ptr = &store[p_ptr->store_num];
	int 			k,x,y,stocked;
	object_type		tmp_obj;
	object_type		*o_ptr = &tmp_obj;
	cave_type		*c_ptr;
	char 			*c;

	/* Normal stores */
	if (p_ptr->store_num != 8)
	{

	    /* Display the items */
    	for (k = 0; k < STORE_INVEN_MAX; k++)
		{
			/* Do not display "dead" items */
			if (k >= st_ptr->stock_num) break;
	
			/* Display that line */
			display_entry(Ind, k);
		}
		return (st_ptr->stock_num);
	}
	
	/* Player owned stores */
	else
	{
		/* Send a "live" inventory */
		
		/* Scan house */
		stocked = 0;
		for(y=houses[p_ptr->player_store_num].y_1; y<=houses[p_ptr->player_store_num].y_2;y++)
		{
			for(x=houses[p_ptr->player_store_num].x_1; x<=houses[p_ptr->player_store_num].x_2;x++)
			{
				/* Get grid */
				c_ptr = &cave[houses[p_ptr->player_store_num].depth][y][x];

				/* Get the object (if any) */
				if (c_ptr->o_idx)
				{
					tmp_obj = o_list[c_ptr->o_idx];
				}
				else
				{
					continue;
				}
						
				/* If there was an object, is it for sale? */
				if (o_ptr->note)
				{
							
					/* Is this item for sale? */
					if((c = strstr(quark_str(o_ptr->note),"for sale")))
					{
						/* Get ask price */
						c += 8; /* skip "for sale" */
						if( *c == ' ' )
						{
							o_ptr->askprice = atoi(c);
						}

						/* List it in the store */
						object_known(o_ptr);
						//o_ptr->number = 1;
						/* Remove any inscription */
						o_ptr->note = 0;
						display_entry_live(Ind, stocked, o_ptr);
						stocked++;
						/* Limited space available */
						if (stocked >= STORE_INVEN_MAX)
						{
							return (stocked);
						}
					}
				}					
			}
		}
	}
	return (stocked);
}


/*
 * Displays players gold					-RAK-
 */
static void store_prt_gold(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_gold(Ind, p_ptr->au);
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
static void display_store(int Ind)
{
	int stockcount;
	player_type *p_ptr = Players[Ind];
	store_type *st_ptr = &store[p_ptr->store_num];

	/* Send the inventory */
	stockcount = display_inventory(Ind);

	/* Send the store info for normal stores */
	if (p_ptr->store_num != 8)
	{
		Send_store_info(Ind, p_ptr->store_num, st_ptr->owner, stockcount);
	}
	/* Player owned stores */
	else
	{
		Send_player_store_info(Ind, p_ptr->store_num, 
			houses[p_ptr->player_store_num].owned, stockcount);		
	}

}



/*
 * Haggling routine					-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool sell_haggle(int Ind, object_type *o_ptr, s32b *price)
{
	player_type *p_ptr = Players[Ind];
	store_type *st_ptr = &store[p_ptr->store_num];

	owner_type *ot_ptr = &owners[p_ptr->store_num][st_ptr->owner];

	s32b               purse, cur_ask, final_ask;

	int			noneed;

	cptr		pmt = "Offer";


	*price = 0;


	/* Obtain the starting offer and the final offer */
	cur_ask = price_item(Ind, o_ptr, ot_ptr->max_inflate, TRUE);
	final_ask = price_item(Ind, o_ptr, ot_ptr->min_inflate, TRUE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* Get the owner's payout limit */
	purse = (s32b)(ot_ptr->max_cost);

	/* No need to haggle */
	if (noneed || TRUE || (final_ask >= purse))
	{
		/* No reason to haggle */
		if (final_ask >= purse)
		{
			/* Message */
			msg_print(Ind, "You instantly agree upon the price.");
			/*msg_print(NULL);*/

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message */
			msg_print(Ind, "You eventually agree upon the price.");
			/*msg_print(NULL);*/
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		pmt = "Final Offer";
	}

	/* Hack -- Return immediately */
	*price = final_ask * o_ptr->number;
	return (FALSE);
}

/*
 * Remove the given item from the players house who owns it and credit
 * this player with some gold for the transaction. Return the number of
 * matching items removed from houses.
 */
int sell_player_item(int Ind, object_type *o_ptr_shop, int number, s32b gold)
{
	player_type *p_ptr = Players[Ind];
	int			x,y,sold,spacex,spacey,spacedepth;
	object_type		*o_ptr;
	cave_type		*c_ptr;
	cave_type		*c_ptr_gold;
	object_type		gold_obj;
	u32b			total;
	bool			have_gold, have_space;
	char			*c;
	object_type object_type_body;
	object_type *i_ptr = &object_type_body;
	
	/* Search for the item in the player house(s) */
	sold = 0;
	have_gold = FALSE;
	have_space = FALSE;

	/* Scan house */
	for(y=houses[p_ptr->player_store_num].y_1; y<=houses[p_ptr->player_store_num].y_2;y++)
	{
		for(x=houses[p_ptr->player_store_num].x_1; x<=houses[p_ptr->player_store_num].x_2;x++)
		{
			/* Get grid */
			c_ptr = &cave[houses[p_ptr->player_store_num].depth][y][x];

			/* Get the object (if any) */
			if (c_ptr->o_idx)
			{
				o_ptr = &o_list[c_ptr->o_idx];
				/* If this is a pile of gold, remember it as we can drop gold here */
				if (o_ptr->tval == TV_GOLD && !have_gold)
				{
					c_ptr_gold = c_ptr;
					have_gold = TRUE;
				}
			}
			else
			{
				/* Remember this emtpy space because we can drop gold here */
				if (!have_space)
				{
					spacex = x;
					spacey = y;
					spacedepth = houses[p_ptr->player_store_num].depth;
					have_space = TRUE;
				}
				continue;
			}
					
			/* Finished with items but still need space to drop gold? */
			if (!number) continue;
						
			/* Is this item for sale? */
			if (!o_ptr->note) continue;
			if((c = strstr(quark_str(o_ptr->note),"for sale")))
			{
				/* Get ask price */
				c += 8; /* skip "for sale" */
				if( *c == ' ' )
				{
					o_ptr->askprice = atoi(c);
				}


				/* Is this the item we've sold? We must be careful that we have found the correct 
				 * item match or else this could be exploited. Even so, I can't help thinking there
				 * must be a better way to do this */
				if (o_ptr_shop->tval == o_ptr->tval && o_ptr_shop->sval == o_ptr->sval &&
				o_ptr_shop->bpval == o_ptr->bpval && o_ptr_shop->pval == o_ptr->pval &&
				o_ptr_shop->discount == o_ptr->discount && o_ptr_shop->name1 == o_ptr->name1 &&
				o_ptr_shop->name2 == o_ptr->name2 && o_ptr_shop->name3 == o_ptr->name3 &&
				o_ptr_shop->xtra1 == o_ptr->xtra1 && o_ptr_shop->xtra2 == o_ptr->xtra2 &&
				o_ptr_shop->to_h == o_ptr->to_h && o_ptr_shop->to_d == o_ptr->to_d &&
				o_ptr_shop->to_a == o_ptr->to_a && o_ptr_shop->ac == o_ptr->ac &&
				o_ptr_shop->dd == o_ptr->dd && o_ptr_shop->ds == o_ptr->ds &&
				o_ptr_shop->askprice == o_ptr->askprice)
				{
	
					/* Found a matching item */
					if (o_ptr->number <= number)
					{
						number -= o_ptr->number;
						sold += o_ptr->number;
						/* Remove the item(s) and keep searching if required */
						delete_object(houses[p_ptr->player_store_num].depth,y,x);
						/* Remember this emtpy space because we can drop gold here */
						if (!have_space)
						{
							spacex = x;
							spacey = y;
							spacedepth = houses[p_ptr->player_store_num].depth;
							have_space = TRUE;
						}
					}
					else if (o_ptr->number > number)
					{
						/* Reduce the pile of items */
						o_ptr->number -= number;
						sold += number;
						number = 0;
					}						
				}
			}
		}
		/* Done aleady? */
		if (!number && (have_gold || have_space)) break;
	}	

	/* Bail out if there was a problem with the sale */
	if (!sold) return(sold);
	
	/* Horrible hack -- use a known copy of the object to get its value */
	/* All this mess NEEDS to be rewritten */
	COPY(i_ptr, o_ptr_shop, object_type);
	object_known(i_ptr);
	
	/* How much total gold is the seller receiving? */
	total = object_value(Ind, i_ptr) * 3;
	if (o_ptr_shop->askprice > total) total = o_ptr_shop->askprice;
	total = total * sold;
	/* Small sales tax */
	total = (total * 90) / 100;
	
	/* Hack -- incase payment is '0 gold' (worthless item) */
	if (!total) have_space = FALSE;

	/* Did we find a pile of gold suitable for leaving a deposit? */
	if (have_gold)
	{
		/* Add some gold to the pile */
		o_ptr = &o_list[c_ptr_gold->o_idx];
		o_ptr->pval += total;
	}
	/* No existing gold pile, hopefully we found some space */
	else if (have_space)
	{
		/* Make some gold */
		invcopy(&gold_obj, lookup_kind(TV_GOLD,SV_PLAYER_GOLD));
		/* How much gold to leave */
		gold_obj.pval = total;
		/* Put it in the house */
		drop_near(&gold_obj,0,spacedepth,spacey,spacex);
	}
	/* Oh dear, no space for payment! */
	else
	{
		/* What to do now? We have already removed the items from the house. */
		/* XXX Do nothing, the seller should ensure available space for payment */
	}

	return(sold);
}

/*
 * Find an item in store, set it to o_ptr, return true on success
 */
bool get_store_item(int Ind, int item, object_type *i_ptr)
{
	bool 		found;
	player_type *p_ptr = Players[Ind];
	int st = p_ptr->store_num;
	store_type *st_ptr = &store[st];
	cave_type		*c_ptr;
	char			*c;	
	int stocked,x,y;
	object_type		*o_ptr;
	
	/* Player owned stores */
	if (p_ptr->store_num == 8)
	{
		/* Scan the store to find the item */
		stocked = 0;
		found = FALSE;
		for(y=houses[p_ptr->player_store_num].y_1; y<=houses[p_ptr->player_store_num].y_2;y++)
		{
			for(x=houses[p_ptr->player_store_num].x_1; x<=houses[p_ptr->player_store_num].x_2;x++)
			{
				/* Get grid */
				c_ptr = &cave[houses[p_ptr->player_store_num].depth][y][x];

				/* Get the object (if any) */
				if (c_ptr->o_idx)
				{
					o_ptr = &o_list[c_ptr->o_idx];
					/* If there was an object, is it for sale? */
					if (o_ptr->note)
					{
						/* Is this item for sale? */
						if((c = strstr(quark_str(o_ptr->note),"for sale")))
						{
							/* Is this the item we are looking for? */
							if (item == stocked)
							{
								/* Get ask price */
								c += 8; /* skip "for sale" */
								if( *c == ' ' )
								{
									o_ptr->askprice = atoi(c);
								}

								found = TRUE; 
								break;
							}
							/* Keep looking */
							stocked++;
						}
					}
				}					
			}
			if (found) break;
		}
		
	}
	/* Normal stores */
	else
	{
		/* Get the actual item */
		o_ptr = &st_ptr->stock[item];
		found = TRUE;
	}
	*(i_ptr) = *(o_ptr);
	return found;
}

/*
 * Buy an item from a store				-RAK-
 */
void store_purchase(int Ind, int item, int amt)
{
	player_type *p_ptr = Players[Ind];
	int st = p_ptr->store_num;
	store_type *st_ptr = &store[st];
	owner_type *ot_ptr = &owners[st][st_ptr->owner];
	int			i, choice, sold;
	int			item_new;
	s32b		price, best;
	object_type		sell_obj;
	char		o_name[80];
	object_type		tmp_obj;
	object_type		*o_ptr = &tmp_obj;

	/* Empty && Not player-owned? */
	if (st != 8 && st_ptr->stock_num <= 0)
	{
		msg_print(Ind, "I am currently out of stock.");
		return;
	}
	
	/* Player-owned && Is his? */
	if (st == 8 && house_owned_by(Ind, p_ptr->player_store_num))
	{
		msg_print(Ind, "You cannot buy from yourself.");
		return;
	} 

	/* Fill o_ptr with correct item */
	if (!get_store_item(Ind, item, o_ptr)) 
	{
			/* Disguise our bug as a feature */ 
			msg_print(Ind,"Sorry, this item is reserved.");
			return;
	}
	
	/* Don't sell if someone has just entered the house (anti-exploit) */
	if (st == 8 )
	{
		for (i = 1; i <= NumPlayers; i++)
		{
			if(house_inside(i, p_ptr->player_store_num))
			{
				/* FIXME Here we should eject the player from the shop for his own
				 * protection. The shop keeper can exploit him if he is inside the
				 * shop when another player is shopping.
				 */
				/* msg_print(Ind, "The shopkeeper is currently restocking."); */
				msg_print(Ind, "WARNING: Shop keeper is restocking, please leave!");
				return;		
			}
		}
	}

	/* Sanity check the number of items */
	if (amt < 1)
	{
		amt = 1;
	}
	if (amt > o_ptr->number)
	{
		amt = o_ptr->number;
	}

	/* Hack -- get a "sample" object */
	sell_obj = *o_ptr;
	sell_obj.number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(Ind, &sell_obj))
	{
		msg_print(Ind, "You cannot carry that many different items.");
		return;
	}

	/* Determine the "best" price (per item) */
	object_known(&sell_obj);
	best = price_item(Ind, &sell_obj, ot_ptr->min_inflate, FALSE);

	/* Create the object to be sold (structure copy) */
	sell_obj = *o_ptr;
	sell_obj.number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(Ind, &sell_obj))
	{
		msg_print(Ind, "You cannot carry that many items.");
		return;
	}

	/* Assume accept */
	choice = 0;

	/* Go directly to the "best" deal */
	price = (best * sell_obj.number);

	/* Player wants it */
	if (choice == 0)
	{
		/* Fix the item price (if "correctly" haggled) */
		if (price == (best * sell_obj.number)) o_ptr->ident |= ID_FIXED;

		/* Player can afford it */
		if (p_ptr->au >= price)
		{
			/* If this is the "back room" we have sold a real item */
			if (p_ptr->store_num == 8)
			{
				/* Remove the item from the players house */
				sold = sell_player_item(Ind, o_ptr, amt, price);
					
				/* Did we really manage to sell this? */
				if (sold)
				{
					if (sold < amt)
					{
						/* Couldn't buy as many as we wanted */
						amt = sold;

						/* Recalculate correct price */
						price = (best * amt);
					}
				}
				else
				{
					/* Error - perhaps someone picked up the item in the house
					 * just before we hit "buy"? */
					msg_format(Ind, "Sorry, this item is reserved.");
					return;
				}
			}

			/* Say "okay" */
			say_comment_1(Ind);

			/* Spend the money */
			p_ptr->au -= price;

			/* Update the display */
			store_prt_gold(Ind);

			/* Hack -- buying an item makes you aware of it */
			object_aware(Ind, &sell_obj);

			/* Hack -- clear the "fixed" flag from the item */
			sell_obj.ident &= ~ID_FIXED;
			
			/* Remove the incription (if player shop) */
			if (st == 8) sell_obj.note = 0;

			/* Describe the transaction */
			object_desc(Ind, o_name, &sell_obj, TRUE, 3);

			/* Message */
			msg_format(Ind, "You bought %s for %ld gold.", o_name, (long)price);

			/* Let the player carry it (as if he picked it up) */
			item_new = inven_carry(Ind, &sell_obj);

			/* Describe the final result */
			object_desc(Ind, o_name, &p_ptr->inventory[item_new], TRUE, 3);

			/* Message */
			msg_format(Ind, "You have %s (%c).",
			           o_name, index_to_label(item_new));

			/* Handle stuff */
			handle_stuff(Ind);

			/* Note how many slots the store used to have */
			i = st_ptr->stock_num;

			if (p_ptr->store_num == 8)
			{
				display_store(Ind);
			}
			else
			{
				/* Remove the bought items from the store */
				store_item_increase(st, item, -amt);
				store_item_optimize(st, item);
												
				/* Resend the basic store info */
				display_store(Ind);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
					/* Shuffle */
					if (rand_int(STORE_SHUFFLE) == 0)
					{
						/* Message */
						msg_print(Ind, "The shopkeeper retires.");
	
						/* Shuffle the store */
						store_shuffle(p_ptr->store_num);
					}
	
					/* Maintain */
					else
					{
						/* Message */
						msg_print(Ind, "The shopkeeper brings out some new stock.");
					}

					/* New inventory */
					for (i = 0; i < 10; i++)
					{
						/* Maintain the store */
						store_maint(p_ptr->store_num);
					}

					/* Redraw everything */
					display_inventory(Ind);
				}
				/* The item is gone */
				else if (st_ptr->stock_num != i)
				{
					/* Redraw everything */
					display_inventory(Ind);
				}

				/* Item is still here */
				else
				{
					/* Redraw the item */
					display_entry(Ind, item);
				}
			}
		}

		/* Player cannot afford it */
		else
		{
			/* Simple message (no insult) */
			msg_print(Ind, "You do not have enough gold.");
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
void store_sell(int Ind, int item, int amt)
{
	player_type *p_ptr = Players[Ind];

	int			choice;

	s32b		price;

	object_type		sold_obj;
	object_type		*o_ptr;

	char		o_name[80];

	/* You can't sell 0 of something. */
	if (amt <= 0) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		o_ptr = &o_list[0 - item];
	}

	/* Sanity check the number of items */
	if (amt > o_ptr->number)
	{
		amt = o_ptr->number;
	}	
	
	/* Check for validity of sale */
	if (!store_will_buy(Ind, o_ptr))
	{
		msg_print(Ind, "I don't want that!");
		return;
	}

#if 0

	/* Not gonna happen XXX inscribe */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Stop */
		return;
	}

#endif

	/* Make sure he hasn't protected it*/
	if( check_guard_inscription( o_ptr->note, 's' )) {
	    msg_print(Ind, "The item's inscription prevents it");
	    return;
	};

	/* Create the object to be sold (structure copy) */
	sold_obj = *o_ptr;
	sold_obj.number = amt;

	/* Get a full description */
	object_desc(Ind, o_name, &sold_obj, TRUE, 3);

	/* Remove any inscription for stores */
	if (p_ptr->store_num != 7) sold_obj.note = 0;

	/* Is there room in the store (or the home?) */
	if (!store_check_num(p_ptr->store_num, &sold_obj))
	{
		if (p_ptr->store_num == 7) msg_print(Ind, "Your home is full.");
		else msg_print(Ind, "I have not the room in my store to keep it.");
		return;
	}

	/* Real store */
	if (p_ptr->store_num != 7)
	{

		/* Describe the transaction */
		msg_format(Ind, "Selling %s (%c).", o_name, index_to_label(item));
		/*msg_print(NULL);*/

		/* Haggle for it */
		choice = sell_haggle(Ind, &sold_obj, &price);

		/* Tell the client about the price */
		Send_store_sell(Ind, price);

		/* Save the info for the confirmation */
		p_ptr->current_selling = item;
		p_ptr->current_sell_amt = amt;
		p_ptr->current_sell_price = price;

		/* Wait for confirmation before actually selling */
		return;
	}

}


void store_confirm(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int item, amt, price;

	object_type *o_ptr, sold_obj;
	char o_name[80];
	int item_pos;

	/* Abort if we shouldn't be getting called */
	if (p_ptr->current_selling == -1)
		return;

	/* Restore the variables */
	item = p_ptr->current_selling;
	amt = p_ptr->current_sell_amt;
	price = p_ptr->current_sell_price;

	/* Trash the saved variables */
	p_ptr->current_selling = -1;
	p_ptr->current_sell_amt = -1;
	p_ptr->current_sell_price = -1;

	/* Sold... */

	/* Say "okay" */
	say_comment_1(Ind);

	/* Be happy */
	/*decrease_insults();*/

	/* Get some money */
	p_ptr->au += price;

	/* Update the display */
	store_prt_gold(Ind);

	/* Get the inventory item */
	o_ptr = &p_ptr->inventory[item];

	/* Become "aware" of the item */
	object_aware(Ind, o_ptr);

	/* Know the item fully */
	object_known(o_ptr);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Re-Create the now-identified object that was sold */
	sold_obj = *o_ptr;
	sold_obj.number = amt;
	
	/* Get the description all over again */
	object_desc(Ind, o_name, &sold_obj, TRUE, 3);

	/* Describe the result (in message buffer) */
	msg_format(Ind, "You sold %s for %ld gold.", o_name, (long)price);

	/* Analyze the prices (and comment verbally) */
	/*purchase_analyze(price, value, dummy);*/

	/* Take the item from the player, describe the result */
	inven_item_increase(Ind, item, -amt);
	inven_item_describe(Ind, item);
	inven_item_optimize(Ind, item);

	/* Handle stuff */
	handle_stuff(Ind);

	/* The store gets that (known) item */
	item_pos = store_carry(p_ptr->store_num, &sold_obj);

	/* Resend the basic store info */
	display_store(Ind);

	/* Re-display if item is now in store */
	if (item_pos >= 0)
	{
		display_inventory(Ind);
	}
}


/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 *
 * pstore is -1 for normal stores or house index for player owned store.
 */
void do_cmd_store(int Ind, int pstore)
{
	player_type *p_ptr = Players[Ind];
	int			which, i;

	cave_type		*c_ptr;

	/* Normal store */
	if (pstore < 0)
	{

		/* Access the player grid */
		c_ptr = &cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px];

		/* Verify a store */
		if (!((c_ptr->feat >= FEAT_SHOP_HEAD) &&
		      (c_ptr->feat <= FEAT_SHOP_TAIL)))
		{
			msg_print(Ind, "You see no store here.");
			return;
		}
	
		/* Extract the store code */
		which = (c_ptr->feat - FEAT_SHOP_HEAD);

		/* Hack -- Check the "locked doors" */
		if (store[which].store_open >= turn)
		{
			msg_print(Ind, "The doors are locked.");
			return;
		}

		/* Save the store number */
		p_ptr->store_num = which;

	}
	
	/* Player owned store */
	else
	{
		/* Store is closed if someone is restocking (anti-exploit) */
		for (i = 1; i <= NumPlayers; i++)
		{
			if(Ind != i && house_inside(i, pstore))
			{
				msg_print(Ind, "The doors are locked.");
				return;		
			}
		}
		p_ptr->store_num = 8;
		p_ptr->player_store_num = pstore;
	}
	
	/* Display the store */
	display_store(Ind);

	/* Do not leave */
	leave_store = FALSE;
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;
	store_type *st_ptr;


	/* Ignore shop stores */
	if (which == 7 || which == 8) return;

	/* Make sure no one is in the store */
	for (i = 1; i <= NumPlayers; i++)
	{
		/* Check this player */
		if (Players[i]->store_num == which)
			return;
	}

	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Pick a new owner */
	for (j = st_ptr->owner; j == st_ptr->owner; )
	{
		st_ptr->owner = rand_int(MAX_OWNERS);
	}

	/* Reset the owner data */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;


	/* Hack -- discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		/* Chance of putting items up for sale */
		if (rand_int(10) == 0)
		{
			object_type *o_ptr;

			/* Get the item */
			o_ptr = &st_ptr->stock[i];

			/* Hack -- Sell all old items for "half price" */
			o_ptr->discount = 50;

			/* Hack -- Items are no longer "fixed price" */
			o_ptr->ident &= ~ID_FIXED;

			/* Mega-Hack -- Note that the item is "on sale" */
			o_ptr->note = quark_add("on sale");
		}
	}
}


/*
 * Maintain the inventory at the stores.
 */
void store_maint(int which)
{
	int         j;

	int		old_rating = rating;
	store_type *st_ptr;
	owner_type *ot_ptr;

	/* Ignore some shops */
	if (which == 7 || which == 8) return;

	/* Make sure no one is in the store */
	for (j = 1; j <= NumPlayers; j++)
	{
		if (Players[j]->store_num == which)
			return;
	}

	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Activate the owner */
	ot_ptr = &owners[store_num][st_ptr->owner];

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;

	/* Mega-Hack -- prune the black market */
	if (store_num == 6)
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if (black_market_crap(o_ptr))
			{
				/* Destroy the item */
				store_item_increase(store_num, j, 0 - o_ptr->number);
				store_item_optimize(store_num, j);
			}
		}
	}

	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "underflow" */
	if (j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete(store_num);


	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "overflow" */
	if (j >= st_ptr->stock_size) j = st_ptr->stock_size - 1;

	/* Acquire some new items */
	while (st_ptr->stock_num < j) store_create(store_num);

	/* Hack -- Restore the rating */
	rating = old_rating;
}


/*
 * Initialize the stores
 */
void store_init(int which)
{
	int         k;
	store_type *st_ptr;
	owner_type *ot_ptr;


	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];


	/* Pick an owner */
	st_ptr->owner = rand_int(MAX_OWNERS);

	/* Activate the new owner */
	ot_ptr = &owners[store_num][st_ptr->owner];


	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		invwipe(&st_ptr->stock[k]);
	}
}
