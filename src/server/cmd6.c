/* File: cmd6.c */

/* Purpose: Object commands */

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
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 *
 * In all cases, if the player becomes "aware" of the item's use
 * by testing it, mark it as "aware" and reward some experience
 * based on the object's level, always rounding up.  If the player
 * remains "unaware", mark that object "kind" as "tried".
 *
 * This code now correctly handles the unstacking of wands, staffs,
 * and rods.  Note the overly paranoid warning about potential pack
 * overflow, which allows the player to use and drop a stacked item.
 *
 * In all "unstacking" scenarios, the "used" object is "carried" as if
 * the player had just picked it up.  In particular, this means that if
 * the use of an item induces pack overflow, that item will be dropped.
 *
 * For simplicity, these routines induce a full "pack reorganization"
 * which not only combines similar items, but also reorganizes various
 * items to obey the current "sorting" method.  This may require about
 * 400 item comparisons, but only occasionally.
 *
 * There may be a BIG problem with any "effect" that can cause "changes"
 * to the inventory.  For example, a "scroll of recharging" can cause
 * a wand/staff to "disappear", moving the inventory up.  Luckily, the
 * scrolls all appear BEFORE the staffs/wands, so this is not a problem.
 * But, for example, a "staff of recharging" could cause MAJOR problems.
 * In such a case, it will be best to either (1) "postpone" the effect
 * until the end of the function, or (2) "change" the effect, say, into
 * giving a staff "negative" charges, or "turning a staff into a stick".
 * It seems as though a "rod of recharging" might in fact cause problems.
 * The basic problem is that the act of recharging (and destroying) an
 * item causes the inducer of that action to "move", causing "o_ptr" to
 * no longer point at the correct item, with horrifying results.
 *
 * Note that food/potions/scrolls no longer use bit-flags for effects,
 * but instead use the "sval" (which is also used to sort the objects).
 */






/*
 * Eat some food (from the pack or floor)
 */
