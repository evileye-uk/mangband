/* File: cmd2.c */

/* Purpose: Movement commands (part 2) */

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
 * Go up one level					-RAK-
 */
void do_cmd_go_up(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	cave_type *c_ptr;

	/* Make sure he hasn't just changed depth */
	if (p_ptr->new_level_flag)
		return;

	/* Player grid */
	c_ptr = &cave[Depth][p_ptr->py][p_ptr->px];

	/* Verify stairs if not a ghost, or admin wizard */
	if (!p_ptr->ghost && (strcmp(p_ptr->name,cfg_dungeon_master)) && c_ptr->feat != FEAT_LESS)
	{
		msg_print(Ind, "I see no up staircase here.");
		return;
	}
	else
	{	
		if (p_ptr->dun_depth <= 0)
		{
			msg_print(Ind, "There is nothing above you.");
			return;
		}
	}

	if (cfg_ironman)
	{
		/* 
		 * Ironmen don't go up
		 */
		if(strcmp(p_ptr->name,cfg_dungeon_master))
		{
			msg_print(Ind, "Morgoth awaits you in the darkness below.");
			return;	
		}
	}

	/* Remove the player from the old location */
	c_ptr->m_idx = 0;

	/* Show everyone that's he left */
	everyone_lite_spot(Depth, p_ptr->py, p_ptr->px);

	/* Forget his lite and viewing area */
	forget_lite(Ind);
	forget_view(Ind);

	/* Hack -- take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Success */
	if (c_ptr->feat == FEAT_LESS)
	{
		msg_print(Ind, "You enter a maze of up staircases.");
		p_ptr->new_level_method = LEVEL_UP;
	}
	else
	{
		msg_print(Ind, "You float upwards.");
		p_ptr->new_level_method = LEVEL_GHOST;
	}

	/* A player has left this depth */
	players_on_depth[p_ptr->dun_depth]--;

	/* Go up the stairs */
	p_ptr->dun_depth--;

	/* And another player has entered this depth */
	players_on_depth[p_ptr->dun_depth]++;

	p_ptr->new_level_flag = TRUE;

	/* Create a way back */
	create_down_stair = TRUE;
}


/*
 * Go down one level
 */
void do_cmd_go_down(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	cave_type *c_ptr;

	/* Make sure he hasn't just changed depth */
	if (p_ptr->new_level_flag)
		return;

	/* Player grid */
	c_ptr = &cave[Depth][p_ptr->py][p_ptr->px];

	/* Verify stairs */

	if (p_ptr->ghost && (
				strcmp(p_ptr->name,cfg_dungeon_master)
				) && !cfg_ghost_diving
		 ) {
		msg_print(Ind, "You seem unable to go down.  Try going up.");
		return;
	};

	if (!p_ptr->ghost && (
				strcmp(p_ptr->name,cfg_dungeon_master)
				) && c_ptr->feat != FEAT_MORE)
	{
		msg_print(Ind, "I see no down staircase here.");
		return;
	}
	else
	{
		/* Can't go down in the wilderness */
		if (p_ptr->dun_depth < 0)
		{
			msg_print(Ind, "There is nothing below you.");
			return;
		}

		/* Verify maximum depth */
		if (p_ptr->dun_depth >= 127)
		{
			msg_print(Ind, "You are at the bottom of the dungeon.");
			return;
		}
	}

	/* Remove the player from the old location */
	c_ptr->m_idx = 0;

	/* Show everyone that's he left */
	everyone_lite_spot(Depth, p_ptr->py, p_ptr->px);

	/* Forget his lite and viewing area */
	forget_lite(Ind);
	forget_view(Ind);

	/* Hack -- take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Success */
	if (c_ptr->feat == FEAT_MORE)
	{
		msg_print(Ind, "You enter a maze of down staircases.");
		p_ptr->new_level_method = LEVEL_DOWN;
	}
	else
	{
		msg_print(Ind, "You float downwards.");
		p_ptr->new_level_method = LEVEL_GHOST;
	}

	/* A player has left this depth */
	players_on_depth[p_ptr->dun_depth]--;

	/* Go down */
	p_ptr->dun_depth++;

	/* Another player has entered this depth */
	players_on_depth[p_ptr->dun_depth]++;

	p_ptr->new_level_flag = TRUE;

	/* Create a way back */
	create_up_stair = TRUE;
}



/*
 * Simple command to "search" for one turn
 */
void do_cmd_search(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Search */
	search(Ind);
}


/*
 * Hack -- toggle search mode
 */
void do_cmd_toggle_search(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Stop searching */
	if (p_ptr->searching)
	{
		/* Clear the searching flag */
		p_ptr->searching = FALSE;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);
	}

	/* Start searching */
	else
	{
		/* Set the searching flag */
		p_ptr->searching = TRUE;

		/* Update stuff */
		p_ptr->update |= (PU_BONUS);

		/* Redraw stuff */
		p_ptr->redraw |= (PR_STATE | PR_SPEED);
	}
}



/*
 * Allocates objects upon opening a chest    -BEN-
 *
 * Disperse treasures from the chest "o_ptr", centered at (x,y).
 *
 * Small chests often contain "gold", while Large chests always contain
 * items.  Wooden chests contain 2 items, Iron chests contain 4 items,
 * and Steel chests contain 6 items.  The "value" of the items in a
 * chest is based on the "power" of the chest, which is in turn based
 * on the level on which the chest is generated.
 */
static void chest_death(int Ind, int y, int x, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int		i, d, ny, nx;
	int		number, small_;


	/* Must be a chest */
	if (o_ptr->tval != TV_CHEST) return;

	/* Small chests often hold "gold" */
	small_ = (o_ptr->sval < SV_CHEST_MIN_LARGE);

	/* Determine how much to drop (see above) */
	number = (o_ptr->sval % SV_CHEST_MIN_LARGE) * 2;

	/* Generate some treasure */
	if (o_ptr->pval && (number > 0))
	{
		/* Drop some objects (non-chests) */
		for (; number > 0; --number)
		{
			/* Try 20 times per item */
			for (i = 0; i < 20; ++i)
			{
				/* Pick a distance */
				d = ((i + 15) / 15);

				/* Pick a location */
				scatter(Depth, &ny, &nx, y, x, d, 0);

				/* Must be a clean floor grid */
				if (!cave_clean_bold(Depth, ny, nx)) continue;

				/* Opening a chest */
				opening_chest = TRUE;

				/* Determine the "value" of the items */
				object_level = ABS(o_ptr->pval) + 10;

				/* Small chests often drop gold */
				if (small_ && (rand_int(100) < 75))
				{
					place_gold(Depth, ny, nx);
				}

				/* Otherwise drop an item */
				else
				{
					place_object(Depth, ny, nx, FALSE, FALSE, 0);
				}

				/* Reset the object level */
				object_level = Depth;

				/* No longer opening a chest */
				opening_chest = FALSE;

				/* Notice it */
				note_spot(Ind, ny, nx);

				/* Display it */
				everyone_lite_spot(Depth, ny, nx);

				/* Under the player */
				if ((ny == p_ptr->py) && (nx == p_ptr->px))
				{
					msg_print(Ind, "You feel something roll beneath your feet.");
				}

				/* Successful placement */
				break;
			}
		}
	}

	/* Empty */
	o_ptr->pval = 0;

	/* Known */
	object_known(o_ptr);
}


/*
 * Chests have traps too.
 *
 * Exploding chest destroys contents (and traps).
 * Note that the chest itself is never destroyed.
 */
static void chest_trap(int Ind, int y, int x, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];

	int  i, trap;


	/* Only analyze chests */
	if (o_ptr->tval != TV_CHEST) return;

	/* Ignore disarmed chests */
	if (o_ptr->pval <= 0) return;

	/* Obtain the traps */
	trap = chest_traps[o_ptr->pval];

	/* Lose strength */
	if (trap & CHEST_LOSE_STR)
	{
		msg_print(Ind, "A small needle has pricked you!");
		take_hit(Ind, damroll(1, 4), "a poison needle");
		(void)do_dec_stat(Ind, A_STR);
	}

	/* Lose constitution */
	if (trap & CHEST_LOSE_CON)
	{
		msg_print(Ind, "A small needle has pricked you!");
		take_hit(Ind, damroll(1, 4), "a poison needle");
		(void)do_dec_stat(Ind, A_CON);
	}

	/* Poison */
	if (trap & CHEST_POISON)
	{
		msg_print(Ind, "A puff of green gas surrounds you!");
		if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
		{
			(void)set_poisoned(Ind, p_ptr->poisoned + 10 + randint(20));
		}
	}

	/* Paralyze */
	if (trap & CHEST_PARALYZE)
	{
		msg_print(Ind, "A puff of yellow gas surrounds you!");
		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(Ind, p_ptr->paralyzed + 10 + randint(20));
		}
	}

	/* Summon monsters */
	if (trap & CHEST_SUMMON)
	{
		int num = 2 + randint(3);
		msg_print(Ind, "You are enveloped in a cloud of smoke!");
		for (i = 0; i < num; i++)
		{
			(void)summon_specific(p_ptr->dun_depth, y, x, p_ptr->dun_depth, 0);
		}
	}

	/* Explode */
	if (trap & CHEST_EXPLODE)
	{
		msg_print(Ind, "There is a sudden explosion!");
		msg_print(Ind, "Everything inside the chest is destroyed!");
		o_ptr->pval = 0;
		take_hit(Ind, damroll(5, 8), "an exploding chest");
	}
}


