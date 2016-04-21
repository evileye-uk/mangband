#include "angband.h"

s16b index_to_label(int i)
{
	/* Indices for "inven" are easy */
	if (i < INVEN_WIELD) return (I2A(i));

	/* Indices for "equip" are offset */
	return (I2A(i - INVEN_WIELD));
}


bool item_tester_okay(object_type *o_ptr)
{
	/* Hack -- allow testing empty slots */
	if (item_tester_full) return (TRUE);

	/* Require an item */
	if (!o_ptr->tval) return (FALSE);

	/* Hack -- ignore "gold" */
	if (o_ptr->tval == TV_GOLD) return (FALSE);

	/* Check the tval */
	if (item_tester_tval)
	{
		if (item_tester_tval != o_ptr->tval) return (FALSE);
	}

	/* Check the hook */
	if (item_tester_hook)
	{
		if (!(*item_tester_hook)(o_ptr)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}


static bool get_item_okay(int i)
{
	/* Illegal items */
	if ((i < 0) || (i >= INVEN_TOTAL)) return (FALSE);

	/* Verify the item */
	if (!item_tester_okay(&inventory[i])) return (FALSE);

	/* Assume okay */
	return (TRUE);
}


static bool verify(cptr prompt, int item)
{
	char	o_name[80];

	char	out_val[160];


	/* Describe */
	strcpy(o_name, inventory_name[item]);

	/* Prompt */
	(void)sprintf(out_val, "%s %s? ", prompt, o_name);

	/* Query */
	return (get_check(out_val));
}


s16b c_label_to_inven(int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1);

	/* Verify the index */
	if ((i < 0) || (i > INVEN_PACK)) return (-1);

	/* Empty slots can never be chosen */
	if (!inventory[i].tval) return (-1);

	/* Return the index */
	return (i);
}

s16b c_label_to_equip(int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1) + INVEN_WIELD;

	/* Verify the index */
	if ((i < INVEN_WIELD) || (i >= INVEN_TOTAL)) return (-1);

	/* Empty slots can never be chosen */
	if (!inventory[i].tval) return (-1);

	/* Return the index */
	return (i);
}


/*
 * Find the "first" inventory object with the given "tag".
 *
 * A "tag" is a char "n" appearing as "@n" anywhere in the
 * inscription of an object.
 *
 * Also, the tag "@xn" will work as well, where "n" is a tag-char,
 * and "x" is the "current" command_cmd code.
 */
static int get_tag(int *cp, char tag)
{
        int i;
        cptr s;


        /* Check every object */
        for (i = 0; i < INVEN_TOTAL; ++i)
        {
                char *buf = inventory_name[i];
		char *buf2;

                /* Skip empty objects */
                if (!buf[0]) continue;

                /* Skip empty inscriptions */
                if (!(buf2 = strchr(buf, '{'))) continue;

                /* Find a '@' */
                s = strchr(buf2, '@');

                /* Process all tags */
                while (s)
                {
                        /* Check the normal tags */
                        if (s[1] == tag)
                        {
                                /* Save the actual inventory ID */
                                *cp = i;

                                /* Success */
                                return (TRUE);
                        }

                        /* Check the special tags */
                        if ((s[1] == command_cmd) && (s[2] == tag))
                        {
                                /* Save the actual inventory ID */
                                *cp = i;

                                /* Success */
                                return (TRUE);
                        }

                        /* Find another '@' */
                        s = strchr(s + 1, '@');
                }
        }

        /* No such tag */
        return (FALSE);
}