void do_cmd_eat_food(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int			ident, lev;

	object_type		*o_ptr;


	/* Restrict choices to food */
	item_tester_tval = TV_FOOD;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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

	if( check_guard_inscription( o_ptr->note, 'E' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	};


	if (o_ptr->tval != TV_FOOD)
	{
		/* Tried to eat non-food */
		return;
	}


	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	/* Analyze the food */
	switch (o_ptr->sval)
	{
		case SV_FOOD_POISON:
			{
				if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
				{
					if (set_poisoned(Ind, p_ptr->poisoned + rand_int(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_FOOD_BLINDNESS:
			{
				if (!p_ptr->resist_blind)
				{
					if (set_blind(Ind, p_ptr->blind + rand_int(200) + 200))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_FOOD_PARANOIA:
			{
				if (!p_ptr->resist_fear)
				{
					if (set_afraid(Ind, p_ptr->afraid + rand_int(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_FOOD_CONFUSION:
			{
				if (!p_ptr->resist_conf)
				{
					if (set_confused(Ind, p_ptr->confused + rand_int(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_FOOD_HALLUCINATION:
			{
				if (!p_ptr->resist_chaos)
				{
					if (set_image(Ind, p_ptr->image + rand_int(250) + 250))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_FOOD_PARALYSIS:
			{
				if (!p_ptr->free_act)
				{
					if (set_paralyzed(Ind, p_ptr->paralyzed + rand_int(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_FOOD_WEAKNESS:
			{
				take_hit(Ind, damroll(6, 6), "poisonous food.");
				(void)do_dec_stat(Ind, A_STR);
				ident = TRUE;
				break;
			}

		case SV_FOOD_SICKNESS:
			{
				take_hit(Ind, damroll(6, 6), "poisonous food.");
				(void)do_dec_stat(Ind, A_CON);
				ident = TRUE;
				break;
			}

		case SV_FOOD_STUPIDITY:
			{
				take_hit(Ind, damroll(8, 8), "poisonous food.");
				(void)do_dec_stat(Ind, A_INT);
				ident = TRUE;
				break;
			}

		case SV_FOOD_NAIVETY:
			{
				take_hit(Ind, damroll(8, 8), "poisonous food.");
				(void)do_dec_stat(Ind, A_WIS);
				ident = TRUE;
				break;
			}

		case SV_FOOD_UNHEALTH:
			{
				take_hit(Ind, damroll(10, 10), "poisonous food.");
				(void)do_dec_stat(Ind, A_CON);
				ident = TRUE;
				break;
			}

		case SV_FOOD_DISEASE:
			{
				take_hit(Ind, damroll(10, 10), "poisonous food.");
				(void)do_dec_stat(Ind, A_STR);
				ident = TRUE;
				break;
			}

		case SV_FOOD_CURE_POISON:
			{
				if (set_poisoned(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_FOOD_CURE_BLINDNESS:
			{
				if (set_blind(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_FOOD_CURE_PARANOIA:
			{
				if (set_afraid(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_FOOD_CURE_CONFUSION:
			{
				if (set_confused(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_FOOD_CURE_SERIOUS:
			{
				if (hp_player(Ind, damroll(4, 8))) ident = TRUE;
				break;
			}

		case SV_FOOD_RESTORE_STR:
			{
				if (do_res_stat(Ind, A_STR)) ident = TRUE;
				break;
			}

		case SV_FOOD_RESTORE_CON:
			{
				if (do_res_stat(Ind, A_CON)) ident = TRUE;
				break;
			}

		case SV_FOOD_RESTORING:
			{
				if (do_res_stat(Ind, A_STR)) ident = TRUE;
				if (do_res_stat(Ind, A_INT)) ident = TRUE;
				if (do_res_stat(Ind, A_WIS)) ident = TRUE;
				if (do_res_stat(Ind, A_DEX)) ident = TRUE;
				if (do_res_stat(Ind, A_CON)) ident = TRUE;
				if (do_res_stat(Ind, A_CHR)) ident = TRUE;
				break;
			}


		case SV_FOOD_RATION:
		case SV_FOOD_BISCUIT:
		case SV_FOOD_JERKY:
		case SV_FOOD_SLIME_MOLD:
			{
				msg_print(Ind, "That tastes good.");
				ident = TRUE;
				break;
			}

		case SV_FOOD_WAYBREAD:
			{
				msg_print(Ind, "That tastes good.");
				(void)set_poisoned(Ind, 0);
				(void)hp_player(Ind, damroll(4, 8));
				ident = TRUE;
				break;
			}

		case SV_FOOD_PINT_OF_ALE:
		case SV_FOOD_PINT_OF_WINE:
			{
				msg_print(Ind, "That tastes good.");
				ident = TRUE;
				break;
			}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* We have tried it */
	object_tried(Ind, o_ptr);

	/* The player is now aware of the object */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Food can feed the player */
	(void)set_food(Ind, p_ptr->food + o_ptr->pval);


	/* Destroy a food in the pack */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Destroy a food on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}




/*
 * Quaff a potion (from the pack or the floor)
 */
void do_cmd_quaff_potion(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int		ident, lev;

	object_type	*o_ptr;


	/* Restrict choices to potions */
	item_tester_tval = TV_POTION;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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

	if( check_guard_inscription( o_ptr->note, 'q' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	};



	if (o_ptr->tval != TV_POTION)
	{
		/* Tried to quaff non-potion */
		return;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	/* Analyze the potion */
	switch (o_ptr->sval)
	{
		case SV_POTION_WATER:
		case SV_POTION_APPLE_JUICE:
		case SV_POTION_SLIME_MOLD:
			{
				msg_print(Ind, "You feel less thirsty.");
				ident = TRUE;
				break;
			}

		case SV_POTION_SLOWNESS:
			{
				if (set_slow(Ind, p_ptr->slow + randint(25) + 15)) ident = TRUE;
				break;
			}

		case SV_POTION_SALT_WATER:
			{
				msg_print(Ind, "The potion makes you vomit!");
				msg_format_near(Ind, "%s vomits!", p_ptr->name);
				/* made salt water less deadly -APD */
				(void)set_food(Ind, (p_ptr->food/2)-400);
				(void)set_poisoned(Ind, 0);
				(void)set_paralyzed(Ind, p_ptr->paralyzed + 4);
				ident = TRUE;
				break;
			}

		case SV_POTION_POISON:
			{
				if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
				{
					if (set_poisoned(Ind, p_ptr->poisoned + rand_int(15) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_POTION_BLINDNESS:
			{
				if (!p_ptr->resist_blind)
				{
					if (set_blind(Ind, p_ptr->blind + rand_int(100) + 100))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_POTION_CONFUSION:
			{
				if (!p_ptr->resist_conf)
				{
					if (set_confused(Ind, p_ptr->confused + rand_int(20) + 15))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_POTION_SLEEP:
			{
				if (!p_ptr->free_act)
				{
					if (set_paralyzed(Ind, p_ptr->paralyzed + rand_int(4) + 4))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_POTION_LOSE_MEMORIES:
			{
				if (!p_ptr->hold_life && (p_ptr->exp > 0))
				{
					msg_print(Ind, "You feel your memories fade.");
					lose_exp(Ind, p_ptr->exp / 4);
					ident = TRUE;
				}
				break;
			}

		case SV_POTION_RUINATION:
			{
				msg_print(Ind, "Your nerves and muscles feel weak and lifeless!");
				take_hit(Ind, damroll(10, 10), "a potion of Ruination");
				(void)dec_stat(Ind, A_DEX, 25, TRUE);
				(void)dec_stat(Ind, A_WIS, 25, TRUE);
				(void)dec_stat(Ind, A_CON, 25, TRUE);
				(void)dec_stat(Ind, A_STR, 25, TRUE);
				(void)dec_stat(Ind, A_CHR, 25, TRUE);
				(void)dec_stat(Ind, A_INT, 25, TRUE);
				ident = TRUE;
				break;
			}

		case SV_POTION_DEC_STR:
			{
				if (do_dec_stat(Ind, A_STR)) ident = TRUE;
				break;
			}

		case SV_POTION_DEC_INT:
			{
				if (do_dec_stat(Ind, A_INT)) ident = TRUE;
				break;
			}

		case SV_POTION_DEC_WIS:
			{
				if (do_dec_stat(Ind, A_WIS)) ident = TRUE;
				break;
			}

		case SV_POTION_DEC_DEX:
			{
				if (do_dec_stat(Ind, A_DEX)) ident = TRUE;
				break;
			}

		case SV_POTION_DEC_CON:
			{
				if (do_dec_stat(Ind, A_CON)) ident = TRUE;
				break;
			}

		case SV_POTION_DEC_CHR:
			{
				if (do_dec_stat(Ind, A_CHR)) ident = TRUE;
				break;
			}

		case SV_POTION_DETONATIONS:
			{
				msg_print(Ind, "Massive explosions rupture your body!");
				msg_format_near(Ind, "%s blows up!", p_ptr->name);
				take_hit(Ind, damroll(50, 20), "a potion of Detonation");
				(void)set_stun(Ind, p_ptr->stun + 75);
				(void)set_cut(Ind, p_ptr->cut + 5000);
				ident = TRUE;
				break;
			}

		case SV_POTION_DEATH:
			{
				msg_print(Ind, "A feeling of Death flows through your body.");
				take_hit(Ind, 5000, "a potion of Death");
				ident = TRUE;
				break;
			}

		case SV_POTION_INFRAVISION:
			{
				if (set_tim_infra(Ind, p_ptr->tim_infra + 100 + randint(100)))
				{
					ident = TRUE;
				}
				break;
			}

		case SV_POTION_DETECT_INVIS:
			{
				if (set_tim_invis(Ind, p_ptr->tim_invis + 12 + randint(12)))
				{
					ident = TRUE;
				}
				break;
			}

		case SV_POTION_SLOW_POISON:
			{
				if (set_poisoned(Ind, p_ptr->poisoned / 2)) ident = TRUE;
				break;
			}

		case SV_POTION_CURE_POISON:
			{
				if (set_poisoned(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_POTION_BOLDNESS:
			{
				if (set_afraid(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_POTION_SPEED:
			{
				if (!p_ptr->fast)
				{
					if (set_fast(Ind, randint(25) + 15)) ident = TRUE;
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				break;
			}

		case SV_POTION_RESIST_HEAT:
			{
				if (set_oppose_fire(Ind, p_ptr->oppose_fire + randint(10) + 10))
				{
					ident = TRUE;
				}
				break;
			}

		case SV_POTION_RESIST_COLD:
			{
				if (set_oppose_cold(Ind, p_ptr->oppose_cold + randint(10) + 10))
				{
					ident = TRUE;
				}
				break;
			}

		case SV_POTION_HEROISM:
			{
				if (hp_player(Ind, 10)) ident = TRUE;
				if (set_afraid(Ind, 0)) ident = TRUE;
				if (set_hero(Ind, p_ptr->hero + randint(25) + 25)) ident = TRUE;
				break;
			}

		case SV_POTION_BESERK_STRENGTH:
			{
				if (hp_player(Ind, 30)) ident = TRUE;
				if (set_afraid(Ind, 0)) ident = TRUE;
				if (set_shero(Ind, p_ptr->shero + randint(25) + 25)) ident = TRUE;
				break;
			}

		case SV_POTION_CURE_LIGHT:
			{
				if (hp_player(Ind, damroll(2, 8))) ident = TRUE;
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, p_ptr->cut - 10)) ident = TRUE;
				break;
			}

		case SV_POTION_CURE_SERIOUS:
			{
				if (hp_player(Ind, damroll(4, 8))) ident = TRUE;
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, (p_ptr->cut / 2) - 50)) ident = TRUE;
				break;
			}

		case SV_POTION_CURE_CRITICAL:
			{
				if (hp_player(Ind, damroll(6, 8))) ident = TRUE;
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_POTION_HEALING:
			{
				if (hp_player(Ind, 300)) ident = TRUE;
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_POTION_STAR_HEALING:
			{
				if (hp_player(Ind, 1200)) ident = TRUE;
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_POTION_LIFE:
			{
				msg_print(Ind, "You feel life flow through your body!");
				restore_level(Ind);
				hp_player(Ind, 5000);
				(void)set_poisoned(Ind, 0);
				(void)set_blind(Ind, 0);
				(void)set_confused(Ind, 0);
				(void)set_image(Ind, 0);
				(void)set_stun(Ind, 0);
				(void)set_cut(Ind, 0);
				(void)do_res_stat(Ind, A_STR);
				(void)do_res_stat(Ind, A_CON);
				(void)do_res_stat(Ind, A_DEX);
				(void)do_res_stat(Ind, A_WIS);
				(void)do_res_stat(Ind, A_INT);
				(void)do_res_stat(Ind, A_CHR);
				ident = TRUE;
				break;
			}

		case SV_POTION_RESTORE_MANA:
			{
				if (p_ptr->csp < p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
					msg_print(Ind, "Your feel your head clear.");
					p_ptr->redraw |= (PR_MANA);
					p_ptr->window |= (PW_PLAYER);
					ident = TRUE;
				}
				break;
			}

		case SV_POTION_RESTORE_EXP:
			{
				if (restore_level(Ind)) ident = TRUE;
				break;
			}

		case SV_POTION_RES_STR:
			{
				if (do_res_stat(Ind, A_STR)) ident = TRUE;
				break;
			}

		case SV_POTION_RES_INT:
			{
				if (do_res_stat(Ind, A_INT)) ident = TRUE;
				break;
			}

		case SV_POTION_RES_WIS:
			{
				if (do_res_stat(Ind, A_WIS)) ident = TRUE;
				break;
			}

		case SV_POTION_RES_DEX:
			{
				if (do_res_stat(Ind, A_DEX)) ident = TRUE;
				break;
			}

		case SV_POTION_RES_CON:
			{
				if (do_res_stat(Ind, A_CON)) ident = TRUE;
				break;
			}

		case SV_POTION_RES_CHR:
			{
				if (do_res_stat(Ind, A_CHR)) ident = TRUE;
				break;
			}

		case SV_POTION_INC_STR:
			{
				if (do_inc_stat(Ind, A_STR)) ident = TRUE;
				break;
			}

		case SV_POTION_INC_INT:
			{
				if (do_inc_stat(Ind, A_INT)) ident = TRUE;
				break;
			}

		case SV_POTION_INC_WIS:
			{
				if (do_inc_stat(Ind, A_WIS)) ident = TRUE;
				break;
			}

		case SV_POTION_INC_DEX:
			{
				if (do_inc_stat(Ind, A_DEX)) ident = TRUE;
				break;
			}

		case SV_POTION_INC_CON:
			{
				if (do_inc_stat(Ind, A_CON)) ident = TRUE;
				break;
			}

		case SV_POTION_INC_CHR:
			{
				if (do_inc_stat(Ind, A_CHR)) ident = TRUE;
				break;
			}

		case SV_POTION_AUGMENTATION:
			{
				if (do_inc_stat(Ind, A_STR)) ident = TRUE;
				if (do_inc_stat(Ind, A_INT)) ident = TRUE;
				if (do_inc_stat(Ind, A_WIS)) ident = TRUE;
				if (do_inc_stat(Ind, A_DEX)) ident = TRUE;
				if (do_inc_stat(Ind, A_CON)) ident = TRUE;
				if (do_inc_stat(Ind, A_CHR)) ident = TRUE;
				break;
			}

		case SV_POTION_ENLIGHTENMENT:
			{
				msg_print(Ind, "An image of your surroundings forms in your mind...");
				wiz_lite(Ind);
				ident = TRUE;
				break;
			}

		case SV_POTION_STAR_ENLIGHTENMENT:
			{
				msg_print(Ind, "You begin to feel more enlightened...");
				msg_print(Ind, NULL);
				wiz_lite(Ind);
				(void)do_inc_stat(Ind, A_INT);
				(void)do_inc_stat(Ind, A_WIS);
				(void)detect_treasure(Ind);
				(void)detect_objects_normal(Ind);
				(void)detect_sdoor(Ind);
				(void)detect_trap(Ind);
				identify_pack(Ind);
				self_knowledge(Ind);
				ident = TRUE;
				break;
			}

		case SV_POTION_SELF_KNOWLEDGE:
			{
				msg_print(Ind, "You begin to know yourself a little better...");
				msg_print(Ind, NULL);
				self_knowledge(Ind);
				ident = TRUE;
				break;
			}

		case SV_POTION_EXPERIENCE:
			{
				if (p_ptr->exp < PY_MAX_EXP)
				{
					s32b ee = (p_ptr->exp / 2) + 10;
					if (ee > 100000L) ee = 100000L;
					msg_print(Ind, "You feel more experienced.");
					gain_exp(Ind, ee);
					ident = TRUE;
				}
				break;
			}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* The item has been tried */
	object_tried(Ind, o_ptr);

	/* An identification was made */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Potions can feed the player */
	(void)set_food(Ind, p_ptr->food + o_ptr->pval);


	/* Destroy a potion in the pack */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}

	/* Destroy a potion on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}


/*
 * Curse the players armor
 */
static bool curse_armor(int Ind)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	char o_name[80];


	/* Curse the body armor */
	o_ptr = &p_ptr->inventory[INVEN_BODY];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Attempt a saving throw for artifacts */
	if (artifact_p(o_ptr) && (rand_int(100) < 50))
	{
		/* Cool */
		msg_format(Ind, "A %s tries to %s, but your %s resists the effects!",
				"terrible black aura", "surround your armor", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format(Ind, "A terrible black aura blasts your %s!", o_name);

		/* Blast the armor */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_BLASTED;
		o_ptr->name3 = 0;
		o_ptr->to_a = 0 - randint(5) - randint(5);
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= ID_CURSED;

		/* Break it */
		o_ptr->ident |= ID_BROKEN;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return (TRUE);
}


/*
 * Curse the players weapon
 */
static bool curse_weapon(int Ind)
{
	player_type *p_ptr = Players[Ind];

	object_type *o_ptr;

	char o_name[80];


	/* Curse the weapon */
	o_ptr = &p_ptr->inventory[INVEN_WIELD];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(Ind, o_name, o_ptr, FALSE, 3);

	/* Attempt a saving throw */
	if (artifact_p(o_ptr) && (rand_int(100) < 50))
	{
		/* Cool */
		msg_format(Ind, "A %s tries to %s, but your %s resists the effects!",
				"terrible black aura", "surround your weapon", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format(Ind, "A terrible black aura blasts your %s!", o_name);

		/* Shatter the weapon */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_SHATTERED;
		o_ptr->name3 = 0;
		o_ptr->to_h = 0 - randint(5) - randint(5);
		o_ptr->to_d = 0 - randint(5) - randint(5);
		o_ptr->to_a = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= ID_CURSED;

		/* Break it */
		o_ptr->ident |= ID_BROKEN;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	/* Notice */
	return (TRUE);
}


/*
 * Read a scroll (from the pack or floor).
 *
 * Certain scrolls can be "aborted" without losing the scroll.  These
 * include scrolls with no effects but recharge or identify, which are
 * cancelled before use.  XXX Reading them still takes a turn, though.
 */
void do_cmd_read_scroll(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];
	int			k, used_up, ident;
	object_type		*o_ptr;


	/* Check some conditions */
	if (p_ptr->blind)
	{
		msg_print(Ind, "You can't see anything.");
		return;
	}
	if (no_lite(Ind))
	{
		msg_print(Ind, "You have no light to read by.");
		return;
	}
	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return;
	}


	/* Restrict choices to scrolls */
	item_tester_tval = TV_SCROLL;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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

	if( check_guard_inscription( o_ptr->note, 'r' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	};

	if (o_ptr->tval != TV_SCROLL)
	{
		/* Tried to read non-scroll */
		return;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Not identified yet */
	ident = FALSE;

	/* Object level */

	/* Assume the scroll will get used up */
	used_up = TRUE;

	/* Analyze the scroll */
	switch (o_ptr->sval)
	{

		case SV_SCROLL_DARKNESS:
			{
				if (unlite_area(Ind, 10, 3)) ident = TRUE;
				if (!p_ptr->resist_blind)
				{
					(void)set_blind(Ind, p_ptr->blind + 3 + randint(5));
				}
				break;
			}

		case SV_SCROLL_AGGRAVATE_MONSTER:
			{
				msg_print(Ind, "There is a high pitched humming noise.");
				aggravate_monsters(Ind, 1);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_CURSE_ARMOR:
			{
				if (curse_armor(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_CURSE_WEAPON:
			{
				if (curse_weapon(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_SUMMON_MONSTER:
			{
				for (k = 0; k < randint(3); k++)
				{
					if (summon_specific(p_ptr->dun_depth, p_ptr->py, p_ptr->px, p_ptr->dun_depth, 0))
					{
						ident = TRUE;
					}
				}
				break;
			}

			/* not adding bounds checking now... because of perma walls
				 hope that I don't need to...... 

				 OK, modified so you cant ressurect ghosts in walls......
				 to prevent bad things from happening in town.
			 */
		case SV_SCROLL_LIFE:
			{/*
					for (y = -1; y <= 1; y++)
					{
					for (x = -1; x <= 1; x++)
					{				
					c_ptr = &cave[p_ptr->dun_depth][p_ptr->py+y][p_ptr->px+x];

					if ((c_ptr->m_idx < 0) && (cave_floor_bold(p_ptr->dun_depth, p_ptr->py+y, p_ptr->px+x)))
					{
					if (Players[0 - c_ptr->m_idx]->ghost)
					{
					resurrect_player(0 - c_ptr->m_idx);
					break;
					}
					}

					}
					}
				*/
				restore_level(Ind);
				do_scroll_life(Ind);
				break;

			}

		case SV_SCROLL_SUMMON_UNDEAD:
			{
				for (k = 0; k < randint(3); k++)
				{
					if (summon_specific(p_ptr->dun_depth, p_ptr->py, p_ptr->px, p_ptr->dun_depth, SUMMON_UNDEAD))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_SCROLL_TRAP_CREATION:
			{
				if (trap_creation(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_PHASE_DOOR:
			{
				teleport_player(Ind, 10);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_TELEPORT:
			{
				teleport_player(Ind, 100);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_TELEPORT_LEVEL:
			{
				(void)teleport_player_level(Ind);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_WORD_OF_RECALL:
			{
				set_recall(Ind, o_ptr);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_IDENTIFY:
			{
				msg_print(Ind, "This is an identify scroll.");
				ident = TRUE;
				(void)ident_spell(Ind);
				used_up = FALSE;
				break;
			}

		case SV_SCROLL_STAR_IDENTIFY:
			{
				msg_print(Ind, "This is an *identify* scroll.");
				ident = TRUE;
				(void)identify_fully(Ind);
				used_up = FALSE;
				break;
			}

		case SV_SCROLL_REMOVE_CURSE:
			{
				if (remove_curse(Ind))
				{
					msg_print(Ind, "You feel as if someone is watching over you.");
					ident = TRUE;
				}
				break;
			}

		case SV_SCROLL_STAR_REMOVE_CURSE:
			{
				remove_all_curse(Ind);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_ENCHANT_ARMOR:
			{
				msg_print(Ind, "This is a scroll of enchant armor.");
				ident = TRUE;
				(void)enchant_spell(Ind, 0, 0, 1);
				used_up = FALSE;
				break;
			}

		case SV_SCROLL_CREATE_ARTIFACT:
			{
#ifndef DEBUG
				if (!strcmp(p_ptr->name, cfg_dungeon_master))
				{
#endif
					(void)create_artifact(Ind);
					used_up = FALSE;
					ident = TRUE;
#ifndef DEBUG
				} else {
					ident = TRUE;
					msg_print(Ind, "You failed to read the scroll properly.");
				}
#endif
				break;
			}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
			{
				msg_print(Ind, "This is a scroll of enchant weapon to-hit.");
				(void)enchant_spell(Ind, 1, 0, 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
			{
				msg_print(Ind, "This is a scroll of enchant weapon to-dam.");
				(void)enchant_spell(Ind, 0, 1, 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
			{
				msg_print(Ind, "This is a scroll of *enchant* armor.");
				(void)enchant_spell(Ind, 0, 0, randint(3) + 2);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
			{
				msg_print(Ind, "This is a scroll of *enchant* weapon.");
				(void)enchant_spell(Ind, randint(3), randint(3), 0);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

		case SV_SCROLL_RECHARGING:
			{
				msg_print(Ind, "This is a scroll of recharging.");
				(void)recharge(Ind, 60);
				used_up = FALSE;
				ident = TRUE;
				break;
			}

		case SV_SCROLL_LIGHT:
			{
				if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
				break;
			}

		case SV_SCROLL_MAPPING:
			{
				map_area(Ind);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_DETECT_GOLD:
			{
				if (detect_treasure(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_DETECT_ITEM:
			{
				if (detect_objects_normal(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_DETECT_TRAP:
			{
				if (detect_trap(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_DETECT_DOOR:
			{
				if (detect_sdoor(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_DETECT_INVIS:
			{
				if (detect_invisible(Ind, TRUE)) ident = TRUE;
				break;
			}

		case SV_SCROLL_SATISFY_HUNGER:
			{
				if (set_food(Ind, PY_FOOD_MAX - 1)) ident = TRUE;
				break;
			}

		case SV_SCROLL_BLESSING:
			{
				if (set_blessed(Ind, p_ptr->blessed + randint(12) + 6)) ident = TRUE;
				break;
			}

		case SV_SCROLL_HOLY_CHANT:
			{
				if (set_blessed(Ind, p_ptr->blessed + randint(24) + 12)) ident = TRUE;
				break;
			}

		case SV_SCROLL_HOLY_PRAYER:
			{
				if (set_blessed(Ind, p_ptr->blessed + randint(48) + 24)) ident = TRUE;
				break;
			}

		case SV_SCROLL_MONSTER_CONFUSION:
			{
				if (p_ptr->confusing == 0)
				{
					msg_print(Ind, "Your hands begin to glow.");
					p_ptr->confusing = TRUE;
					ident = TRUE;
				}
				break;
			}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
			{
				k = 3 * p_ptr->lev;
				if (set_protevil(Ind, p_ptr->protevil + randint(25) + k)) ident = TRUE;
				break;
			}

		case SV_SCROLL_RUNE_OF_PROTECTION:
			{
				warding_glyph(Ind);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
			{
				if (destroy_doors_touch(Ind)) ident = TRUE;
				break;
			}

		case SV_SCROLL_STAR_DESTRUCTION:
			{
				destroy_area(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 15, TRUE);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_DISPEL_UNDEAD:
			{
				if (dispel_undead(Ind, 60)) ident = TRUE;
				break;
			}

		case SV_SCROLL_GENOCIDE:
			{
				msg_print(Ind, "This is a banishment scroll.");
				(void)banishment(Ind);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_MASS_GENOCIDE:
			{
				msg_print(Ind, "This is a mass banishment scroll.");
				(void)mass_banishment(Ind);
				ident = TRUE;
				break;
			}

		case SV_SCROLL_ACQUIREMENT:
			{
				ident = TRUE;
				/* Don't allow this in the wilderness */
				if(p_ptr->dun_depth < 0)
				{
					msg_print(Ind, "The Valar grant no gifts in the wilderness.");
					break;
				}
				acquirement(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 1);
				break;
			}

		case SV_SCROLL_STAR_ACQUIREMENT:
			{
				ident = TRUE;
				/* Don't allow this in the wilderness */
				if(p_ptr->dun_depth < 0)
				{
					msg_print(Ind, "The Valar grant no gifts in the wilderness.");
					break;
				}
				acquirement(p_ptr->dun_depth, p_ptr->py, p_ptr->px, randint(2) + 1);
				break;
			}
	}

	/* Hack -- allow certain scrolls to be "preserved" */
	if (!used_up) 
	{
		p_ptr->current_scroll = item;
		return;
	}

	do_cmd_read_scroll_end(Ind, item, ident);
}
void do_cmd_read_scroll_end(int Ind, int item, bool ident)
{
	player_type *p_ptr = Players[Ind];
	object_type		*o_ptr;
	int lev;

	/* Get the scroll (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}
	/* Get the scroll (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	} 

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* The item was tried */
	object_tried(Ind, o_ptr);

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	/* An identification was made */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Destroy a scroll in the pack */
	if (item >= 0)
	{
		inven_item_increase(Ind, item, -1);
		inven_item_describe(Ind, item);
		inven_item_optimize(Ind, item);
	}
	/* Destroy a scroll on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	} 

	p_ptr->current_scroll = -1;
}






/*
 * Use a staff.			-RAK-
 *
 * One charge of one staff disappears.
 *
 * Hack -- staffs of identify can be "cancelled".
 */
void do_cmd_use_staff(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int			ident, chance, k, lev;

	object_type		*o_ptr;

	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;


	/* Restrict choices to wands */
	item_tester_tval = TV_STAFF;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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

	if( check_guard_inscription( o_ptr->note, 'u' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}; 

	if (o_ptr->tval != TV_STAFF)
	{
		/* Tried to use non-staff */
		return;
	}

	/* Mega-Hack -- refuse to use a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the staffs.");
		return;
	}

	/* Verify potential overflow */
	/*if ((inven_cnt >= INVEN_PACK) &&
		(o_ptr->number > 1))
		{*/
	/* Verify with the player */
	/*if (other_query_flag &&
		!get_check("Your pack might overflow.  Continue? ")) return;
		}*/


	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Not identified yet */
	ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print(Ind, "You failed to use the staff properly.");
		return;
	}

	/* Notice empty staffs */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
		msg_print(Ind, "The staff has no charges left.");
		o_ptr->ident |= ID_EMPTY;

		/* Redraw */
		p_ptr->window |= (PW_INVEN);

		return;
	}


	/* Analyze the staff */
	switch (o_ptr->sval)
	{
		case SV_STAFF_DARKNESS:
			{
				if (unlite_area(Ind, 10, 3)) ident = TRUE;
				if (!p_ptr->resist_blind)
				{
					if (set_blind(Ind, p_ptr->blind + 3 + randint(5))) ident = TRUE;
				}
				break;
			}

		case SV_STAFF_SLOWNESS:
			{
				if (set_slow(Ind, p_ptr->slow + randint(30) + 15)) ident = TRUE;
				break;
			}

		case SV_STAFF_HASTE_MONSTERS:
			{
				if (speed_monsters(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_SUMMONING:
			{
				for (k = 0; k < randint(4); k++)
				{
					if (summon_specific(p_ptr->dun_depth, p_ptr->py, p_ptr->px, p_ptr->dun_depth, 0))
					{
						ident = TRUE;
					}
				}
				break;
			}

		case SV_STAFF_TELEPORTATION:
			{
				msg_format_near(Ind, "%s teleports away!", p_ptr->name);
				teleport_player(Ind, 100);
				ident = TRUE;
				break;
			}

		case SV_STAFF_IDENTIFY:
			{
				//if (!
				ident_spell(Ind);
				//)
				use_charge = FALSE;
				ident = TRUE;
				break;
			}

		case SV_STAFF_REMOVE_CURSE:
			{
				if (remove_curse(Ind))
				{
					if (!p_ptr->blind)
					{
						msg_print(Ind, "The staff glows blue for a moment...");
					}
					ident = TRUE;
				}
				break;
			}

		case SV_STAFF_STARLITE:
			{
				if (!p_ptr->blind)
				{
					msg_print(Ind, "The end of the staff glows brightly...");
				}
				for (k = 0; k < 8; k++) lite_line(Ind, ddd[k]);
				ident = TRUE;
				break;
			}

		case SV_STAFF_LITE:
			{
				msg_format_near(Ind, "%s calls light.", p_ptr->name);
				if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
				break;
			}

		case SV_STAFF_MAPPING:
			{
				map_area(Ind);
				ident = TRUE;
				break;
			}

		case SV_STAFF_DETECT_GOLD:
			{
				if (detect_treasure(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_DETECT_ITEM:
			{
				if (detect_objects_normal(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_DETECT_TRAP:
			{
				if (detect_trap(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_DETECT_DOOR:
			{
				if (detect_sdoor(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_DETECT_INVIS:
			{
				if (detect_invisible(Ind, TRUE)) ident = TRUE;
				break;
			}

		case SV_STAFF_DETECT_EVIL:
			{
				if (detect_evil(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_CURE_LIGHT:
			{
				if (hp_player(Ind, randint(8))) ident = TRUE;
				break;
			}

		case SV_STAFF_CURING:
			{
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_STAFF_HEALING:
			{
				if (hp_player(Ind, 300)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_STAFF_THE_MAGI:
			{
				if (do_res_stat(Ind, A_INT)) ident = TRUE;
				if (p_ptr->csp < p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
					ident = TRUE;
					msg_print(Ind, "Your feel your head clear.");
					p_ptr->redraw |= (PR_MANA);
					p_ptr->window |= (PW_PLAYER);
				}
				break;
			}

		case SV_STAFF_SLEEP_MONSTERS:
			{
				if (sleep_monsters(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_SLOW_MONSTERS:
			{
				if (slow_monsters(Ind)) ident = TRUE;
				break;
			}

		case SV_STAFF_SPEED:
			{
				if (!p_ptr->fast)
				{
					if (set_fast(Ind, randint(30) + 15)) ident = TRUE;
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				break;
			}

		case SV_STAFF_PROBING:
			{
				probing(Ind);
				ident = TRUE;
				break;
			}

		case SV_STAFF_DISPEL_EVIL:
			{
				if (dispel_evil(Ind, 60)) ident = TRUE;
				break;
			}

		case SV_STAFF_POWER:
			{
				if (dispel_monsters(Ind, 120)) ident = TRUE;
				break;
			}

		case SV_STAFF_HOLINESS:
			{
				if (dispel_evil(Ind, 120)) ident = TRUE;
				k = 3 * p_ptr->lev;
				if (set_protevil(Ind, p_ptr->protevil + randint(25) + k)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_afraid(Ind, 0)) ident = TRUE;
				if (hp_player(Ind, 50)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				break;
			}

		case SV_STAFF_GENOCIDE:
			{
				(void)banishment(Ind);
				ident = TRUE;
				break;
			}

		case SV_STAFF_EARTHQUAKES:
			{
				msg_format_near(Ind, "%s causes the ground to shake!", p_ptr->name);
				earthquake(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 10);
				ident = TRUE;
				break;
			}

		case SV_STAFF_DESTRUCTION:
			{
				msg_format_near(Ind, "%s unleashes great power!", p_ptr->name);
				destroy_area(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 15, TRUE);
				ident = TRUE;
				break;
			}
	}

	/* Hack -- some uses are "free" */
	if (!use_charge)
	{
		p_ptr->current_staff = item; 
		return;
	}

	do_cmd_use_staff_discharge(Ind, item, ident);
}

void do_cmd_use_staff_discharge(int Ind, int item, bool ident)
{
	player_type *p_ptr = Players[Ind];

	int lev;

	object_type		*o_ptr;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}
	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the item */
	object_tried(Ind, o_ptr);

	/* An identification was made */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Use a single charge */
	o_ptr->pval--;

	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore the charges */
		o_ptr->pval++;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your staff.");
	}

	/* Describe charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(Ind, item);
	}

	/* Describe charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}

	p_ptr->current_staff = -1; 
}

/*
 * Aim a wand (from the pack or floor).
 *
 * Use a single charge from a single item.
 * Handle "unstacking" in a logical manner.
 *
 * For simplicity, you cannot use a stack of items from the
 * ground.  This would require too much nasty code.
 *
 * There are no wands which can "destroy" themselves, in the inventory
 * or on the ground, so we can ignore this possibility.  Note that this
 * required giving "wand of wonder" the ability to ignore destruction
 * by electric balls.
 *
 * All wands can be "cancelled" at the "Direction?" prompt for free.
 *
 * Note that the basic "bolt" wands do slightly less damage than the
 * basic "bolt" rods, but the basic "ball" wands do the same damage
 * as the basic "ball" rods.
 */
void do_cmd_aim_wand(int Ind, int item, int dir)
{
	player_type *p_ptr = Players[Ind];

	int			lev, ident, chance, sval;

	object_type		*o_ptr;


	/* Restrict choices to wands */
	item_tester_tval = TV_WAND;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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
	if( check_guard_inscription( o_ptr->note, 'a' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}; 
	/* Mega-Hack -- refuse to aim a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the wands.");
		return;
	}

	/* Hack -- verify potential overflow */
	/*if ((inven_cnt >= INVEN_PACK) &&
		(o_ptr->number > 1))
		{*/
	/* Verify with the player */
	/*	if (other_query_flag &&
			!get_check("Your pack might overflow.  Continue? ")) return;
			}*/


	/* Allow direction to be cancelled for free */
	/*if (!get_aim_dir(&dir)) return;*/


	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Not identified yet */
	ident = FALSE;

	/* Get the level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print(Ind, "You failed to use the wand properly.");
		return;
	}

	/* The wand is already empty! */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
		msg_print(Ind, "The wand has no charges left.");
		o_ptr->ident |= ID_EMPTY;

		/* Redraw */
		p_ptr->window |= (PW_INVEN);

		return;
	}



	/* XXX Hack -- Extract the "sval" effect */
	sval = o_ptr->sval;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER) sval = rand_int(SV_WAND_WONDER);

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_MONSTER:
			{
				if (heal_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_HASTE_MONSTER:
			{
				if (speed_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_CLONE_MONSTER:
			{
				if (clone_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_TELEPORT_AWAY:
			{
				if (teleport_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_DISARMING:
			{
				if (disarm_trap(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_TRAP_DOOR_DEST:
			{
				if (destroy_door(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_STONE_TO_MUD:
			{
				if (wall_to_mud(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_LITE:
			{
				msg_print(Ind, "A line of blue shimmering light appears.");
				lite_line(Ind, dir);
				ident = TRUE;
				break;
			}

		case SV_WAND_SLEEP_MONSTER:
			{
				if (sleep_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_SLOW_MONSTER:
			{
				if (slow_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_CONFUSE_MONSTER:
			{
				if (confuse_monster(Ind, dir, 10)) ident = TRUE;
				break;
			}

		case SV_WAND_FEAR_MONSTER:
			{
				if (fear_monster(Ind, dir, 10)) ident = TRUE;
				break;
			}

		case SV_WAND_DRAIN_LIFE:
			{
				if (drain_life(Ind, dir, 75)) ident = TRUE;
				break;
			}

		case SV_WAND_POLYMORPH:
			{
				if (poly_monster(Ind, dir)) ident = TRUE;
				break;
			}

		case SV_WAND_STINKING_CLOUD:
			{
				msg_format_near(Ind, "%s fires a stinking cloud.", p_ptr->name);
				fire_ball(Ind, GF_POIS, dir, 12, 2);
				ident = TRUE;
				break;
			}

		case SV_WAND_MAGIC_MISSILE:
			{
				msg_format_near(Ind, "%s fires a magic missile.", p_ptr->name);
				fire_bolt_or_beam(Ind, 20, GF_MISSILE, dir, damroll(2, 6));
				ident = TRUE;
				break;
			}

		case SV_WAND_ACID_BOLT:
			{
				msg_format_near(Ind, "%s fires an acid bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 20, GF_ACID, dir, damroll(5, 8));
				ident = TRUE;
				break;
			}

		case SV_WAND_ELEC_BOLT:
			{
				msg_format_near(Ind, "%s fires a lightning bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 20, GF_ELEC, dir, damroll(3, 8));
				ident = TRUE;
				break;
			}

		case SV_WAND_FIRE_BOLT:
			{
				msg_format_near(Ind, "%s fires a fire bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 20, GF_FIRE, dir, damroll(6, 8));
				ident = TRUE;
				break;
			}

		case SV_WAND_COLD_BOLT:
			{
				msg_format_near(Ind, "%s fires a frost bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 20, GF_COLD, dir, damroll(3, 8));
				ident = TRUE;
				break;
			}

		case SV_WAND_ACID_BALL:
			{
				msg_format_near(Ind, "%s fires a ball of acid.", p_ptr->name);
				fire_ball(Ind, GF_ACID, dir, 60, 2);
				ident = TRUE;
				break;
			}

		case SV_WAND_ELEC_BALL:
			{
				msg_format_near(Ind, "%s fires a ball of electricity.", p_ptr->name);
				fire_ball(Ind, GF_ELEC, dir, 32, 2);
				ident = TRUE;
				break;
			}

		case SV_WAND_FIRE_BALL:
			{
				msg_format_near(Ind, "%s fires a fire ball.", p_ptr->name);
				fire_ball(Ind, GF_FIRE, dir, 72, 2);
				ident = TRUE;
				break;
			}

		case SV_WAND_COLD_BALL:
			{
				msg_format_near(Ind, "%s fires a frost ball.", p_ptr->name);
				fire_ball(Ind, GF_COLD, dir, 48, 2);
				ident = TRUE;
				break;
			}

		case SV_WAND_WONDER:
			{
				/* Oops.  Wand of wonder activated */
				break;
			}

		case SV_WAND_DRAGON_FIRE:
			{
				msg_format_near(Ind, "%s shoots dragon fire!", p_ptr->name);
				fire_ball(Ind, GF_FIRE, dir, 100, 3);
				ident = TRUE;
				break;
			}

		case SV_WAND_DRAGON_COLD:
			{
				msg_format_near(Ind, "%s shoots dragon frost!", p_ptr->name);
				fire_ball(Ind, GF_COLD, dir, 80, 3);
				ident = TRUE;
				break;
			}

		case SV_WAND_DRAGON_BREATH:
			{
				switch (randint(5))
				{
					case 1:
						{
							msg_format_near(Ind, "%s shoots dragon acid!", p_ptr->name);
							fire_ball(Ind, GF_ACID, dir, 100, 3);
							break;
						}

					case 2:
						{
							msg_format_near(Ind, "%s shoots dragon lightning!", p_ptr->name);
							fire_ball(Ind, GF_ELEC, dir, 80, 3);
							break;
						}

					case 3:
						{
							msg_format_near(Ind, "%s shoots dragon fire!", p_ptr->name);
							fire_ball(Ind, GF_FIRE, dir, 100, 3);
							break;
						}

					case 4:
						{
							msg_format_near(Ind, "%s shoots dragon frost!", p_ptr->name);
							fire_ball(Ind, GF_COLD, dir, 80, 3);
							break;
						}

					default:
						{
							msg_format_near(Ind, "%s shoots dragon poison!", p_ptr->name);
							fire_ball(Ind, GF_POIS, dir, 60, 3);
							break;
						}
				}

				ident = TRUE;
				break;
			}

		case SV_WAND_ANNIHILATION:
			{
				if (drain_life(Ind, dir, 125)) ident = TRUE;
				break;
			}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Mark it as tried */
	object_tried(Ind, o_ptr);

	/* Apply identification */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Use a single charge */
	o_ptr->pval--;

	/* Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore the charges */
		o_ptr->pval++;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your wand.");
	}

	/* Describe the charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(Ind, item);
	}

	/* Describe the charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}





/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 */
void do_cmd_zap_rod(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int                 ident, chance, lev;

	object_type		*o_ptr;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;


	/* Restrict choices to rods */
	item_tester_tval = TV_ROD;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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
	if( check_guard_inscription( o_ptr->note, 'z' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}; 


	if (o_ptr->tval != TV_ROD)
	{
		/* Tried to zap non-rod */
		return;
	}

	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the rods.");
		return;
	}

	/* Get a direction (unless KNOWN not to need it) */
	if ((o_ptr->sval >= SV_ROD_MIN_DIRECTION) || !object_aware_p(Ind, o_ptr))
	{
		/* Get a direction, then return */
		p_ptr->current_rod = item;
		get_aim_dir(Ind);
		return;
	}


	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Not identified yet */
	ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print(Ind, "You failed to use the rod properly.");
		return;
	}

	/* Still charging */
	if (o_ptr->pval)
	{
		if (flush_failure) flush();
		msg_print(Ind, "The rod is still charging.");
		return;
	}


	/* Analyze the rod */
	switch (o_ptr->sval)
	{
		case SV_ROD_DETECT_TRAP:
			{
				if (detect_trap(Ind)) ident = TRUE;
				o_ptr->pval = 50;
				break;
			}

		case SV_ROD_DETECT_DOOR:
			{
				if (detect_sdoor(Ind)) ident = TRUE;
				o_ptr->pval = 70;
				break;
			}

		case SV_ROD_IDENTIFY:
			{
				ident = TRUE;
				if (!ident_spell(Ind)) use_charge = FALSE;
				o_ptr->pval = 10;
				break;
			}

		case SV_ROD_RECALL:
			{
				set_recall(Ind,o_ptr);
				ident = TRUE;
				o_ptr->pval = 60;
				break;
			}

		case SV_ROD_ILLUMINATION:
			{
				msg_format_near(Ind, "%s calls light.", p_ptr->name);
				if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
				o_ptr->pval = 30;
				break;
			}

		case SV_ROD_MAPPING:
			{
				map_area(Ind);
				ident = TRUE;
				o_ptr->pval = 99;
				break;
			}

		case SV_ROD_DETECTION:
			{
				detection(Ind);
				ident = TRUE;
				o_ptr->pval = 99;
				break;
			}

		case SV_ROD_PROBING:
			{
				probing(Ind);
				ident = TRUE;
				o_ptr->pval = 50;
				break;
			}

		case SV_ROD_CURING:
			{
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				o_ptr->pval = 999;
				break;
			}

		case SV_ROD_HEALING:
			{
				if (hp_player(Ind, 500)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				o_ptr->pval = 999;
				break;
			}

		case SV_ROD_RESTORATION:
			{
				if (restore_level(Ind)) ident = TRUE;
				if (do_res_stat(Ind, A_STR)) ident = TRUE;
				if (do_res_stat(Ind, A_INT)) ident = TRUE;
				if (do_res_stat(Ind, A_WIS)) ident = TRUE;
				if (do_res_stat(Ind, A_DEX)) ident = TRUE;
				if (do_res_stat(Ind, A_CON)) ident = TRUE;
				if (do_res_stat(Ind, A_CHR)) ident = TRUE;
				o_ptr->pval = 999;
				break;
			}

		case SV_ROD_SPEED:
			{
				if (!p_ptr->fast)
				{
					if (set_fast(Ind, randint(30) + 15)) ident = TRUE;
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->pval = 99;
				break;
			}

		default:
			{
				/* Directional rod zapped in non-directional function */
				return;
			}
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the object */
	object_tried(Ind, o_ptr);

	/* Successfully determined the object function */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Hack -- deal with cancelled zap */
	if (!use_charge)
	{
		o_ptr->pval = 0;
		return;
	}


	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore "charge" */
		o_ptr->pval = 0;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your rod.");
	}
}



/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 */
void do_cmd_zap_rod_dir(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	int                 item, ident, chance, lev;

	object_type		*o_ptr;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;


	item = p_ptr->current_rod;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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
	if( check_guard_inscription( o_ptr->note, 'z' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}; 

	if (o_ptr->tval != TV_ROD)
	{
		/* Tried to zap non-rod */
		return;
	}

	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(Ind, "You must first pick up the rods.");
		return;
	}

	/* Hack -- verify potential overflow */
	/*if ((inven_cnt >= INVEN_PACK) &&
		(o_ptr->number > 1))
		{*/
	/* Verify with the player */
	/*if (other_query_flag &&
		!get_check("Your pack might overflow.  Continue? ")) return;
		}*/

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Not identified yet */
	ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print(Ind, "You failed to use the rod properly.");
		return;
	}

	/* Still charging */
	if (o_ptr->pval)
	{
		if (flush_failure) flush();
		msg_print(Ind, "The rod is still charging.");
		return;
	}


	/* Analyze the rod */
	switch (o_ptr->sval)
	{
		case SV_ROD_TELEPORT_AWAY:
			{
				if (teleport_monster(Ind, dir)) ident = TRUE;
				o_ptr->pval = 25;
				break;
			}

		case SV_ROD_DISARMING:
			{
				if (disarm_trap(Ind, dir)) ident = TRUE;
				o_ptr->pval = 30;
				break;
			}

		case SV_ROD_LITE:
			{
				msg_print(Ind, "A line of blue shimmering light appears.");
				lite_line(Ind, dir);
				ident = TRUE;
				o_ptr->pval = 9;
				break;
			}

		case SV_ROD_SLEEP_MONSTER:
			{
				if (sleep_monster(Ind, dir)) ident = TRUE;
				o_ptr->pval = 18;
				break;
			}

		case SV_ROD_SLOW_MONSTER:
			{
				if (slow_monster(Ind, dir)) ident = TRUE;
				o_ptr->pval = 20;
				break;
			}

		case SV_ROD_DRAIN_LIFE:
			{
				if (drain_life(Ind, dir, 75)) ident = TRUE;
				o_ptr->pval = 23;
				break;
			}

		case SV_ROD_POLYMORPH:
			{
				if (poly_monster(Ind, dir)) ident = TRUE;
				o_ptr->pval = 25;
				break;
			}

		case SV_ROD_ACID_BOLT:
			{
				msg_format_near(Ind, "%s fires an acid bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 10, GF_ACID, dir, damroll(6, 8));
				ident = TRUE;
				o_ptr->pval = 12;
				break;
			}

		case SV_ROD_ELEC_BOLT:
			{
				msg_format_near(Ind, "%s fires a lightning bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 10, GF_ELEC, dir, damroll(3, 8));
				ident = TRUE;
				o_ptr->pval = 11;
				break;
			}

		case SV_ROD_FIRE_BOLT:
			{
				msg_format_near(Ind, "%s fires a fire bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 10, GF_FIRE, dir, damroll(8, 8));
				ident = TRUE;
				o_ptr->pval = 15;
				break;
			}

		case SV_ROD_COLD_BOLT:
			{
				msg_format_near(Ind, "%s fires a frost bolt.", p_ptr->name);
				fire_bolt_or_beam(Ind, 10, GF_COLD, dir, damroll(5, 8));
				ident = TRUE;
				o_ptr->pval = 13;
				break;
			}

		case SV_ROD_ACID_BALL:
			{
				msg_format_near(Ind, "%s fires an acid ball.", p_ptr->name);
				fire_ball(Ind, GF_ACID, dir, 60, 2);
				ident = TRUE;
				o_ptr->pval = 27;
				break;
			}

		case SV_ROD_ELEC_BALL:
			{
				msg_format_near(Ind, "%s fires a lightning ball.", p_ptr->name);
				fire_ball(Ind, GF_ELEC, dir, 32, 2);
				ident = TRUE;
				o_ptr->pval = 23;
				break;
			}

		case SV_ROD_FIRE_BALL:
			{
				msg_format_near(Ind, "%s fires a fire ball.", p_ptr->name);
				fire_ball(Ind, GF_FIRE, dir, 72, 2);
				ident = TRUE;
				o_ptr->pval = 30;
				break;
			}

		case SV_ROD_COLD_BALL:
			{
				msg_format_near(Ind, "%s fires a frost ball.", p_ptr->name);
				fire_ball(Ind, GF_COLD, dir, 48, 2);
				ident = TRUE;
				o_ptr->pval = 25;
				break;
			}

			/* All of the following are needed if we tried zapping one of */
			/* these but we didn't know what it was. */
		case SV_ROD_DETECT_TRAP:
			{
				if (detect_trap(Ind)) ident = TRUE;
				o_ptr->pval = 50;
				break;
			}

		case SV_ROD_DETECT_DOOR:
			{
				if (detect_sdoor(Ind)) ident = TRUE;
				o_ptr->pval = 70;
				break;
			}

		case SV_ROD_IDENTIFY:
			{
				ident = TRUE;
				if (!ident_spell(Ind)) use_charge = FALSE;
				o_ptr->pval = 10;
				break;
			}

		case SV_ROD_RECALL:
			{
				set_recall(Ind,o_ptr);
				ident = TRUE;
				o_ptr->pval = 60;
				break;
			}

		case SV_ROD_ILLUMINATION:
			{
				msg_format_near(Ind, "%s calls light.", p_ptr->name);
				if (lite_area(Ind, damroll(2, 8), 2)) ident = TRUE;
				o_ptr->pval = 30;
				break;
			}

		case SV_ROD_MAPPING:
			{
				map_area(Ind);
				ident = TRUE;
				o_ptr->pval = 99;
				break;
			}

		case SV_ROD_DETECTION:
			{
				detection(Ind);
				ident = TRUE;
				o_ptr->pval = 99;
				break;
			}

		case SV_ROD_PROBING:
			{
				probing(Ind);
				ident = TRUE;
				o_ptr->pval = 50;
				break;
			}

		case SV_ROD_CURING:
			{
				if (set_blind(Ind, 0)) ident = TRUE;
				if (set_poisoned(Ind, 0)) ident = TRUE;
				if (set_confused(Ind, 0)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				o_ptr->pval = 999;
				break;
			}

		case SV_ROD_HEALING:
			{
				if (hp_player(Ind, 500)) ident = TRUE;
				if (set_stun(Ind, 0)) ident = TRUE;
				if (set_cut(Ind, 0)) ident = TRUE;
				o_ptr->pval = 999;
				break;
			}

		case SV_ROD_RESTORATION:
			{
				if (restore_level(Ind)) ident = TRUE;
				if (do_res_stat(Ind, A_STR)) ident = TRUE;
				if (do_res_stat(Ind, A_INT)) ident = TRUE;
				if (do_res_stat(Ind, A_WIS)) ident = TRUE;
				if (do_res_stat(Ind, A_DEX)) ident = TRUE;
				if (do_res_stat(Ind, A_CON)) ident = TRUE;
				if (do_res_stat(Ind, A_CHR)) ident = TRUE;
				o_ptr->pval = 999;
				break;
			}

		case SV_ROD_SPEED:
			{
				if (!p_ptr->fast)
				{
					if (set_fast(Ind, randint(30) + 15)) ident = TRUE;
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + 5);
				}
				o_ptr->pval = 99;
				break;
			}

		default:
			{
				msg_print(Ind, "SERVER ERROR: Tried to zap non-directional rod in directional function!");
				return;
			}
	}


	/* Clear the current rod */
	p_ptr->current_rod = -1;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the object */
	object_tried(Ind, o_ptr);

	/* Successfully determined the object function */
	if (ident && !object_aware_p(Ind, o_ptr))
	{
		object_aware(Ind, o_ptr);
		gain_exp(Ind, (lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Hack -- deal with cancelled zap */
	if (!use_charge)
	{
		o_ptr->pval = 0;
		return;
	}


	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		/* Make a fake item */
		object_type tmp_obj;
		tmp_obj = *o_ptr;
		tmp_obj.number = 1;

		/* Restore "charge" */
		o_ptr->pval = 0;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= tmp_obj.weight;
		item = inven_carry(Ind, &tmp_obj);

		/* Message */
		msg_print(Ind, "You unstack your rod.");
	}
}



/*
 * Hook to determine if an object is activatable
 */
static bool item_tester_hook_activate(int Ind, object_type *o_ptr)
{
	u32b f1, f2, f3;

	/* Not known */
	if (!object_known_p(Ind, o_ptr)) return (FALSE);

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Check activation flag */
	if (f3 & TR3_ACTIVATE) return (TRUE);

	/* Assume not */
	return (FALSE);
}



/*
 * Hack -- activate the ring of power
 */
static void ring_of_power(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	/* Pick a random effect */
	switch (randint(10))
	{
		case 1:
		case 2:
			{
				/* Message */
				msg_print(Ind, "You are surrounded by a malignant aura.");

				/* Decrease all stats (permanently) */
				(void)dec_stat(Ind, A_STR, 50, TRUE);
				(void)dec_stat(Ind, A_INT, 50, TRUE);
				(void)dec_stat(Ind, A_WIS, 50, TRUE);
				(void)dec_stat(Ind, A_DEX, 50, TRUE);
				(void)dec_stat(Ind, A_CON, 50, TRUE);
				(void)dec_stat(Ind, A_CHR, 50, TRUE);

				/* Lose some experience (permanently) */
				p_ptr->exp -= (p_ptr->exp / 4);
				p_ptr->max_exp -= (p_ptr->exp / 4);
				check_experience(Ind);

				break;
			}

		case 3:
			{
				/* Message */
				msg_print(Ind, "You are surrounded by a powerful aura.");

				/* Dispel monsters */
				dispel_monsters(Ind, 1000);

				break;
			}

		case 4:
		case 5:
		case 6:
			{
				/* Mana Ball */
				fire_ball(Ind, GF_MANA, dir, 300, 3);

				break;
			}

		case 7:
		case 8:
		case 9:
		case 10:
			{
				/* Mana Bolt */
				fire_bolt(Ind, GF_MANA, dir, 250);

				break;
			}
	}
}




/*
 * Enchant some bolts
 */
static bool brand_bolts(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int i;

	/* Use the first (XXX) acceptable bolts */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &p_ptr->inventory[i];

		/* Skip non-bolts */
		if (o_ptr->tval != TV_BOLT) continue;

		/* Skip artifacts and ego-items */
		if (artifact_p(o_ptr) || ego_item_p(o_ptr)) continue;

		/* Skip cursed/broken items */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) continue;

		/* Randomize */
		if (rand_int(100) < 75) continue;

		/* Message */
		msg_print(Ind, "Your bolts are covered in a fiery aura!");

		/* Ego-item */
		o_ptr->name2 = EGO_FLAME;

		/* Enchant */
		enchant(Ind, o_ptr, rand_int(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		/* Notice */
		return (TRUE);
	}

	/* Flush */
	if (flush_failure) flush();

	/* Fail */
	msg_print(Ind, "The fiery enchantment failed.");

	/* Notice */
	return (TRUE);
}


/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 */
void do_cmd_activate(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];

	int         i, k, lev, chance;

	object_type *o_ptr;


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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

	if( check_guard_inscription( o_ptr->note, 'A' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}; 

	/* Test the item */
	if (!item_tester_hook_activate(Ind, o_ptr))
	{
		msg_print(Ind, "You cannot activate that item.");
		return;
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Hack -- use artifact level instead */
	if (artifact_p(o_ptr)) lev = a_info[o_ptr->name1].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* High level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print(Ind, "You failed to activate it properly.");
		return;
	}

	/* Check the recharge */
	if (o_ptr->timeout)
	{
		msg_print(Ind, "It whines, glows and fades...");
		return;
	}


	/* Wonder Twin Powers... Activate! */
	msg_print(Ind, "You activate it...");


	/* Artifacts activate by name (except randarts!) */
	if (true_artifact_p(o_ptr))
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];
		char o_name[80];	

		/* Get the basic name of the object */
		object_desc(Ind, o_name, o_ptr, FALSE, 0);

		switch (a_ptr->activation)
		{
			case ACT_ILLUMINATION:
				{
					msg_format(Ind, "The %s wells with clear light...", o_name);
					lite_area(Ind, damroll(2, 15), 3);
					break;
				}

			case ACT_MAGIC_MAP:
				{
					msg_format(Ind, "The %s shines brightly...", o_name);
					map_area(Ind);
					break;
				}

			case ACT_CLAIRVOYANCE:
				{
					msg_format(Ind, "The %s glows a deep green...", o_name);
					/* (void)detection(Ind); */
					wiz_lite(Ind);
					(void)detect_sdoor(Ind);
					(void)detect_trap(Ind);
					break;
				}

			case ACT_PROT_EVIL:
				{
					msg_format(Ind, "The %s lets out a shrill wail...", o_name);
					k = 3 * p_ptr->lev;
					(void)set_protevil(Ind, p_ptr->protevil + randint(25) + k);
					break;
				}

			case ACT_DISP_EVIL:
				{
					msg_format(Ind, "The %s floods the area with goodness...", o_name);
					dispel_evil(Ind, p_ptr->lev * 5);
					break;
				}

			case ACT_HASTE2:
				{
					msg_format(Ind, "The %s glows brightly...", o_name);
					if (!p_ptr->fast)
					{
						(void)set_fast(Ind, randint(75) + 75);
					}
					else
					{
						(void)set_fast(Ind, p_ptr->fast + 5);
					}
					break;
				}

			case ACT_FIRE3:
				{
					msg_format(Ind, "The %s glows deep red...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FROST5:
				{
					msg_format(Ind, "The %s glows bright white...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_ELEC2:
				{
					msg_format(Ind, "The %s glows deep blue...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;			
				}

			case ACT_BIZZARE:
				{
					msg_format(Ind, "The %s glows intensely black...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_STAR_BALL:
				{
					msg_format(Ind, "Your %s is surrounded by lightning...", o_name);
					for (i = 0; i < 8; i++) fire_ball(Ind, GF_ELEC, ddd[i], 150, 3);
					break;
				}

			case ACT_RAGE_BLESS_RESIST:
				{
					msg_format(Ind, "Your %s glows many colours...", o_name);
					(void)hp_player(Ind, 30);
					(void)set_afraid(Ind, 0);
					(void)set_shero(Ind, p_ptr->shero + randint(50) + 50);
					(void)set_blessed(Ind, p_ptr->blessed + randint(50) + 50);
					(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(50) + 50);
					(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(50) + 50);
					(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(50) + 50);
					(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(50) + 50);
					(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(50) + 50);
					break;
				}

			case ACT_HEAL2:
				{
					msg_format(Ind, "Your %s glows a bright white...", o_name);
					msg_print(Ind, "You feel much better...");
					(void)hp_player(Ind, 1000);
					(void)set_cut(Ind, 0);
					break;
				}

			case ACT_PHASE:
				{
					msg_format(Ind, "Your %s twists space around you...", o_name);
					teleport_player(Ind, 10);
					break;
				}

			case ACT_BANISHMENT:
				{
					msg_format(Ind, "Your %s glows deep blue...", o_name);
					(void)banishment(Ind);
					break;
				}

			case ACT_TRAP_DOOR_DEST:
				{
					msg_format(Ind, "Your %s glows bright red...", o_name);
					destroy_doors_touch(Ind);
					break;
				}

			case ACT_DETECT:
				{
					msg_format(Ind, "Your %s glows bright white...", o_name);
					msg_print(Ind, "An image forms in your mind...");
					(void)detection(Ind);
					break;
				}

			case ACT_HEAL1:
				{
					msg_format(Ind, "Your %s glows deep blue...", o_name);
					msg_print(Ind, "You feel a warm tingling inside...");
					(void)hp_player(Ind, 500);
					(void)set_cut(Ind, 0);
					break;
				}

			case ACT_RESIST:
				{
					msg_format(Ind, "Your %s glows many colours...", o_name);
					(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
					(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
					(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
					(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
					(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
					break;
				}

			case ACT_SLEEP:
				{
					msg_format(Ind, "Your %s glows deep blue...", o_name);
					sleep_monsters_touch(Ind);
					break;
				}

			case ACT_RECHARGE1:
				{
					msg_format(Ind, "Your %s glows bright yellow...", o_name);
					/* msg_print(Ind, "You hear a low humming noise..."); */
					if (!recharge(Ind, 60)) return;
					break;
				}

			case ACT_TELEPORT:
				{
					msg_format(Ind, "Your %s twists space around you...", o_name);
					teleport_player(Ind, 100);
					break;
				}

			case ACT_RESTORE_LIFE:
				{
					msg_format(Ind, "Your %s glows a deep red...", o_name);
					restore_level(Ind);
					break;
				}

			case ACT_MISSILE:
				{
					msg_format(Ind, "Your %s glows extremely brightly...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FIRE1:
				{
					msg_format(Ind, "Your %s is covered in fire...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FROST1:
				{
					msg_format(Ind, "Your %s is covered in frost...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_LIGHTNING_BOLT:
				{
					msg_format(Ind, "Your %s is covered in sparks...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_ACID1:
				{
					msg_format(Ind, "Your %s is covered in acid...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_ARROW:
				{
					msg_format(Ind, "Your %s grows magical spikes...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_HASTE1:
				{
					msg_format(Ind, "Your %s glows bright green...", o_name);
					if (!p_ptr->fast)
					{
						(void)set_fast(Ind, randint(20) + 20);
					}
					else
					{
						(void)set_fast(Ind, p_ptr->fast + 5);
					}
					break;
				}

			case ACT_REM_FEAR_POIS:
				{
					msg_format(Ind, "Your %s glows deep blue...", o_name);
					(void)set_afraid(Ind, 0);
					(void)set_poisoned(Ind, 0);
					break;
				}

			case ACT_STINKING_CLOUD:
				{
					msg_format(Ind, "Your %s throbs deep green...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FROST2:
				{
					msg_format(Ind, "Your %s is covered in frost...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FROST4:
				{
					msg_format(Ind, "Your %s glows a pale blue...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FROST3:
				{
					msg_format(Ind, "Your %s glows a intense blue...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FIRE2:
				{
					msg_format(Ind, "Your %s rages in fire...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_DRAIN_LIFE2:
				{
					msg_format(Ind, "Your %s glows black...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_STONE_TO_MUD:
				{
					msg_format(Ind, "Your %s pulsates...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_MASS_BANISHMENT:
				{
					msg_format(Ind, "Your %s lets out a long, shrill note...", o_name);
					(void)mass_banishment(Ind);
					break;
				}

			case ACT_CURE_WOUNDS:
				{
					msg_format(Ind, "Your %s radiates deep purple...", o_name);
					hp_player(Ind, damroll(4, 8));
					(void)set_cut(Ind, (p_ptr->cut / 2) - 50);
					break;
				}

			case ACT_TELE_AWAY:
				{
					msg_format(Ind, "Your %s glows deep red...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_WOR:
				{
					msg_format(Ind, "Your %s glows soft white...", o_name);
					set_recall(Ind,o_ptr);
					break;
				}

			case ACT_CONFUSE:
				{
					msg_format(Ind, "Your %s glows in scintillating colours...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_IDENTIFY:
				{
					msg_format(Ind, "Your %s glows yellow...", o_name);
					(void)ident_spell(Ind);
					break;
				}

			case ACT_PROBE:
				{
					msg_format(Ind, "Your %s glows brightly...", o_name);
					probing(Ind);
					break;
				}

			case ACT_DRAIN_LIFE1:
				{
					msg_format(Ind, "Your %s glows white...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_FIREBRAND:
				{
					msg_format(Ind, "Your %s glows deep red...", o_name);
					(void)brand_bolts(Ind);
					break;
				}

			case ACT_STARLIGHT:
				{
					msg_format(Ind, "Your %s glows with the light of a thousand stars...", o_name);
					for (k = 0; k < 8; k++) strong_lite_line(Ind, ddd[k]);
					break;
				}

			case ACT_MANA_BOLT:
				{
					msg_format(Ind, "Your %s glows white...", o_name);
					p_ptr->current_activation = item;
					get_aim_dir(Ind);
					return;
				}

			case ACT_BERSERKER:
				{
					msg_format(Ind, "Your %s glows in anger...", o_name);
					set_shero(Ind, p_ptr->shero + randint(50) + 50);
					break;
				}
		}


		/* Set the recharge time */
		if (a_ptr->randtime)
			o_ptr->timeout = a_ptr->time + (byte)randint(a_ptr->randtime);
		else
			o_ptr->timeout = a_ptr->time;

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}

	/* Some ego items can be activated */
	else if (o_ptr->name2)
	{
		switch (o_ptr->name2)
		{
			case EGO_CLOAK_LORDLY_RES:
				{
					msg_print(Ind, "Your cloak flashes many colors...");

					(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(40) + 40);
					(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(40) + 40);
					(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(40) + 40);
					(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(40) + 40);
					(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(40) + 40);

					o_ptr->timeout = rand_int(50) + 150;
					break;
				}
		}
		/* Update equipment window */
		p_ptr->window |= PW_EQUIP;

		/* Done ego item activation */
		return;
	}

	/* Amulets of the moon can be activated for sleep monster */
	if ((o_ptr->tval == TV_AMULET) && (o_ptr->sval == SV_AMULET_THE_MOON))
	{
		msg_print(Ind, "Your amulet glows a deep blue...");
		sleep_monsters(Ind);

		o_ptr->timeout = rand_int(100) + 100;

		/* Update equipment window */
		p_ptr->window |= PW_EQUIP;

		return;
	}

	/* Hack -- some Rings can be activated for double resist and element ball */
	if (o_ptr->tval == TV_RING)
	{
		if ( (o_ptr->sval == SV_RING_FLAMES) || (o_ptr->sval == SV_RING_ACID) || (o_ptr->sval == SV_RING_ICE) || (o_ptr->sval == SV_RING_LIGHTNING) ) {
			p_ptr->current_activation = item;
			get_aim_dir(Ind);
			return;
		}
	}

	/* Hack -- Dragon Scale Mail can be activated as well */
	if (o_ptr->tval == TV_DRAG_ARMOR)
	{
		/* Get a direction for breathing (or abort) */
		p_ptr->current_activation = item;
		get_aim_dir(Ind);
		return;
	}


	/* Mistake */
	msg_print(Ind, "That object cannot be activated.");
}


void do_cmd_activate_dir(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	object_type *o_ptr;

	int item, chance;

	item = p_ptr->current_activation;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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

	if( check_guard_inscription( o_ptr->note, 'A' )) {
		msg_print(Ind, "The item's inscription prevents it");
		return;
	}; 

	/* Artifacts activate by name (except randarts!) */
	if (true_artifact_p(o_ptr))
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];
		char o_name[80];	

		/* Get the basic name of the object */
		object_desc(Ind, o_name, o_ptr, FALSE, 0);

		switch (a_ptr->activation)
		{
			case ACT_FIRE1:
				{
					fire_bolt(Ind, GF_FIRE, dir, damroll(9, 8));
					o_ptr->timeout = rand_int(8) + 8;
					break;
				}

			case ACT_FIRE2:
				{
					fire_ball(Ind, GF_FIRE, dir, 72, 3);
					break;
				}

			case ACT_FIRE3:
				{
					fire_ball(Ind, GF_FIRE, dir, 120, 3);
					o_ptr->timeout = rand_int(20) + 20;
					break;
				}

			case ACT_FROST1:
				{
					fire_bolt(Ind, GF_COLD, dir, damroll(6, 8));
					o_ptr->timeout = rand_int(7) + 7;
					break;
				}

			case ACT_FROST2:
				{
					fire_ball(Ind, GF_COLD, dir, 48, 2);
					break;
				}

			case ACT_FROST3:
				{
					fire_ball(Ind, GF_COLD, dir, 100, 2);
					o_ptr->timeout = 40;
					break;
				}

			case ACT_FROST4:
				{
					fire_bolt(Ind, GF_COLD, dir, damroll(12, 8));
					break;
				}

			case ACT_FROST5:
				{
					fire_ball(Ind, GF_COLD, dir, 200, 3);
					o_ptr->timeout = rand_int(20) + 20;
					break;
				}

			case ACT_ELEC2:
				{
					fire_ball(Ind, GF_ELEC, dir, 250, 3);
					o_ptr->timeout = rand_int(20) + 20;
					break;
				}

			case ACT_ACID1:
				{
					fire_bolt(Ind, GF_ACID, dir, damroll(5, 8));
					o_ptr->timeout = rand_int(5) + 5;
					break;
				}

			case ACT_BIZZARE:
				{
					ring_of_power(Ind, dir);
					o_ptr->timeout = rand_int(30) + 30;
					break;
				}

			case ACT_MANA_BOLT:
				{
					fire_bolt(Ind, GF_MANA, dir, damroll(12, 8));
					o_ptr->timeout = rand_int(30) + 30;
					break;
				}

			case ACT_MISSILE:
				{
					fire_bolt(Ind, GF_MISSILE, dir, damroll(2, 6));
					o_ptr->timeout = 2;
					break;
				}

			case ACT_ARROW:
				{
					fire_bolt(Ind, GF_ARROW, dir, 150);
					o_ptr->timeout = rand_int(30) + 30;
					break;
				}

			case ACT_LIGHTNING_BOLT:
				{
					fire_bolt(Ind, GF_ELEC, dir, damroll(4, 8));
					o_ptr->timeout = rand_int(6) + 6;
					break;
				}

			case ACT_STINKING_CLOUD:
				{
					fire_ball(Ind, GF_POIS, dir, 12, 3);
					o_ptr->timeout = rand_int(4) + 4;
					break;
				}

			case ACT_DRAIN_LIFE1:
				{
					drain_life(Ind, dir, 90);
					break;
				}

			case ACT_DRAIN_LIFE2:
				{
					drain_life(Ind, dir, 120);
					break;
				}

			case ACT_STONE_TO_MUD:
				{
					wall_to_mud(Ind, dir);
					break;
				}

			case ACT_TELE_AWAY:
				{
					teleport_monster(Ind, dir);
					break;
				}

			case ACT_CONFUSE:
				{
					confuse_monster(Ind, dir, 20);
					break;
				}
		}

		/* Clear activation */
		p_ptr->current_activation = -1;

		/* Set the recharge time */
		if (a_ptr->randtime)
			o_ptr->timeout = a_ptr->time + (byte)randint(a_ptr->randtime);
		else
			o_ptr->timeout = a_ptr->time;

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}

	/* Hack -- Dragon Scale Mail can be activated as well */
	if (o_ptr->tval == TV_DRAG_ARMOR)
	{
		switch (o_ptr->sval)
		{
			case SV_DRAGON_BLUE:
				{
					msg_print(Ind, "You breathe lightning.");
					fire_ball(Ind, GF_ELEC, dir, 100, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_WHITE:
				{
					msg_print(Ind, "You breathe frost.");
					fire_ball(Ind, GF_COLD, dir, 110, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_BLACK:
				{
					msg_print(Ind, "You breathe acid.");
					fire_ball(Ind, GF_ACID, dir, 130, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_GREEN:
				{
					msg_print(Ind, "You breathe poison gas.");
					fire_ball(Ind, GF_POIS, dir, 150, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_RED:
				{
					msg_print(Ind, "You breathe fire.");
					fire_ball(Ind, GF_FIRE, dir, 200, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_MULTIHUED:
				{
					chance = rand_int(5);
					msg_format(Ind, "You breathe %s.",
							((chance == 1) ? "lightning" :
							 ((chance == 2) ? "frost" :
								((chance == 3) ? "acid" :
								 ((chance == 4) ? "poison gas" : "fire")))));

					fire_ball(Ind, ((chance == 1) ? GF_ELEC :
								((chance == 2) ? GF_COLD :
								 ((chance == 3) ? GF_ACID :
									((chance == 4) ? GF_POIS : GF_FIRE)))),
							dir, 250, 2);
					o_ptr->timeout = rand_int(225) + 225;
					break;
				}

			case SV_DRAGON_BRONZE:
				{
					msg_print(Ind, "You breathe confusion.");
					fire_ball(Ind, GF_CONFUSION, dir, 120, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_GOLD:
				{
					msg_print(Ind, "You breathe sound.");
					fire_ball(Ind, GF_SOUND, dir, 130, 2);
					o_ptr->timeout = rand_int(450) + 450;
					break;
				}

			case SV_DRAGON_CHAOS:
				{
					chance = rand_int(2);
					msg_format(Ind, "You breathe %s.",
							((chance == 1 ? "chaos" : "disenchantment")));

					fire_ball(Ind, (chance == 1 ? GF_CHAOS : GF_DISENCHANT),
							dir, 220, 2);
					o_ptr->timeout = rand_int(300) + 300;
					break;
				}

			case SV_DRAGON_LAW:
				{
					chance = rand_int(2);
					msg_format(Ind, "You breathe %s.",
							((chance == 1 ? "sound" : "shards")));

					fire_ball(Ind, (chance == 1 ? GF_SOUND : GF_SHARDS),
							dir, 230, 2);
					o_ptr->timeout = rand_int(300) + 300;
					break;
				}

			case SV_DRAGON_BALANCE:
				{
					chance = rand_int(4);
					msg_format(Ind, "You breathe %s.",
							((chance == 1) ? "chaos" :
							 ((chance == 2) ? "disenchantment" :
								((chance == 3) ? "sound" : "shards"))));

					fire_ball(Ind, ((chance == 1) ? GF_CHAOS :
								((chance == 2) ? GF_DISENCHANT :
								 ((chance == 3) ? GF_SOUND : GF_SHARDS))),
							dir, 250, 2);
					o_ptr->timeout = rand_int(300) + 300;
					break;
				}

			case SV_DRAGON_SHINING:
				{
					chance = rand_int(2);
					msg_format(Ind, "You breathe %s.",
							((chance == 1 ? "light" : "darkness")));

					fire_ball(Ind, (chance == 1 ? GF_LITE : GF_DARK),
							dir, 200, 2);
					o_ptr->timeout = rand_int(300) + 300;
					break;
				}

			case SV_DRAGON_POWER:
				{
					msg_print(Ind, "You breathe the elements.");
					fire_ball(Ind, GF_MISSILE, dir, 300, 2);
					o_ptr->timeout = rand_int(300) + 300;
					break;
				}

			default:
				{
					msg_print(Ind, "SERVER ERROR: Directional activation called for non-directional activatee!");
					p_ptr->current_activation = -1;
					return;
				}
		}
	}

	/* Hack -- some Rings can be activated for double resist and element ball */
	if (o_ptr->tval == TV_RING)
	{
		//msg_print(Ind, "Your ring glows brightly..."); // this message is not in angband 3.0.6
		/* Branch on the sub-type */
		switch (o_ptr->sval)
		{
			case SV_RING_ACID:
				{
					fire_ball(Ind, GF_ACID, dir, 70, 2);
					set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
					o_ptr->timeout = rand_int(50) + 50;
					break;
				}

			case SV_RING_FLAMES:
				{
					fire_ball(Ind, GF_FIRE, dir, 80, 2);
					set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
					o_ptr->timeout = rand_int(50) + 50;
					break;
				}

			case SV_RING_ICE:
				{
					fire_ball(Ind, GF_COLD, dir, 75, 2);
					set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
					o_ptr->timeout = rand_int(50) + 50;
					break;
				}

			case SV_RING_LIGHTNING:
				{
					fire_ball(Ind, GF_ELEC, dir, 85, 2);
					set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
					o_ptr->timeout = rand_int(50) + 50;
					break;
				}
		}
	}

	/* Clear current activation */
	p_ptr->current_activation = -1;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Success */
	return;
}
