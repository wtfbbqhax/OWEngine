//===========================================================================
//
// Name:			major_i.c
// Function:		
// Programmer:		Mr Elusive (MrElusive@idsoftware.com)
// Last update:		1999-09-08
// Tab Size:		4 (real tabs)
//===========================================================================

#include "inv.h"

//initial health/armor states
#define FS_HEALTH			2
#define FS_ARMOR			2

//initial weapon weights
#define W_KNIFE					10
#define W_LUGER					20
#define W_COLT					20
#define W_MP40					40
#define W_THOMPSON				40
#define W_STEN					39
#define W_MAUSER				40
#define W_VENOM					50
#define W_FLAMETHROWER			50
#define W_PANZERFAUST			50
#define W_GRENADE				10

//the bot has the weapons, so the weights change a little bit
#define GWW_KNIFE				10
#define GWW_LUGER				20
#define GWW_COLT				20
#define GWW_MP40				40
#define GWW_THOMPSON			40
#define GWW_STEN				39
#define GWW_MAUSER				40
#define GWW_VENOM				50
#define GWW_FLAMETHROWER		50
#define GWW_PANZERFAUST			50
#define GWW_GRENADE				10

//initial powerup weights
#define W_TELEPORTER			40
#define W_MEDKIT				40
#define W_QUAD					400
#define W_ENVIRO				40
#define W_HASTE					40
#define W_INVISIBILITY			40
#define W_REGEN					40
#define W_FLIGHT				40

//flag weight
#define FLAG_WEIGHT				50

//
#include "fw_items.c"

