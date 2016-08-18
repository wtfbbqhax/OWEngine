//===========================================================================
//
// Name:			items.c
// Function:		item configuration
// Programmer:		Mr Elusive
// Last update:		1999-12-28
// Tab Size:		4 (real tabs)
//===========================================================================

#include "inv.h"

#define ITEM_NONE					0
#define ITEM_AMMO					1
#define ITEM_WEAPON					2
#define ITEM_HEALTH					3
#define ITEM_ARMOR					4
#define ITEM_POWERUP				5
#define ITEM_KEY					6
#define ITEM_FLAG					7
#define ITEM_ROAM					8


//===================================
// HEALTH
//===================================

iteminfo "item_health_small"
{
	name					"5 Health"
	model					"models/powerups/health/small_cross.md3"
	modelindex				MODELINDEX_HEALTHSMALL
	type					ITEM_HEALTH
	index					INVENTORY_HEALTH
	respawntime				30
	mins					{-15,-15,-15}
	maxs					{15,15,15}
} //end iteminfo

iteminfo "item_health"
{
	name					"25 Health"
	model					"models/powerups/health/medium_cross.md3"
	modelindex				MODELINDEX_HEALTH
	type					ITEM_HEALTH
	index					INVENTORY_HEALTH
	respawntime				30
	mins					{-15,-15,-15}
	maxs					{15,15,15}
} //end iteminfo

iteminfo "item_health_large"
{
	name					"50 Health"
	model					"models/powerups/health/large_cross.md3"
	modelindex				MODELINDEX_HEALTHLARGE
	type					ITEM_HEALTH
	index					INVENTORY_HEALTH
	respawntime				30
	mins					{-15,-15,-15}
	maxs					{15,15,15}
} //end iteminfo

iteminfo "item_health_mega"
{
	name					"Mega Health"
	model					"models/powerups/health/mega_cross.md3"
	modelindex				MODELINDEX_HEALTHMEGA
	type					ITEM_HEALTH
	index					INVENTORY_HEALTH
	respawntime				30
	mins					{-15,-15,-15}
	maxs					{15,15,15}
} //end iteminfo


//===================================
// AMMO
//===================================



//===================================
// CTF flags
//===================================

iteminfo "team_CTF_redflag"
{
	name					"Red Flag"
	model					"models/flags/r_flag.md3"
	modelindex				MODELINDEX_REDFLAG
	type					ITEM_FLAG
	index					INVENTORY_REDFLAG
	mins					{-15,-15,-15}
	maxs					{15,15,15}
} //end iteminfo

iteminfo "team_CTF_blueflag"
{
	name					"Blue Flag"
	model					"models/flags/b_flag.md3"
	modelindex				MODELINDEX_BLUEFLAG
	type					ITEM_FLAG
	index					INVENTORY_BLUEFLAG
	mins					{-15,-15,-15}
	maxs					{15,15,15}
} //end iteminfo


//===================================
// bot roam item
//===================================

iteminfo "item_botroam"
{
	name					"Bot Roam Goal"
	model					""
	modelindex				0
	type					ITEM_ROAM
	index					0
	mins					{-15,-15,-15}
	maxs					{15,15,15}
}