/*
 * Return the index of a house given an coordinate pair
 */
int pick_house(int Depth, int y, int x)
{
	int i;

	/* Check each house */
	for (i = 0; i < num_houses; i++)
	{
		/* Check this one */
		if (houses[i].door_x == x && houses[i].door_y == y && houses[i].depth == Depth)
		{
			/* Return */
			return i;
		}
	}

	/* Failure */
	return -1;
}

/*
 * Determine if the player is inside the house
 */
bool house_inside(int Ind, int house)
{
	player_type *p_ptr = Players[Ind];

	if (house >= 0 && house < num_houses)
	{
		if (houses[house].depth == p_ptr->dun_depth
				&& p_ptr->px >= houses[house].x_1 && p_ptr->px <= houses[house].x_2 
				&& p_ptr->py >= houses[house].y_1 && p_ptr->py <= houses[house].y_2)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Determine if the given house is owned
 */
bool house_owned(int house)
{
	if (house >= 0 && house < num_houses)
	{
		if (houses[house].owned[0])
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Determine if the given player owns the given house
 */
bool house_owned_by(int Ind, int house)
{
	player_type *p_ptr = Players[Ind];

	/* If not owned at all, obviously not owned by this player */
	if (!house_owned(house)) return FALSE;

	/* It's owned, is it by this player */
	if (!strcmp(p_ptr->name,houses[house].owned))
	{
		return TRUE;
	}
	return FALSE;
}

/*
 * Return the number of houses owned by the player
 */
int houses_owned(int Ind)
{
	int i;
	int owned = 0;

	/* Check each house */
	for (i = 0; i < num_houses; i++)
	{
		if(house_owned_by(Ind,i))
		{
			owned++;
		}
	}

	return owned;
}


/*
 * Set the owner of the given house
 */
bool set_house_owner(int Ind, int house)
{
	player_type *p_ptr = Players[Ind];

	/* Not if it's already owned */
	if (house_owned(house)) return FALSE;

	/* Set the player as the owner */
	strncpy(houses[house].owned,p_ptr->name,MAX_NAME_LEN);

	return TRUE;
}

/*
 * Set the given house as unowned
 */
void disown_house(int house)
{
	cave_type *c_ptr;
	int i,j, Depth;

	if (house >= 0 && house < num_houses)
	{
		Depth = houses[house].depth;
		houses[house].owned[0] = '\0';
		houses[house].strength = 0;
		/* Remove all players from the house */
		for (i = 1; i < NumPlayers + 1; i++)
		{
			if (house_inside(i, house))
			{
				msg_print(i, "You have been expelled from the house.");
				teleport_player(i, 5);
			}
		}
		/* Clear any items from the house */
		for (i = houses[house].y_1; i <= houses[house].y_2; i++) 
		{
			for (j = houses[house].x_1; j <= houses[house].x_2; j++) 
			{ 
				delete_object(houses[house].depth,i,j); 
			} 
		} 		

		/* Paranoia! */		
		if (!cave[Depth]) return;

		/* Get requested grid */
		c_ptr = &cave[Depth][houses[house].door_y][houses[house].door_x];

		/* Close the door */
		c_ptr->feat = FEAT_HOME_HEAD + houses[house].strength;

		/* Reshow */
		everyone_lite_spot(Depth, houses[house].door_y, houses[house].door_x);	

	}		
}

/*
 * Open a closed door or closed chest.
 *
 * Note unlocking a locked door/chest is worth one experience point.
 */
void do_cmd_open(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	player_type *q_ptr;
	int Depth = p_ptr->dun_depth;

	int				y, x, i, j, k;
	int				flag;

	cave_type		*c_ptr;
	object_type		*o_ptr;

	bool more = FALSE;


	/* Ghosts cannot open doors */
	if ((p_ptr->ghost) || (p_ptr->fruit_bat))
	{
		msg_print(Ind, "You cannot open things!");
		return;
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (dir > 0 && dir < 10)
	{
		/* Get requested location */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get requested grid */
		c_ptr = &cave[Depth][y][x];

		/* Get the object (if any) */
		o_ptr = &o_list[c_ptr->o_idx];

		/* Nothing useful */
		if (!((c_ptr->feat >= FEAT_DOOR_HEAD) &&
					(c_ptr->feat <= FEAT_DOOR_TAIL)) &&
				!((c_ptr->feat >= FEAT_HOME_HEAD) &&
					(c_ptr->feat <= FEAT_HOME_TAIL)) &&
				(o_ptr->tval != TV_CHEST))
		{
			/* Message */
			msg_print(Ind, "You see nothing there to open.");
		}

		/* Monster in the way */
		else if (c_ptr->m_idx > 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a monster in the way!");

			/* Attack */
			py_attack(Ind, y, x);
		}

		/* Open a closed chest. */
		else if (o_ptr->tval == TV_CHEST)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Assume opened successfully */
			flag = TRUE;

			/* Attempt to unlock it */
			if (o_ptr->pval > 0)
			{
				/* Assume locked, and thus not open */
				flag = FALSE;

				/* Get the "disarm" factor */
				i = p_ptr->skill_dis;

				/* Penalize some conditions */
				if (p_ptr->blind || no_lite(Ind)) i = i / 10;
				if (p_ptr->confused || p_ptr->image) i = i / 10;

				/* Extract the difficulty */
				j = i - o_ptr->pval;

				/* Always have a small chance of success */
				if (j < 2) j = 2;

				/* Success -- May still have traps */
				if (rand_int(100) < j)
				{
					msg_print(Ind, "You have picked the lock.");
					gain_exp(Ind, 1);
					flag = TRUE;
				}

				/* Failure -- Keep trying */
				else
				{
					/* We may continue repeating */
					more = TRUE;
					if (flush_failure) flush();
					msg_print(Ind, "You failed to pick the lock.");
				}
			}

			/* Allowed to open */
			if (flag)
			{
				/* Apply chest traps, if any */
				chest_trap(Ind, y, x, o_ptr);

				/* Let the Chest drop items */
				chest_death(Ind, y, x, o_ptr);
			}
		}

		/* Jammed door */
		else if (c_ptr->feat >= FEAT_DOOR_HEAD + 0x08 && c_ptr->feat <= FEAT_DOOR_TAIL)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Stuck */
			msg_print(Ind, "The door appears to be stuck.");
		}

		/* Locked door */
		else if (c_ptr->feat >= FEAT_DOOR_HEAD + 0x01 && c_ptr->feat <= FEAT_DOOR_HEAD + 0x07)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Disarm factor */
			i = p_ptr->skill_dis;

			/* Penalize some conditions */
			if (p_ptr->blind || no_lite(Ind)) i = i / 10;
			if (p_ptr->confused || p_ptr->image) i = i / 10;

			/* Extract the lock power */
			j = c_ptr->feat - FEAT_DOOR_HEAD;

			/* Extract the difficulty XXX XXX XXX */
			j = i - (j * 4);

			/* Always have a small chance of success */
			if (j < 2) j = 2;

			/* Success */
			if (rand_int(100) < j)
			{
				/* Message */
				msg_print(Ind, "You have picked the lock.");

				/* Experience */
				gain_exp(Ind, 1);

				/* Open the door */
				c_ptr->feat = FEAT_OPEN;

				/* Notice */
				note_spot_depth(Depth, y, x);

				/* Redraw */
				everyone_lite_spot(Depth, y, x);

				/* Update some things */
				p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS);
			}

			/* Failure */
			else
			{
				/* Failure */
				if (flush_failure) flush();

				/* Message */
				msg_print(Ind, "You failed to pick the lock.");

				/* We may keep trying */
				more = TRUE;
			}
		}

		/* Home */
		else if (c_ptr->feat >= FEAT_HOME_HEAD && c_ptr->feat <= FEAT_HOME_TAIL)
		{
			i = pick_house(Depth, y, x);

			/* Do we own this house? */
			if (house_owned_by(Ind,i) || (!(strcmp(p_ptr->name,cfg_dungeon_master))) )
			{

				/* If someone is in our store, we can't enter (anti-exploit) */
				/* FIXME This can be removed once we have a mechanism to eject a
				 * client from a shop.  See store_purchase. 
				 */
				for (k = 1; k <= NumPlayers; k++ )
				{
					/* We don't block the owner from getting out! */
					q_ptr = Players[k];
					if(q_ptr && Ind != k)
					{
						/* We do block the owner getting in */
						if(q_ptr->player_store_num == i && q_ptr->store_num == 8)
						{
							msg_print(Ind, "The shoppers block the entrance!");
							return;		
						}
					}				
				}

				/* Open the door */
				c_ptr->feat = FEAT_HOME_OPEN;

				/* Take half a turn */
				p_ptr->energy -= level_speed(p_ptr->dun_depth)/2;

				/* Notice */
				note_spot_depth(Depth, y, x);

				/* Redraw */
				everyone_lite_spot(Depth, y, x);

				/* Update some things */
				p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS);

				/* Done */
				return;
			}

			/* He's not the owner, check if owned */
			else if (house_owned(i))
			{
				/* Player owned store! */

				/* Disturb */
				disturb(Ind, 0);

				/* Hack -- Enter store */
				command_new = '_';
				do_cmd_store(Ind,i);
			}
			else
			{
				int price, factor;

				/* Take CHR into account */
				factor = adj_chr_gold[p_ptr->stat_ind[A_CHR]];
				price = (unsigned long) houses[i].price * factor / 100;
				if(Depth==0) {
					price = (unsigned long)price *5L;
				};

				/* Tell him the price */
				msg_format(Ind, "This house costs %ld gold.", price);
			}
		}

		/* Closed door */
		else
		{
			/* Take half a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth)/2;

			/* Open the door */
			c_ptr->feat = FEAT_OPEN;

			/* Notice */
			note_spot_depth(Depth, y, x);

			/* Redraw */
			everyone_lite_spot(Depth, y, x);

			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(Ind, 0);
}


/*
 * Close an open door.
 */
void do_cmd_close(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int			y, x, i;
	cave_type		*c_ptr;

	bool more = FALSE;


	/* Ghosts cannot close */
	if ( (p_ptr->ghost) || (p_ptr->fruit_bat) )
	{
		if(strcmp(p_ptr->name,cfg_dungeon_master))  {
			msg_print(Ind, "You cannot close things!");
			return;
		};
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (dir > 0 && dir < 10)
	{
		/* Get requested location */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[Depth][y][x];

		/* Broken door */
		if (c_ptr->feat == FEAT_BROKEN)
		{
			/* Message */
			msg_print(Ind, "The door appears to be broken.");
		}

		/* Require open door */
		else if (c_ptr->feat != FEAT_OPEN && c_ptr->feat != FEAT_HOME_OPEN)
		{
			/* Message */
			msg_print(Ind, "You see nothing there to close.");
		}

		/* Monster in the way */
		else if (c_ptr->m_idx > 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a monster in the way!");

			/* Attack */
			py_attack(Ind, y, x);
		}
		/* Player in the way */
		else if (c_ptr->m_idx < 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a player in the way!");

		}

		/* House door, close it */
		else if (c_ptr->feat == FEAT_HOME_OPEN)
		{
			/* Find this house */
			i = pick_house(Depth, y, x);

			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Close the door */
			c_ptr->feat = FEAT_HOME_HEAD + houses[i].strength;

			/* Notice */
			note_spot_depth(Depth, y, x);

			/* Redraw */
			everyone_lite_spot(Depth, y, x);

			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS);
		}

		/* Close the door */
		else
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Close the door */
			c_ptr->feat = FEAT_DOOR_HEAD + 0x00;

			/* Notice */
			note_spot_depth(Depth, y, x);

			/* Redraw */
			everyone_lite_spot(Depth, y, x);

			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_MONSTERS);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(Ind, 0);
}



/*
 * Tunnel through wall.  Assumes valid location.
 *
 * Note that it is impossible to "extend" rooms past their
 * outer walls (which are actually part of the room).
 *
 * This will, however, produce grids which are NOT illuminated
 * (or darkened) along with the rest of the room.
 */
static bool twall(int Ind, int y, int x)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	cave_type	*c_ptr = &cave[Depth][y][x];
	byte		*w_ptr = &p_ptr->cave_flag[y][x];

	/* Paranoia -- Require a wall or door or some such */
	if (cave_floor_bold(Depth, y, x)) return (FALSE);

	/* Remove the feature */
	if (Depth > 0)
		c_ptr->feat = FEAT_FLOOR;
	else
		c_ptr->feat = FEAT_DIRT;

	/* Forget the "field mark" */
	*w_ptr &= ~CAVE_MARK;

	/* Notice */
	note_spot_depth(Depth, y, x);

	/* Redisplay the grid */
	everyone_lite_spot(Depth, y, x);

	/* Update some things */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MONSTERS);

	/* Result */
	return (TRUE);
}



