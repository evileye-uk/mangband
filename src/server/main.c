/* File: main.c */

/* Purpose: initialization, main() function and main loop */

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
 * Some machines have a "main()" function in their "main-xxx.c" file,
 * all the others use this file for their "main()" function.
 */


#if !defined(MACINTOSH) && !defined(ACORN)

#ifdef SET_UID

/*
 * Check "wizard permissions"
 */
static bool is_wizard(int uid)
{
	FILE	*fp;

	bool	allow = FALSE;

	char	buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_TEXT, "wizards.txt");

	/* Open the wizard file */
	fp = my_fopen(buf, "r");

	/* No file, allow everyone */
	if (!fp) return (TRUE);

	/* Scan the wizard file */
	while (0 == my_fgets(fp, buf, 1024))
	{
		int test;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Look for valid entries */
		if (sscanf(buf, "%d", &test) != 1) continue;

		/* Look for matching entries */
		if (test == uid) allow = TRUE;

		/* Done */
		if (allow) break;
	}

	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (allow);
}

#endif


/*
 * A hook for "quit()".
 *
 * Close down, then fall back into "quit()".
 *
 * Unnecessary, as the server doesn't open any "terms".  --KLJ--
 */
static void quit_hook(cptr s)
{
#ifdef UNIX_SOCKETS
	SocketCloseAll();
#endif
#if 0
	int j;

	/* Scan windows */
	for (j = 8 - 1; j >= 0; j--)
	{
		/* Unused */
		if (!ang_term[j]) continue;

		/* Nuke it */
		term_nuke(ang_term[j]);
	}
#endif
}



/*
 * Set the stack size (for the Amiga)
 */
#ifdef AMIGA
# include <dos.h>
__near long __stack = 32768L;
#endif


/*
 * Set the stack size and overlay buffer (see main-286.c")
 */
#ifdef USE_286
# include <dos.h>
extern unsigned _stklen = 32768U;
extern unsigned _ovrbuffer = 0x1500;
#endif

/*
 * Initialize and verify the file paths, and the score file.
 *
 * Use the ANGBAND_PATH environment var if possible, else use
 * DEFAULT_PATH, and in either case, branch off appropriately.
 *
 * First, we'll look for the ANGBAND_PATH environment variable,
 * and then look for the files in there.  If that doesn't work,
 * we'll try the DEFAULT_PATH constant.  So be sure that one of
 * these two things works...
 *
 * We must ensure that the path ends with "PATH_SEP" if needed,
 * since the "init_file_paths()" function will simply append the
 * relevant "sub-directory names" to the given path.
 *
 * Note that the "path" must be "Angband:" for the Amiga, and it
 * is ignored for "VM/ESA", so I just combined the two.
 */
static void init_stuff(void)
{
	char path[1024];

#if defined(AMIGA) || defined(VM)

	/* Hack -- prepare "path" */
	strcpy(path, "Angband:");

#else /* AMIGA / VM */

	cptr tail;

	/* Get the environment variable */
	tail = getenv("ANGBAND_PATH");

	/* Use the angband_path, or a default */
	strcpy(path, tail ? tail : DEFAULT_PATH);

	/* Hack -- Add a path separator (only if needed) */
	if (!suffix(path, PATH_SEP)) strcat(path, PATH_SEP);

#endif /* AMIGA / VM */

	/* Initialize */
	init_file_paths(path);
}

/*
 * Server logging hook.
 * We should be cautious, as we may be called from a signal handler in a panic.
 */
static void server_log(cptr str)
{
	char buf[16];
	time_t t;
	struct tm *local;
	
	/* Grab the time */
	time(&t);
	local = localtime(&t);
	strftime(buf, 16, "%d%m%y %H%M%S", local);
	
	/* Output the message timestamped */
	fprintf(stderr,"%s %s\n", buf, str);
}

/*
 * Some machines can actually parse command line args
 *
 * XXX XXX XXX The "-c", "-d", and "-i" options should probably require
 * that their "arguments" do NOT end in any path separator.
 *
 * The "path" options should probably be simplified into some form of
 * "-dWHAT=PATH" syntax for simplicity.
 */
