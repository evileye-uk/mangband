/* File: cmd3.c */

/* Purpose: Inventory commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define SERVER

#include "angband.h"




/*
 * Move an item from equipment list to pack
 * Note that only one item at a time can be wielded per slot.
 * Note that taking off an item when "full" will cause that item
 * to fall to the ground.
 */
static void inven_takeoff(int Ind, int item, int amt)
{
	player_type *p_ptr = Players[Ind];

	int			posn;

	object_type		*o_ptr;
	object_type		tmp_obj;

	cptr		act;

	char		o_name[80];


	/* Get the item to take off */
	o_ptr = &(p_ptr->inventory[item]);

	/* Paranoia */
	if (amt <= 0) return;

        if( check_guard_inscription( o_ptr->note, 't' )) {
		msg_print(Ind, "The item's inscription prevents it.");
                return;
        };


	/* Verify */
	if (amt > o_ptr->number) amt = o_ptr->number;

	/* Make a copy to carry */
	tmp_obj = *o_ptr;
	tmp_obj.number = amt;

	/* What are we "doing" with the object */
	if (amt < o_ptr->number)
	{
		act = "Took off";
	}
	else if (item == INVEN_WIELD)
	{
		act = "Was wielding";
	}
	else if (item == INVEN_BOW)
	{
		act = "Was shooting with";
	}
	else if (item == INVEN_LITE)
	{
		act = "Light source was";
	}
	else
	{
		act = "Was wearing";
	}

	/* Carry the object, saving the slot it went in */
	posn = inven_carry(Ind, &tmp_obj);

	/* Describe the result */
	object_desc(Ind, o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format(Ind, "%^s %s (%c).", act, o_name, index_to_label(posn));

	/* Delete (part of) it */
	inven_item_increase(Ind, item, -amt);
	inven_item_optimize(Ind, item);

	/* Window stuff */
	p_ptr->window |= (PW_EQUIP);

	/* Redraw */
	p_ptr->redraw |= (PR_PLUSSES | PR_OFLAGS);
}




/*
 * Drops (some of) an item from inventory to "near" the current location
 */
static void inven_drop(int Ind, int item, int amt)
{
	player_type *p_ptr = Players[Ind];

	object_type		*o_ptr;
	object_type		 tmp_obj;

	cptr		act;

	char		o_name[80];



	/* Access the slot to be dropped */
	o_ptr = &(p_ptr->inventory[item]);

	/* Error check */
	if (amt <= 0) return;

	/* Not too many */
	if (amt > o_ptr->number) amt = o_ptr->number;

	/* Nothing done? */
	if (amt <= 0) return;

	/* check for !d  or !* in inscriptions */

	if( check_guard_inscription( o_ptr->note, 'd' )) {
		msg_print(Ind, "The item's inscription prevents it.");
		return;
	};

	/* Never drop artifacts above their base depth */
	if (artifact_p(o_ptr) && (p_ptr->dun_depth < a_info[o_ptr->name1].level) )
	{
		msg_print(Ind, "You can not drop this here.");
		return;	
	}	

	/* Make a "fake" object */
	tmp_obj = *o_ptr;
	tmp_obj.number = amt;

	/* What are we "doing" with the object */
	if (amt < o_ptr->number)
	{
		act = "Dropped";
	}
	else if (item == INVEN_WIELD)
	{
		act = "Was wielding";
	}
	else if (item == INVEN_BOW)
	{
		act = "Was shooting with";
	}
	else if (item == INVEN_LITE)
	{
		act = "Light source was";
	}
	else if (item >= INVEN_WIELD)
	{
		act = "Was wearing";
	}
	else
	{
		act = "Dropped";
	}
	
	/* Dropping from equipment? Update object flags! */
	if (item >= INVEN_WIELD)
		p_ptr->redraw |= (PR_OFLAGS);

	/* Message */
	object_desc(Ind, o_name, &tmp_obj, TRUE, 3);

	/* Message */
	msg_format(Ind, "%^s %s (%c).", act, o_name, index_to_label(item));

	/* Drop it (carefully) near the player */
	drop_near(&tmp_obj, 0, p_ptr->dun_depth, p_ptr->py, p_ptr->px);

	/* Decrease the item, optimize. */
	inven_item_increase(Ind, item, -amt);
	inven_item_describe(Ind, item);
	inven_item_optimize(Ind, item);
}





/*
 * Display inventory
 *
 * This is handled by the client now --KLJ--
 */
void do_cmd_inven(void)
{
	/*char out_val[160];*/


	/* Note that we are in "inventory" mode */
	/*command_wrk = FALSE;*/


	/* Save the screen */
	/*Term_save();*/

	/* Hack -- show empty slots */
	/*item_tester_full = TRUE;*/

	/* Display the inventory */
	/*show_inven();*/

	/* Hack -- hide empty slots */
	/*item_tester_full = FALSE;*/

	/* Build a prompt */
	/*sprintf(out_val, "Inventory (carrying %d.%d pounds). Command: ",
	        total_weight / 10, total_weight % 10);*/

	/* Get a command */
	/*prt(out_val, 0, 0);*/

	/* Get a new command */
	/*command_new = inkey();*/

	/* Restore the screen */
	/*Term_load();*/


	/* Process "Escape" */
	/*if (command_new == ESCAPE)
	{*/
		/* Reset stuff */
		/*command_new = 0;
		command_gap = 50;
	}*/

	/* Process normal keys */
	/*else
	{*/
		/* Hack -- Use "display" mode */
		/*command_see = TRUE;
	}*/
}


/*
 * Display equipment
 *
 * This is handled be the client --KLJ--
 */
void do_cmd_equip(void)
{
	/*char out_val[160];*/


	/* Note that we are in "equipment" mode */
	/*command_wrk = TRUE;*/


	/* Save the screen */
	/*Term_save();*/

	/* Hack -- show empty slots */
	/*item_tester_full = TRUE;*/

	/* Display the equipment */
	/*show_equip();*/

	/* Hack -- undo the hack above */
	/*item_tester_full = FALSE;*/

	/* Build a prompt */
	/*sprintf(out_val, "Equipment (carrying %d.%d pounds). Command: ",
	        total_weight / 10, total_weight % 10);*/

	/* Get a command */
	/*prt(out_val, 0, 0);*/

	/* Get a new command */
	/*command_new = inkey();*/

	/* Restore the screen */
	/*Term_load();*/


	/* Process "Escape" */
	/*if (command_new == ESCAPE)
	{*/
		/* Reset stuff */
		/*command_new = 0;
		command_gap = 50;
	}*/

	/* Process normal keys */
	/*else
	{*/
		/* Enter "display" mode */
		/*command_see = TRUE;
	}*/
}


/*
 * The "wearable" tester
 */
static bool item_tester_hook_wear(int Ind, object_type *o_ptr)
{
	/* Check for a usable slot */
	if (wield_slot(Ind, o_ptr) >= INVEN_WIELD) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


/*
 * Wield or wear a single item from the pack or floor
 */
void do_cmd_wield(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int slot;
	object_type tmp_obj;
	object_type *o_ptr;
	object_type *x_ptr;

	cptr act;

	char o_name[80];


	/* Restrict the choices */
	/*item_tester_hook = item_tester_hook_wear;*/


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}

	if( check_guard_inscription( o_ptr->note, 'w' )) {
		msg_print(Ind, "The item's inscription prevents it.");
		return;
	};


	if (!item_tester_hook_wear(Ind, o_ptr))
	{
		msg_print(Ind, "You may not wield that item.");
		return;
	}

	/* Check the slot */
	slot = wield_slot(Ind, o_ptr);

	/* Prevent wielding into a cursed slot */
	if (cursed_p(&(p_ptr->inventory[slot])))
	{
		/* Describe it */
		object_desc(Ind, o_name, &(p_ptr->inventory[slot]), FALSE, 0);

		/* Message */
		msg_format(Ind, "The %s you are %s appears to be cursed.",
		           o_name, describe_use(Ind, slot));

		/* Cancel the command */
		return;
	}

	x_ptr = &(p_ptr->inventory[slot]);

	if( check_guard_inscription( x_ptr->note, 't' )) {
		msg_print(Ind, "The item's inscription prevents it.");
		return;
	};

	/* Hack -- MAngband-specific: if it is an artifact and pack is full, base depth must match */
	if (item < 0 && artifact_p(x_ptr) && !inven_carry_okay(Ind, x_ptr) && (p_ptr->dun_depth < a_info[x_ptr->name1].level))
	{
		object_desc(Ind, o_name, x_ptr, FALSE, 0);
		msg_format(Ind, "Your pack is full and you can't drop %s here.", o_name);
		return;
	}

#if 0
	/* Verify potential overflow */
	if ((p_ptr->inven_cnt >= INVEN_PACK) &&
	    ((item < 0) || (o_ptr->number > 1)))
	{
		/* Verify with the player */
		if (other_query_flag &&
		    !get_check(Ind, "Your pack may overflow.  Continue? ")) return;
	}
#endif

	/* Mega-hack -- prevent anyone but total winners from wielding the Massive Iron
	 * Crown of Morgoth or the Mighty Hammer 'Grond'.
	 */
	if (!p_ptr->total_winner)
	{
		/* Attempting to wear the crown if you are not a winner is a very, very bad thing
		 * to do.
		 */
		if (o_ptr->name1 == ART_MORGOTH)
		{
			msg_print(Ind, "You are blasted by the Crown's power!");
			/* This should pierce invulnerability */
			take_hit(Ind, 10000, "the Massive Iron Crown of Morgoth");
			return;
		}
		/* Attempting to wield Grond isn't so bad. */
		if (o_ptr->name1 == ART_GROND)
		{
			msg_print(Ind, "You are far too weak to wield the mighty Grond.");
			return;
		}
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Get a copy of the object to wield */
	tmp_obj = *o_ptr;
	tmp_obj.number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_optimize(Ind, item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &(p_ptr->inventory[slot]);

	/* Take off the "entire" item if one is there */
	if (p_ptr->inventory[slot].k_idx) inven_takeoff(Ind, slot, 255);

	/*** Could make procedure "inven_wield()" ***/

	/* Wear the new stuff */
	*o_ptr = tmp_obj;

	/* Increase the weight */
	p_ptr->total_weight += o_ptr->weight;

	/* Increment the equip counter by hand */
	p_ptr->equip_cnt++;

	/* Where is the item now */
	if (slot == INVEN_WIELD)
	{
		act = "You are wielding";
	}
	else if (slot == INVEN_BOW)
	{
		act = "You are shooting with";
	}
	else if (slot == INVEN_LITE)
	{
		act = "Your light source is";
	}
	else
	{
		act = "You are wearing";
	}

	/* Describe the result */
	object_desc(Ind, o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format(Ind, "%^s %s (%c).", act, o_name, index_to_label(slot));

	/* Cursed! */
	if (cursed_p(o_ptr))
	{
		/* Warn the player */
		msg_print(Ind, "Oops! It feels deathly cold!");

		/* Note the curse */
		o_ptr->ident |= ID_SENSE;
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	/* Redraw */
	p_ptr->redraw |= (PR_PLUSSES | PR_OFLAGS);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
}



/*
 * Take off an item
 */
void do_cmd_takeoff(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	/* Verify potential overflow */
	if (p_ptr->inven_cnt >= INVEN_PACK)
	{
		msg_print(Ind, "Your pack is full and would overflow!");
		return;
	}

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		/* We can't "takeoff" something that is on the floor */
		return;
	}

	if( check_guard_inscription( o_ptr->note, 't' )) {
		msg_print(Ind, "The item's inscription prevents it.");
		return;
	};


	/* Item is cursed */
	if (cursed_p(o_ptr))
	{
		/* Oops */
		msg_print(Ind, "Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}


	/* Take a partial turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth) / 2;

	/* Take off the item */
	inven_takeoff(Ind, item, 255);
}


/*
 * Drop an item
 */
void do_cmd_drop(int Ind, int item, int quantity)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	/* Handle the newbies_cannot_drop option */	
	if ((p_ptr->lev == 1) && (cfg_newbies_cannot_drop))
	{
		msg_print(Ind, "You are not experienced enough to drop items.");
		return;
	}

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}
	else
		return;
	/* Get the item (on the floor) */
	/* Impossible
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}
	*/

	if( check_guard_inscription( o_ptr->note, 'd' )) {
		msg_print(Ind, "The item's inscription prevents it.");
		return;
	};


	/* Cannot remove cursed items */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
		msg_print(Ind, "Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}



#if 0
	/* Mega-Hack -- verify "dangerous" drops */
	if (cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx)
	{
		/* XXX XXX Verify with the player */
		if (other_query_flag &&
		    !get_check(Ind, "The item may disappear.  Continue? ")) return;
	}
#endif


	/* Take a partial turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth) / 2;

	/* Drop (some of) the item */
	inven_drop(Ind, item, quantity);
}


/*
 * Drop some gold
 */
void do_cmd_drop_gold(int Ind, s32b amt)
{
	player_type *p_ptr = Players[Ind];

	object_type tmp_obj;

	/* Handle the newbies_cannot_drop option */
	if ((p_ptr->lev == 1) && (cfg_newbies_cannot_drop))
	{
		msg_print(Ind, "You are not experienced enough to drop gold.");
		return;
	}
	

	/* Error checks */
	if (amt <= 0) return;
	if (amt > p_ptr->au)
	{
		msg_print(Ind, "You do not have that much gold.");
		return;
	}

	/* Use "gold" object kind */
	invcopy(&tmp_obj, lookup_kind(TV_GOLD,SV_PLAYER_GOLD));

	/* Setup the "worth" */
	tmp_obj.pval = amt;

	/* Drop it */
	drop_near(&tmp_obj, 0, p_ptr->dun_depth, p_ptr->py, p_ptr->px);

	/* Subtract from the player's gold */
	p_ptr->au -= amt;

	/* Message */
	msg_format(Ind, "You drop %ld pieces of gold.", amt);

	/* Redraw gold */
	p_ptr->redraw |= (PR_GOLD);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);
}


/*
 * Destroy an item
 */
void do_cmd_destroy(int Ind, int item, int quantity)
{
	player_type *p_ptr = Players[Ind];

	int			old_number;

	bool		force = FALSE;

	object_type		*o_ptr;

	char		o_name[80];

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = quantity;
	object_desc(Ind, o_name, o_ptr, TRUE, 3);
	o_ptr->number = old_number;

	if( check_guard_inscription( o_ptr->note, 'k' )) {
		msg_print(Ind, "The item's inscription prevents it.");
		return;
	};

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Artifacts cannot be destroyed */
	if (artifact_p(o_ptr))
	{
		cptr feel = "special";

		/* Message */
		msg_format(Ind, "You cannot destroy %s.", o_name);
 		
		/* Hack -- Handle icky artifacts */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) feel = "terrible";

		/* Hack -- inscribe the artifact */
		o_ptr->note = quark_add(feel);

		/* We have "felt" it (again) */
		o_ptr->ident |= (ID_SENSE);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}

	/* Cursed, equipped items cannot be destroyed */
	if (item >= INVEN_WIELD && cursed_p(o_ptr))
	{
		/* Message */
		msg_print(Ind, "Hmm, that seems to be cursed.");

		/* Done */
		return;
	}

	/* Destroying from equipment? Update object flags! */
	if (item >= INVEN_WIELD)
		p_ptr->redraw |= (PR_OFLAGS);

	/* Message */
	msg_format(Ind, "You destroy %s.", o_name);

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -quantity);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -quantity);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}


/*
 * Observe an item which has been *identify*-ed
 */
void do_cmd_observe(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	object_type		*o_ptr;

	char		o_name[80];

	/* Get the item (in the store) */
	if (p_ptr->store_num != -1) {
		object_type		tmp_obj;
		o_ptr = &tmp_obj;
		/* Fill o_ptr with correct item */
		if (!get_store_item(Ind, item, o_ptr)) 
		{
			/* Disguise our bug as a feature */ 
			msg_print(Ind,"Sorry, this item is exclusive.");
			return;
		}
		
			/* Get name */
			object_desc_store(Ind, o_name, o_ptr, TRUE, 3);
			/* Identify this store item */
			object_known(o_ptr);
	} else {
		/* Get the item (in the pack) */
		if (item >= 0)
		{
			o_ptr = &(p_ptr->inventory[item]);
		}
	
		/* Get the item (on the floor) */
		else
		{
			item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
			if (item == 0) {
				msg_print(Ind, "There's nothing on the floor.");
				return;
			}
			o_ptr = &o_list[0 - item];
		}
		
		/* Get name */
		object_desc(Ind, o_name, o_ptr, TRUE, 3);
	}

	/* Inform */
	msg_format(Ind, "Examining %s...", o_name);

	/* Capitalize object name for header */
	o_name[0] = toupper(o_name[0]);
	
	/* Describe it fully */
	identify_fully_aux(Ind, o_ptr);
	
	/* Notify player */
	Send_special_other(Ind, o_name);
}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
void do_cmd_uninscribe(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}

	/* Nothing to remove */
	if (!o_ptr->note)
	{
		msg_print(Ind, "That item had no inscription to remove.");
		return;
	}

	/* Message */
	msg_print(Ind, "Inscription removed.");

	/* Remove the incription */
	o_ptr->note = 0;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}


