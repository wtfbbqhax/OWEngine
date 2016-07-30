//===========================================================================
//
// Name:			fw_weap.c
// Function:
// Programmer:		Mr Elusive
// Last update:		1999-09-08
// Tab Size:		4 (real tabs)
//===========================================================================

#ifndef W_KNIFE
#define W_KNIFE					10
#endif
#ifndef W_LUGER
#define W_LUGER					20
#endif
#ifndef W_COLT
#define W_COLT					20
#endif
#ifndef W_MP40
#define W_MP40					40
#endif
#ifndef W_THOMPSON
#define W_THOMPSON				40
#endif
#ifndef W_STEN
#define W_STEN					39
#endif
#ifndef W_MAUSER
#define W_MAUSER				40
#endif
#ifndef W_VENOM
#define W_VENOM					50
#endif
#ifndef W_FLAMETHROWER
#define W_FLAMETHROWER			50
#endif
#ifndef W_PANZERFAUST
#define W_PANZERFAUST			50
#endif
#ifndef W_GRENADE
#define W_GRENADE				10
#endif

weight "Knife"
{
	return W_KNIFE;
} //end weight

weight "Luger"
{
	switch(INVENTORY_LUGER)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_9MM)
			{
				case 1: return 0;
				default:
				{
					return W_LUGER;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Colt"
{
	switch(INVENTORY_COLT)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_45CAL)
			{
				case 1: return 0;
				default:
				{
					return W_COLT;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "MP40"
{
	switch(INVENTORY_MP40)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_9MM)
			{
				case 1: return 0;
				default:
				{
					return W_MP40;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Thompson"
{
	switch(INVENTORY_THOMPSON)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_45CAL)
			{
				case 1: return 0;
				default:
				{
					return W_THOMPSON;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Sten"
{
	switch(INVENTORY_STEN)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_9MM)
			{
				case 1: return 0;
				default:
				{
					return W_STEN;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Mauser"
{
	switch(INVENTORY_MAUSER)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_792MM)
			{
				case 1: return 0;
				default:
				{
					return W_MAUSER;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Grenade Launcher"
{
	switch(INVENTORY_GRENADELAUNCHER)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_GRENADES)
			{
				case 1: return 0;
				default:
				{
					return W_GRENADE;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Grenade Pineapple"
{
	switch(INVENTORY_GRENADE_PINEAPPLE)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_GRENADES_AMERICAN)
			{
				case 1: return 0;
				default:
				{
					return W_GRENADE;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Panzerfaust"
{
	switch(INVENTORY_PANZERFAUST)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_PANZERFAUST_AMMO)
			{
				case 1: return 0;
				default:
				{
					return W_PANZERFAUST;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Flamethrower"
{
	switch(INVENTORY_FLAMETHROWER)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_FUEL)
			{
				case 1: return 0;
				default:
				{
					//the lightning gun has a limited range
					switch(ENEMY_HORIZONTAL_DIST)
					{
						case 700: return W_FLAMETHROWER;
						default: return $evalint(W_FLAMETHROWER*0.1);
					} //end switch
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight

weight "Venom"
{
	switch(INVENTORY_VENOM)
	{
		case 1: return 0;
		default:
		{
			switch(INVENTORY_127MM)
			{
				case 1: return 0;
				default:
				{
					return W_VENOM;
				} //end default
			} //end switch
		} //end default
	} //end switch
} //end weight