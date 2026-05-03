#include "cbase.h"
#include "env_rope.h"

// memdbgon must be the last include file in a .cpp file
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( env_rope, CEnvRope );

BEGIN_DATADESC( CEnvRope )
	DEFINE_KEYFIELD( m_flRopeLength,   FIELD_FLOAT,  "rope_length" ),
	DEFINE_KEYFIELD( m_flRopeWidth,    FIELD_FLOAT,  "rope_width" ),
	DEFINE_KEYFIELD( m_nSubdivisions,  FIELD_INTEGER,"rope_subdiv" ),
	DEFINE_KEYFIELD( m_flAttachRadius, FIELD_FLOAT,  "auto_attach_radius" ),
	DEFINE_KEYFIELD( m_iszRopeMaterial,FIELD_STRING, "rope_texture" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CEnvRope, DT_EnvRope )
END_SEND_TABLE()

void CEnvRope::Spawn( void )
{
	BaseClass::Spawn();
	SetThink( &CEnvRope::Think );
	SetNextThink( gpGlobals->curtime );
}

void CEnvRope::Think( void )
{
	SetNextThink( gpGlobals->curtime + TICK_INTERVAL );
}
