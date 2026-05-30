#include "cbase.h"
#include "env_rope.h"
#include "movevars_shared.h"

// memdbgon must be the last include file in a .cpp file
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( env_rope, CEnvRope );

BEGIN_DATADESC( CEnvRope )
	DEFINE_KEYFIELD( m_flRopeLength,    FIELD_FLOAT,   "rope_length" ),
	DEFINE_KEYFIELD( m_flRopeWidth,     FIELD_FLOAT,   "rope_width" ),
	DEFINE_KEYFIELD( m_nSubdivisions,   FIELD_INTEGER, "rope_subdiv" ),
	DEFINE_KEYFIELD( m_flAttachRadius,  FIELD_FLOAT,   "auto_attach_radius" ),
	DEFINE_KEYFIELD( m_iszRopeMaterial, FIELD_STRING,  "rope_texture" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CEnvRope, DT_EnvRope )
	SendPropInt(   SENDINFO( m_nActiveNodes ), 5, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO( m_flRopeLength ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_flRopeWidth  ), 0, SPROP_NOSCALE ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  0 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  1 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  2 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  3 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  4 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  5 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  6 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  7 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  8 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes,  9 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes, 10 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes, 11 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes, 12 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes, 13 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes, 14 ), -1, SPROP_COORD ),
	SendPropVector( SENDINFO_NETWORKARRAYELEM( m_vecNodes, 15 ), -1, SPROP_COORD ),
END_SEND_TABLE()

void CEnvRope::Precache()
{
	PrecacheModel( "models/props_shared/ropes/gameplay_rope_10.mdl" );
	BaseClass::Precache();
}

void CEnvRope::Spawn()
{
	Precache();
	BaseClass::Spawn();

	// Defaults for any keyfields not set in Hammer
	if ( m_flRopeLength <= 0.0f )  m_flRopeLength  = 128.0f;
	if ( m_flRopeWidth  <= 0.0f )  m_flRopeWidth   = 2.0f;
	if ( m_nSubdivisions < 2 )     m_nSubdivisions = 8;
	if ( m_flAttachRadius <= 0.0f) m_flAttachRadius = 48.0f;

	SetModel( "models/props_shared/ropes/gameplay_rope_10.mdl" );
	SetSolid( SOLID_NONE );

	InitNodes();

	SetThink( &CEnvRope::Think );
	SetNextThink( gpGlobals->curtime );
}

void CEnvRope::InitNodes()
{
	m_nActiveNodes  = clamp( m_nSubdivisions, 2, ROPE_MAX_NODES );
	m_flSegmentLength = m_flRopeLength / ( m_nActiveNodes - 1 );

	Vector anchor = GetAbsOrigin();
	for ( int i = 0; i < m_nActiveNodes; i++ )
	{
		Vector pos = anchor + Vector( 0, 0, -i * m_flSegmentLength );
		m_vecNodes.Set( i, pos );
		m_vecPrevNodes[i] = pos;
	}
}

void CEnvRope::StepVerlet()
{
	float dt = TICK_INTERVAL;
	Vector gravity( 0, 0, -sv_gravity.GetFloat() * dt * dt );

	for ( int i = 1; i < m_nActiveNodes; i++ )
	{
		Vector pos  = m_vecNodes[i];
		Vector prev = m_vecPrevNodes[i];
		// velocity = pos - prev, dampen slightly, add gravity
		Vector newPos = pos + ( pos - prev ) * 0.98f + gravity;
		m_vecPrevNodes[i] = pos;
		m_vecNodes.Set( i, newPos );
	}
}

void CEnvRope::SolveConstraints()
{
	for ( int iter = 0; iter < ROPE_CONSTRAINT_ITERS; iter++ )
	{
		// Re-pin the anchor at the start of every pass so no interior correction
		// can accumulate drift in node 0 over successive iterations.
		m_vecNodes.Set( 0, GetAbsOrigin() );

		for ( int i = 0; i < m_nActiveNodes - 1; i++ )
		{
			Vector posA = m_vecNodes[i];
			Vector posB = m_vecNodes[i + 1];

			Vector delta = posB - posA;
			float  dist  = delta.Length();
			if ( dist < 0.001f )
				continue;

			float  diff       = ( dist - m_flSegmentLength ) / dist;
			Vector correction = delta * diff;

			if ( i == 0 )
			{
				// Anchor is fixed — only adjust the next node.
				m_vecNodes.Set( 1, posB - correction );
			}
			else
			{
				// Both nodes are free — split the correction evenly.
				m_vecNodes.Set( i,     posA + correction * 0.5f );
				m_vecNodes.Set( i + 1, posB - correction * 0.5f );
			}
		}
	}

	// Final anchor pin; also zero its stored velocity so node 0 never drifts.
	m_vecNodes.Set( 0, GetAbsOrigin() );
	m_vecPrevNodes[0] = GetAbsOrigin();
}

void CEnvRope::Think()
{
	StepVerlet();
	SolveConstraints();
	SetNextThink( gpGlobals->curtime + TICK_INTERVAL );
}

int CEnvRope::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}