bool c_get_spike()
{
	int i;
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->number) continue;
		
		/* Check the "tval" code */
		if (o_ptr->tval == TV_SPIKE)
		{
			/* (*item) = i; */
			return TRUE;
		}
	}	
	
	/* Oops */
	return FALSE;
}
/* 
	Every instance of c_get_item in the code specifies 'floor' as FALSE,
	so inside this function, we're always treating it as NOT(floor), since
	we DO need floor checks.
*/
bool c_get_item(int *cp, cptr pmt, bool equip, bool inven, bool floor)
{
	char which;

	int	k, i1, i2, e1, e2;
	bool	ver, done, item;
	
	bool allow_floor = FALSE;

	char	tmp_val[160];
	char	out_val[160];

	/* The top line is icky */
	topline_icky = TRUE;

	/* Not done */	
	done = FALSE;

	/* No item selected */
	item = FALSE;

	/* Default to "no item" */
	*cp = -1;

	/* Paranoia */
	if (!inven && !equip) return (FALSE);

	/* Full inventory */
	i1 = 0;
	i2 = INVEN_PACK - 1;

	/* Forbid inventory */
	if (!inven) i2 = -1;

	/* Restrict inventory indices */
	while ((i1 <= i2) && (!get_item_okay(i1))) i1++;
	while ((i1 <= i2) && (!get_item_okay(i2))) i2--;


	/* Full equipment */
	e1 = INVEN_WIELD;
	e2 = INVEN_TOTAL - 1;

	/* Restrict equipment indices */
	while ((e1 <= e2) && (!get_item_okay(e1))) e1++;
	while ((e1 <= e2) && (!get_item_okay(e2))) e2--;


	/* Check floor thing */
	if (floor_tval && !floor) {
		if( (!item_tester_tval) || (item_tester_tval && floor_tval == item_tester_tval)) allow_floor = TRUE;
	}
	
	if ((i1 > i2) && (e1 > e2) && !allow_floor)
	{
		/* Cancel command_see */
		command_see = FALSE;

		/* Hack -- Nothing to choose */	
		*cp = -2;

		/* Done */
		done = TRUE;
	}

	/* Analyze choices */
	else
	{
		/* Hack -- reset display width */
		if (!command_see) command_gap = 50;

		/* Hack -- Start on equipment if requested */
		if (command_see && command_wrk && equip)
		{
			command_wrk = TRUE;
		}

		/* Use inventory if allowed */
		else if (inven)
		{
			command_wrk = FALSE;
		}

		/* Use equipment if allowed */
		else if (equip)
		{
			command_wrk = TRUE;
		}
	}

	/* Hack -- start out in "display" mode */
	if (command_see) 
	{
		screen_icky = TRUE;
		Term_save();
	}

	
	/* Repeat while done */
	while (!done)
	{
		if (!command_wrk)
		{
			/* Redraw if needed */
			if (command_see) show_inven();
		}

		/* Equipment screen */
		else
		{
			/* Redraw if needed */
			if (command_see) show_equip();
		}

		/* Viewing inventory */
		if (!command_wrk)
		{
			/* Begin the prompt */	
			sprintf(out_val, "Inven:");

			/* Some legal items */
			if (i1 <= i2)
			{
				/* Build the prompt */
				sprintf(tmp_val, " %c-%c,",
					index_to_label(i1), index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
			if (!command_see) strcat(out_val, " * to see,");

			/* Append */
			if (equip) strcat(out_val, " / for Equip,");
		}

		/* Viewing equipment */
		else
		{
			/* Begin the prompt */
			sprintf(out_val, "Equip:");

			/* Some legal items */
			if (e1 <= e2)
			{
				/* Build the prompt */
				sprintf(tmp_val, " %c-%c",
					index_to_label(e1), index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate the ability to "view" */
			if (!command_see) strcat(out_val, " * to see,");

			/* Append */	
			if (inven) strcat(out_val, " / for Inven,");
		}

		/* Append floor if possible */
		if (allow_floor) strcat(out_val, " - for floor,");

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);


		/* Get a key */
		which = inkey();

		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				command_gap = 50;
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Show/hide the list */
				if (!command_see)
				{
					screen_icky = TRUE;

					Term_save();
					command_see = TRUE;
				}
				else
				{
					Term_load();
					command_see = FALSE;

					screen_icky = FALSE;

					/* Flush any events */
					Flush_queue();
				}
				break;
			}
			case '-':
				/* Floor */
				
				(*cp) = -1;
				item = TRUE;
				done = TRUE;
				
				break;
			case '/':
			{
				/* Verify legality */
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Fix screen */
				if (command_see)
				{
					Term_load();
					Flush_queue();
					Term_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;

				/* Need to redraw */
				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				/* XXX XXX Look up that tag */
				if (!get_tag(&k, which))
				{
					bell();
					break;
				}

				/* Hack -- verify item */
				if ((k < INVEN_WIELD) ? !inven : !equip)
				{
					bell();
					break;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

#if 0
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}
#endif

				/* Use that item */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}

			case '\n':
			case '\r':
			{
				/* Choose "default" inventory item */
				if (!command_wrk)
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

#if 0
				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}
#endif

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}

			default:
			{
				/* Extract "query" setting */
				ver = isupper(which);
				if (ver) which = tolower(which);

				/* Convert letter to inventory index */
				if (!command_wrk)
				{
					k = c_label_to_inven(which);
				}

				/* Convert letter to equipment index */
				else
				{
					k = c_label_to_equip(which);
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Verify, abort if requested */
				if (ver && !verify("Try", k))
				{
					done = TRUE;
					break;
				}

#if 0
				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}
#endif

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
	}


	/* Fix the screen if necessary */
	if (command_see) 
	{
		Term_load();
		screen_icky = FALSE;
	}

	/* Fix the top line */
	topline_icky = FALSE;

	/* Flush any events */
	Flush_queue();

	/* Hack -- Cancel "display" */
	command_see = FALSE;


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	/* Forget the item_tester_hook restriction */
	item_tester_hook = 0;


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Return TRUE if something was picked */
	return (item);
}