/*
 * Inscribe an object with a comment
 */
void do_cmd_inscribe(int Ind, int item, cptr inscription)
{
	player_type *p_ptr = Players[Ind];
	object_type		*o_ptr;
	char		o_name[80];
	s32b		price;
	char		*c;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}

	/* Don't allow certain inscriptions when selling */
	if ((c = strstr(inscription, "for sale")))
	{	
		/* Can't sell unindentified items */
		if (!object_known_p(Ind, o_ptr))
		{
			msg_print(Ind,"You must identify this item first");
			return;
		}
		/* Can't sell overpriced items */
		c += 8; /* skip "for sale" */
		if( *c == ' ' )
		{
			price = atoi(c);
			if (price > 9999999)
			{
				msg_print(Ind,"Your price is too high!");
				return;
			}
		}		
	}
	
	/* Describe the activity */
	object_desc(Ind, o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format(Ind, "Inscribing %s.", o_name);
	msg_print(Ind, NULL);

	/* Save the inscription */
	o_ptr->note = quark_add(inscription);

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}


/*
 * Attempt to steal from another player
 */
void do_cmd_steal(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind], *q_ptr;
	cave_type *c_ptr;

	int success, notice;
	bool fail = TRUE;

	/* Ghosts cannot steal */
	if ((p_ptr->ghost) || (p_ptr->fruit_bat))
	{
	        msg_print(Ind, "You cannot steal things!");
	        return;
	}	                                                        

	/* Examine target grid */
	c_ptr = &cave[p_ptr->dun_depth][p_ptr->py + ddy[dir]][p_ptr->px + ddx[dir]];

	/* May only steal from players */
	if (c_ptr->m_idx >= 0)
	{
		/* Message */
		msg_print(Ind, "You see nothing there to steal from.");

		return;
	}

	/* May not steal from yourself */
	if (!dir || dir == 5) return;

	/* Examine target */
	q_ptr = Players[0 - c_ptr->m_idx];

	/* May not steal from hostile players */
	if (check_hostile(0 - c_ptr->m_idx, Ind))
	{
		/* Message */
		msg_format(Ind, "%^s is on guard against you.", q_ptr->name);

		return;
	}

	/* Make sure we have enough room */
	if (p_ptr->inven_cnt >= INVEN_PACK)
	{
		/* Message */
		msg_print(Ind, "You have no room to steal anything.");

		return;
	}

	/* Compute chance of success */
	success = 3 * (adj_dex_safe[p_ptr->stat_ind[A_DEX]] - adj_dex_safe[q_ptr->stat_ind[A_DEX]]);

	/* Compute base chance of being noticed */
	notice = 5 * (adj_mag_stat[q_ptr->stat_ind[A_INT]] - p_ptr->skill_stl);

	/* Hack -- Rogues get bonuses to chances */
	if (p_ptr->pclass == CLASS_ROGUE)
	{
		/* Increase chance by level */
		success += 3 * p_ptr->lev;
		notice -= 3 * p_ptr->lev;
	}

	/* Hack -- Always small chance to succeed */
	if (success < 2) success = 2;

	/* Check for success */
	if (rand_int(100) < success)
	{
		/* Steal gold 25% of the time */
		if (rand_int(100) < 25)
		{
			int amt = q_ptr->au / 10;

			/* Transfer gold */
			if (amt)
			{
				/* Move from target to thief */
				q_ptr->au -= amt;
				p_ptr->au += amt;

				/* Redraw */
				p_ptr->redraw |= (PR_GOLD);
				q_ptr->redraw |= (PR_GOLD);

				/* Tell thief */
				msg_format(Ind, "You steal %ld gold.", amt);

				/* Check for target noticing */
				if (rand_int(100) < notice)
				{
					/* Make target hostile */
					add_hostility(0 - c_ptr->m_idx, p_ptr->name);

					/* Message */
					msg_format(0 - c_ptr->m_idx, "You notice %s stealing %ld gold!",
					           p_ptr->name, amt);
				}
				fail = FALSE;
			}
		}
		else
		{
			int item;
			object_type *o_ptr, forge;
			char o_name[80];

			/* Steal an item */
			item = rand_int(q_ptr->inven_cnt);

			/* Get object */
			o_ptr = &q_ptr->inventory[item];
			
			/* Don't steal (nothing)s */
			if (o_ptr->k_idx)
			{
				forge = *o_ptr;
	
				/* Give one item to thief */
				forge.number = 1;
				inven_carry(Ind, &forge);
	
				/* Take one from target */
				inven_item_increase(0 - c_ptr->m_idx, item, -1);
				inven_item_optimize(0 - c_ptr->m_idx, item);
	
				/* Tell thief what he got */
				object_desc(Ind, o_name, &forge, TRUE, 3);
				msg_format(Ind, "You stole %s.", o_name);
	
				/* Easier to notice heavier objects */
				notice += forge.weight;
	
				/* Check for target noticing */
				if (rand_int(100) < notice)
				{
					/* Make target hostile */
					add_hostility(0 - c_ptr->m_idx, p_ptr->name);
	
					/* Message */
					msg_format(0 - c_ptr->m_idx, "You notice %s stealing %s!",
					           p_ptr->name, o_name);
				}
				fail = FALSE; 
			}  
		}
	}
	
	if (fail)
	{
		/* Message */
		msg_print(Ind, "You fail to steal anything.");

		/* Easier to notice a failed attempt */
		if (rand_int(100) < notice + 50)
		{
			/* Make target hostile */
			add_hostility(0 - c_ptr->m_idx, p_ptr->name);

			/* Message */
			msg_format(0 - c_ptr->m_idx, "You notice %s try to steal from you!",
			           p_ptr->name);
		}
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);
}