/*
 * Tunnels through "walls" (including rubble and closed doors)
 *
 * Note that tunneling almost always takes time, since otherwise
 * you can use tunnelling to find monsters.  Also note that you
 * must tunnel in order to hit invisible monsters in walls (etc).
 *
 * Digging is very difficult without a "digger" weapon, but can be
 * accomplished by strong players using heavy weapons.
 */
void do_cmd_tunnel(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int			y, x;

	cave_type		*c_ptr;

	bool old_floor;

	bool more = FALSE;


	/* Ghosts have no need to tunnel */
	if ( (p_ptr->ghost) || (p_ptr->fruit_bat) )
	{
		/* Message */
		msg_print(Ind, "You cannot tunnel.");

		return;
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction to tunnel, or Abort */
	if (dir > 0 && dir < 10)
	{
		/* Get location */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get grid */
		c_ptr = &cave[Depth][y][x];

		/* Check the floor-hood */
		old_floor = cave_floor_bold(Depth, y, x);

		/* No tunnelling through emptiness */
		if ( (cave_floor_bold(Depth, y, x)) || (c_ptr->feat == FEAT_PERM_CLEAR) )
		{
			/* Message */
			msg_print(Ind, "You see nothing there to tunnel through.");
		}

		/* No tunnelling through doors */
		else if ((c_ptr->feat < FEAT_SECRET || c_ptr->feat >= FEAT_DRAWBRIDGE) 
				&& (c_ptr->feat != FEAT_TREE && c_ptr->feat != FEAT_EVIL_TREE)	)
		{
			/* Message */
			msg_print(Ind, "You cannot tunnel through doors.");
		}

		/* A monster is in the way */
		else if (c_ptr->m_idx > 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a monster in the way!");

			/* Attack */
			py_attack(Ind, y, x);
		}

		/* Okay, try digging */
		else
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Titanium */
			if (c_ptr->feat >= FEAT_PERM_EXTRA && c_ptr->feat <= FEAT_PERM_SOLID)
			{
				msg_print(Ind, "This seems to be permanent rock.");
			}

			/* Granite */
			else if (c_ptr->feat >= FEAT_WALL_EXTRA && c_ptr->feat <= FEAT_WALL_SOLID)
			{
				/* Tunnel */
				if ((p_ptr->skill_dig > 40 + rand_int(1600)) && twall(Ind, y, x))
				{
					msg_print(Ind, "You have finished the tunnel.");
				}

				/* Keep trying */
				else
				{
					/* We may continue tunelling */
					msg_print(Ind, "You tunnel into the granite wall.");
					more = TRUE;
				}
			}

			/* Quartz / Magma */
			else if (c_ptr->feat >= FEAT_MAGMA && c_ptr->feat <= FEAT_QUARTZ_K)
			{
				bool okay;
				bool gold = FALSE;
				bool hard = FALSE;

				/* Found gold */
				if (c_ptr->feat >= FEAT_MAGMA_H) gold = TRUE;

				/* Extract "quartz" flag XXX XXX XXX */
				if ((c_ptr->feat - FEAT_MAGMA) & 0x01) hard = TRUE;

				/* Quartz */
				if (hard)
				{
					okay = (p_ptr->skill_dig > 20 + rand_int(800));
				}

				/* Magma */
				else
				{
					okay = (p_ptr->skill_dig > 10 + rand_int(400));
				}

				/* Success */
				if (okay && twall(Ind, y, x))
				{
					/* Found treasure */
					if (gold)
					{
						/* Place some gold */
						place_gold(Depth, y, x);

						/* Notice it */
						note_spot_depth(Depth, y, x);

						/* Display it */
						everyone_lite_spot(Depth, y, x);

						/* Message */
						msg_print(Ind, "You have found something!");
					}

					/* Found nothing */
					else
					{
						/* Message */
						msg_print(Ind, "You have finished the tunnel.");
					}
				}

				/* Failure (quartz) */
				else if (hard)
				{
					/* Message, continue digging */
					msg_print(Ind, "You tunnel into the quartz vein.");
					more = TRUE;
				}

				/* Failure (magma) */
				else
				{
					/* Message, continue digging */
					msg_print(Ind, "You tunnel into the magma vein.");
					more = TRUE;
				}
			}

			/* Rubble */
			else if (c_ptr->feat == FEAT_RUBBLE)
			{
				/* Remove the rubble */
				if ((p_ptr->skill_dig > rand_int(200)) && twall(Ind, y, x))
				{
					/* Message */
					msg_print(Ind, "You have removed the rubble.");

					/* Hack -- place an object */
					if (rand_int(100) < 10)
					{
						place_object(Depth, y, x, FALSE, FALSE, 0);
						if (player_can_see_bold(Ind, y, x))
						{
							msg_print(Ind, "You have found something!");
						}
					}

					/* Notice */
					note_spot_depth(Depth, y, x);

					/* Display */
					everyone_lite_spot(Depth, y, x);
				}

				else
				{
					/* Message, keep digging */
					msg_print(Ind, "You dig in the rubble.");
					more = TRUE;
				}
			}

			else if (c_ptr->feat == FEAT_TREE)
			{
				/* mow down the vegetation */
				if ((p_ptr->skill_dig > rand_int(400)) && twall(Ind, y, x))
				{
					if (Depth == 0) trees_in_town--;				

					/* Message */
					msg_print(Ind, "You hack your way through the vegetation.");

					/* Notice */
					note_spot_depth(Depth, y, x);

					/* Display */
					everyone_lite_spot(Depth, y, x);
				}
				else
				{
					/* Message, keep digging */
					msg_print(Ind, "You attempt to clear a path.");
					more = TRUE;
				}
			}

			else if (c_ptr->feat == FEAT_EVIL_TREE)
			{
				/* mow down the vegetation */
				if ((p_ptr->skill_dig > rand_int(600)) && twall(Ind, y, x))
				{
					/* Message */
					msg_print(Ind, "You hack your way through the vegetation.");

					/* Notice */
					note_spot_depth(Depth, y, x);

					/* Display */
					everyone_lite_spot(Depth, y, x);
				}
				else
				{
					/* Message, keep digging */
					msg_print(Ind, "You attempt to clear a path.");
					more = TRUE;
				}
			}

			/* Default to secret doors */
			else /* if (c_ptr->feat == FEAT_SECRET) */
			{
				/* Message, keep digging */
				msg_print(Ind, "You tunnel into the granite wall.");
				more = TRUE;

				/* Hack -- Occasional Search */
				if (rand_int(100) < 25) search(Ind);
			}
		}

		/* Notice "blockage" changes */
		if (old_floor != cave_floor_bold(Depth, y, x))
		{
			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MONSTERS);
		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(Ind, 0);
}


/*
 * Disarms a trap, or chest	-RAK-
 */
void do_cmd_disarm(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int                 y, x, i, j, power;

	cave_type		*c_ptr;
	byte			*w_ptr;
	object_type		*o_ptr;

	bool		more = FALSE;


	/* Ghosts cannot disarm */
	if ( (p_ptr->ghost) || (p_ptr->fruit_bat) )
	{
		/* Message */
		msg_print(Ind, "You cannot disarm things!");

		return;
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction (or abort) */
	if (dir > 0 && dir < 10)
	{
		/* Get location */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[Depth][y][x];
		w_ptr = &p_ptr->cave_flag[y][x];

		/* Access the item */
		o_ptr = &o_list[c_ptr->o_idx];

		/* Nothing useful */
		if (!((c_ptr->feat >= FEAT_TRAP_HEAD) &&
					(c_ptr->feat <= FEAT_TRAP_TAIL)) &&
				(o_ptr->tval != TV_CHEST))
		{
			/* Message */
			msg_print(Ind, "You see nothing there to disarm.");
		}

		/* Monster in the way */
		else if (c_ptr->m_idx > 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a monster in the way!");

			/* Attack */
			py_attack(Ind, y, x);
		}

		/* Normal disarm */
		else if (o_ptr->tval == TV_CHEST)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Get the "disarm" factor */
			i = p_ptr->skill_dis;

			/* Penalize some conditions */
			if (p_ptr->blind || no_lite(Ind)) i = i / 10;
			if (p_ptr->confused || p_ptr->image) i = i / 10;

			/* Extract the difficulty */
			j = i - o_ptr->pval;

			/* Always have a small chance of success */
			if (j < 2) j = 2;

			/* Must find the trap first. */
			if (!object_known_p(Ind, o_ptr))
			{
				msg_print(Ind, "I don't see any traps.");
			}

			/* Already disarmed/unlocked */
			else if (o_ptr->pval <= 0)
			{
				msg_print(Ind, "The chest is not trapped.");
			}

			/* No traps to find. */
			else if (!chest_traps[o_ptr->pval])
			{
				msg_print(Ind, "The chest is not trapped.");
			}

			/* Success (get a lot of experience) */
			else if (rand_int(100) < j)
			{
				msg_print(Ind, "You have disarmed the chest.");
				gain_exp(Ind, o_ptr->pval);
				o_ptr->pval = (0 - o_ptr->pval);
			}

			/* Failure -- Keep trying */
			else if ((i > 5) && (randint(i) > 5))
			{
				/* We may keep trying */
				more = TRUE;
				if (flush_failure) flush();
				msg_print(Ind, "You failed to disarm the chest.");
			}

			/* Failure -- Set off the trap */
			else
			{
				msg_print(Ind, "You set off a trap!");
				chest_trap(Ind, y, x, o_ptr);
			}
		}

		/* Disarm a trap */
		else
		{
			cptr name = (f_name + f_info[c_ptr->feat].name);

			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Get the "disarm" factor */
			i = p_ptr->skill_dis;

			/* Penalize some conditions */
			if (p_ptr->blind || no_lite(Ind)) i = i / 10;
			if (p_ptr->confused || p_ptr->image) i = i / 10;

			/* XXX XXX XXX Variable power? */

			/* Extract trap "power" */
			power = 5;

			/* Extract the difficulty */
			j = i - power;

			/* Always have a small chance of success */
			if (j < 2) j = 2;

			/* Success */
			if (rand_int(100) < j)
			{
				/* Message */
				msg_format(Ind, "You have disarmed the %s.", name);

				/* Reward */
				gain_exp(Ind, power);

				/* Remove the trap */
				c_ptr->feat = FEAT_FLOOR;

				/* Forget the "field mark" */
				everyone_forget_spot(Depth, y, x);

				/* Notice */
				note_spot_depth(Depth, y, x);

				/* Redisplay the grid */
				everyone_lite_spot(Depth, y, x);

				/* move the player onto the trap grid */
				move_player(Ind, dir, FALSE);
			}

			/* Failure -- Keep trying */
			else if ((i > 5) && (randint(i) > 5))
			{
				/* Failure */
				if (flush_failure) flush();

				/* Message */
				msg_format(Ind, "You failed to disarm the %s.", name);

				/* We may keep trying */
				more = TRUE;
			}

			/* Failure -- Set off the trap */
			else
			{
				/* Message */
				msg_format(Ind, "You set off the %s!", name);

				/* Move the player onto the trap */
				move_player(Ind, dir, FALSE);
			}
		}
	}

	/* Cancel repeat unless told not to */
	if (!more) disturb(Ind, 0);
}


/*
 * Bash open a door, success based on character strength
 *
 * For a closed door, pval is positive if locked; negative if stuck.
 *
 * For an open door, pval is positive for a broken door.
 *
 * A closed door can be opened - harder if locked. Any door might be
 * bashed open (and thereby broken). Bashing a door is (potentially)
 * faster! You move into the door way. To open a stuck door, it must
 * be bashed. A closed door can be jammed (see do_cmd_spike()).
 *
 * Breatures can also open or bash doors, see elsewhere.
 *
 * We need to use character body weight for something, or else we need
 * to no longer give female characters extra starting gold.
 */
void do_cmd_bash(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int                 y, x;

	int			bash, temp;

	cave_type		*c_ptr;

	bool		more = FALSE;


	/* Ghosts cannot bash */
	if ( (p_ptr->ghost) || (p_ptr->fruit_bat) )
	{
		/* Message */
		msg_print(Ind, "You cannot bash things!");

		return;
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (dir > 0 && dir < 10)
	{
		/* Bash location */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get grid */
		c_ptr = &cave[Depth][y][x];

		/* Nothing useful */
		if (!((c_ptr->feat >= FEAT_DOOR_HEAD) &&
					(c_ptr->feat <= FEAT_DOOR_TAIL)))
		{
			/* Message */
			msg_print(Ind, "You see nothing there to bash.");
		}

		/* Monster in the way */
		else if (c_ptr->m_idx > 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a monster in the way!");

			/* Attack */
			py_attack(Ind, y, x);
		}

		/* Bash a closed door */
		else
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "You smash into the door!");

			/* Hack -- Bash power based on strength */
			/* (Ranges from 3 to 20 to 100 to 200) */
			bash = adj_str_blow[p_ptr->stat_ind[A_STR]];

			/* Extract door power */
			temp = ((c_ptr->feat - FEAT_DOOR_HEAD) & 0x07);

			/* Compare bash power to door power XXX XXX XXX */
			temp = (bash - (temp * 10));

			/* Hack -- always have a chance */
			if (temp < 1) temp = 1;

			/* Hack -- attempt to bash down the door */
			if (rand_int(100) < temp)
			{
				/* Message */
				msg_print(Ind, "The door crashes open!");

				/* Break down the door */
				if (rand_int(100) < 50)
				{
					c_ptr->feat = FEAT_BROKEN;
				}

				/* Open the door */
				else
				{
					c_ptr->feat = FEAT_OPEN;
				}

				/* Notice */
				note_spot_depth(Depth, y, x);

				/* Redraw */
				everyone_lite_spot(Depth, y, x);

				/* Hack -- Fall through the door */
				move_player(Ind, dir, FALSE);

				/* Update some things */
				p_ptr->update |= (PU_VIEW | PU_LITE);
				p_ptr->update |= (PU_DISTANCE);
			}

			/* Saving throw against stun */
			else if (rand_int(100) < adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
					p_ptr->lev)
			{
				/* Message */
				msg_print(Ind, "The door holds firm.");

				/* Allow repeated bashing */
				more = TRUE;
			}

			/* High dexterity yields coolness */
			else
			{
				/* Message */
				msg_print(Ind, "You are off-balance.");

				/* Hack -- Lose balance ala paralysis */
				(void)set_paralyzed(Ind, p_ptr->paralyzed + 2 + rand_int(2));
			}
		}
	}

	/* Unless valid action taken, cancel bash */
	if (!more) disturb(Ind, 0);
}



