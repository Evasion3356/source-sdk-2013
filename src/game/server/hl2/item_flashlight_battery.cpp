//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Flashlight battery pickup - charges the independent flashlight battery,
//          not the HEV suit power.
//
//=============================================================================//

#include "cbase.h"
#include "hl2_player.h"
#include "items.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define FLASHLIGHT_BATTERY_AMOUNT 33.0f	// ~30 seconds of light per battery

class CItemFlashlightBattery : public CItem
{
public:
	DECLARE_CLASS( CItemFlashlightBattery, CItem );

	void Spawn( void )
	{
		Precache();
		SetModel( "models/items/battery.mdl" );
		BaseClass::Spawn();
	}

	void Precache( void )
	{
		PrecacheModel( "models/items/battery.mdl" );
		PrecacheScriptSound( "ItemBattery.Touch" );
	}

	bool MyTouch( CBasePlayer *pPlayer )
	{
		CHL2_Player *pHL2Player = dynamic_cast<CHL2_Player *>( pPlayer );
		if ( !pHL2Player )
			return false;

		if ( !pHL2Player->GiveFlashlightBattery( FLASHLIGHT_BATTERY_AMOUNT ) )
			return false;

		CPASAttenuationFilter filter( this, "ItemBattery.Touch" );
		EmitSound( filter, entindex(), "ItemBattery.Touch" );

		CSingleUserRecipientFilter user( pPlayer );
		user.MakeReliable();
		UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( "item_flashlight_battery" );
		MessageEnd();

		return true;
	}
};

LINK_ENTITY_TO_CLASS( item_flashlight_battery, CItemFlashlightBattery );
PRECACHE_REGISTER( item_flashlight_battery );