/*
 * An "item_tester_hook" for refilling lanterns
 */
static bool item_tester_refill_lantern(object_type *o_ptr)
{
    /* Randarts are not refillable */
    if (o_ptr->name3) return (FALSE);

	/* Flasks of oil are okay */
	if (o_ptr->tval == TV_FLASK) return (TRUE);

	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refill the players lamp (from the pack or floor)
 */
static void do_cmd_refill_lamp(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;
	object_type *j_ptr;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_lantern;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}

	if (!item_tester_hook(o_ptr))
	{
		msg_print(Ind, "You cannot refill with that!");
		return;
	}


	/* Take a partial turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth) / 2;

	/* Access the lantern */
	j_ptr = &(p_ptr->inventory[INVEN_LITE]);

	/* Refuel */
	j_ptr->pval += o_ptr->pval;

	/* Message */
	msg_print(Ind, "You fuel your lamp.");

	/* Comment */
	if (j_ptr->pval >= FUEL_LAMP)
	{
		j_ptr->pval = FUEL_LAMP;
		msg_print(Ind, "Your lamp is full.");
	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
	
	/* Hack - Force Equipment Update */
	p_ptr->window |= (PW_EQUIP);
}



/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_TORCH)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refuel the players torch (from the pack or floor)
 */
static void do_cmd_refill_torch(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;
	object_type *j_ptr;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_torch;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}

	/* Get the item (on the floor) */
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		if (item == 0) {
			msg_print(Ind, "There's nothing on the floor.");
			return;
		}
		o_ptr = &o_list[0 - item];
	}

	if (!item_tester_hook(o_ptr))
	{
		msg_print(Ind, "You cannot refill with that!");
		return;
	}


	/* Take a partial turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth) / 2;

	/* Access the primary torch */
	j_ptr = &(p_ptr->inventory[INVEN_LITE]);

	/* Refuel */
	j_ptr->pval += o_ptr->pval + 5;

	/* Message */
	msg_print(Ind, "You combine the torches.");

	/* Over-fuel message */
	if (j_ptr->pval >= FUEL_TORCH)
	{
		j_ptr->pval = FUEL_TORCH;
		msg_print(Ind, "Your torch is fully fueled.");
	}

	/* Refuel message */
	else
	{
		msg_print(Ind, "Your torch glows more brightly.");
	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
	
	/* Hack - Force Equipment Update */
	p_ptr->window |= (PW_EQUIP);
}