/*
 * Find the index of some "spikes", if possible.
 *
 * XXX XXX XXX Let user choose a pile of spikes, perhaps?
 */
static bool get_spike(int Ind, int *ip)
{
	player_type *p_ptr = Players[Ind];

	int i;

	/* Check every item in the pack */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &(p_ptr->inventory[i]);

		/* Check the "tval" code */
		if (o_ptr->tval == TV_SPIKE)
		{
			/* Save the spike index */
			(*ip) = i;

			/* Success */
			return (TRUE);
		}
	}

	/* Oops */
	return (FALSE);
}



/*
 * Jam a closed door with a spike
 *
 * This command may NOT be repeated
 */
void do_cmd_spike(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int                  y, x, item;

	cave_type		*c_ptr;


	/* Ghosts cannot spike */
	if ( (p_ptr->ghost) || (p_ptr->fruit_bat) )
	{
		/* Message */
		msg_print(Ind, "You cannot spike doors!");

		return;
	}

	/* Get a "repeated" direction */
	if (dir > 0 && dir < 10)
	{
		/* Get location */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[Depth][y][x];

		/* Require closed door */
		if (!((c_ptr->feat >= FEAT_DOOR_HEAD) &&
					(c_ptr->feat <= FEAT_DOOR_TAIL)))
		{
			/* Message */
			msg_print(Ind, "You see nothing there to spike.");
		}

		/* Get a spike */
		else if (!get_spike(Ind, &item))
		{
			/* Message */
			msg_print(Ind, "You have no spikes!");
		}

		/* Is a monster in the way? */
		else if (c_ptr->m_idx > 0)
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Message */
			msg_print(Ind, "There is a monster in the way!");

			/* Attack */
			py_attack(Ind, y, x);
		}

		/* Go for it */
		else
		{
			/* Take a turn */
			p_ptr->energy -= level_speed(p_ptr->dun_depth);

			/* Successful jamming */
			msg_print(Ind, "You jam the door with a spike.");

			/* Convert "locked" to "stuck" XXX XXX XXX */
			if (c_ptr->feat < FEAT_DOOR_HEAD + 0x08) c_ptr->feat += 0x08;

			/* Add one spike to the door */
			if (c_ptr->feat < FEAT_DOOR_TAIL) c_ptr->feat++;

			/* Use up, and describe, a single spike, from the bottom */
			inven_item_increase(Ind, item, -1);
			inven_item_describe(Ind, item);
			inven_item_optimize(Ind, item);
		}
	}
}



