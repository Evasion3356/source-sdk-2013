#ifndef ENV_ROPE_H
#define ENV_ROPE_H
#pragma once

#include "baseentity.h"

#define ROPE_MAX_NODES 16

class CEnvRope : public CBaseEntity
{
public:
	DECLARE_CLASS( CEnvRope, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	void Spawn() override;
	void Think() override;
	int  UpdateTransmitState() override;

	// Player attachment interface
	int    GetNodeCount()      const { return m_nActiveNodes; }
	float  GetSegmentLength()  const { return m_flSegmentLength; }
	float  GetAttachRadius()   const { return m_flAttachRadius; }
	Vector GetNodePos( int i ) const { return m_vecNodes[i]; }
	Vector GetNodeVelocity( int i ) const { return ( m_vecNodes[i] - m_vecPrevNodes[i] ) / TICK_INTERVAL; }
	void   ApplyNodeImpulse( int i, const Vector &impulse )
	{
		if ( i <= 0 || i >= m_nActiveNodes )
			return;
		Vector newPos = m_vecNodes[i] + impulse;
		// Clamp node velocity (pos - prev) so the rope can't exceed MAX_SWING_SPEED u/tick
		static const float MAX_SWING_SPEED = 15.0f; // ~990 u/s at 66 tick
		Vector vel = newPos - m_vecPrevNodes[i];
		if ( vel.Length() > MAX_SWING_SPEED )
			newPos = m_vecPrevNodes[i] + vel.Normalized() * MAX_SWING_SPEED;
		m_vecNodes.Set( i, newPos );
	}
	// Teleport a node to a new position and zero its Verlet velocity.
	// Used to pull the grip node back when the player is stopped by geometry.
	void   SnapNodeTo( int i, const Vector &pos )
	{
		if ( i <= 0 || i >= m_nActiveNodes )
			return;
		m_vecNodes.Set( i, pos );
		m_vecPrevNodes[i] = pos;
	}

private:
	void InitNodes();
	void StepVerlet();
	void SolveConstraints();

	// Keyfields (server-only)
	int		m_nSubdivisions;
	float	m_flAttachRadius;
	string_t m_iszRopeMaterial;

	// Networked
	CNetworkVar( float,  m_flRopeLength );
	CNetworkVar( float,  m_flRopeWidth );
	CNetworkVar( int,    m_nActiveNodes );
	CNetworkArray( Vector, m_vecNodes, ROPE_MAX_NODES );

	// Server-only physics state
	Vector	m_vecPrevNodes[ROPE_MAX_NODES];
	float	m_flSegmentLength;
};

#endif // ENV_ROPE_H
