//===========================================================================
//
// Name:			weapons.c
// Function:		weapon configuration
// Programmer:		Mr Elusive
// Last update:		1999-09-08
// Tab Size:		4 (real tabs)
//===========================================================================

#include "inv.h"

#define VEC_ORIGIN						{0, 0, 0}
//projectile flags
#define PFL_WINDOWDAMAGE			1		//projectile damages through window
#define PFL_RETURN					2		//set when projectile returns to owner
//weapon flags
#define WFL_FIRERELEASED			1		//set when projectile is fired with key-up event
//damage types
#define DAMAGETYPE_IMPACT			1		//damage on impact
#define DAMAGETYPE_RADIAL			2		//radial damage
#define DAMAGETYPE_VISIBLE			4		//damage to all entities visible to the projectile
#define DAMAGETYPE_IGNOREARMOR	8		//projectile goes right through armor

//===========================================================================
// Knife
//===========================================================================

projectileinfo //for Knife
{
	name				"knifedamage"
	damage				10
	damagetype			DAMAGETYPE_IMPACT
}

weaponinfo //Knife
{
	name				"Knife"
	number				WEAPONINDEX_KNIFE
	projectile			"knifedamage"
	numprojectiles		1
	speed				0
} //end weaponinfo

//===========================================================================
// Luger
//===========================================================================

projectileinfo //for Luger
{
	name				"lugerbullet"
	damage				18
	damagetype			DAMAGETYPE_IMPACT
}

weaponinfo //Luger
{
	name				"Luger"
	number				WEAPONINDEX_LUGER
	projectile			"lugerbullet"
	numprojectiles		1
	speed				0
} //end weaponinfo

//===========================================================================
// MP40
//===========================================================================

projectileinfo //for MP40
{
	name				"mp40bullet"
	damage				14
	damagetype			DAMAGETYPE_IMPACT
}

weaponinfo //MP40
{
	name				"MP40"
	number				WEAPONINDEX_MP40
	projectile			"mp40bullet"
	numprojectiles		1
	speed				0
} //end weaponinfo

//===========================================================================
// Sten
//===========================================================================

projectileinfo //for Sten
{
	name				"stenbullet"
	damage				14
	damagetype			DAMAGETYPE_IMPACT
}

weaponinfo //Sten
{
	name				"Sten"
	number				WEAPONINDEX_STEN
	projectile			"stenbullet"
	numprojectiles		1
	speed				0
} //end weaponinfo

//===========================================================================
// Colt
//===========================================================================

projectileinfo //for Colt
{
	name				"coltbullet"
	damage				18
	damagetype			DAMAGETYPE_IMPACT
}

weaponinfo //Colt
{
	name				"Colt"
	number				WEAPONINDEX_COLT
	projectile			"coltbullet"
	numprojectiles		1
	speed				0
} //end weaponinfo

//===========================================================================
// Thompson
//===========================================================================

projectileinfo //for Thompson
{
	name				"thompsonbullet"
	damage				18
	damagetype			DAMAGETYPE_IMPACT
}

weaponinfo //Thompson
{
	name				"Thompson"
	number				WEAPONINDEX_THOMPSON
	projectile			"thompsonbullet"
	numprojectiles		1
	speed				0
} //end weaponinfo

//===========================================================================
// Flamethrower
//===========================================================================

projectileinfo //for Flamethrower
{
	name				"fuel"
	damage				9999	// take into account flames last longer
	radius				500
	damagetype			$evalint(DAMAGETYPE_IMPACT|DAMAGETYPE_RADIAL)
}

weaponinfo //Flamethrower
{
	name				"Flamethrower"
	number				WEAPONINDEX_FLAMETHROWER
	projectile			"fuel"
	numprojectiles		1
	speed				700
} //end weaponinfo

//===========================================================================
// Panzerfaust
//===========================================================================

projectileinfo //for Panzerfaust
{
	name				"rocket"
	damage				500
	radius				500
	damagetype			$evalint(DAMAGETYPE_IMPACT|DAMAGETYPE_RADIAL)
}

weaponinfo //Panzerfaust
{
	name				"Panzerfaust"
	number				WEAPONINDEX_PANZERFAUST
	projectile			"rocket"
	numprojectiles		1
	speed				2500
} //end weaponinfo

//===========================================================================
// Grenade Launcher
//===========================================================================

projectileinfo //for Grenade Launcher
{
	name				"grenade"
	damage				250
	radius				250
	damagetype			$evalint(DAMAGETYPE_IMPACT/*|DAMAGETYPE_RADIAL*/)
}

weaponinfo //Grenade Launcher
{
	name				"Grenade Launcher"
	number				WEAPONINDEX_GRENADE_LAUNCHER
	projectile			"grenade"
	numprojectiles		1
	speed				400
	flags				WFL_FIRERELEASED
} //end weaponinfo

weaponinfo //Grenade Pineapple
{
	name				"Grenade Pineapple"
	number				WEAPONINDEX_GRENADE_PINEAPPLE
	projectile			"grenade"
	numprojectiles		1
	speed				900
	flags				WFL_FIRERELEASED
} //end weaponinfo