/*
 * Support code for the "Walk" and "Jump" commands
 */
void do_cmd_walk(int Ind, int dir, int pickup)
{
	player_type *p_ptr = Players[Ind];
	cave_type *c_ptr;

	bool more = FALSE;


	/* Make sure he hasn't just switched levels */
	if (p_ptr->new_level_flag) return;

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (dir)
	{
		/* Hack -- handle confusion */
		if (p_ptr->confused)
		{
			dir = 5;

			/* Prevent walking nowhere */
			while (dir == 5)
				dir = rand_int(9) + 1;
		}

		/* Handle the cfg_door_bump_open option */
		if (cfg_door_bump_open)
		{
			/* Get requested grid */
			c_ptr = &cave[p_ptr->dun_depth][p_ptr->py+ddy[dir]][p_ptr->px+ddx[dir]];

			if (((c_ptr->feat >= FEAT_DOOR_HEAD) && 
						(c_ptr->feat <= FEAT_DOOR_TAIL)) ||
					((c_ptr->feat >= FEAT_HOME_HEAD) &&
					 (c_ptr->feat <= FEAT_HOME_TAIL))) 
			{
				do_cmd_open(Ind, dir);
				return;
			}
		}

		/* Actually move the character */
		move_player(Ind, dir, pickup);

		/* Take a turn */
		p_ptr->energy -= level_speed(p_ptr->dun_depth);

		/* Allow more walking */
		more = TRUE;
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(Ind, 0);
}



/*
 * Start running.
 */
/* Hack -- since this command has different cases of energy requirements and
 * if we don't have enough energy sometimes we want to queue and sometimes we
 * don't, we do all of the energy checking within this function.  If after all
 * is said and done we want to queue the command, we return a 0.  If we don't,
 * we return a 2.
 */
int do_cmd_run(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	cave_type *c_ptr;

	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return 2;
	}

	/* Ignore if we are already running in this direction */
	if (p_ptr->running && (dir == p_ptr->find_current) ) return 2;

	/* Get a "repeated" direction */
	if (dir > 0 && dir < 10)
	{
		/* Make sure we have an empty space to run into */
		if (see_wall(Ind, dir, p_ptr->py, p_ptr->px) && p_ptr->energy >= level_speed(p_ptr->dun_depth))
		{
			/* Handle the cfg_door_bump option */
			if (cfg_door_bump_open)
			{
				/* Get requested grid */
				c_ptr = &cave[p_ptr->dun_depth][p_ptr->py+ddy[dir]][p_ptr->px+ddx[dir]];

				if (((c_ptr->feat >= FEAT_DOOR_HEAD) && 
							(c_ptr->feat <= FEAT_DOOR_TAIL)) ||
						((c_ptr->feat >= FEAT_HOME_HEAD) &&
						 (c_ptr->feat <= FEAT_HOME_TAIL))) 
				{
					/* Check if we have enough energy to open the door */
					if (p_ptr->energy >= level_speed(p_ptr->dun_depth))
					{
						/* If so, open it. */
						do_cmd_open(Ind, dir);
					}
					return 2;
				}
			}

			/* Message */
			msg_print(Ind, "You cannot run in that direction.");

			/* Disturb */
			disturb(Ind, 0);

			/* Waste a little bit of energy for trying */
			p_ptr->energy -= level_speed(p_ptr->dun_depth)/5;

			return 2;
		}

		/* Make sure we have enough energy to start running */
		if (p_ptr->energy >= level_speed(p_ptr->dun_depth))
		{
			/* Hack -- Set the run counter */
			p_ptr->running = (command_arg ? command_arg : 1000);

			/* First step */
			run_step(Ind, dir);

			/* Reset the player's energy so he can't sprint several spaces
			 * in the first round of running.  */
			p_ptr->energy = level_speed(p_ptr->dun_depth);
			return 2;
		}
		/* If we don't have enough energy to run and monsters aren't around,
		 * try to queue the run command.
		 */
		else return 0;
	}
	return 2;
}



/*
 * Stay still.  Search.  Enter stores.
 * Pick up treasure if "pickup" is true.
 */
void do_cmd_stay(int Ind, int pickup)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;
	cave_type *c_ptr;

	if (p_ptr->new_level_flag) return;

	c_ptr = &cave[Depth][p_ptr->py][p_ptr->px];


	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		/*command_rep = command_arg - 1;*/

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}


	/* We don't want any of this */
#if 0
	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);


	/* Spontaneous Searching */
	if ((p_ptr->skill_fos >= 50) || (0 == rand_int(50 - p_ptr->skill_fos)))
	{
		search(Ind);
	}

	/* Continuous Searching */
	if (p_ptr->searching)
	{
		search(Ind);
	}