/*
 * Refill the players lamp, or restock his torches
 */
void do_cmd_refill(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	/* Get the light */
	o_ptr = &(p_ptr->inventory[INVEN_LITE]);

	if( check_guard_inscription( o_ptr->note, 'F' )) {
		msg_print(Ind, "The item's incription prevents it.");
		return;
	};

	/* It is nothing */
	if (o_ptr->tval != TV_LITE)
	{
		msg_print(Ind, "You are not wielding a light.");
	}

	/* It's a lamp */
	else if (o_ptr->sval == SV_LITE_LANTERN)
	{
		do_cmd_refill_lamp(Ind, item);
	}

	/* It's a torch */
	else if (o_ptr->sval == SV_LITE_TORCH)
	{
		do_cmd_refill_torch(Ind, item);
	}

	/* No torch to refill */
	else
	{
		msg_print(Ind, "Your light cannot be refilled.");
	}
}






/*
 * Target command
 */
void do_cmd_target(int Ind, int dir)
{
	/* Set the target */
	if (target_set(Ind, dir))
	{
		/*msg_print(Ind, "Target Selected.");*/
	}
	else
	{
		/*msg_print(Ind, "Target Aborted.");*/
	}
}

void do_cmd_target_friendly(int Ind, int dir)
{
	/* Set the target */
	if (target_set_friendly(Ind, dir))
	{
		/*msg_print(Ind, "Target Selected.");*/
	}
	else
	{
		/*msg_print(Ind, "Target Aborted.");*/
	}
}


