/* Client-side spell stuff */

#include "angband.h"

static void print_spells(int book)
{
	int	i, col;

	/* Print column */
	col = 20;

	/* Title the list */
	prt("", 1, col);
	put_str("Name", 1, col + 5);
	put_str("Lv Mana Fail", 1, col + 35);

	/* Dump the spells */
	for (i = 0; i < 9; i++)
	{
	/* Clear line */
	prt("", 2 + i, col);

		/* Check for end of the book */
		if (spell_info[book][i][0] == '\0')
			break;

		/* Dump the info */
		prt(spell_info[book][i], 2 + i, col);
	}

	/* Clear the bottom line */
	prt("", 2 + i, col);
}

/*
 * Allow user to choose a spell/prayer from the given book.
 */

/* modified to accept certain capital letters for priest spells. -AD- */ 
 
static int get_spell(int *sn, cptr prompt, int book)
{
	int		i, num = 0;
	bool		flag, redraw;
	char		choice;
	char		out_val[160];
	cptr p;
	
	p = ((p_class == CLASS_PRIEST || p_class == CLASS_PALADIN) ? "prayer" : "spell");

	if (p_ptr->ghost)
		p = "power";

	/* Assume no spells available */
	(*sn) = -2;

	/* Check for available spells */
	for (i = 0; i < 9; i++)
	{
		/* Check for end of the book */
		if (spell_info[book][i][0] == '\0') break;

        /* Spell is available */
        num++;
	}

	/* No "okay" spells */
	if (!num) return (FALSE);

	/* Assume cancelled */
	(*sn) = -1;

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt (accept all spells) */
	strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
		p, I2A(0), I2A(num - 1), prompt, p);

	/* Get a spell from the user */
	while (!flag && get_com(out_val, &choice))
	{
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Show the list */
			if (!redraw)
			{
				/* Show list */
				redraw = TRUE;

				/* The screen is icky */
				screen_icky = TRUE;

				/* Save the screen */
				Term_save();

				/* Display a list of spells */
				print_spells(book);
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				Term_load();

				/* The screen is OK now */
				screen_icky = FALSE;

				/* Flush any events */
				Flush_queue();
			}

			/* Ask again */
			continue;
		}
		
		/* hack for CAPITAL prayers (heal other) */
        if ((p_class == CLASS_PRIEST) || (p_class == CLASS_PALADIN) )
		{
			/* lowercase */
			if (islower(choice))
			{
				i = A2I(choice);
				if (i >= num) i = -1;
			}
				
			/* uppercase... hope this is portable. */
			else if (isupper(choice)) 
			{
				i = (choice - 'A') + 64;
				if (i-64 >= num) i = -1;
			}	
			else i = -1;			
		}
		else
		{
			/* extract request */
			i = (islower(choice) ? A2I(choice) : -1);
			if (i >= num) i = -1;
		}

		/* Totally Illegal */
		if (i < 0)
		{
			bell();
			continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw)
	{
		Term_load();
		screen_icky = FALSE;

		/* Flush any events */
		Flush_queue();
	}


	/* Abort if needed */
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = i;

	/* Success */
	return (TRUE);
}


/*
 * Peruse the spells/prayers in a Book
 *
 * Note that *all* spells in the book are listed
 */
void show_browse(int book)
{
	/* The screen is icky */
	screen_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Display the spells */
	print_spells(book);
	
	/* Clear the top line */
	prt("", 0, 0);

	/* Prompt user */
	put_str("[Press any key to continue]", 0, 23);

	/* Wait for key */
	(void)inkey();

	/* Restore the screen */
	Term_load();

	/* Screen is OK now */
	screen_icky = FALSE;

	/* Flush any events */
	Flush_queue();
}

/*
 * Study a book to gain a new spell/prayer
 */
void do_study(int book)
{
	int j;
	cptr p = ((p_class == CLASS_PRIEST || p_class == CLASS_PALADIN) ? "prayer" : "spell");

	/* Mage -- Learn a selected spell */
	if (!strcmp(p, "spell"))
	{
		/* Ask for a spell, allow cancel */
		if (!get_spell(&j, "study", book)) return;
	}

	/* Priest -- Learn random spell */
	else j = -1;

	/* Tell the server */
	/* Note that if we are a priest, the server ignores the spell parameter */
	Send_gain(book, j);
}

/*
 * Cast a spell
 */
void do_cast(int book)
{
	int j;

	/* Ask for a spell, allow cancel */
	if (!get_spell(&j, "cast", book)) return;

	/* Tell the server */
	Send_cast(book, j);
}

/*
 * Pray a spell
 */
void do_pray(int book)
{
	int j;

	/* Ask for a spell, allow cancel */
	if (!get_spell(&j, "pray", book)) return;

	/* Tell the server */
	Send_pray(book, j);
}

/*
 * Use a ghost ability
 */
void do_ghost(void)
{
	int j;

	/* Ask for an ability, allow cancel */
	if (!get_spell(&j, "use", 0)) return;

	/* Tell the server */
	Send_ghost(j);
}