#endif


	/* Hack -- enter a store if we are on one */
	if ((c_ptr->feat >= FEAT_SHOP_HEAD) &&
			(c_ptr->feat <= FEAT_SHOP_TAIL))
	{
		/* Disturb */
		disturb(Ind, 0);

		/* Hack -- enter store */
		command_new = '_';
	}


	/* Try to Pick up anything under us */
	carry(Ind, pickup, 1);
}






/*
 * Resting allows a player to safely restore his hp	-RAK-
 */
#if 0
void do_cmd_rest(void)
{
	/* Prompt for time if needed */
	if (command_arg <= 0)
	{
		cptr p = "Rest (0-9999, '*' for HP/SP, '&' as needed): ";

		char out_val[80];

		/* Default */
		strcpy(out_val, "&");

		/* Ask for duration */
		if (!get_string(p, out_val, 4)) return;

		/* Rest until done */
		if (out_val[0] == '&')
		{
			command_arg = (-2);
		}

		/* Rest a lot */
		else if (out_val[0] == '*')
		{
			command_arg = (-1);
		}

		/* Rest some */
		else
		{
			command_arg = atoi(out_val);
			if (command_arg <= 0) return;
		}
	}


	/* Paranoia */
	if (command_arg > 9999) command_arg = 9999;


	/* Take a turn XXX XXX XXX (?) */
	energy -= level_speed(p_ptr->dun_depth);

	/* Save the rest code */
	resting = command_arg;

	/* Cancel searching */
	p_ptr->searching = FALSE;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Refresh */
	Term_fresh();
}
#endif






/*
 * Determines the odds of an object breaking when thrown at a monster
 *
 * Note that artifacts never break, see the "drop_near()" function.
 */
static int breakage_chance(object_type *o_ptr)
{
	/* Examine the item type */
	switch (o_ptr->tval)
	{
		/* Always break */
		case TV_FLASK:
		case TV_POTION:
		case TV_BOTTLE:
		case TV_FOOD:
		case TV_JUNK:
			{
				return (100);
			}

			/* Often break */
		case TV_LITE:
		case TV_SCROLL:
		case TV_ARROW:
		case TV_SKELETON:
			{
				return (50);
			}

			/* Sometimes break */
		case TV_WAND:
		case TV_SHOT:
		case TV_BOLT:
		case TV_SPIKE:
			{
				return (25);
			}
	}

	/* Rarely break */
	return (10);
}


/*
 * Fire an object from the pack or floor.
 *
 * You may only fire items that "match" your missile launcher.
 *
 * You must use slings + pebbles/shots, bows + arrows, xbows + bolts.
 *
 * See "calc_bonuses()" for more calculations and such.
 *
 * Note that "firing" a missile is MUCH better than "throwing" it.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Objects are more likely to break if they "attempt" to hit a monster.
 *
 * Rangers (with Bows) and Anyone (with "Extra Shots") get extra shots.
 *
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 *
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 *
 * Note that Bows of "Extra Shots" give an extra shot.
 */
void do_cmd_fire(int Ind, int dir, int item)
{
	player_type *p_ptr = Players[Ind], *q_ptr;
	int Depth = p_ptr->dun_depth;

	int			i, j, y, x, ny, nx, ty, tx;
	int			tdam, tdis, thits, tmul;
	int			bonus, chance;
	int			cur_dis, visible;

	object_type         throw_obj;
	object_type		*o_ptr;

	object_type		*j_ptr;

	bool		hit_body = FALSE;

	int			missile_attr;
	int			missile_char;

	char		o_name[80];
	bool                magic = FALSE;


	/* Get the "bow" (if any) */
	j_ptr = &(p_ptr->inventory[INVEN_BOW]);

	/* Require a launcher */
	if (!j_ptr->tval)
	{
		msg_print(Ind, "You have nothing to fire with.");
		return;
	}

	if (!p_ptr->tval_ammo)
	{
		msg_print(Ind, "You have nothing to fire.");
		return;
	}

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		o_ptr = &o_list[0 - item];
	}

	if( check_guard_inscription( o_ptr->note, 'f' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}


	if (o_ptr->tval != p_ptr->tval_ammo)
	{
		msg_print(Ind, "You cannot fire that!");
		return;
	}

	if (!o_ptr->tval)
	{
		msg_print(Ind, "You cannot fire that!");
		return;
	}

	/* Magic ammo */
	if ((o_ptr->sval == SV_AMMO_MAGIC) || artifact_p(o_ptr))
		magic = TRUE;

	/* Only fire in direction 5 if we have a target */
	if ((dir == 5) && !target_okay(Ind))
		return;

	/* Create a "local missile object" */
	throw_obj = *o_ptr;
	throw_obj.number = 1;

	if (!magic)
	{
		/* Reduce and describe inventory */
		if (item >= 0)
		{
			inven_item_increase(Ind, item, -1);
			inven_item_describe(Ind, item);
			inven_item_optimize(Ind, item);
		}

		/* Reduce and describe floor item */
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_optimize(0 - item);
		}
	}

	/* Use the missile object */
	o_ptr = &throw_obj;

	/* Describe the object */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Find the color and symbol for the object for throwing */
	missile_attr = object_attr(o_ptr);
	missile_char = object_char(o_ptr);


	/* Use the proper number of shots */
	thits = p_ptr->num_fire;

	/* Use a base distance */
	tdis = 10;

	/* Base damage from thrown object plus launcher bonus */
	tdam = damroll(o_ptr->dd, o_ptr->ds) + o_ptr->to_d + j_ptr->to_d;

	/* Actually "fire" the object */
	bonus = (p_ptr->to_h + o_ptr->to_h + j_ptr->to_h);
	chance = (p_ptr->skill_thb + (bonus * BTH_PLUS_ADJ));

	/* Assume a base multiplier */
	tmul = 1;

	/* Analyze the launcher */
	switch (j_ptr->sval)
	{
		/* Sling and ammo */
		case SV_SLING:
			{
				tmul = 2;
				break;
			}

			/* Short Bow and Arrow */
		case SV_SHORT_BOW:
			{
				tmul = 2;
				break;
			}

			/* Long Bow and Arrow */
		case SV_LONG_BOW:
			{
				tmul = 3;
				break;
			}

			/* Light Crossbow and Bolt */
		case SV_LIGHT_XBOW:
			{
				tmul = 3;
				break;
			}

			/* Heavy Crossbow and Bolt */
		case SV_HEAVY_XBOW:
			{
				tmul = 4;
				break;
			}
	}

	/* Get extra "power" from "extra might" */
	if (p_ptr->xtra_might) tmul++;

	/* Boost the damage */
	tdam *= tmul;

	/* Base range */
	tdis = 10 + 5 * tmul;


	/* Take a (partial) turn */
	p_ptr->energy -= (level_speed(p_ptr->dun_depth) / thits);


	/* Start at the player */
	y = p_ptr->py;
	x = p_ptr->px;

	/* Predict the "target" location */
	tx = p_ptr->px + 99 * ddx[dir];
	ty = p_ptr->py + 99 * ddy[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay(Ind))
	{
		tx = p_ptr->target_col;
		ty = p_ptr->target_row;
	}


	/* Hack -- Handle stuff */
	handle_stuff(Ind);


	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, p_ptr->py, p_ptr->px, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_floor_bold(Depth, ny, nx)) break;

		/* Advance the distance */
		cur_dis++;

		/* Save the new location */
		x = nx;
		y = ny;

		/* Save the old "player pointer" */
		q_ptr = p_ptr;

		/* Display it for each player */
		for (i = 1; i < NumPlayers + 1; i++)
		{
			int dispx, dispy;

			/* Use this player */
			p_ptr = Players[i];

			/* If he's not here, skip him */
			if (p_ptr->dun_depth != Depth)
				continue;

			/* The player can see the (on screen) missile */
			if (panel_contains(y, x) && player_can_see_bold(i, y, x))
			{
				/* Draw, Hilite, Fresh, Pause, Erase */
				dispy = y - p_ptr->panel_row_prt;
				dispx = x - p_ptr->panel_col_prt;

				/* Remember the projectile */
				p_ptr->scr_info[dispy][dispx].c = missile_char;
				p_ptr->scr_info[dispy][dispx].a = missile_attr;

				/* Tell the client */
				Send_char(i, dispx, dispy, missile_attr, missile_char);

				/* Flush and wait */
				if (cur_dis % tmul) Send_flush(i);

				/* Restore */
				lite_spot(i, y, x);
			}

			/* The player cannot see the missile */
			else
			{
				/* Pause anyway, for consistancy */
				/*Term_xtra(TERM_XTRA_DELAY, msec);*/
			}
		}

		/* Restore the player pointer */
		p_ptr = q_ptr;

		/* Player here, hit him */
		if (cave[Depth][y][x].m_idx < 0)
		{
			cave_type *c_ptr = &cave[Depth][y][x];

			q_ptr = Players[0 - c_ptr->m_idx];

			/* AD hack -- "pass over" players in same party */
			if ((!player_in_party(p_ptr->party, 0 - c_ptr->m_idx)) || (p_ptr->party == 0)){ 

				/* Check the visibility */
				visible = p_ptr->play_vis[0 - c_ptr->m_idx];

				/* Note the collision */
				hit_body = TRUE;

				/* Did we hit it (penalize range) */
				if (test_hit_fire(chance - cur_dis, q_ptr->ac + q_ptr->to_a, visible))
				{
					char pvp_name[80];

					/* Get the name */
					strcpy(pvp_name, q_ptr->name);

					/* Don't allow if players aren't hostile */
					if (!check_hostile(Ind, 0 - c_ptr->m_idx) || !check_hostile(0 - c_ptr->m_idx, Ind))
					{
						return;
					}

					/* Handle unseen player */
					if (!visible)
					{
						/* Invisible player */
						msg_format(Ind, "The %s finds a mark.", o_name);
						msg_format(0 - c_ptr->m_idx, "You are hit by a %s!", o_name);
					}

					/* Handle visible player */
					else
					{
						/* Messages */
						msg_format(Ind, "The %s hits %s.", o_name, pvp_name);
						msg_format(0 - c_ptr->m_idx, "%^s hits you with a %s.", p_ptr->name, o_name);

						/* Track this player's health */
						health_track(Ind, c_ptr->m_idx);
					}

					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux_player(o_ptr, tdam, q_ptr);
					tdam = critical_shot(Ind, o_ptr->weight, o_ptr->to_h, tdam);

					/* No negative damage */
					if (tdam < 0) tdam = 0;

					/* XXX Reduce damage by 1/3 */
					tdam = (tdam + 2) / 3;

					/* Take damage */
					take_hit(0 - c_ptr->m_idx, tdam, p_ptr->name);

					/* Stop looking */
					break;
				}

			} /* end hack */
		}

		/* Monster here, Try to hit it */
		if (cave[Depth][y][x].m_idx > 0)
		{
			cave_type *c_ptr = &cave[Depth][y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Check the visibility */
			visible = p_ptr->mon_vis[c_ptr->m_idx];

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if (test_hit_fire(chance - cur_dis, r_ptr->ac, visible))
			{
				bool fear = FALSE;

				/* Assume a default death */
				cptr note_dies = " dies.";

				/* Some monsters get "destroyed" */
				if ((r_ptr->flags3 & RF3_DEMON) ||
						(r_ptr->flags3 & RF3_UNDEAD) ||
						(r_ptr->flags2 & RF2_STUPID) ||
						(strchr("Evg", r_ptr->d_char)))
				{
					/* Special note at death */
					note_dies = " is destroyed.";
				}


				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
					msg_format(Ind, "The %s finds a mark.", o_name);
				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(Ind, m_name, c_ptr->m_idx, 0);

					/* Message */
					msg_format(Ind, "The %s hits %s.", o_name, m_name);

					/* Hack -- Track this monster race */
					if (visible) recent_track(m_ptr->r_idx);

					/* Hack -- Track this monster */
					if (visible) health_track(Ind, c_ptr->m_idx);
				}

				/* Apply special damage XXX XXX XXX */
				tdam = tot_dam_aux(o_ptr, tdam, m_ptr);
				tdam = critical_shot(Ind, o_ptr->weight, o_ptr->to_h, tdam);

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Complex message */
				if (wizard)
				{
					msg_format(Ind, "You do %d (out of %d) damage.",
							tdam, m_ptr->hp);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(Ind, c_ptr->m_idx, tdam, &fear, note_dies))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					/* Message */
					message_pain(Ind, c_ptr->m_idx, tdam);

					/* Take note */
					if (fear && visible && !(r_ptr->flags2 & RF2_WANDERER))
					{
						char m_name[80];

						/* Sound */
						sound(Ind, SOUND_FLEE);

						/* Get the monster name (or "it") */
						monster_desc(Ind, m_name, c_ptr->m_idx, 0);

						/* Message */
						msg_format(Ind, "%^s flees in terror!", m_name);
					}
				}

				/* Stop looking */
				break;
			}
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(o_ptr) : 0);

	/* Drop (or break) near that location */
	if (!magic) drop_near(o_ptr, j, Depth, y, x);
}