/*
 * Hack -- determine if a given location is "interesting" to a player
 */
static bool do_cmd_look_accept(int Ind, int y, int x)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	cave_type *c_ptr;
	byte *w_ptr;


	/* Examine the grid */
	c_ptr = &cave[Depth][y][x];
	w_ptr = &p_ptr->cave_flag[y][x];

	/* Player grids */
	if (c_ptr->m_idx < 0)
	{
        if (player_has_los_bold(Ind, y, x) || (p_ptr->telepathy == TR3_TELEPATHY))
			return (TRUE);
	}

	/* Visible monsters */
	if (c_ptr->m_idx > 0)
	{
		/* Visible monsters */
		if (p_ptr->mon_vis[c_ptr->m_idx]) return (TRUE);
	}

	/* Objects */
	if (c_ptr->o_idx)
	{
		/* Memorized object */
		if (p_ptr->obj_vis[c_ptr->o_idx]) return (TRUE);
	}

	/* Interesting memorized features */
	if (*w_ptr & CAVE_MARK)
	{
		/* Notice glyphs */
		if (c_ptr->feat == FEAT_GLYPH) return (TRUE);

		/* Notice doors */
		if (c_ptr->feat == FEAT_OPEN) return (TRUE);
		if (c_ptr->feat == FEAT_BROKEN) return (TRUE);

		/* Notice stairs */
		if (c_ptr->feat == FEAT_LESS) return (TRUE);
		if (c_ptr->feat == FEAT_MORE) return (TRUE);

		/* Notice shops */
		if ((c_ptr->feat >= FEAT_SHOP_HEAD) &&
		    (c_ptr->feat <= FEAT_SHOP_TAIL)) return (TRUE);

		/* Notice traps */
		if ((c_ptr->feat >= FEAT_TRAP_HEAD) &&
		    (c_ptr->feat <= FEAT_TRAP_TAIL)) return (TRUE);

		/* Notice doors */
		if ((c_ptr->feat >= FEAT_DOOR_HEAD) &&
		    (c_ptr->feat <= FEAT_DOOR_TAIL)) return (TRUE);

		/* Notice rubble */
		if (c_ptr->feat == FEAT_RUBBLE) return (TRUE);

		/* Notice veins with treasure */
		if (c_ptr->feat == FEAT_MAGMA_K) return (TRUE);
		if (c_ptr->feat == FEAT_QUARTZ_K) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Describe a floor tile (for looking and targeting routines)
 *	
 * if !active, activities such as tracking are disabled
 */
void describe_floor_tile(cave_type *c_ptr, cptr out_val, int Ind, bool active, byte cave_flag)
{
	player_type *p_ptr = Players[Ind];
	player_type *q_ptr;
	monster_type *m_ptr;
	object_type *o_ptr;
	char o_name[80];
	bool found = FALSE;
	bool self = FALSE;
	if (c_ptr->m_idx < 0)
	{
		q_ptr = Players[0 - c_ptr->m_idx];

		self = (0 - c_ptr->m_idx == Ind ? TRUE : FALSE); 

		if (p_ptr->play_vis[0 - c_ptr->m_idx] || self)
		{
			if (active && !self)
			{
				/* Track health */
				if (p_ptr->play_vis[0 - c_ptr->m_idx]) health_track(Ind, c_ptr->m_idx);
		
				/* Track with cursor */
				if (p_ptr->play_vis[0 - c_ptr->m_idx]) cursor_track(Ind, c_ptr->m_idx);
			}

		/* Format string */
		sprintf(out_val, "%s the %s %s", q_ptr->name, p_name + p_info[q_ptr->prace].name, c_name + c_info[q_ptr->pclass].name);
		
		found = TRUE;
		}
		
	}
	else if (c_ptr->m_idx > 0)
	{
		monster_race *r_ptr = &r_info[m_list[c_ptr->m_idx].r_idx];

		if (p_ptr->mon_vis[c_ptr->m_idx]) 
		{
			if (active)
			{
				/* Track health */
				if (p_ptr->mon_vis[c_ptr->m_idx]) health_track(Ind, c_ptr->m_idx);
		
				/* Track with cursor */
				if (p_ptr->mon_vis[c_ptr->m_idx]) cursor_track(Ind, c_ptr->m_idx);
			}
					
		/* Format string */
		sprintf(out_val, "%s (%s)", r_name + r_ptr->name, look_mon_desc(c_ptr->m_idx));
		
		found = TRUE;
		}
	}
	if (!found && c_ptr->o_idx)
	{
		if (p_ptr->obj_vis[c_ptr->o_idx])
		{
			o_ptr = &o_list[c_ptr->o_idx];
	
			/* Release Tracking */
			if (active) p_ptr->cursor_who = 0;
	
			/* Obtain an object description */
			object_desc(Ind, o_name, o_ptr, TRUE, 3);
	
			sprintf(out_val, "You see %s", o_name);
			
			found = TRUE;
		}
	}
	if (!found)
	{
		int feat = f_info[c_ptr->feat].mimic;
		cptr name = f_name + f_info[feat].name;
		
		cptr p1 = "A ";

		/* Hack -- handle unknown grids */
		if (!(cave_flag & CAVE_MARK) ) name = "unknown grid";
		
		if (is_a_vowel(name[0])) p1 = "An ";

		/* Hack -- special description for store doors */
		if ((feat >= FEAT_SHOP_HEAD) && (feat <= FEAT_SHOP_TAIL))
		{
			p1 = "The entrance to the ";
		}

		/* Release Tracking */
		if (active) p_ptr->cursor_who = 0;
		
		/* Message */
		sprintf(out_val, "%s%s", p1, name);
	}

}


/*
 * A new "look" command, similar to the "target" command.
 *
 * The "player grid" is always included in the "look" array, so
 * that this command will normally never "fail".
 *
 * XXX XXX XXX Allow "target" inside the "look" command (?)
 */
void do_cmd_look(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	player_type *q_ptr;
	int Depth = p_ptr->dun_depth;

	int		y, x, i;

	cave_type *c_ptr;
	monster_type *m_ptr;
	object_type *o_ptr;

	char o_name[80];
	char out_val[160];

	/* Cancel */
	if (dir == 5 || dir == 64 + 5)
	{
		p_ptr->cursor_who = 0;
		return;
	}

	/* Blind */
	if (p_ptr->blind)
	{
		msg_print(Ind, "You can't see a damn thing!");
		return;
	}

	/* Hallucinating */
	if (p_ptr->image)
	{
		msg_print(Ind, "You can't believe what you are seeing!");
		return;
	}

	/* Manual mode */
	if (dir >= 64)
	{
		target_free_aux(Ind, dir, FALSE);
		return;
	}

	/* Reset "temp" array */
	/* Do it Every time (unless canceled before) */
	/* if (!dir) */
	{
		p_ptr->target_n = 0;

		/* Scan the current panel */
		for (y = p_ptr->panel_row_min; y <= p_ptr->panel_row_max; y++)
		{
			for (x = p_ptr->panel_col_min; x <= p_ptr->panel_col_max; x++)
			{
				/* Require line of sight, unless "look" is "expanded" */
				if (!expand_look && !player_has_los_bold(Ind, y, x)) continue;
	
				/* Require interesting contents */
				if (!do_cmd_look_accept(Ind, y, x)) continue;
	
				/* Save the location */
				p_ptr->target_x[p_ptr->target_n] = x;
				p_ptr->target_y[p_ptr->target_n] = y;
				p_ptr->target_n++;
			}
		}

		/* Only if this is the first time, or if we've been asked to reset */
		if (!dir) {
			/* Start near the player */
			p_ptr->look_index = 0;
		}
		
		/* Paranoia */
		if (!p_ptr->target_n)
		{
			msg_print(Ind, "You see nothing special.");
			return;
		}
	
	
		/* Set the sort hooks */
		ang_sort_comp = ang_sort_comp_distance;
		ang_sort_swap = ang_sort_swap_distance;
	
		/* Sort the positions */
		ang_sort(Ind, p_ptr->target_x, p_ptr->target_y, p_ptr->target_n);

		/* Collect monster and player indices */
		for (i = 0; i < p_ptr->target_n; i++)
		{
			c_ptr = &cave[Depth][p_ptr->target_y[i]][p_ptr->target_x[i]];

			if (c_ptr->m_idx != 0)
				p_ptr->target_idx[i] = c_ptr->m_idx;
			else p_ptr->target_idx[i] = 0;
		}
	}
	
	/* Just be cautius */
	if (p_ptr->look_index > p_ptr->target_n) p_ptr->look_index = p_ptr->target_n;
	
	/* Motion */
	if (dir)
	{
		/* Reset the locations */
		for (i = 0; i < p_ptr->target_n; i++)
		{
			if (p_ptr->target_idx[i] > 0)
			{
				m_ptr = &m_list[p_ptr->target_idx[i]];

				p_ptr->target_y[i] = m_ptr->fy;
				p_ptr->target_x[i] = m_ptr->fx;
			}
			else if (p_ptr->target_idx[i] < 0)
			{
				q_ptr = Players[0 - p_ptr->target_idx[i]];

				/* Check for player leaving */
				if (((0 - p_ptr->target_idx[i]) > NumPlayers) ||
				     (q_ptr->dun_depth != p_ptr->dun_depth))
				{
					p_ptr->target_y[i] = 0;
					p_ptr->target_x[i] = 0;
				}
				else
				{
					p_ptr->target_y[i] = q_ptr->py;
					p_ptr->target_x[i] = q_ptr->px;
				}
			}
		}

		/* Find a new grid if possible */
		i = target_pick(Ind, p_ptr->target_y[p_ptr->look_index], p_ptr->target_x[p_ptr->look_index], ddy[dir], ddx[dir]);

		/* Use that grid */
		if (i >= 0) p_ptr->look_index = i;
	}

	/* Describe */
	y = p_ptr->target_y[p_ptr->look_index];
	x = p_ptr->target_x[p_ptr->look_index];

	/* Paranoia */
	if (!cave[Depth]) return;

	c_ptr = &cave[Depth][y][x];
	
	describe_floor_tile(c_ptr, out_val, Ind, TRUE, p_ptr->cave_flag[y][x]);
	
	/* Append a little info */
	strcat(out_val, " [<dir>, q, p]");

	/* Tell the client */
	Send_target_info(Ind, x - p_ptr->panel_col_prt, y - p_ptr->panel_row_prt, out_val);
}




/*
 * Allow the player to examine other sectors on the map
 */
void do_cmd_locate(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	int		y1, x1, y2, x2;

	char	tmp_val[80];

	char	out_val[160];

  if (dir < 0 || dir > 10) return;

	/* No direction, recenter */
	if (!dir)
	{
		/* Recenter map around the player */
		verify_panel(Ind);

		/* Update stuff */
		p_ptr->update |= (PU_MONSTERS);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD);

		/* Handle stuff */
		handle_stuff(Ind);

		return;
	}

	/* Initialize */
	if (dir == 5)
	{
		/* Remember current panel */
		p_ptr->panel_row_old = p_ptr->panel_row;
		p_ptr->panel_col_old = p_ptr->panel_col;
	}

	/* Start at current panel */
	y2 = p_ptr->panel_row;
	x2 = p_ptr->panel_col;

	/* Initial panel */
	y1 = p_ptr->panel_row_old;
	x1 = p_ptr->panel_col_old;

	/* Apply the motion */
	y2 += ddy[dir];
	x2 += ddx[dir];

	/* Verify the row */
	if (y2 > p_ptr->max_panel_rows) y2 = p_ptr->max_panel_rows;
	else if (y2 < 0) y2 = 0;

	/* Verify the col */
	if (x2 > p_ptr->max_panel_cols) x2 = p_ptr->max_panel_cols;
	else if (x2 < 0) x2 = 0;

	/* Describe the location */
	if ((y2 == y1) && (x2 == x1))
	{
		tmp_val[0] = '\0';
	}
	else
	{
		sprintf(tmp_val, "%s%s of",
		        ((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
		        ((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
	}

	/* Prepare to ask which way to look */
	sprintf(out_val,
	        "Map sector [%d,%d], which is%s your sector.  Direction?",
	        y2, x2, tmp_val);

	msg_print(Ind, out_val);

	/* Set the panel location */
	p_ptr->panel_row = y2;
	p_ptr->panel_col = x2;

	/* Recalculate the boundaries */
	panel_bounds(Ind);

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff(Ind);
}






/*
 * The table of "symbol info" -- each entry is a string of the form
 * "X:desc" where "X" is the trigger, and "desc" is the "info".
 */
static cptr ident_info[] =
{
	" :A dark grid",
	"!:A potion (or oil)",
	"\":An amulet (or necklace)",
	"#:A wall (or secret door)",
	"$:Treasure (gold or gems)",
	"%:A vein (magma or quartz)",
	/* "&:unused", */
	"':An open door",
	"(:Soft armor",
	"):A shield",
	"*:A vein with treasure",
	"+:A closed door",
	",:Food (or mushroom patch)",
	"-:A wand (or rod)",
	".:Floor",
	"/:A polearm (Axe/Pike/etc)",
	/* "0:unused", */
	"1:Entrance to General Store",
	"2:Entrance to Armory",
	"3:Entrance to Weaponsmith",
	"4:Entrance to Temple",
	"5:Entrance to Alchemy shop",
	"6:Entrance to Magic store",
	"7:Entrance to Black Market",
	"8:Entrance to Tavern",
	/* "9:unused", */
	"::Rubble",
	";:A glyph of warding",
	"<:An up staircase",
	"=:A ring",
	">:A down staircase",
	"?:A scroll",
	"@:You",
	"A:Angel",
	"B:Bird",
	"C:Canine",
	"D:Ancient Dragon/Wyrm",
	"E:Elemental",
	"F:Dragon Fly",
	"G:Ghost",
	"H:Hybrid",
	"I:Insect",
	"J:Snake",
	"K:Killer Beetle",
	"L:Lich",
	"M:Multi-Headed Reptile",
	/* "N:unused", */
	"O:Ogre",
	"P:Giant Humanoid",
	"Q:Quylthulg (Pulsing Flesh Mound)",
	"R:Reptile/Amphibian",
	"S:Spider/Scorpion/Tick",
	"T:Troll",
	"U:Major Demon",
	"V:Vampire",
	"W:Wight/Wraith/etc",
	"X:Xorn/Xaren/etc",
	"Y:Yeti",
	"Z:Zephyr Hound",
	"[:Hard armor",
	"\\:A hafted weapon (mace/whip/etc)",
	"]:Misc. armor",
	"^:A trap",
	"_:A staff",
	/* "`:unused", */
	"a:Ant",
	"b:Bat",
	"c:Centipede",
	"d:Dragon",
	"e:Floating Eye",
	"f:Feline",
	"g:Golem",
	"h:Hobbit/Elf/Dwarf",
	"i:Icky Thing",
	"j:Jelly",
	"k:Kobold",
	"l:Louse",
	"m:Mold",
	"n:Naga",
	"o:Orc",
	"p:Person/Human",
	"q:Quadruped",
	"r:Rodent",
	"s:Skeleton",
	"t:Townsperson",
	"u:Minor Demon",
	"v:Vortex",
	"w:Worm/Worm-Mass",
	/* "x:unused", */
	"y:Yeek",
	"z:Zombie/Mummy",
	"{:A missile (arrow/bolt/shot)",
	"|:An edged weapon (sword/dagger/etc)",
	"}:A launcher (bow/crossbow/sling)",
	"~:A tool (or miscellaneous item)",
	NULL
};



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
#if 0
static bool ang_sort_comp_hook(int Ind, vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;

	Ind = Ind;

	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = r_info[w1].r_pkills;
		z2 = r_info[w2].r_pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = r_info[w1].r_tkills;
		z2 = r_info[w2].r_tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}
#endif


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
#if 0
static void ang_sort_swap_hook(int Ind, vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* XXX XXX */
	v = v ? v : 0;

	Ind = Ind;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}
#endif


/*
 * Identify a character
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
void do_cmd_query_symbol(int Ind, char sym)
{
	int		i;
	char	buf[128];


	/* If no symbol, abort --KLJ-- */
	if (!sym)
		return;

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
	}

	/* Display the result */
	msg_print(Ind, buf);
}
