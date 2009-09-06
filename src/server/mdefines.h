/* File: defines.h */

/* Purpose: server-side global constants and macro definitions */


#define	OPT_MAX 			40
#define	MAX_OPTION_GROUPS   4

/*
 * Available Options
 *
 * It is very important that all options stay together
 * without any gaps, OPT_MAX must not exceed them too.
 * Order, on the other hand, is not important.
 *
 *  *** WRONG ***			*** CORRECT ***
 * OPT_MAX       64			OPT_MAX       2
 * OPT_NO_GHOST  5          OPT_NO_GHOST  0
 * OPT_SUN_LIGHT 9          OPT_SUN_LIGHT 1
 */
#define OPT_NO_GHOST    		0
#define OPT_DEPTH_IN_FEET   	1
#define OPT_DUNGEON_ALIGN   	2
#define OPT_AVOID_OTHER 		3
#define OPT_USE_COLOR   		4
#define OPT_AUTO_SCUM       	5
#define OPT_EXPAND_LOOK 		6
#define OPT_SHOW_DETAILS    	7
#define OPT_CARRY_QUERY_FLAG	8
#define OPT_ALWAYS_PICKUP   	9
#define OPT_USE_OLD_TARGET  	10
#define OPT_STACK_FORCE_NOTES	11
#define OPT_STACK_FORCE_COSTS	12
#define OPT_STACK_ALLOW_ITEMS	13
#define OPT_STACK_ALLOW_WANDS	14
#define OPT_FIND_IGNORE_STAIRS	15
#define OPT_FIND_IGNORE_DOORS	16
#define OPT_FIND_CUT        	17
#define OPT_FIND_EXAMINE    	18
#define OPT_DISTURB_MOVE    	19
#define OPT_DISTURB_NEAR    	20
#define OPT_DISTURB_PANEL   	21
#define OPT_DISTURB_STATE   	22
#define OPT_DISTURB_MINOR   	23
#define OPT_DISTURB_OTHER   	24
#define OPT_VIEW_PERMA_GRIDS	25 
#define OPT_VIEW_TORCH_GRIDS	26 
#define OPT_VIEW_REDUCE_LITE	27
#define OPT_VIEW_REDUCE_VIEW	28
#define OPT_VIEW_YELLOW_LITE	29
#define OPT_VIEW_BRIGHT_LITE	30
#define OPT_VIEW_GRANITE_LITE	31
#define OPT_VIEW_SPECIAL_LITE	32
#define OPT_VIEW_ORANGE_LITE	33
#define OPT_EASY_ALTER			34
#define OPT_ALERT_HITPOINT		35
#define OPT_HILITE_LEADER   	36
#define OPT_PAUSE_AFTER_DETECT 	37
#define OPT_DISTURB_LOOK    	38
#define OPT_UNSETH_BONUS    	39
#define option_p(A,B) (A->options[OPT_ ## B])


/*
 * item_tester_hook defines 
 */
#define ITH_WEAR        	1
#define ITH_WEAPON      	2
#define ITH_ARMOR       	3
#define ITH_AMMO        	4
#define ITH_RECHARGE       	5
#define ITH_ACTIVATE    	6
#define ITH_REFILL			7
#define item_test(A) (TV_MAX + (ITH_ ## A))
#define ITEM_ANY        	0

/*
 * item_tester_flag defines 
 */
#define ITF_WEAR	0x01 /* Can wear/wield item */
#define ITF_ACT 	0x02 /* Can activate item */
#define ITF_FUEL 	0x04 /* Can be used as fuel */
#define ITF_FLAG_0 	0x08 

#define ITF_FLAG_1 	ITEM_ASK_AIM  /* Reserved */ 
#define ITF_FLAG_2 	ITEM_ASK_ITEM /* Reserved */
#define ITF_FLAG_3 	0x40 /* Variant-specific */
#define ITF_FLAG_4 	0x80 /* Variant-specific */

/*
 * Chat Channels defines 
 */
/* channel modes */
#define	CM_KEYLOCK	0x01 /* +k More similar to IRC's +A */
#define	CM_SECRET	0x02 /* +s */
#define	CM_MODERATE 0x04 /* +m */
#define	CM_SERVICE 	0x08 /* +! service channel */
#define	CM_PLOG 	0x10 /* +p log to plog */
/* user-channel modes */
#define	UCM_EAR  	0x01
#define	UCM_VOICE	0x02
#define	UCM_OPER 	0x04
#define	UCM_BAN  	0x08