/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
void do_cmd_throw(int Ind, int dir, int item)
{
	player_type *p_ptr = Players[Ind], *q_ptr;
	int Depth = p_ptr->dun_depth;

	int			i, j, y, x, ny, nx, ty, tx;
	int			chance, tdam, tdis;
	int			mul, div;
	int			cur_dis, visible;

	object_type         throw_obj;
	object_type		*o_ptr;

	bool		hit_body = FALSE;

	int			missile_attr;
	int			missile_char;

	char		o_name[80];

	/*int			msec = delay_factor * delay_factor * delay_factor;*/


	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &(p_ptr->inventory[item]);
	}
	else
	{
		item = -cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].o_idx;
		o_ptr = &o_list[0 - item];
	}
	if(!o_ptr->tval) {
		msg_print(Ind, "There is nothing there to throw");
		return;
	};

	if( check_guard_inscription( o_ptr->note, 'v' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	};

	/* Never throw artifacts */
	if (artifact_p(o_ptr))
	{
		msg_print(Ind, "You can not throw this!");
		return;	
	}	

	/* Create a "local missile object" */
	throw_obj = *o_ptr;
	throw_obj.number = 1;

	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Use the local object */
	o_ptr = &throw_obj;

	/* Description */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Find the color and symbol for the object for throwing */
	missile_attr = object_attr(o_ptr);
	missile_char = object_char(o_ptr);


	/* Extract a "distance multiplier" */
	mul = 10;

	/* Enforce a minimum "weight" of one pound */
	div = ((o_ptr->weight > 10) ? o_ptr->weight : 10);

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->stat_ind[A_STR]] + 20) * mul / div;

	/* Max distance of 10 */
	if (tdis > 10) tdis = 10;

	/* Hack -- Base damage from thrown object */
	tdam = damroll(o_ptr->dd, o_ptr->ds) + o_ptr->to_d;

	/* Chance of hitting */
	chance = (p_ptr->skill_tht + (p_ptr->to_h * BTH_PLUS_ADJ));


	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);


	/* Start at the player */
	y = p_ptr->py;
	x = p_ptr->px;

	/* Predict the "target" location */
	tx = p_ptr->px + 99 * ddx[dir];
	ty = p_ptr->py + 99 * ddy[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay(Ind))
	{
		tx = p_ptr->target_col;
		ty = p_ptr->target_row;
	}


	/* Hack -- Handle stuff */
	handle_stuff(Ind);


	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, p_ptr->py, p_ptr->px, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_floor_bold(Depth, ny, nx)) 
		{
			/* Special case: potion VS house door */
			if (o_ptr->tval == TV_POTION &&
					cave[Depth][ny][nx].feat >= FEAT_HOME_HEAD && 
					cave[Depth][ny][nx].feat <= FEAT_HOME_TAIL) 
			{
				/* Break it */
				hit_body = TRUE;

				/* Find suitable color */
				for (i = FEAT_HOME_HEAD; i < FEAT_HOME_TAIL + 1; i++)
				{
					if (f_info[i].z_attr == missile_attr || f_info[i].z_attr == color_opposite(missile_attr)) 
					{
						/* Pick a house */
						if ((j = pick_house(Depth, ny, nx)) == -1) break;

						/* Must own the house */
						if (!house_owned_by(Ind,j)) break;

						/* Chance to fail */
						if (randint(100) > p_ptr->sc) break;

						/* Perform colorization */
						houses[j].strength = i - FEAT_HOME_HEAD;
						cave[Depth][ny][nx].feat = i;
						everyone_lite_spot(Depth, ny, nx);

						/* Done */
						break;
					}
				}		
			}
			break;
		}

		/* Advance the distance */
		cur_dis++;

		/* Save the new location */
		x = nx;
		y = ny;

		/* Save the old "player pointer" */
		q_ptr = p_ptr;

		/* Display it for each player */
		for (i = 1; i < NumPlayers + 1; i++)
		{
			int dispx, dispy;

			/* Use this player */
			p_ptr = Players[i];

			/* If he's not here, skip him */
			if (p_ptr->dun_depth != Depth)
				continue;

			/* The player can see the (on screen) missile */
			if (panel_contains(y, x) && player_can_see_bold(i, y, x))
			{
				/* Draw, Hilite, Fresh, Pause, Erase */
				dispy = y - p_ptr->panel_row_prt;
				dispx = x - p_ptr->panel_col_prt;

				/* Remember the projectile */
				p_ptr->scr_info[dispy][dispx].c = missile_char;
				p_ptr->scr_info[dispy][dispx].a = missile_attr;

				/* Tell the client */
				Send_char(i, dispx, dispy, missile_attr, missile_char);

				/* Flush and wait */
				if (cur_dis % 2) Send_flush(i);

				/* Restore */
				lite_spot(i, y, x);
			}

			/* The player cannot see the missile */
			else
			{
				/* Pause anyway, for consistancy */
				/*Term_xtra(TERM_XTRA_DELAY, msec);*/
			}
		}

		/* Restore the player pointer */
		p_ptr = q_ptr;


		/* Player here, try to hit him */
		if (cave[Depth][y][x].m_idx < 0)
		{
			cave_type *c_ptr = &cave[Depth][y][x];

			q_ptr = Players[0 - c_ptr->m_idx];

			/* Check the visibility */
			visible = p_ptr->play_vis[0 - c_ptr->m_idx];

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit him (penalize range) */
			if (test_hit_fire(chance - cur_dis, q_ptr->ac + q_ptr->to_a, visible))
			{
				char pvp_name[80];

				/* Get the name */
				strcpy(pvp_name, q_ptr->name);

				/* Don't allow if players aren't hostile */
				if (!check_hostile(Ind, 0 - c_ptr->m_idx) || !check_hostile(0 - c_ptr->m_idx, Ind))
				{
					return;
				}

				/* Handle unseen player */
				if (!visible)
				{
					/* Messages */
					msg_format(Ind, "The %s finds a mark!", o_name);
					msg_format(0 - c_ptr->m_idx, "You are hit by a %s!", o_name);
				}

				/* Handle visible player */
				else
				{
					/* Messages */
					msg_format(Ind, "The %s hits %s.", o_name, pvp_name);
					msg_format(0 - c_ptr->m_idx, "%s hits you with a %s!", p_ptr->name, o_name);

					/* Track player's health */
					health_track(Ind, c_ptr->m_idx);
				}

				/* Apply special damage XXX XXX XXX */
				tdam = tot_dam_aux_player(o_ptr, tdam, q_ptr);
				tdam = critical_shot(Ind, o_ptr->weight, o_ptr->to_h, tdam);

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* XXX Reduce damage by 1/3 */
				tdam = (tdam + 2) / 3;

				/* Take damage */
				take_hit(0 - c_ptr->m_idx, tdam, p_ptr->name);

				/* Stop looking */
				break;
			}
		}

		/* Monster here, Try to hit it */
		if (cave[Depth][y][x].m_idx > 0)
		{
			cave_type *c_ptr = &cave[Depth][y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Check the visibility */
			visible = p_ptr->mon_vis[c_ptr->m_idx];

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if (test_hit_fire(chance - cur_dis, r_ptr->ac, visible))
			{
				bool fear = FALSE;

				/* Assume a default death */
				cptr note_dies = " dies.";

				/* Some monsters get "destroyed" */
				if ((r_ptr->flags3 & RF3_DEMON) ||
						(r_ptr->flags3 & RF3_UNDEAD) ||
						(r_ptr->flags2 & RF2_STUPID) ||
						(strchr("Evg", r_ptr->d_char)))
				{
					/* Special note at death */
					note_dies = " is destroyed.";
				}


				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
					msg_format(Ind, "The %s finds a mark.", o_name);
				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(Ind, m_name, c_ptr->m_idx, 0);

					/* Message */
					msg_format(Ind, "The %s hits %s.", o_name, m_name);

					/* Hack -- Track this monster race */
					if (visible) recent_track(m_ptr->r_idx);

					/* Hack -- Track this monster */
					if (visible) health_track(Ind, c_ptr->m_idx);
				}

				/* Apply special damage XXX XXX XXX */
				tdam = tot_dam_aux(o_ptr, tdam, m_ptr);
				tdam = critical_shot(Ind, o_ptr->weight, o_ptr->to_h, tdam);

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Complex message */
				if (wizard)
				{
					msg_format(Ind, "You do %d (out of %d) damage.",
							tdam, m_ptr->hp);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(Ind, c_ptr->m_idx, tdam, &fear, note_dies))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					/* Message */
					message_pain(Ind, c_ptr->m_idx, tdam);

					/* Take note */
					if (fear && visible && !(r_ptr->flags2 & RF2_WANDERER))
					{
						char m_name[80];

						/* Sound */
						sound(Ind, SOUND_FLEE);

						/* Get the monster name (or "it") */
						monster_desc(Ind, m_name, c_ptr->m_idx, 0);

						/* Message */
						msg_format(Ind, "%^s flees in terror!", m_name);
					}
				}

				/* Stop looking */
				break;
			}
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(o_ptr) : 0);

	/* Drop (or break) near that location */
	drop_near(o_ptr, j, Depth, y, x);
}


