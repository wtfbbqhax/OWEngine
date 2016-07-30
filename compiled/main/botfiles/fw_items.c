//===========================================================================
//
// Name:			fw_items.c
// Function:		file to start with for fuzzy logic interbreeding sessions
// Programmer:		Mr Elusive
// Tab Size:		4 (real tabs)
// Notes:			-
//===========================================================================


//#define WEAPONS_STAY

//balance range
#define BR_HEALTH				30

//balance macros
#define MZ(value)				(value) < 0 ? 0 : (value)
#define HEALTH_SCALE(v)			balance($evalfloat(MZ(FS_HEALTH*v)), $evalfloat(MZ(FS_HEALTH*v-BR_HEALTH)), $evalfloat(MZ(FS_HEALTH*v+BR_HEALTH)))

//=============================================
// HEALTH
//=============================================

weight "item_health"
{
	switch(INVENTORY_HEALTH)
	{
		case 10: return HEALTH_SCALE(90);
		case 20: return HEALTH_SCALE(90);
		case 30: return HEALTH_SCALE(90);
		case 40: return HEALTH_SCALE(90);
		case 50: return HEALTH_SCALE(80);
		case 60: return HEALTH_SCALE(80);
		case 70: return HEALTH_SCALE(60);
		case 80: return HEALTH_SCALE(60);
		case 90: return HEALTH_SCALE(50);
		case 100: return HEALTH_SCALE(50);
		default: return 0;
	} //end switch
} //end weight

//=============================================
// bot roam goal
//=============================================

weight "item_botroam"
{
	return 1;
} //end weight


