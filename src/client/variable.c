#include "angband.h"

/* Client global variables */

char nick[MAX_NAME_LEN];
char pass[80];

char real_name[80];

char server_name[80];
int server_port;

object_type inventory[INVEN_TOTAL];	/* The client-side copy of the inventory */
char inventory_name[INVEN_TOTAL][80];	/* The client-side copy of the inventory names */

store_type store;			/* The general info about the current store */
owner_type store_owner;			/* The current owner of the store */
char player_owner[MAX_NAME_LEN]; /* The store owner for player owned stores */
int store_prices[STORE_INVEN_MAX];	/* The prices of the items in the store */
char store_names[STORE_INVEN_MAX][80];	/* The names of the stuff in the store */
s16b store_num;				/* The current store number */
char floor_tval;		/* Currect tval on a floor */

char spell_info[26][9][80];		/* Spell information */

char party_info[160];			/* Information about your party */

server_setup_t Setup;				/* The information given to us by the server */
client_setup_t Client_setup;		/* The information we give to the server */

bool shopping;				/* Are we in a store? */

s16b last_line_info;			/* Last line of info we've received */
s16b max_line;				/* Maximum amount of "special" info */
s16b cur_line;				/* Current displayed line of "special" info */

s16b lag_minus;

player_type player;			/* The client-side copy of some of the player information */
player_type *p_ptr = &player;
player_type *Players = &player;

s32b exp_adv;				/* Amount of experience required to advance a level */

s16b command_see;
s16b command_gap;
s16b command_wrk;

bool item_tester_full;
byte item_tester_tval;
bool (*item_tester_hook)(object_type *o_ptr);

int special_line_type;
char special_line_header[80];

bool inkey_base = FALSE;
bool inkey_scan = FALSE;
bool inkey_flag = FALSE;

bool first_escape = FALSE;

s16b macro__num;
cptr *macro__pat;
cptr *macro__act;
bool *macro__cmd;
char *macro__buf;

u16b message__next;
u16b message__last;
u16b message__head;
u16b message__tail;
u16b *message__ptr;
char *message__buf;


bool msg_flag;


term *ang_term[8];
u32b window_flag[8];

byte color_table[256][4];

cptr ANGBAND_SYS;

byte keymap_cmds[128];
byte keymap_dirs[128];

s16b command_cmd;
s16b command_dir;


s16b race;
s16b p_class;
s16b sex;

s16b stat_order[6];			/* Desired order of stats */


bool topline_icky;
bool screen_icky;
bool party_mode;
bool cursor_icky;

/*
 * The player race arrays
 */

cptr race_title[] = {
	"Human", "Half-elf", "Elf", "Hobbit",
	"Gnome", "Dwarf", "Half-Orc", "Half-Troll",
    "Dunadan", "High-elf"
	};

cptr class_title[] = {
	"Warrior", "Mage", "Priest", "Rogue",
    "Ranger", "Paladin"
	};

cptr ANGBAND_DIR;
cptr ANGBAND_DIR_APEX;
cptr ANGBAND_DIR_BONE;
cptr ANGBAND_DIR_DATA;
cptr ANGBAND_DIR_EDIT;
cptr ANGBAND_DIR_FILE;
cptr ANGBAND_DIR_HELP;
cptr ANGBAND_DIR_INFO;
cptr ANGBAND_DIR_SAVE;
cptr ANGBAND_DIR_USER;
cptr ANGBAND_DIR_XTRA;


bool use_graphics;
bool use_sound;

int char_screen_mode;
bool target_position;