int main(int argc, char *argv[])
{
	bool new_game = FALSE;
	int catch_signals = TRUE;
#ifdef WINDOWS
  WSADATA wsadata;
#endif

	/* Setup our logging hook */
	plog_aux = server_log;	

	/* Note we are starting up */
	plog("Game Restarted");

	/* Save the "program name" */
	argv0 = argv[0];

#ifdef WINDOWS
	/* Load our debugging library on Windows, to give us nice stack dumps */
	/* We use exchndl.dll from the mingw-utils package */
	LoadLibrary("exchndl.dll");
	
	/* Initialise WinSock */
	/* Initialize WinSock */
	WSAStartup(MAKEWORD(1, 1), &wsadata);	
#endif

#ifdef USE_286
	/* Attempt to use XMS (or EMS) memory for swap space */
	if (_OvrInitExt(0L, 0L))
	{
		_OvrInitEms(0, 0, 64);
	}
#endif


#ifdef SET_UID

	/* Default permissions on files */
	(void)umask(022);

# ifdef SECURE
	/* Authenticate */
	Authenticate();
# endif

#endif


	/* Get the file paths */
	init_stuff();

#ifdef SET_UID

	/* Get the user id (?) */
	player_uid = getuid();

#ifdef VMS
	/* Mega-Hack -- Factor group id */
	player_uid += (getgid() * 1000);
#endif

# ifdef SAFE_SETUID

#  ifdef _POSIX_SAVED_IDS

	/* Save some info for later */
	player_euid = geteuid();
	player_egid = getegid();

#  endif

#  if 0	/* XXX XXX XXX */

	/* Redundant setting necessary in case root is running the game */
	/* If not root or game not setuid the following two calls do nothing */

	if (setgid(getegid()) != 0)
	{
		quit("setgid(): cannot set permissions correctly!");
	}

	if (setuid(geteuid()) != 0)
	{
		quit("setuid(): cannot set permissions correctly!");
	}

#  endif

# endif

#endif


	/* Assume "Wizard" permission */
	can_be_wizard = TRUE;

#ifdef SET_UID

	/* Check for "Wizard" permission */
	can_be_wizard = is_wizard(player_uid);

	/* Initialize the "time" checker */
	if (check_time_init() || check_time())
	{
		quit("The gates to Angband are closed (bad time).");
	}

	/* Initialize the "load" checker */
	if (check_load_init() || check_load())
	{
		quit("The gates to Angband are closed (bad load).");
	}

#if 0
	/* Acquire the "user name" as a default player name */
	user_name(player_name, player_uid);
#endif

#endif


	/* Process the command line arguments */
	for (--argc, ++argv; argc > 0; --argc, ++argv)
	{
		/* Require proper options */
		if (argv[0][0] != '-') goto usage;

		/* Analyze option */
		switch (argv[0][1])
		{
			case 'c':
			case 'C':
			ANGBAND_DIR_USER = &argv[0][2];
			break;

#ifndef VERIFY_SAVEFILE
			case 'd':
			case 'D':
			ANGBAND_DIR_SAVE = &argv[0][2];
			break;
#endif

			case 'i':
			case 'I':
			ANGBAND_DIR_TEXT = &argv[0][2];
			break;

			case 'r':
			case 'R':
			new_game = TRUE;
			break;

			case 'F':
			case 'f':
			arg_fiddle = TRUE;
			break;

#ifdef SET_UID
#if 0
			case 'P':
			case 'p':
			if (can_be_wizard)
			{
				player_uid = atoi(&argv[0][2]);
				user_name(player_name, player_uid);
			}
			break;
#endif
#endif

			case 'W':
			case 'w':
			if (can_be_wizard) arg_wizard = TRUE;
			break;

			case 'Z':
			case 'z':
			catch_signals = FALSE;
			break;

#if 0
			case 'u':
			case 'U':
			if (!argv[0][2]) goto usage;
			strcpy(player_name, &argv[0][2]);
			break;
#endif

			default:
			usage:

			/* Note -- the Term is NOT initialized */
			puts("Usage: mangband [options]");
			puts("  -r	 Reset the server");
			puts("  -f       Activate 'fiddle' mode");
			puts("  -w       Activate 'wizard' mode");
			puts("  -z       Don't catch signals");
			puts("  -p<uid>  Play with the <uid> userid");
			puts("  -c<path> Look for pref files in the directory <path>");
			puts("  -d<path> Look for save files in the directory <path>");
			puts("  -i<path> Look for info files in the directory <path>");

			/* Actually abort the process */
			quit(NULL);
		}
	}

#if 0

	/* Process the player name */
	process_player_name(TRUE);
#endif

/* This is all removed, as there is no need for the server to open a term */
#if 0

	/* Drop privs (so X11 will work correctly) */
	safe_setuid_drop();


#ifdef USE_XAW
	/* Attempt to use the "main-xaw.c" support */
	if (!done)
	{
		extern errr init_xaw(void);
		if (0 == init_xaw()) done = TRUE;
		if (done) ANGBAND_SYS = "xaw";
	}
#endif

#ifdef USE_X11
	/* Attempt to use the "main-x11.c" support */
	if (!done)
	{
		extern errr init_x11(void);
		if (0 == init_x11()) done = TRUE;
		if (done) ANGBAND_SYS = "x11";
	}
#endif


#ifdef USE_GCU
	/* Attempt to use the "main-gcu.c" support */
	if (!done)
	{
		extern errr init_gcu(void);
		if (0 == init_gcu()) done = TRUE;
		if (done) ANGBAND_SYS = "gcu";
	}
#endif

#ifdef USE_CAP
	/* Attempt to use the "main-cap.c" support */
	if (!done)
	{
		extern errr init_cap(void);
		if (0 == init_cap()) done = TRUE;
		if (done) ANGBAND_SYS = "cap";
	}
#endif


#ifdef USE_IBM
	/* Attempt to use the "main-ibm.c" support */
	if (!done)
	{
		extern errr init_ibm(void);
		if (0 == init_ibm()) done = TRUE;
		if (done) ANGBAND_SYS = "ibm";
	}
#endif

#ifdef USE_EMX
	/* Attempt to use the "main-emx.c" support */
	if (!done)
	{
		extern errr init_emx(void);
		if (0 == init_emx()) done = TRUE;
		if (done) ANGBAND_SYS = "emx";
	}
#endif


#ifdef USE_SLA
	/* Attempt to use the "main-sla.c" support */
	if (!done)
	{
		extern errr init_sla(void);
		if (0 == init_sla()) done = TRUE;
		if (done) ANGBAND_SYS = "sla";
	}
#endif


#ifdef USE_LSL
	/* Attempt to use the "main-lsl.c" support */
	if (!done)
	{
		extern errr init_lsl(void);
		if (0 == init_lsl()) done = TRUE;
		if (done) ANGBAND_SYS = "lsl";
	}
#endif


#ifdef USE_AMI
	/* Attempt to use the "main-ami.c" support */
	if (!done)
	{
		extern errr init_ami(void);
		if (0 == init_ami()) done = TRUE;
		if (done) ANGBAND_SYS = "ami";
	}
#endif


#ifdef USE_VME
	/* Attempt to use the "main-vme.c" support */
	if (!done)
	{
		extern errr init_vme(void);
		if (0 == init_vme()) done = TRUE;
		if (done) ANGBAND_SYS = "vme";
	}
#endif


	/* Grab privs (dropped above for X11) */
	safe_setuid_grab();


#endif

#if 0
	/* Make sure we have a display! */
	if (!done) quit("Unable to prepare any 'display module'!");
#endif

	/* Tell "quit()" to call "Term_nuke()" */
	quit_aux = quit_hook;


	/* Catch nasty signals */
	if (catch_signals == TRUE)
		signals_init();

	/* Catch nasty "signals" on Windows */
#ifdef WINDOWS
#ifndef HANDLE_SIGNALS
	setup_exit_handler();
#endif
#endif

	/* Load the mangband.cfg options */
	load_server_cfg();

	/* Initialize the arrays */
	init_some_arrays();

	/* Wait for response */
	/*pause_line(23);*/

	/* Play the game */
	play_game(new_game);

	/* Quit */
	quit(NULL);

	/* Exit */
	return (0);
}

#endif