#define UCM_LEAVE 	(UCM_EAR | UCM_VOICE | UCM_OPER)
#define on_channel(P,I) ((P)->on_channel[(I)] & UCM_EAR)

/* can_talk(p_ptr, channel_index) test:
 * Line 1. Present on channel
 * Line 2. Not banned
 * Line 3. Not moderated
 * Line 4. Moderated, BUT
 * Line 5.  user has Voice or Op */
#define can_talk(P,I) \
	(on_channel((P),(I)) && \
	!((P)->on_channel[(I)] & UCM_BAN) && \
	(!(channels[(I)].mode & CM_MODERATE) || ( \
	(channels[(I)].mode & CM_MODERATE) && \
		( (P)->on_channel[(I)] & UCM_VOICE || (P)->on_channel[(I)] & UCM_OPER ) \
	)))

#define clog(C,M) if (chan_ ## C) msg_channel(chan_ ## C, (M))
#define audit(M) clog(audit, (M))
#define debug(M) clog(debug, (M))
#define cheat(M) clog(cheat, (M))
/*
 * The types of special file perusal.
 */
#define SPECIAL_FILE_NONE	0
#define SPECIAL_FILE_OTHER	1
#define SPECIAL_FILE_ARTIFACT	2
#define SPECIAL_FILE_PLAYER	3
#define SPECIAL_FILE_UNIQUE	4
#define SPECIAL_FILE_SCORES	5
#define SPECIAL_FILE_HELP	6
#define SPECIAL_FILE_KNOWLEDGE	7
#define SPECIAL_FILE_HOUSES	8
#define SPECIAL_FILE_OBJECT	9
#define SPECIAL_FILE_KILL	10
#define SPECIAL_FILE_HISTORY	11
#define SPECIAL_FILE_SELF	12
#define SPECIAL_FILE_MASTER	13
#define SPECIAL_FILE_INPUT	14

/* Hack -- check if object is owned by player */
#define obj_own_p(P,O) ((!(O)->owner_id || (P)->id == (O)->owner_id))
/* Hack -- shorthand alias for "check_prevent_inscription" */
#define CPI(P,M) (P)->prevents[(byte)(M)]
/* Hack -- shorthand alias for "check_guard_inscription" */
#define CGI(O,M) check_guard_inscription( (O)->note, (M) )
/* Hack -- overloaded guard-inscriptions */
#define protected_p(P,O,M) (!is_dm_p((P)) && !obj_own_p((P), (O)) && CGI((O), (M)))
/* Hack -- check guard inscription and abort (chunk of code) */ 
#define __trap(I,X) if ((X)) { msg_print((I), "The item's inscription prevents it."); return; }

/*
 * Per-player artifact states
 */
#define ARTS_NOT_FOUND	0
#define ARTS_FOUND  	1
#define ARTS_ABANDONED	2
#define ARTS_SOLD   	3

#define set_artifact_p(P, A, I) if ((P)->a_info[(A)] < (I)) (P)->a_info[(A)] = (I)

/*
 * Monster sorting flags
 */
#define SORT_EXP	0x0001
#define SORT_LEVEL	0x0002
#define SORT_PKILL	0x0004
#define SORT_TKILL	0x0008
#define SORT_EASY	0x000F
#define SORT_RARITY	0x0010
#define SORT_RICH	0x0020
#define SORT_UNIQUE	0x0040
#define SORT_QUEST	0x0080


/* 
 * MAngband-specific miscellaneous hacks
 */
#define PURSE_MULTIPLIER 5 /* For store owners */
#define RIFT_RESIST_TELEPORT 127 /* For monsters */
#define PROJECTED_CHANCE_RATIO 50 /* (1.5) For players */
#define SHARE_ITEM_AWARENESS /* Undefine to disable */ 

/*
 * Dungeon master flags
 */