/*
 * Buy a house.  It is assumed that the player already knows the
 * price.

 Hacked to sell houses for half price. -APD-

 */
void do_cmd_purchase_house(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int Depth = p_ptr->dun_depth;

	int y, x, i, factor, price;
	cave_type *c_ptr;

	/* Ghosts cannot buy houses */
	if(strcmp(p_ptr->name,cfg_dungeon_master))  {
		if ( (p_ptr->ghost) || (p_ptr->fruit_bat) )
		{
			/* Message */
			msg_print(Ind, "You cannot buy a house.");

			return;
		}	
	}

	/* Check for no-direction -- confirmation (when selling house) */
	if (!dir)
	{
		i = p_ptr->current_house;
		p_ptr->current_house = -1;	

		if (i == -1)
		{
			/* No house, message */
			msg_print(Ind, "You see nothing to sell there.");
			return;
		}

		/* Get requested grid */
		c_ptr = &cave[Depth][houses[i].door_y][houses[i].door_x];

		/* Take player's CHR into account */
		factor = adj_chr_gold[p_ptr->stat_ind[A_CHR]];
		price = (unsigned long) houses[i].price * factor / 100;

		if (house_owned(i))
		{
			/* Is it owned by this player? */
			if (house_owned_by(Ind,i))
			{	
				/* house is no longer owned */
				disown_house(i);

				msg_format(Ind, "You sell your house for %ld gold.", price/2);

				/* Get the money */
				p_ptr->au += price / 2;

				/* Window */
				p_ptr->window |= (PW_INVEN);

				/* Redraw */
				p_ptr->redraw |= (PR_GOLD);

				/* Done */
				return;			
			}
		}

		/* No house, message */
		msg_print(Ind, "You don't own this house.");
		return;
	}

	/* Be sure we have a direction */
	if (dir > 0 && dir < 10)
	{
		/* Get requested direction */
		y = p_ptr->py + ddy[dir];
		x = p_ptr->px + ddx[dir];

		/* Get requested grid */
		c_ptr = &cave[Depth][y][x];

		/* Check for a house */
		if ((i = pick_house(Depth, y, x)) == -1)
		{
			/* No house, message */
			msg_print(Ind, "You see nothing to buy there.");
			return;
		}

		/* Take player's CHR into account */
		factor = adj_chr_gold[p_ptr->stat_ind[A_CHR]];
		price = (unsigned long) houses[i].price * factor / 100;


		/* Check for already-owned house */
		if (house_owned(i))
		{

			/* Is it owned by this player? */
			if (house_owned_by(Ind,i))
			{
				if (house_inside(Ind, i)) 
				{
					/* Hack -- Enter own store */
					command_new = '_';
					do_cmd_store(Ind,i);
				}
				else
				{
					/* Delay house transaction */
					p_ptr->current_house = i;			

					/* Tell the client about the price */
					Send_store_sell(Ind, price/2);
				}
				return;		
			}

			if (!strcmp(p_ptr->name,cfg_dungeon_master))
			{
				disown_house(i);

				msg_format(Ind, "The house has been reset.");

				return;
			}

			/* Message */
			msg_print(Ind, "That house is already owned.");

			/* No sale */
			return;
		}

		if(Depth == 0) 
		{
			/* houses in town are *ASTRONOMICAL* in price due to location, location, location. */
			price =(unsigned long)price *5L; 
		}

		/* Check for enough funds */
		if (price > p_ptr->au)
		{
			/* Not enough money, message */
			msg_print(Ind, "You do not have enough money.");
			return;
		}

		/* Check if we have too many houses already */
		if( cfg_max_houses && houses_owned(Ind) >= cfg_max_houses )
		{
			/* Too many houses owned already */
			msg_print(Ind, "You own too many houses already.");
			return;
		}

		/* Open the door */
		c_ptr->feat = FEAT_HOME_OPEN;

		/* Reshow */
		everyone_lite_spot(Depth, y, x);

		/* Take some of the player's money */
		p_ptr->au -= price;

		/* The house is now owned */
		set_house_owner(Ind,i);

		/* Redraw */
		p_ptr->redraw |= (PR_GOLD);
	}
}