#define DM_IS_MASTER    	0x00000001
#define DM_SECRET_PRESENCE	0x00000002	/* cfg_secret_dungeon_master helper */
#define DM_CAN_MUTATE_SELF	0x00000004	/* This option allows change of the DM_ options */
#define DM_CAN_ASSIGN   	0x00000008
#define DM___MENU       	0x000000F0	/* Dungeon Master Menu: (shortcut to set all) */
#define DM_CAN_BUILD    	0x00000010  /*  building menu */
#define DM_LEVEL_CONTROL	0x00000020	/*  static/unstatic level */ 
#define DM_CAN_SUMMON   	0x00000040	/*  summon monsters */
#define DM_CAN_GENERATE 	0x00000080	/*  generate vaults / items */
#define DM_MONSTER_FRIEND	0x00000100
#define DM_INVULNERABLE 	0x00000200
#define DM_GHOST_HANDS  	0x00000400	/* Can interact with world (like open/close doors) even as a ghost */ 
#define DM_GHOST_BODY   	0x00000800	/* Can carry/wield items even as a ghost */
#define DM_NEVER_DISTURB	0x00001000
#define DM_SEE_LEVEL    	0x00002000	/* See all level */
#define DM_SEE_MONSTERS 	0x00004000	/* Free ESP + Monster spoilers */
#define DM_SEE_PLAYERS  	0x00008000	/* Full Info + "Player spoilers" */
#define DM_HOUSE_CONTROL	0x00010000	/* Can reset houses */
#define DM_KEEP_LITE    	0x00020000	/* Lite radius never changes */
#define DM_OBJECT_CONTROL	0x00040000	/* Can reset objects */
#define DM_ARTIFACT_CONTROL	0x00080000	/* Can reset artifacts */
#define DM_MISC_XXX1    	0x10000000
#define DM_MISC_XXX2    	0x20000000	/* DM_MISC_XXX -- For grouping some minor features together */
#define DM_MISC_XXX3    	0x40000000
#define DM_MISC_XXX4    	0x80000000

#define is_dm(IND) \
	((Players[IND]->dm_flags & DM_IS_MASTER) ? TRUE : FALSE)

#define is_dm_p(P) \
	(((P)->dm_flags & DM_IS_MASTER) ? TRUE : FALSE)
	
#define dm_flag(IND,F) \
	((Players[(IND)]->dm_flags & & DM_ ## F) ? TRUE : FALSE)

#define dm_flag_p(P,F) \
	(((P)->dm_flags & DM_ ## F) ? TRUE : FALSE)

#define c_put_p(P,A,C,Y,X) { (P)->info[(Y)][(X)].a = (A); (P)->info[(Y)][(X)].c = (C); }
#define c_put(IND,A,C,Y,X) c_put_p(Players[IND],A,C,Y,X)

/*
 * Maximum number of "normal" pack slots, and the index of the "overflow"
 * slot, which can hold an item, but only temporarily, since it causes the
 * pack to "overflow", dropping the "last" item onto the ground.  Since this
 * value is used as an actual slot, it must be less than "INVEN_WIELD" (below).
 * Note that "INVEN_PACK" is probably hard-coded by its use in savefiles, and
 * by the fact that the screen can only show 23 items plus a one-line prompt.
 */
#define INVEN_PACK		23

/*
 * Indexes used for various "equipment" slots (hard-coded by savefiles, etc).
 */
#define INVEN_WIELD		24
#define INVEN_BOW       25
#define INVEN_LEFT      26
#define INVEN_RIGHT     27
#define INVEN_NECK      28
#define INVEN_LITE      29
#define INVEN_BODY      30
#define INVEN_OUTER     31
#define INVEN_ARM       32
#define INVEN_HEAD      33
#define INVEN_HANDS     34
#define INVEN_FEET      35

/*
 * Total number of inventory slots (hard-coded).
 */
#define INVEN_TOTAL	36

/*
 * Legal restrictions for "summon_specific()"
 */
#define SUMMON_ANIMAL		1
#define SUMMON_SPIDER		2
#define SUMMON_HOUND		3
#define SUMMON_HYDRA		4
#define SUMMON_ANGEL		5
#define SUMMON_DEMON		6
#define SUMMON_UNDEAD		7
#define SUMMON_DRAGON		8
#define SUMMON_HI_UNDEAD	9
#define SUMMON_HI_DRAGON	10
#define SUMMON_HI_DEMON		11
#define SUMMON_WRAITH		12
#define SUMMON_UNIQUE		13
#define SUMMON_KIN			14
#define SUMMON_ORC			15